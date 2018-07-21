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


std::vector<ggColorUInt8> ggUtility::ColorTable()
{
  std::vector<ggColorUInt8> vColorTable(256);
  for (ggUSize vIndex = 0; vIndex < vColorTable.size(); vIndex++) {
    ggUInt8 vI = static_cast<ggUInt8>(vIndex);
    if      (vIndex <   4) vColorTable[vIndex].Set(2*vI,          0,         8*vI,       64*vI);
    else if (vIndex <  32) vColorTable[vIndex].Set(2*vI,          0,         8*vI,       255  );
    else if (vIndex <  64) vColorTable[vIndex].Set(2*vI,          0,         255,        255  );
    else if (vIndex <  96) vColorTable[vIndex].Set(3*(vI-64)+128, 0,         4*(127-vI), 255  );
    else if (vIndex < 128) vColorTable[vIndex].Set(1*(vI-96)+224, 2*(vI-96), 4*(127-vI), 255  );
    else if (vIndex < 160) vColorTable[vIndex].Set(255,           2*(vI-96), 0,          255  );
    else if (vIndex < 192) vColorTable[vIndex].Set(255,           2*(vI-96), 0,          255  );
    else if (vIndex < 224) vColorTable[vIndex].Set(255,           2*(vI-96), 4*(vI-192), 255  );
    else                   vColorTable[vIndex].Set(255,           255,       4*(vI-192), 255  );
  }
  return vColorTable;
}
