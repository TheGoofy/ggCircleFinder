#include "LibImage/ggImageLabeling.h"


/**
 * Adds a translation from one label to the other label.
 * The "aLabelMap" will be modified, that the label with the bigger value can
 * be translated to the label with the lower value (the bigger label will be
 * the key of this map).
 *
 *  A)              | B)              | C)
 *        ==>       |       ==>       |       ==>
 *  1 -> 1 : 1 -> 1 | 1 -> 1 : 1 -> 1 | 1 -> 1 : 1 -> 1
 *  2 -> 1 : 2 -> 1 | 2 -> 2 : 2 -> 1 | 2 -> 2 : 2 -> 1
 *         : 3 -> 1 | 3 -> 2 : 3 -> 1 | 3 -> 2 : 3 -> 1
 *    +    :        | 4 -> 2 : 4 -> 1 | 4 -> 2 : 4 -> 1
 *         :        |        :        |        :
 *  3 -> 2 :        |   +    :        |   +    :
 *                  |        :        |        :
 *                  | 2 -> 1 :        | 3 -> 1 :
 *
 */
void ggImageLabeling::AddLabelConnection(ggUSize aLabelA,
                                         ggUSize aLabelB,
                                         std::vector<ggUSize>& aLabelMap)
{
  // sort the labels for translation from higher label A to lower label B
  if (aLabelA < aLabelB) {
    std::swap(aLabelA, aLabelB);
  }

  // adjust label map size (translation of new labels into itself)
  ggUSize vLabelLast = aLabelMap.size();
  while (aLabelA >= vLabelLast) aLabelMap.push_back(vLabelLast++);

  // translation to itself is handled when adjusting the map size
  if (aLabelA == aLabelB) return;

  // translate label B into final label (Case A)
  aLabelB = aLabelMap[aLabelB];

  // check previous translation of label A
  if (aLabelMap[aLabelA] == aLabelA) {
    // label A was not translated to another label -> add new translation (Case B)
    ggUSize vLabelMapSize = aLabelMap.size();
    for (ggUSize vLabel = 0; vLabel < vLabelMapSize; vLabel++) {
      if (aLabelMap[vLabel] == aLabelA) {
        aLabelMap[vLabel] = aLabelB;
      }
    }
  }
  else {
    // label A is already translated into other label but not to label B -> adjust translation (Case C)
    if (aLabelMap[aLabelA] != aLabelB) {
      // (don't panic! recursion depth is maximum 2)
      AddLabelConnection(aLabelB, aLabelMap[aLabelA], aLabelMap);
    }
  }
}


void ggImageLabeling::AdjustLabelToNeighborFG(ggInt32& aLabel,
                                              const ggInt32& aNeighborLabel,
                                              std::vector<ggUSize>& aLabelMapFG)
{
  // only needs to be adjusted, if foreground, and different
  if ((aNeighborLabel > 0) && (aLabel != aNeighborLabel)) {
    // check if current label is not yet assigned
    if (aLabel == 0) {
      // an unassigned pixel get's the label from its neighbor
      aLabel = aNeighborLabel;
    }
    else {
      // differently labelled neighbors ==> connect them
      AddLabelConnection(static_cast<ggUSize>(aLabel), static_cast<ggUSize>(aNeighborLabel), aLabelMapFG);
    }
  }
}


void ggImageLabeling::AdjustLabelToNeighborBG(ggInt32& aLabel,
                                              const ggInt32& aNeighborLabel,
                                              std::vector<ggUSize>& aLabelMapBG)
{
  // only needs to be adjusted, if background, and different
  if ((aNeighborLabel < 0) && (aLabel != aNeighborLabel)) {
    // check if current label is not yet assigned
    if (aLabel == 0) {
      // an unassigned pixel get's the label from its neighbor
      aLabel = aNeighborLabel;
    }
    else {
      // differently labelled neighbors ==> connect them
      AddLabelConnection(static_cast<ggUSize>(-aLabel), static_cast<ggUSize>(-aNeighborLabel), aLabelMapBG);
    }
  }
}


void ggImageLabeling::CompactLabelMap(std::vector<ggUSize>& aLabelMap)
{
  // create map which translate the translated label B into a new B (with smaller value)
  std::vector<ggUSize> vRemapB(aLabelMap.size(), 0);
  // 0 indicates that a value for B is not used, 1 for used
  for (ggUSize vLabelA = 0; vLabelA < aLabelMap.size(); vLabelA++) {
    vRemapB[aLabelMap[vLabelA]] = 1;
  }
  // re-enumerate values for label B
  ggUSize vNewLabelB = 0;
  for (ggUSize vLabelB = 0; vLabelB < vRemapB.size(); vLabelB++) {
    vRemapB[vLabelB] = (vRemapB[vLabelB] != 0) ? vNewLabelB++ : vNewLabelB;
  }
  // change original label translations
  for (ggUSize vLabelA = 0; vLabelA < aLabelMap.size(); vLabelA++) {
    aLabelMap[vLabelA] = vRemapB[aLabelMap[vLabelA]];
  }
}


const ggVector2Int32 ggImageLabeling::mDistanceInfiniteBG(std::numeric_limits<ggInt32>::lowest(),
                                                          std::numeric_limits<ggInt32>::lowest());

const ggVector2Int32 ggImageLabeling::mDistanceInfiniteFG(std::numeric_limits<ggInt32>::max(),
                                                          std::numeric_limits<ggInt32>::max());
