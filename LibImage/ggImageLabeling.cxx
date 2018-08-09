#include "LibImage/ggImageLabeling.h"


const ggInt32 ggImageLabeling::mLabelUnassigned(0);
const ggInt32 ggImageLabeling::mLabelUnassignedBG(std::numeric_limits<ggInt32>::lowest());
const ggInt32 ggImageLabeling::mLabelUnassignedFG(std::numeric_limits<ggInt32>::max());


bool ggImageLabeling::IsLabelBG(const ggInt32 aLabel)
{
  return aLabel < 0;
}


bool ggImageLabeling::IsLabelFG(const ggInt32 aLabel)
{
  return aLabel > 0;
}


bool ggImageLabeling::IsLabelUnassigned(const ggInt32 aLabel)
{
  return (aLabel == mLabelUnassigned) ||
         (aLabel == mLabelUnassignedBG) ||
         (aLabel == mLabelUnassignedFG);
}


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
  if (IsLabelFG(aNeighborLabel) && (aLabel != aNeighborLabel)) {
    // check if current label is not yet assigned
    if (IsLabelUnassigned(aLabel)) {
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
  if (IsLabelBG(aNeighborLabel) && (aLabel != aNeighborLabel)) {
    // check if current label is not yet assigned
    if (IsLabelUnassigned(aLabel)) {
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


void ggImageLabeling::CalculateConnectedComponentsPrivate(ggImageT<ggInt32>& vLabelImage,
                                                          cConnectivity aConnectivity)
{
  std::vector<ggUSize> vLabelMapFG;
  std::vector<ggUSize> vLabelMapBG;
  AddLabelConnection(mLabelUnassigned, mLabelUnassigned, vLabelMapFG);
  AddLabelConnection(mLabelUnassigned, mLabelUnassigned, vLabelMapBG);

  // lamda function which adjusts a pixel label depending on it's neighbors
  auto vConnectionCheck = [&aConnectivity, &vLabelImage, &vLabelMapFG, &vLabelMapBG]
                          (ggSize aIndexX, ggSize aIndexY) {

    // the pixel, which needs to be labelled
    ggInt32& vLabel = vLabelImage(aIndexX, aIndexY);

    // test if the pixel is foreground or background
    if (IsDistanceFG(vLabel)) {
      // try to connect with foreground neighborhood
      if (aIndexX > 0) {
        AdjustLabelToNeighborFG(vLabel, vLabelImage(aIndexX-1, aIndexY), vLabelMapFG);
        if ((aIndexY > 0) && (aConnectivity == cConnectivity::eCorner)) {
          AdjustLabelToNeighborFG(vLabel, vLabelImage(aIndexX-1, aIndexY-1), vLabelMapFG);
        }
      }
      if (aIndexY > 0) {
        AdjustLabelToNeighborFG(vLabel, vLabelImage(aIndexX, aIndexY-1), vLabelMapFG);
        if ((aIndexX + 1 < vLabelImage.GetSizeX()) && (aConnectivity == cConnectivity::eCorner)) {
          AdjustLabelToNeighborFG(vLabel, vLabelImage(aIndexX+1, aIndexY-1), vLabelMapFG);
        }
      }
      // in case it's still unassigned, a new label needs to be registered
      if (IsLabelUnassigned(vLabel)) {
        vLabel = static_cast<ggInt32>(vLabelMapFG.size());
        AddLabelConnection(static_cast<ggUSize>(vLabel), static_cast<ggUSize>(vLabel), vLabelMapFG);
      }
    }
    else {
      // try to connect with background neighborhood
      if (aIndexX > 0) {
        AdjustLabelToNeighborBG(vLabel, vLabelImage(aIndexX-1, aIndexY), vLabelMapBG);
        if ((aIndexY > 0) && (aConnectivity != cConnectivity::eCorner)) {
          AdjustLabelToNeighborBG(vLabel, vLabelImage(aIndexX-1, aIndexY-1), vLabelMapBG);
        }
      }
      if (aIndexY > 0) {
        AdjustLabelToNeighborBG(vLabel, vLabelImage(aIndexX, aIndexY-1), vLabelMapBG);
        if ((aIndexX + 1 < vLabelImage.GetSizeX()) && (aConnectivity != cConnectivity::eCorner)) {
          AdjustLabelToNeighborBG(vLabel, vLabelImage(aIndexX+1, aIndexY-1), vLabelMapBG);
        }
      }
      // in case it's still unassigned, a new label needs to be registered
      if (IsLabelUnassigned(vLabel)) {
        vLabel = -static_cast<ggInt32>(vLabelMapBG.size());
        AddLabelConnection(static_cast<ggUSize>(-vLabel), static_cast<ggUSize>(-vLabel), vLabelMapBG);
      }
    }
  };

  // calculate all pixel-labels by iterating with index
  vLabelImage.ProcessIndex(vConnectionCheck);

  // eliminate unused label values (enumerate components continously from 1..n)
  CompactLabelMap(vLabelMapFG);
  CompactLabelMap(vLabelMapBG);

  // lamda function which translates connected labels
  auto vApplyLabelMap = [&vLabelMapFG, &vLabelMapBG] (ggInt32& aLabel) {
    if (IsLabelUnassigned(aLabel)) {
      aLabel = mLabelUnassigned;
    }
    else {
      if (IsLabelFG(aLabel)) {
        aLabel = static_cast<ggInt32>(vLabelMapFG[static_cast<ggUSize>(aLabel)]);
      }
      else if (IsLabelBG(aLabel)) {
        aLabel = -static_cast<ggInt32>(vLabelMapBG[static_cast<ggUSize>(-aLabel)]);
      }
    }
  };

  // calculate final labels
  vLabelImage.ProcessValues(vApplyLabelMap);
}


const ggInt32 ggImageLabeling::mDistanceInfiniteBG(std::numeric_limits<ggInt32>::lowest());
const ggInt32 ggImageLabeling::mDistanceInfiniteFG(std::numeric_limits<ggInt32>::max());


bool ggImageLabeling::IsDistanceFG(const ggInt32& aDistance)
{
  return aDistance > 0;
}


bool ggImageLabeling::IsDistanceBG(const ggInt32& aDistance)
{
  return aDistance < 0;
}


void ggImageLabeling::AdjustDistance(ggInt32& aDistance,
                                     const ggInt32 aDistanceNeighbor,
                                     const ggInt32 aDistanceDelta)
{
  if (IsDistanceFG(aDistance)) {
    // check neighbor pixel
    if (IsDistanceFG(aDistanceNeighbor)) {
      // neighbor is foreground: check if the path there is shorter
      if (aDistanceNeighbor != mDistanceInfiniteFG) {
        ggInt32 vDistanceNew = aDistanceNeighbor + aDistanceDelta;
        if (vDistanceNew < aDistance) aDistance = vDistanceNew;
      }
    }
    else {
      // neighbor is background: use delta if shorter than current
      if (aDistanceDelta < aDistance) aDistance = aDistanceDelta;
    }
  }
  else {
    // check neighbor pixel
    if (IsDistanceBG(aDistanceNeighbor)) {
      // neighbor is background: check if the path there is shorter
      if (aDistanceNeighbor != mDistanceInfiniteBG) {
        ggInt32 vDistanceNew = aDistanceNeighbor - aDistanceDelta;
        if (vDistanceNew > aDistance) aDistance = vDistanceNew;
      }
    }
    else {
      // neighbor is foreground: use delta if shorter than current
      if (-aDistanceDelta > aDistance) aDistance = -aDistanceDelta;
    }
  }
}


void ggImageLabeling::CalculateDistanceTransformCDA3Private(ggImageT<ggInt32>& aDistanceImage,
                                                            cConnectivity aConnectivity)
{
  const ggInt32 vDeltaX = 3;
  const ggInt32 vDeltaY = 3;
  const ggInt32 vDeltaXY = 4;

  // distance calculation for forward loop
  auto vDistanceCheckForward = [&aConnectivity, &aDistanceImage] (ggSize aIndexX, ggSize aIndexY) {
    ggInt32& vDistance = aDistanceImage(aIndexX, aIndexY);
    if (aIndexX > 0) {
      AdjustDistance(vDistance, aDistanceImage(aIndexX - 1, aIndexY), vDeltaX);
      if ((aIndexY > 0) && (aConnectivity == cConnectivity::eCorner)) {
        AdjustDistance(vDistance, aDistanceImage(aIndexX - 1, aIndexY - 1), vDeltaXY);
      }
    }
    if (aIndexY > 0) {
      AdjustDistance(vDistance, aDistanceImage(aIndexX, aIndexY - 1), vDeltaY);
      if ((aIndexX + 1 < aDistanceImage.GetSizeX()) && (aConnectivity == cConnectivity::eCorner)) {
        AdjustDistance(vDistance, aDistanceImage(aIndexX + 1, aIndexY - 1), vDeltaXY);
      }
    }
  };

  // distance calculation for forward loop
  auto vDistanceCheckReverse = [&aConnectivity, &aDistanceImage] (ggSize aIndexX, ggSize aIndexY) {
    ggInt32& vDistance = aDistanceImage(aIndexX, aIndexY);
    if (aIndexX + 1 < aDistanceImage.GetSizeX()) {
      AdjustDistance(vDistance, aDistanceImage(aIndexX + 1, aIndexY), vDeltaX);
      if ((aIndexY + 1 < aDistanceImage.GetSizeY()) && (aConnectivity == cConnectivity::eCorner)) {
        AdjustDistance(vDistance, aDistanceImage(aIndexX + 1, aIndexY + 1), vDeltaXY);
      }
    }
    if (aIndexY + 1 < aDistanceImage.GetSizeY()) {
      AdjustDistance(vDistance, aDistanceImage(aIndexX, aIndexY + 1), vDeltaY);
      if ((aIndexX > 0) && (aConnectivity == cConnectivity::eCorner)) {
        AdjustDistance(vDistance, aDistanceImage(aIndexX - 1, aIndexY + 1), vDeltaXY);
      }
    }
  };

  // forward loop
  aDistanceImage.ProcessIndex(vDistanceCheckForward);

  // backward loop
  aDistanceImage.ProcessIndexReverse(vDistanceCheckReverse);
}


const ggVector2Int32 ggImageLabeling::mDistance2InfiniteBG(std::numeric_limits<ggInt32>::lowest(),
                                                           std::numeric_limits<ggInt32>::lowest());

const ggVector2Int32 ggImageLabeling::mDistance2InfiniteFG(std::numeric_limits<ggInt32>::max(),
                                                           std::numeric_limits<ggInt32>::max());


bool ggImageLabeling::IsDistanceFG(const ggVector2Int32& aDistance)
{
  return aDistance.Max() > 0;
}


bool ggImageLabeling::IsDistanceBG(const ggVector2Int32& aDistance)
{
  return aDistance.Min() < 0;
}


bool ggImageLabeling::IsShorterFG(const ggVector2Int32& aDistanceA,
                                  const ggVector2Int32& aDistanceB)
{
  if (aDistanceA.X() == mDistance2InfiniteFG.X()) return false;
  if (aDistanceA.Y() == mDistance2InfiniteFG.Y()) return false;
  if (aDistanceB.X() == mDistance2InfiniteFG.X()) return true;
  if (aDistanceB.Y() == mDistance2InfiniteFG.Y()) return true;
  return aDistanceA.IsShorter(aDistanceB);
}


bool ggImageLabeling::IsShorterBG(const ggVector2Int32& aDistanceA,
                                  const ggVector2Int32& aDistanceB)
{
  if (aDistanceA.X() == mDistance2InfiniteBG.X()) return false;
  if (aDistanceA.Y() == mDistance2InfiniteBG.Y()) return false;
  if (aDistanceB.X() == mDistance2InfiniteBG.X()) return true;
  if (aDistanceB.Y() == mDistance2InfiniteBG.Y()) return true;
  return aDistanceA.IsShorter(aDistanceB);
}


void ggImageLabeling::AdjustDistance(ggVector2Int32& aDistance,
                                     const ggVector2Int32& aDistanceNeighbor,
                                     const ggVector2Int32& aDistanceDelta)
{
  // check foreground or background
  if (IsDistanceFG(aDistance)) {
    // it's foreground. check neighbor ...
    if (IsDistanceFG(aDistanceNeighbor)) {
      // neighbor is foreground too: if that distance is known, check if the accumulated distance shorter
      if (aDistanceNeighbor != mDistance2InfiniteFG) {
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
    if (IsDistanceBG(aDistanceNeighbor)) {
      // neighbor is background too: if that distance is known, check if the accumulated distance shorter
      if (aDistanceNeighbor != mDistance2InfiniteBG) {
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


void ggImageLabeling::CalculateDistanceTransform8SEDPrivate(ggImageT<ggVector2Int32>& aDistanceImage)
{
  const ggVector2Int32 vDistanceDeltaX(2,0);
  const ggVector2Int32 vDistanceDeltaY(0,2);
  const ggVector2Int32 vDistanceDeltaXY(2,2);

  for (ggSize vIndexY = 1; vIndexY < aDistanceImage.GetSizeY(); vIndexY++) {
    for (ggSize vIndexX = 0; vIndexX < aDistanceImage.GetSizeX(); vIndexX++) {
      AdjustDistance(aDistanceImage(vIndexX, vIndexY), aDistanceImage(vIndexX, vIndexY-1), vDistanceDeltaY);
    }
    for (ggSize vIndexX = 1; vIndexX < aDistanceImage.GetSizeX(); vIndexX++) {
      AdjustDistance(aDistanceImage(vIndexX, vIndexY), aDistanceImage(vIndexX-1, vIndexY), vDistanceDeltaX);
      AdjustDistance(aDistanceImage(vIndexX, vIndexY), aDistanceImage(vIndexX-1, vIndexY-1), vDistanceDeltaXY);
    }
    for (ggSize vIndexX = aDistanceImage.GetSizeX()-2; vIndexX >= 0; vIndexX--) {
      AdjustDistance(aDistanceImage(vIndexX, vIndexY), aDistanceImage(vIndexX+1, vIndexY), vDistanceDeltaX);
      AdjustDistance(aDistanceImage(vIndexX, vIndexY), aDistanceImage(vIndexX+1, vIndexY-1), vDistanceDeltaXY);
    }
  }

  for (ggSize vIndexY = aDistanceImage.GetSizeY()-2; vIndexY >= 0; vIndexY--) {
    for (ggSize vIndexX = 0; vIndexX < aDistanceImage.GetSizeX(); vIndexX++) {
      AdjustDistance(aDistanceImage(vIndexX, vIndexY), aDistanceImage(vIndexX, vIndexY+1), vDistanceDeltaY);
    }
    for (ggSize vIndexX = 1; vIndexX < aDistanceImage.GetSizeX(); vIndexX++) {
      AdjustDistance(aDistanceImage(vIndexX, vIndexY), aDistanceImage(vIndexX-1, vIndexY), vDistanceDeltaX);
      AdjustDistance(aDistanceImage(vIndexX, vIndexY), aDistanceImage(vIndexX-1, vIndexY+1), vDistanceDeltaXY);
    }
    for (ggSize vIndexX = aDistanceImage.GetSizeX()-2; vIndexX >= 0; vIndexX--) {
      AdjustDistance(aDistanceImage(vIndexX, vIndexY), aDistanceImage(vIndexX+1, vIndexY), vDistanceDeltaX);
      AdjustDistance(aDistanceImage(vIndexX, vIndexY), aDistanceImage(vIndexX+1, vIndexY+1), vDistanceDeltaXY);
    }
  }
}
