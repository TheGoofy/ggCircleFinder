#ifndef GGIMAGELABELING_H
#define GGIMAGELABELING_H

#include <limits>
#include <vector>
#include <iostream>

#include "LibImage/ggImageT.h"

class ggImageLabeling
{
public:

  enum class cConnectivity {
    eEdge,
    eCorner
  };

  // positive label is foreground region, negative label is background region, zero is unassigned (should not happen)
  template <typename TValueType, typename TValueIsForeground>
  static ggImageT<ggInt32> CalculateConnectedComponents(const ggImageT<TValueType>& aImage,
                                                        TValueIsForeground aValueIsForeground,
                                                        cConnectivity aConnectivity)
  {
    ggImageT<ggInt32> vImageLabeled(aImage.GetSize());
    std::vector<ggUSize> vLabelMapFG;
    std::vector<ggUSize> vLabelMapBG;
    AddLabelConnection(0, 0, vLabelMapFG); // unassigned
    AddLabelConnection(0, 0, vLabelMapBG); // unassigned

    // lamda function which adjusts a pixel label depending on it's neighbors
    auto vConnectionCheck = [&aImage,
                             &aValueIsForeground,
                             &aConnectivity,
                             &vImageLabeled,
                             &vLabelMapFG,
                             &vLabelMapBG]
                            (ggSize aIndexX,
                             ggSize aIndexY) {

      // the pixel, which needs to be labelled
      ggInt32& vLabel = vImageLabeled(aIndexX, aIndexY);

      // initially set it to "unassigned"
      vLabel = 0;

      // test if the pixel is foreground or background
      if (aValueIsForeground(aImage(aIndexX, aIndexY))) {
        // try to connect with foreground neighborhood
        if (aIndexX > 0) {
          AdjustLabelToNeighborFG(vLabel, vImageLabeled(aIndexX-1, aIndexY), vLabelMapFG);
          if ((aIndexY > 0) && (aConnectivity == cConnectivity::eCorner)) {
            AdjustLabelToNeighborFG(vLabel, vImageLabeled(aIndexX-1, aIndexY-1), vLabelMapFG);
          }
        }
        if (aIndexY > 0) {
          AdjustLabelToNeighborFG(vLabel, vImageLabeled(aIndexX, aIndexY-1), vLabelMapFG);
          if ((aIndexX + 1 < aImage.GetSizeX()) && (aConnectivity == cConnectivity::eCorner)) {
            AdjustLabelToNeighborFG(vLabel, vImageLabeled(aIndexX+1, aIndexY-1), vLabelMapFG);
          }
        }
        // in case it's still unassigned, a new label needs to be registered
        if (vLabel == 0) {
          vLabel = static_cast<ggInt32>(vLabelMapFG.size());
          AddLabelConnection(static_cast<ggUSize>(vLabel), static_cast<ggUSize>(vLabel), vLabelMapFG);
        }
      }
      else {
        // try to connect with background neighborhood
        if (aIndexX > 0) {
          AdjustLabelToNeighborBG(vLabel, vImageLabeled(aIndexX-1, aIndexY), vLabelMapBG);
          if ((aIndexY > 0) && (aConnectivity != cConnectivity::eCorner)) {
            AdjustLabelToNeighborBG(vLabel, vImageLabeled(aIndexX-1, aIndexY-1), vLabelMapBG);
          }
        }
        if (aIndexY > 0) {
          AdjustLabelToNeighborBG(vLabel, vImageLabeled(aIndexX, aIndexY-1), vLabelMapBG);
          if ((aIndexX + 1 < aImage.GetSizeX()) && (aConnectivity != cConnectivity::eCorner)) {
            AdjustLabelToNeighborBG(vLabel, vImageLabeled(aIndexX+1, aIndexY-1), vLabelMapBG);
          }
        }
        // in case it's still unassigned, a new label needs to be registered
        if (vLabel == 0) {
          vLabel = -static_cast<ggInt32>(vLabelMapBG.size());
          AddLabelConnection(static_cast<ggUSize>(-vLabel), static_cast<ggUSize>(-vLabel), vLabelMapBG);
        }
      }

    };

    // calculate all pixel-labels by iterating with index
    aImage.ProcessIndex(vConnectionCheck);

    // eliminate unused label values (enumerate components continously from 1..n)
    CompactLabelMap(vLabelMapFG);
    CompactLabelMap(vLabelMapBG);

    // lamda function which translates connected labels
    auto vApplyLabelMap = [&vLabelMapFG, &vLabelMapBG] (ggInt32& aValue) {
      aValue = aValue > 0 ? vLabelMapFG[static_cast<ggUSize>(aValue)] : -vLabelMapBG[static_cast<ggUSize>(-aValue)];
    };

    // calculate final labels
    vImageLabeled.ProcessValues(vApplyLabelMap);

    return vImageLabeled;
  }


  static void AdjustDistance(ggInt32& aDistance, const ggInt32 aDistanceNeighbor, const ggInt32 aDelta)
  {
    // check neighbor pixel
    if (aDistanceNeighbor > 0) {
      // neighbor is foreground: check if the path there is shorter
      if (aDistanceNeighbor < std::numeric_limits<ggInt32>::max()) {
        ggInt32 vDistanceNew = aDistanceNeighbor + aDelta;
        if (vDistanceNew < aDistance) aDistance = vDistanceNew;
      }
    }
    else {
      // neighbor is background: use delta if shorter than current
      if (aDelta < aDistance) aDistance = aDelta;
    }
  }

  /**
   * Chamfer distance algorithm.
   *
   * see: Borgefors G. 1986. Distance transformations in digital images.
   *      Comput Vision Graphics Image Process 34:344–371.
   */
  template <typename TValueType, typename TValueIsForeground>
  static ggImageT<ggInt32> CalculateDistanceTransformCDA3x3(const ggImageT<TValueType>& aImage,
                                                            TValueIsForeground aValueIsForeground,
                                                            cConnectivity aConnectivity)
  {
    // initialize with "infinite" distances
    ggImageT<ggInt32> vDistanceImage(aImage.GetSize());
    vDistanceImage.ProcessValues(aImage, [&aValueIsForeground] (ggInt32& aDistance, const TValueType& aValue) {
      aDistance = aValueIsForeground(aValue) ? std::numeric_limits<ggInt32>::max() : 0; // std::numeric_limits<ggInt32>::lowest();
    });

    const ggInt32 vDeltaX = 3;
    const ggInt32 vDeltaY = 3;
    const ggInt32 vDeltaXY = 4;

    // distance calculation for forward loop
    auto vDistanceForward = [&aConnectivity,
                             &vDistanceImage]
                            (ggSize aIndexX,
                             ggSize aIndexY) {
      ggInt32& vDistance = vDistanceImage(aIndexX, aIndexY);
      if (vDistance > 0) {
        if (aIndexX > 0) {
          AdjustDistance(vDistance, vDistanceImage(aIndexX - 1, aIndexY), vDeltaX);
          if ((aIndexY > 0) && (aConnectivity == cConnectivity::eCorner)) {
            AdjustDistance(vDistance, vDistanceImage(aIndexX - 1, aIndexY - 1), vDeltaXY);
          }
        }
        if (aIndexY > 0) {
          AdjustDistance(vDistance, vDistanceImage(aIndexX, aIndexY - 1), vDeltaY);
          if ((aIndexX + 1 < vDistanceImage.GetSizeX()) && (aConnectivity == cConnectivity::eCorner)) {
            AdjustDistance(vDistance, vDistanceImage(aIndexX + 1, aIndexY - 1), vDeltaXY);
          }
        }
      }
    };

    // distance calculation for forward loop
    auto vDistanceReverse = [&aConnectivity,
                             &vDistanceImage]
                            (ggSize aIndexX,
                             ggSize aIndexY) {
      ggInt32& vDistance = vDistanceImage(aIndexX, aIndexY);
      if (vDistance > 0) {
        if (aIndexX + 1 < vDistanceImage.GetSizeX()) {
          AdjustDistance(vDistance, vDistanceImage(aIndexX + 1, aIndexY), vDeltaX);
          if ((aIndexY + 1 < vDistanceImage.GetSizeY()) && (aConnectivity == cConnectivity::eCorner)) {
            AdjustDistance(vDistance, vDistanceImage(aIndexX + 1, aIndexY + 1), vDeltaXY);
          }
        }
        if (aIndexY + 1 < vDistanceImage.GetSizeY()) {
          AdjustDistance(vDistance, vDistanceImage(aIndexX, aIndexY + 1), vDeltaY);
          if ((aIndexX > 0) && (aConnectivity == cConnectivity::eCorner)) {
            AdjustDistance(vDistance, vDistanceImage(aIndexX - 1, aIndexY + 1), vDeltaXY);
          }
        }
      }
    };

    // forward loop
    vDistanceImage.ProcessIndex(vDistanceForward);

    // backward loop
    vDistanceImage.ProcessIndexReverse(vDistanceReverse);

    //
    return vDistanceImage;
  }


  static bool IsForeground(const ggVector2Int32& aDistance) {
    return aDistance.Max() > 0;
  }


  static bool IsBackground(const ggVector2Int32& aDistance) {
    return aDistance.Min() < 0;
  }

  template <typename TValueType>
  static bool IsShorterFG(const ggVector2T<TValueType>& aDistanceA,
                          const ggVector2T<TValueType>& aDistanceB) {
    if (aDistanceA.X() == std::numeric_limits<TValueType>::max()) return false;
    if (aDistanceA.Y() == std::numeric_limits<TValueType>::max()) return false;
    if (aDistanceB.X() == std::numeric_limits<TValueType>::max()) return true;
    if (aDistanceB.Y() == std::numeric_limits<TValueType>::max()) return true;
    return aDistanceA.IsShorter(aDistanceB);
  }

  template <typename TValueType>
  static bool IsShorterBG(const ggVector2T<TValueType>& aDistanceA,
                          const ggVector2T<TValueType>& aDistanceB) {
    if (aDistanceA.X() == std::numeric_limits<TValueType>::lowest()) return false;
    if (aDistanceA.Y() == std::numeric_limits<TValueType>::lowest()) return false;
    if (aDistanceB.X() == std::numeric_limits<TValueType>::lowest()) return true;
    if (aDistanceB.Y() == std::numeric_limits<TValueType>::lowest()) return true;
    return aDistanceA.IsShorter(aDistanceB);
  }

  static void AdjustDistance(ggVector2Int32& aDistance,
                             const ggVector2Int32& aDistanceNeighbor,
                             const ggVector2Int32& aDistanceDelta)
  {
    // check foreground or background
    if (IsForeground(aDistance)) {
      // it's foreground. check neighbor ...
      if (IsForeground(aDistanceNeighbor)) {
        // neighbor is foreground too: if that distance is known, check if the accumulated distance shorter
        if (aDistanceNeighbor != mDistanceInfiniteFG) {
          ggVector2Int32 vDistanceNew(aDistanceNeighbor + aDistanceDelta);
          if (IsShorterFG(vDistanceNew, aDistance)) aDistance = vDistanceNew;
        }
      }
      else {
        // neighbor is background: use delta if shorter than current
        ggVector2Int32 vDistanceDeltaHalf(aDistanceDelta / 2);
        if (IsShorterFG(vDistanceDeltaHalf, aDistance)) {
          aDistance = vDistanceDeltaHalf;
        }
      }
    }
    else {
      // it's background. check neighbor ...
      if (IsBackground(aDistanceNeighbor)) {
        // neighbor is background too: if that distance is known, check if the accumulated distance shorter
        if (aDistanceNeighbor != mDistanceInfiniteBG) {
          ggVector2Int32 vDistanceNew(aDistanceNeighbor - aDistanceDelta);
          if (IsShorterBG(vDistanceNew, aDistance)) aDistance = vDistanceNew;
        }
      }
      else {
        // neighbor is foreground: use delta if shorter than current
        ggVector2Int32 vDistanceDeltaHalf(aDistanceDelta / -2);
        if (IsShorterBG(vDistanceDeltaHalf, aDistance)) {
          aDistance = vDistanceDeltaHalf;
        }
      }
    }
  }

  /**
   * Sequential Euclidean Distance Transformation. 8-Neighnorhood.
   *
   * see: Danielsson P-E. 1980. Euclidean distance mapping. Comput Graphics Image Process 14:227–248
   * see: Grevera G.J. (2007) Distance Transform Algorithms And Their Implementation And Evaluation.
   */
  template <typename TValueType, typename TValueIsForeground>
  static ggImageT<ggFloat> CalculateDistanceTransform8SED(const ggImageT<TValueType>& aImage,
                                                          TValueIsForeground aValueIsForeground)
  {
    // initialize distance vectorfield with "infinite" distances
    ggImageT<ggVector2Int32> vDistanceImage(aImage.GetSize());
    vDistanceImage.ProcessValues(aImage, [&aValueIsForeground] (ggVector2Int32& aDistance, const TValueType& aValue) {
      aDistance = aValueIsForeground(aValue) ? mDistanceInfiniteFG : mDistanceInfiniteBG;
    });

    const ggVector2Int32 vDistanceDeltaX(2,0);
    const ggVector2Int32 vDistanceDeltaY(0,2);
    const ggVector2Int32 vDistanceDeltaXY(2,2);

    for (ggSize vIndexY = 1; vIndexY < vDistanceImage.GetSizeY(); vIndexY++) {
      for (ggSize vIndexX = 0; vIndexX < vDistanceImage.GetSizeX(); vIndexX++) {
        AdjustDistance(vDistanceImage(vIndexX, vIndexY), vDistanceImage(vIndexX, vIndexY-1), vDistanceDeltaY);
      }
      for (ggSize vIndexX = 1; vIndexX < vDistanceImage.GetSizeX(); vIndexX++) {
        AdjustDistance(vDistanceImage(vIndexX, vIndexY), vDistanceImage(vIndexX-1, vIndexY), vDistanceDeltaX);
        AdjustDistance(vDistanceImage(vIndexX, vIndexY), vDistanceImage(vIndexX-1, vIndexY-1), vDistanceDeltaXY);
      }
      for (ggSize vIndexX = vDistanceImage.GetSizeX()-2; vIndexX >= 0; vIndexX--) {
        AdjustDistance(vDistanceImage(vIndexX, vIndexY), vDistanceImage(vIndexX+1, vIndexY), vDistanceDeltaX);
        AdjustDistance(vDistanceImage(vIndexX, vIndexY), vDistanceImage(vIndexX+1, vIndexY-1), vDistanceDeltaXY);
      }
    }

    for (ggSize vIndexY = vDistanceImage.GetSizeY()-2; vIndexY >= 0; vIndexY--) {
      for (ggSize vIndexX = 0; vIndexX < vDistanceImage.GetSizeX(); vIndexX++) {
        AdjustDistance(vDistanceImage(vIndexX, vIndexY), vDistanceImage(vIndexX, vIndexY+1), vDistanceDeltaY);
      }
      for (ggSize vIndexX = 1; vIndexX < vDistanceImage.GetSizeX(); vIndexX++) {
        AdjustDistance(vDistanceImage(vIndexX, vIndexY), vDistanceImage(vIndexX-1, vIndexY), vDistanceDeltaX);
        AdjustDistance(vDistanceImage(vIndexX, vIndexY), vDistanceImage(vIndexX-1, vIndexY+1), vDistanceDeltaXY);
      }
      for (ggSize vIndexX = vDistanceImage.GetSizeX()-2; vIndexX >= 0; vIndexX--) {
        AdjustDistance(vDistanceImage(vIndexX, vIndexY), vDistanceImage(vIndexX+1, vIndexY), vDistanceDeltaX);
        AdjustDistance(vDistanceImage(vIndexX, vIndexY), vDistanceImage(vIndexX+1, vIndexY+1), vDistanceDeltaXY);
      }
    }

    // return image with euclidean distances
    return vDistanceImage.GetProcessed<ggFloat>([] (const ggVector2Int32& aDistance) {
      ggFloat vSign = IsForeground(aDistance) ? 1.0f : -1.0f; // label background distances negative
      return vSign * sqrtf((aDistance.X()*aDistance.X() + aDistance.Y()*aDistance.Y()) / 4);
    });
  }


  enum class cRidgeType {
    eUnknown,
    ePlateau,
    eFlank,
    eLocalMin,
    eRidge2,
    eRidge3,
    eRidge4,
    eLocalMax
  };


  template <typename TValueType>
  static ggImageT<cRidgeType> CalculateRidges(const ggImageT<TValueType>& aImage)
  {
    ggImageT<cRidgeType> vRidgeImage(aImage.GetSize(), cRidgeType::eUnknown);

    static const std::vector<ggVector2Size> vOff = {{-1,-1},
                                                    { 0,-1},
                                                    { 1,-1},
                                                    { 1, 0},
                                                    { 1, 1},
                                                    { 0, 1},
                                                    {-1, 1},
                                                    {-1, 0}};

    aImage.ProcessIndexBorderInside(1, [&aImage, &vRidgeImage] (ggSize aIndexX, ggSize aIndexY) {

      const ggVector2Size vIndex(aIndexX, aIndexY);
      const TValueType& vValue = aImage(vIndex);

      bool vIsLocalMin = true;
      bool vIsLocalMax = true;
      ggInt32 vNumNighborExtremas = 0;
      bool vDirectionKnown = false;
      bool vGoingUp = false;

      ggUSize vIndexOffEnd = vOff.size();
      for (ggUSize vIndexOff = 0; vIndexOff < vIndexOffEnd; vIndexOff++) {

        const ggVector2Size vIndexA(vIndex + vOff[(vIndexOff + 0) % vOff.size()]);
        const ggVector2Size vIndexB(vIndex + vOff[(vIndexOff + 1) % vOff.size()]);
        const TValueType& vValueA = aImage(vIndexA);
        const TValueType& vValueB = aImage(vIndexB);

        // check local max or min
        vIsLocalMin &= vValue < vValueA;
        vIsLocalMax &= vValue > vValueA;

        // check neighborhood
        if (vValueA != vValueB) {
          if (vDirectionKnown) {
            bool vGoingUpNew = vValueA < vValueB;
            if (vGoingUpNew != vGoingUp) {
              vGoingUp = vGoingUpNew;
              vNumNighborExtremas++;
            }
          }
          else {
            vDirectionKnown = true;
            vGoingUp = vValueA < vValueB;
            vIndexOffEnd = vIndexOff + vOff.size() + 1;
          }
        }
      }

      if (vIsLocalMin) {
        vRidgeImage(vIndex) = cRidgeType::eLocalMin;
      }
      else if (vIsLocalMax) {
        vRidgeImage(vIndex) = cRidgeType::eLocalMax;
      }
      else if (vNumNighborExtremas == 0) {
        vRidgeImage(vIndex) = cRidgeType::ePlateau;
      }
      else if (vNumNighborExtremas == 2) {
        vRidgeImage(vIndex) = cRidgeType::eFlank;
      }
      else if (vNumNighborExtremas == 4) {
        vRidgeImage(vIndex) = cRidgeType::eRidge2;
      }
      else if (vNumNighborExtremas == 6) {
        vRidgeImage(vIndex) = cRidgeType::eRidge3;
      }
      else if (vNumNighborExtremas == 8) {
        vRidgeImage(vIndex) = cRidgeType::eRidge4;
      }
      else {
        GG_ASSERT(false); // odd number of extremas not possible
      }
    });

    return vRidgeImage;
  }


private:

  static void AddLabelConnection(ggUSize aLabelA,
                                 ggUSize aLabelB,
                                 std::vector<ggUSize>& aLabelMap);

  static void AdjustLabelToNeighborFG(ggInt32& aLabel,
                                      const ggInt32& aNeighborLabel,
                                      std::vector<ggUSize>& aLabelMapFG);

  static void AdjustLabelToNeighborBG(ggInt32& aLabel,
                                      const ggInt32& aNeighborLabel,
                                      std::vector<ggUSize>& aLabelMapBG);

  static void CompactLabelMap(std::vector<ggUSize>& aLabelMap);

  static const ggVector2Int32 mDistanceInfiniteBG;
  static const ggVector2Int32 mDistanceInfiniteFG;

};

#endif // GGIMAGELABELING_H
