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

  /**
   * @brief FloodFill sets labels connected to a start-position and within a region, if a connected position needs a label
   * @param aRegionIndexBegin
   * @param aRegionIndexEnd
   * @param aStartIndex
   * @param aNeedsLabel: function should return "true", if the specified position is a candidate to be labeled
   * @param aHasLabel: function should return "true", if the specified position already has a label (no need to call "aSetLabel")
   * @param aSetLabel: function should set a label for the specified position (subsequent call of "aHasLabel" needs to return "true")
   */
  static void FloodFill(const ggVector2Size& aRegionIndexBegin,
                        const ggVector2Size& aRegionIndexEnd,
                        const ggVector2Size& aStartIndex,
                        std::function<bool (const ggVector2Size& aIndex)> aNeedsLabel,
                        std::function<bool (const ggVector2Size& aIndex)> aHasLabel,
                        std::function<void (const ggVector2Size& aIndex)> aSetLabel);

  /**
   * Fills all pixels connected to a start-index with a specified value, if the callback-
   * function "aReplaceValue" returns "true".
   */
  template <typename TValueType>
  static void FloodFill(ggImageT<TValueType>& aImage,
                        const ggVector2Size& aStartIndex,
                        const TValueType& aValueNew,
                        std::function<bool (const TValueType& aValue)> aReplaceValue)
  {
    auto vNeedsLabel = [&aImage, &aReplaceValue] (const ggVector2Size& aIndex) -> bool {
      return aReplaceValue(aImage(aIndex));
    };

    auto vHasLabel = [&aImage, &aValueNew] (const ggVector2Size& aIndex) -> bool {
      return aImage(aIndex) == aValueNew;
    };

    auto vSetLabel = [&aImage, &aValueNew] (const ggVector2Size& aIndex) {
      aImage(aIndex) = aValueNew;
    };

    FloodFill(ggVector2Size(0, 0),
              aImage.GetSize(),
              aStartIndex,
              vNeedsLabel,
              vHasLabel,
              vSetLabel);
  }

  /**
   * Fills all pixels connected to a start-index with the same value.
   */
  template <typename TValueType>
  static void FloodFill(ggImageT<TValueType>& aImage,
                        const ggVector2Size& aStartIndex,
                        const TValueType& aValueToFill)
  {
    const TValueType& vValueReference = aImage(aStartIndex);

    auto vValueNeedsFilled = [] (const TValueType& aValue) -> bool {
      return aValue == vValueReference;
    };

    FloodFill(aImage, aStartIndex, aValueToFill, vValueNeedsFilled);
  }

  /**
   * Labels all local minima and maxima. All local minima, which consist of a single pixel (8
   * neighbor pixels have a higher value), will be labeled with -1. Single pixel maxima are labeled
   * with +1. If a local minimum is extended over multiple neighboring pixels (they all have the
   * same value, and their beighbors have higher values), the whole region is labeled with a negative
   * label. Maxima regions reveive a positive label. Each extended local extremum has a unique label.
   * Regions with neighbors of lower and higher values are labeled with 0 (no extremum).
   */
  template <typename TValueType>
  static ggImageT<ggInt32> CalculateLocalMinMax(const ggImageT<TValueType>& aImage)
  {
    const ggInt32 vLabelUnassigned = std::numeric_limits<ggInt32>::lowest();
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

    TValueType vValueForNeedingLabel;
    auto vNeedsLabel = [&aImage, &vValueForNeedingLabel] (const ggVector2Size& aIndex) -> bool {
      return aImage(aIndex) == vValueForNeedingLabel;
    };

    auto vHasLabel = [&vMinMaxImage] (const ggVector2Size& aIndex) -> bool {
      return vMinMaxImage(aIndex) != vLabelUnassigned;
    };

    ggInt32 vLabelToSet = vLabelIndexNoMinMax;
    TValueType vValueNeighborMin = std::numeric_limits<TValueType>::max();
    TValueType vValueNeighborMax = std::numeric_limits<TValueType>::lowest();
    auto vSetLabel = [&vMinMaxImage, &vLabelToSet, &aImage, &vValueNeighborMin, &vValueNeighborMax] (const ggVector2Size& aIndex) {
      vMinMaxImage(aIndex) = vLabelToSet;
      GetNeighborMinMax(aImage, aIndex, vValueNeighborMin, vValueNeighborMax);
    };

    auto vCalculateMinMax = [&aImage,
                             &vMinMaxImage,
                             &vLabelMap,
                             &vLabelNoMinMax,
                             &vLabelLocalMin,
                             &vLabelLocalMax,
                             &vValueForNeedingLabel,
                             &vNeedsLabel,
                             &vLabelToSet,
                             &vHasLabel,
                             &vSetLabel,
                             &vValueNeighborMin,
                             &vValueNeighborMax] (ggSize aIndexX, ggSize aIndexY) {

      ggInt32& vLabel = vMinMaxImage(aIndexX, aIndexY);

      if (vLabel == vLabelUnassigned) {

        const TValueType& vValue = aImage(aIndexX, aIndexY);

        vValueNeighborMin = std::numeric_limits<TValueType>::max();
        vValueNeighborMax = std::numeric_limits<TValueType>::lowest();
        GetNeighborMinMax(aImage, ggVector2Size(aIndexX, aIndexY), vValueNeighborMin, vValueNeighborMax);

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
          vValueForNeedingLabel = vValue;
          vLabelToSet = static_cast<ggInt32>(vLabelMap.size());
          FloodFill(ggVector2Size(0, 0), aImage.GetSize(),
                    ggVector2Size(aIndexX, aIndexY),
                    vNeedsLabel, vHasLabel, vSetLabel);
          if ((vValue <= vValueNeighborMin) && (vValue != vValueNeighborMax)) {
            vLabelMap.push_back(vLabelLocalMin--);
          }
          else if ((vValue >= vValueNeighborMax) && (vValue != vValueNeighborMin)) {
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
                                                          cConnectivity aConnectivity = cConnectivity::eCorner,
                                                          bool aProcessForeground = true,
                                                          bool aProcessBackground = false)
  {
    // initialize with "infinite" distances
    ggImageT<ggInt32> vDistanceImage(aImage.GetSize());
    vDistanceImage.ProcessValues(aImage, [&aValueIsForeground] (ggInt32& aDistance, const TValueType& aValue) {
      aDistance = aValueIsForeground(aValue) ? mDistanceInfiniteFG : mDistanceInfiniteBG;
    });

    // do the distance transformation
    CalculateDistanceTransformCDA3Private(vDistanceImage, aConnectivity, aProcessForeground, aProcessBackground);

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
                                                          std::function<bool (const TValueType& aValue)> aValueIsForeground,
                                                          bool aProcessForeground = true,
                                                          bool aProcessBackground = false)
  {
    // initialize distance vectorfield with "infinite" distances
    ggImageT<ggVector2Int32> vDistanceImage(aImage.GetSize());
    vDistanceImage.ProcessValues(aImage, [&aValueIsForeground] (ggVector2Int32& aDistance, const TValueType& aValue) {
      aDistance = aValueIsForeground(aValue) ? mDistance2InfiniteFG : mDistance2InfiniteBG;
    });

    // do the actual distance calculation
    CalculateDistanceTransform8SEDPrivate(vDistanceImage, aProcessForeground, aProcessBackground);

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
                             const ggInt32 aDistanceDelta,
                             bool aProcessForeground,
                             bool aProcessBackground);

  static void CalculateDistanceTransformCDA3Private(ggImageT<ggInt32>& aDistanceImage,
                                                    cConnectivity aConnectivity,
                                                    bool aProcessForeground,
                                                    bool aProcessBackground);

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
                             const ggVector2Int32& aDistanceDelta,
                             bool aProcessForeground,
                             bool aProcessBackground);

  static void CalculateDistanceTransform8SEDPrivate(ggImageT<ggVector2Int32>& aDistanceImage,
                                                    bool aProcessForeground,
                                                    bool aProcessBackground);

  static void AddIndexToStack(std::function<bool (const ggVector2Size& aIndex)> aNeedsLabel,
                              std::function<bool (const ggVector2Size& aIndex)> aHasLabel,
                              const ggVector2Size& aIndex,
                              std::vector<ggVector2Size>& aIndexStack,
                              bool& aScanLineStartDetected);

  template <typename TValueType>
  static void GetNeighborMinMax(const ggImageT<TValueType>& aImage,
                                const ggVector2Size& aIndex,
                                TValueType& aValueNeighborMin,
                                TValueType& aValueNeighborMax) {
    ggSize vIndexMinX = std::max<ggSize>(aIndex.X() - 1, 0);
    ggSize vIndexMaxX = std::min<ggSize>(aIndex.X() + 1, aImage.GetSizeX() - 1);
    ggSize vIndexMinY = std::max<ggSize>(aIndex.Y() - 1, 0);
    ggSize vIndexMaxY = std::min<ggSize>(aIndex.Y() + 1, aImage.GetSizeY() - 1);
    for (ggSize vIndexY = vIndexMinY; vIndexY <= vIndexMaxY; vIndexY++) {
      for (ggSize vIndexX = vIndexMinX; vIndexX <= vIndexMaxX; vIndexX++) {
        if ((vIndexX != aIndex.X()) || (vIndexY != aIndex.Y())) {
          const TValueType& vValueNeighbor = aImage(vIndexX, vIndexY);
          if (vValueNeighbor > aValueNeighborMax) aValueNeighborMax = vValueNeighbor;
          if (vValueNeighbor < aValueNeighborMin) aValueNeighborMin = vValueNeighbor;
        }
      }
    }
  }

};

#endif // GGIMAGELABELING_H
