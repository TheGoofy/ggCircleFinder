#ifndef GGIMAGELABELING_H
#define GGIMAGELABELING_H

#include <vector>

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

};

#endif // GGIMAGELABELING_H
