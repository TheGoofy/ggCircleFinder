// 0) include own header
#include "ggUtility.h"

// 1) include system
#include <math.h>
#include <vector>

// 2) include own project-related (sort by component dependency)
#include "LibBase/ggWalkerT.h"


const std::vector<ggFloat> ggRoundFactors20 =
  {-10.0f, -5.0f, -2.0f, -1.0f,
     1.0f,  2.0f,  5.0f, 10.0f};

const std::vector<ggFloat> ggRoundFactors15 =
  {-10.0f, -7.0f, -5.0f, -3.0f, -2.0f, -1.5f, -1.0f,
     1.0f,  1.5f,  2.0f,  3.0f,  5.0f,  7.0f, 10.0f};

const std::vector<ggFloat> ggRoundFactors12 =
  {-10.0f, -8.0f, -7.0f, -6.0f, -5.0f, -4.0f, -3.0f, -2.5f, -2.0f, -1.7f, -1.5f, -1.2f, -1.0f,
     1.0f,  1.2f,  1.5f,  1.7f,  2.0f,  2.5f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f, 10.0f};


ggFloat ggUtility::RoundTo125(ggFloat aValue, cRoundType aRoundType)
{
  // nothing to do, if value is 0
  if (aValue == 0.0f) return aValue;

  // preliminary result
  ggFloat vResult = aValue;

  // calculate the order of magnitude (negative if value below 1)
  ggInt32 vDecimals = static_cast<ggInt32>(log10(fabs(aValue)));
  vDecimals = fabs(aValue) < 1.0f ? vDecimals - 1 : vDecimals;

  // select proper factors for rounding
  const std::vector<ggFloat>* vFactors = &ggRoundFactors20;
  switch (aRoundType) {
    case cRoundType::eFactor20: vFactors = &ggRoundFactors20; break;
    case cRoundType::eFactor15: vFactors = &ggRoundFactors15; break;
    case cRoundType::eFactor12: vFactors = &ggRoundFactors12; break;
  }

  // search the value, which is as close as possible to the value
  ggFloat vErrorMin = std::numeric_limits<ggFloat>::max();
  ggWalkerT<std::vector<ggFloat>::const_iterator> vFactorsWalker(*vFactors);
  while (vFactorsWalker) {
    ggFloat vValue = *vFactorsWalker * powf(10.0f, vDecimals);
    if (vErrorMin > fabs(vValue - aValue)) {
      vErrorMin = fabs(vValue - aValue);
      vResult = vValue;
    }
  }

  // done! :-)
  return vResult;
}


ggFloat ggUtility::RoundToSD(ggFloat aValue, ggUInt16 aSignificantDigits)
{
  // nothing to do, if value is 0
  if (aValue == 0.0f) return aValue;

  // calculate the order of magnitude / position of decimal point (negative if value below 1)
  ggInt32 vDecimals = static_cast<ggInt32>(log10(fabs(aValue)));
  vDecimals = fabs(aValue) < 1.0f ? vDecimals - 1 : vDecimals;

  // compute a factor, which is a power of 10
  ggFloat vFactor = powf(10.0f, vDecimals - aSignificantDigits + 1);

  // divide, round, and multiply
  ggFloat vRoundOffset = (aValue > 0.0f) ? 0.5f : -0.5f;
  ggFloat vResult = static_cast<ggInt32>(aValue / vFactor + vRoundOffset) * vFactor;

  return vResult;
}


std::vector<ggColorUInt8> ggUtility::ColorTableHot()
{
  std::vector<ggColorUInt8> vColorTable(256);
  for (ggUSize vIndex = 0; vIndex < 256; vIndex++) {
    ggUInt8 vI = static_cast<ggUInt8>(vIndex);
    if      (vIndex <   4) vColorTable[vIndex].Set(         2*vI,         0,       8*vI, 64*vI); //  4 steps: black => dark blue (transparent => opaque)
    else if (vIndex <  32) vColorTable[vIndex].Set(         2*vI,         0,       8*vI,   255); // 28 steps: dark blue => blue
    else if (vIndex <  64) vColorTable[vIndex].Set(         2*vI,         0,        255,   255); // 32 steps: blue => blue purple
    else if (vIndex <  96) vColorTable[vIndex].Set(3*(vI-64)+128,         0, 4*(127-vI),   255); // 32 steps: blue purple => magenta
    else if (vIndex < 128) vColorTable[vIndex].Set(1*(vI-96)+224, 2*(vI-96), 4*(127-vI),   255); // 32 steps: magenta => light red
    else if (vIndex < 160) vColorTable[vIndex].Set(          255, 2*(vI-96),          0,   255); // 32 steps: light red => orange
    else if (vIndex < 192) vColorTable[vIndex].Set(          255, 2*(vI-96),          0,   255); // 32 steps: orange => orange yellow
    else if (vIndex < 224) vColorTable[vIndex].Set(          255, 2*(vI-96), 4*(vI-192),   255); // 32 steps: orange yellow => light yellow
    else                   vColorTable[vIndex].Set(          255,       255, 4*(vI-192),   255); // 32 steps: light yellow => white
  }
  return vColorTable;
}


std::vector<ggColorUInt8> ggUtility::ColorTableRainbow(bool aIndexZeroTransparent)
{
  std::vector<ggColorUInt8> vColorTable(256);
  for (ggUSize vIndex = 0; vIndex < 256; vIndex++) {
    ggUInt8 vI = static_cast<ggUInt8>(vIndex);
    if      (vIndex <  50) vColorTable[vIndex].Set(           255,              0, 250-5*(vI-0), 255); // 50 steps (250..005): purple => red
    else if (vIndex < 100) vColorTable[vIndex].Set(           255,    5+5*(vI-50),            0, 255); // 50 steps (005..250): red => yellow
    else if (vIndex < 128) vColorTable[vIndex].Set(249-9*(vI-100),            255,            0, 255); // 28 steps (249..006): yellow => green
    else if (vIndex < 156) vColorTable[vIndex].Set(             0,            255, 6+9*(vI-128), 255); // 28 steps (006..249): green => cyan
    else if (vIndex < 206) vColorTable[vIndex].Set(             0, 250-5*(vI-156),          255, 255); // 50 steps (250..005): cyan => blue
    else                   vColorTable[vIndex].Set(  5+5*(vI-206),              0,          255, 255); // 50 steps (005..250): blue => purple
  }
  if (aIndexZeroTransparent) vColorTable[0].SetA(0);
  return vColorTable;
}


std::vector<ggColorUInt8> ggUtility::ColorTableRandom(bool aIndexZeroTransparent)
{
  // use rainbow table, pick random color and swap it with the last color
  std::vector<ggColorUInt8> vColorTable(ColorTableRainbow(false));
  for (ggUSize vIndex = 1; vIndex < vColorTable.size(); vIndex++) {
    ggUSize vIndexLast = vColorTable.size() - vIndex;
    ggUSize vIndexRand = static_cast<ggUSize>(rand()) % vIndexLast;
    std::swap(vColorTable[vIndexRand], vColorTable[vIndexLast]);
  }
  if (aIndexZeroTransparent) vColorTable[0].SetA(0);
  return vColorTable;
}
