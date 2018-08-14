#ifndef GGIMAGELABELING_H
#define GGIMAGELABELING_H

#include <limits>
#include <vector>
#include <iostream>
#include <functional>

#include "LibImage/ggImageT.h"

class ggImageLabeling
{
public:

  enum class cConnectivity {
    eEdge,
    eCorner
  };

  /**
   * Positive label is foreground region, negative label is background
   * region, zero is unassigned (should not happen).
   */
  template <typename TValueType>
  static ggImageT<ggInt32> CalculateConnectedComponents(const ggImageT<TValueType>& aImage,
                                                        std::function<bool (const TValueType& aValue)> aValueIsForeground,
                                                        cConnectivity aConnectivity)
  {
    // initialize with "unassigned" labels
    ggImageT<ggInt32> vLabelImage(aImage.GetSize());
    vLabelImage.ProcessValues(aImage, [&aValueIsForeground] (ggInt32& aLabel, const TValueType& aValue) {
      aLabel = aValueIsForeground(aValue) ? mLabelUnassignedFG : mLabelUnassignedBG;
    });

    // label the connected comtonents
    CalculateConnectedComponentsPrivate(vLabelImage, aConnectivity);

    // done...
    return vLabelImage;
  }


  static void AddLabelConnection(ggInt32 aLabelA,
                                 ggInt32 aLabelB,
                                 std::vector<ggUSize>& aLabelMap)
  {
    AddLabelConnection(static_cast<ggUSize>(aLabelA < 0 ? -aLabelA : aLabelA),
                       static_cast<ggUSize>(aLabelB < 0 ? -aLabelB : aLabelB),
                       aLabelMap);
  }


  static ggInt32 GetLabel(const std::vector<ggUSize>& aLabelMap, ggInt32 aLabel)
  {
    return static_cast<ggInt32>(aLabelMap[static_cast<ggUSize>(aLabel)]);
  }


  static ggInt32 GetNewLabelCandidateMin(const std::vector<ggUSize>& aLabelMap)
  {
    return -static_cast<ggInt32>(aLabelMap.size());
  }


  static ggInt32 GetNewLabelCandidateMax(const std::vector<ggUSize>& aLabelMap)
  {
    return static_cast<ggInt32>(aLabelMap.size());
  }


  template <typename TValueType>
  static ggImageT<ggInt32> CalculateLocalMinMax(const ggImageT<TValueType>& aImage)
  {
    const ggInt32 vLabelUnassigned = std::numeric_limits<ggInt32>::max();
    const ggInt32 vLabelNoMinMax = 0;
    ggInt32 vLabelLocalMin = -1;
    ggInt32 vLabelLocalMax = 1;

    std::vector<ggInt32> vLabelMap(3);
    const ggInt32 vLabelIndexNoMinMax = 0;
    const ggInt32 vLabelIndexLocalMin = 1;
    const ggInt32 vLabelIndexLocalMax = 2;
    vLabelMap[vLabelIndexNoMinMax] = vLabelNoMinMax;
    vLabelMap[vLabelIndexLocalMin] = vLabelLocalMin--;
    vLabelMap[vLabelIndexLocalMax] = vLabelLocalMax++;

    ggImageT<ggInt32> vMinMaxImage(aImage.GetSize(), vLabelUnassigned);

    // adds index to stack, if label not yet assigned. then increments index-x
    auto vAddIndexToStack = [&aImage, &vMinMaxImage] (ggVector2Size& aIndex,
                                                      const TValueType& aValue,
                                                      std::vector<ggVector2Size>& aIndexStack,
                                                      bool& aScanLineStartDetected) {
      if ((vMinMaxImage(aIndex) == vLabelUnassigned) && (aImage(aIndex) == aValue)) {
        if (!aScanLineStartDetected) {
          aScanLineStartDetected = true;
          aIndexStack.push_back(aIndex);
        }
      }
      else {
        aScanLineStartDetected = false;
      }
      ++aIndex.X();
    };

    //
    auto vGetNeighborMinMax = [&aImage] (ggSize aIndexX, ggSize aIndexY, TValueType& aValueNeighborMin, TValueType& aValueNeighborMax) {
      ggSize vIndexMinX = std::max<ggSize>(aIndexX - 1, 0);
      ggSize vIndexMaxX = std::min<ggSize>(aIndexX + 1, aImage.GetSizeX() - 1);
      ggSize vIndexMinY = std::max<ggSize>(aIndexY - 1, 0);
      ggSize vIndexMaxY = std::min<ggSize>(aIndexY + 1, aImage.GetSizeY() - 1);
      for (ggSize vIndexY = vIndexMinY; vIndexY <= vIndexMaxY; vIndexY++) {
        for (ggSize vIndexX = vIndexMinX; vIndexX <= vIndexMaxX; vIndexX++) {
          if ((vIndexX != aIndexX) || (vIndexY != aIndexY)) {
            const TValueType& vValueNeighbor = aImage(vIndexX, vIndexY);
            if (vValueNeighbor > aValueNeighborMax) aValueNeighborMax = vValueNeighbor;
            if (vValueNeighbor < aValueNeighborMin) aValueNeighborMin = vValueNeighbor;
          }
        }
      }
    };

    // fills all connected pixels (aIndexX, aIndexY) with the same label
    auto vFloodFill = [&aImage, &vMinMaxImage, &vGetNeighborMinMax, &vAddIndexToStack] (ggSize aIndexX, ggSize aIndexY, TValueType& aValueNeighborMin, TValueType& aValueNeighborMax) {

      // add the starting-index to the stack
      ggVector2Size vIndexStart(aIndexX, aIndexY);
      const TValueType& vValue = aImage(vIndexStart);
      const ggInt32& vLabel = vMinMaxImage(vIndexStart);
      std::vector<ggVector2Size> vIndexStack(1, vIndexStart);

      // the stack contains starting indices
      while (!vIndexStack.empty()) {

        // pop an index from the stack
        ggVector2Size vIndex = vIndexStack.back();
        vIndexStack.pop_back();

        // find the leftmost pixel (begin of scan-line)
        while ((vIndex.X() > 0) && (aImage(vIndex.X() - 1, vIndex.Y()) == vValue)) --vIndex.X();

        // 8-neighborhood: check above-left and above (above-right will be checked during the x-scan)
        bool vScanAbove = false;
        bool vScanLineStartDetectedA = false;
        ggVector2Size vIndexA(vIndex);
        if (vIndexA.Y() + 1 < aImage.GetSizeY()) {
          vScanAbove = true;
          ++vIndexA.Y();
          if (vIndexA.X() > 0) {
            --vIndexA.X();
            vAddIndexToStack(vIndexA, vValue, vIndexStack, vScanLineStartDetectedA);
          }
          vAddIndexToStack(vIndexA, vValue, vIndexStack, vScanLineStartDetectedA);
        }

        // 8-neighborhood: check below-left and below (below-right will be checked during the x-scan)
        bool vScanBelow = false;
        bool vScanLineStartDetectedB = false;
        ggVector2Size vIndexB(vIndex);
        if (vIndexB.Y() > 0) {
          vScanBelow = true;
          --vIndexB.Y();
          if (vIndexB.X() > 0) {
            --vIndexB.X();
            vAddIndexToStack(vIndexB, vValue, vIndexStack, vScanLineStartDetectedB);
          }
          vAddIndexToStack(vIndexB, vValue, vIndexStack, vScanLineStartDetectedB);
        }

        // fill scan-line, check neighbors above and below
        while ((vIndex.X() < aImage.GetSizeX()) && (aImage(vIndex) == vValue)) {
          vMinMaxImage(vIndex) = vLabel;
          vGetNeighborMinMax(vIndex.X(), vIndex.Y(), aValueNeighborMin, aValueNeighborMax);
          if (vIndex.X() + 1 < aImage.GetSizeX()) {
            if (vScanAbove) vAddIndexToStack(vIndexA, vValue, vIndexStack, vScanLineStartDetectedA);
            if (vScanBelow) vAddIndexToStack(vIndexB, vValue, vIndexStack, vScanLineStartDetectedB);
          }
          ++vIndex.X();
        }
      }
    };

    auto vCalculateMinMax = [&aImage, &vMinMaxImage, &vLabelMap, &vLabelNoMinMax, &vLabelLocalMin, &vLabelLocalMax, &vGetNeighborMinMax, &vFloodFill] (ggSize aIndexX, ggSize aIndexY) {

      ggInt32& vLabel = vMinMaxImage(aIndexX, aIndexY);

      if (vLabel == vLabelUnassigned) {

        const TValueType& vValue = aImage(aIndexX, aIndexY);

        TValueType vValueNeighborMin = std::numeric_limits<TValueType>::max();
        TValueType vValueNeighborMax = std::numeric_limits<TValueType>::lowest();
        vGetNeighborMinMax(aIndexX, aIndexY, vValueNeighborMin, vValueNeighborMax);

        GG_ASSERT(vValueNeighborMin <= vValueNeighborMax);

        if (vValue < vValueNeighborMin) {
          vLabel = vLabelIndexLocalMin;
        }
        else if (vValue > vValueNeighborMax) {
          vLabel = vLabelIndexLocalMax;
        }
        else if ((vValueNeighborMin < vValue) && (vValue < vValueNeighborMax)) {
          vLabel = vLabelIndexNoMinMax;
        }
        else {
          vLabel = static_cast<ggInt32>(vLabelMap.size());
          vFloodFill(aIndexX, aIndexY, vValueNeighborMin, vValueNeighborMax);
          if (vValue <= vValueNeighborMin) {
            vLabelMap.push_back(vLabelLocalMin--);
          }
          else if (vValue >= vValueNeighborMax) {
            vLabelMap.push_back(vLabelLocalMax++);
          }
          else {
            vLabelMap.push_back(vLabelNoMinMax);
          }
        }
      }

    };

    aImage.ProcessIndex(vCalculateMinMax);

    // lamda function which translates labels
    auto vApplyLabelMap = [&vLabelMap] (ggInt32& aLabel) {
      aLabel = vLabelMap[static_cast<ggUSize>(aLabel)];
    };

    // calculate final labels
    vMinMaxImage.ProcessValues(vApplyLabelMap);

    return vMinMaxImage;
  }


  /**
   * Chamfer distance algorithm.
   * Not as accurate as 8SED, but roughly 3 times faster.
   *
   * see: Borgefors G. 1986. Distance transformations in digital images.
   *      Comput Vision Graphics Image Process 34:344–371.
   */
  template <typename TValueType>
  static ggImageT<ggInt32> CalculateDistanceTransformCDA3(const ggImageT<TValueType>& aImage,
                                                          std::function<bool (const TValueType& aValue)> aValueIsForeground,
                                                          cConnectivity aConnectivity)
  {
    // initialize with "infinite" distances
    ggImageT<ggInt32> vDistanceImage(aImage.GetSize());
    vDistanceImage.ProcessValues(aImage, [&aValueIsForeground] (ggInt32& aDistance, const TValueType& aValue) {
      aDistance = aValueIsForeground(aValue) ? mDistanceInfiniteFG : mDistanceInfiniteBG;
    });

    // do the distance transformation
    CalculateDistanceTransformCDA3Private(vDistanceImage, aConnectivity);

    // return "raw" integer distances
    return vDistanceImage;
  }

  /**
   * Sequential Euclidean Distance Transformation. 8-Neighnorhood.
   *
   * see: Danielsson P-E. 1980. Euclidean distance mapping. Comput Graphics Image Process 14:227–248
   * see: Grevera G.J. (2007) Distance Transform Algorithms And Their Implementation And Evaluation.
   */
  template <typename TValueType>
  static ggImageT<ggFloat> CalculateDistanceTransform8SED(const ggImageT<TValueType>& aImage,
                                                          std::function<bool (const TValueType& aValue)> aValueIsForeground)
  {
    // initialize distance vectorfield with "infinite" distances
    ggImageT<ggVector2Int32> vDistanceImage(aImage.GetSize());
    vDistanceImage.ProcessValues(aImage, [&aValueIsForeground] (ggVector2Int32& aDistance, const TValueType& aValue) {
      aDistance = aValueIsForeground(aValue) ? mDistance2InfiniteFG : mDistance2InfiniteBG;
    });

    // do the actual distance calculation
    CalculateDistanceTransform8SEDPrivate(vDistanceImage);

    // return image with euclidean distances (length of distance vectors)
    return vDistanceImage.GetProcessed<ggFloat>([] (const ggVector2Int32& aDistance) {
      ggFloat vScale = IsDistanceFG(aDistance) ? 0.5f : -0.5f; // label background distances negative
      return vScale * sqrtf((aDistance.X()*aDistance.X() + aDistance.Y()*aDistance.Y()));
    });
  }


  enum class cTopologyType {
    eUnknown,
    ePlateau,
    eFlank,
    eLocalMin,
    eValley,
    eSaddle,
    eRidge,
    eLocalMax
  };


  template <typename TValueType>
  static ggImageT<cTopologyType> CalculateTopology(const ggImageT<TValueType>& aImage)
  {
    ggImageT<cTopologyType> vTopologyImage(aImage.GetSize(), cTopologyType::eUnknown);

    static const std::vector<ggVector2Size> vOff = {{-1,-1},
                                                    { 0,-1},
                                                    { 1,-1},
                                                    { 1, 0},
                                                    { 1, 1},
                                                    { 0, 1},
                                                    {-1, 1},
                                                    {-1, 0}};

    aImage.ProcessIndexBorderInside(1, [&aImage, &vTopologyImage] (ggSize aIndexX, ggSize aIndexY) {

      const ggVector2Size vIndex(aIndexX, aIndexY);
      const TValueType& vValue = aImage(vIndex);

      bool vNeighborDirectionKnown = false;
      bool vNeighborIsIncreasing = false;

      bool vIsLocalMin = true;
      bool vIsLocalMax = true;
      ggInt32 vNumNeighborExtremas = 0;
      TValueType vValueNeighborLocalMinHighest = std::numeric_limits<TValueType>::lowest();
      TValueType vValueNeighborLocalMaxLowest = std::numeric_limits<TValueType>::max();

      ggUSize vIndexOffEnd = vOff.size();
      for (ggUSize vIndexOff = 0; vIndexOff < vIndexOffEnd; vIndexOff++) {

        const ggVector2Size vIndexNeighborA(vIndex + vOff[(vIndexOff + 0) % vOff.size()]);
        const ggVector2Size vIndexNeighborB(vIndex + vOff[(vIndexOff + 1) % vOff.size()]);
        const TValueType& vValueNeighborA = aImage(vIndexNeighborA);
        const TValueType& vValueNeighborB = aImage(vIndexNeighborB);

        // check local max or min
        vIsLocalMin &= vValue < vValueNeighborA;
        vIsLocalMax &= vValue > vValueNeighborA;

        // check neighborhood
        if (vValueNeighborA != vValueNeighborB) {
          if (vNeighborDirectionKnown) {
            bool vNeighborValueIncreasingNew = vValueNeighborA < vValueNeighborB;
            if (vNeighborValueIncreasingNew != vNeighborIsIncreasing) {
              vNeighborIsIncreasing = vNeighborValueIncreasingNew;
              vNumNeighborExtremas++;
              if (vNeighborIsIncreasing) { // when B is going up now, A was a local minimum
                if (vValueNeighborA > vValueNeighborLocalMinHighest) vValueNeighborLocalMinHighest = vValueNeighborA;
              }
              else { // when B is going down now, A was a local maximum
                if (vValueNeighborA < vValueNeighborLocalMaxLowest) vValueNeighborLocalMaxLowest = vValueNeighborA;
              }
            }
          }
          else {
            vNeighborDirectionKnown = true;
            vNeighborIsIncreasing = vValueNeighborA < vValueNeighborB;
            vIndexOffEnd = vIndexOff + vOff.size() + 1;
          }
        }
      }

      if (vIsLocalMin) {
        vTopologyImage(vIndex) = cTopologyType::eLocalMin;
      }
      else if (vIsLocalMax) {
        vTopologyImage(vIndex) = cTopologyType::eLocalMax;
      }
      else if (vNumNeighborExtremas == 0) {
        vTopologyImage(vIndex) = cTopologyType::ePlateau;
      }
      else if (vNumNeighborExtremas == 2) {
        vTopologyImage(vIndex) = cTopologyType::eFlank;
      }
      else if (vValue >= vValueNeighborLocalMaxLowest && vValue > vValueNeighborLocalMinHighest) {
        vTopologyImage(vIndex) = cTopologyType::eRidge;
      }
      else if (vValue <= vValueNeighborLocalMinHighest && vValue < vValueNeighborLocalMaxLowest) {
        vTopologyImage(vIndex) = cTopologyType::eValley;
      }
      else if (vNumNeighborExtremas % 2 == 0) {
        vTopologyImage(vIndex) = cTopologyType::eSaddle;
      }
      else {
        GG_ASSERT(false); // odd number of extremas not possible
      }
    });

    return vTopologyImage;
  }


private:

  static const ggInt32 mLabelUnassigned;
  static const ggInt32 mLabelUnassignedBG;
  static const ggInt32 mLabelUnassignedFG;

  static bool IsLabelBG(const ggInt32 aLabel);
  static bool IsLabelFG(const ggInt32 aLabel);
  static bool IsLabelUnassigned(const ggInt32 aLabel);

  static void AddLabelConnection(ggUSize aLabelA,
                                 ggUSize aLabelB,
                                 std::vector<ggUSize>& aLabelMap);

  static void AdjustLabelToNeighbor(ggInt32& aLabel,
                                    const ggInt32& aNeighborLabel,
                                    std::vector<ggUSize>& aLabelMapFG);

  static void AdjustLabelToNeighborFG(ggInt32& aLabel,
                                      const ggInt32& aNeighborLabel,
                                      std::vector<ggUSize>& aLabelMapFG);

  static void AdjustLabelToNeighborBG(ggInt32& aLabel,
                                      const ggInt32& aNeighborLabel,
                                      std::vector<ggUSize>& aLabelMapBG);

  static void CompactLabelMap(std::vector<ggUSize>& aLabelMap);

  static void CalculateConnectedComponentsPrivate(ggImageT<ggInt32>& aLabelImage,
                                                  cConnectivity aConnectivity);

  static const ggInt32 mDistanceInfiniteBG;
  static const ggInt32 mDistanceInfiniteFG;

  static bool IsDistanceFG(const ggInt32& aDistance);
  static bool IsDistanceBG(const ggInt32& aDistance);

  static void AdjustDistance(ggInt32& aDistance,
                             const ggInt32 aDistanceNeighbor,
                             const ggInt32 aDistanceDelta);

  static void CalculateDistanceTransformCDA3Private(ggImageT<ggInt32>& aDistanceImage,
                                                    cConnectivity aConnectivity);

  static const ggVector2Int32 mDistance2InfiniteBG;
  static const ggVector2Int32 mDistance2InfiniteFG;

  static bool IsDistanceFG(const ggVector2Int32& aDistance);
  static bool IsDistanceBG(const ggVector2Int32& aDistance);
  static bool IsShorterFG(const ggVector2Int32& aDistanceA,
                          const ggVector2Int32& aDistanceB);
  static bool IsShorterBG(const ggVector2Int32& aDistanceA,
                          const ggVector2Int32& aDistanceB);

  static void AdjustDistance(ggVector2Int32& aDistance,
                             const ggVector2Int32& aDistanceNeighbor,
                             const ggVector2Int32& aDistanceDelta);

  static void CalculateDistanceTransform8SEDPrivate(ggImageT<ggVector2Int32>& aDistanceImage);

};

#endif // GGIMAGELABELING_H
