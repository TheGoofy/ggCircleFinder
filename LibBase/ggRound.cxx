#include "LibBase/ggRound.h"

#include <math.h>
#include <vector>

#include "LibBase/ggWalkerT.h"


static const std::vector<ggDouble> ggRoundFactors20 =
  {-10.0, -5.0, -2.0, -1.0,
     1.0,  2.0,  5.0, 10.0};

static const std::vector<ggDouble> ggRoundFactors15 =
  {-10.0, -7.0, -5.0, -3.0, -2.0, -1.5, -1.0,
     1.0,  1.5,  2.0,  3.0,  5.0,  7.0, 10.0};

static const std::vector<ggDouble> ggRoundFactors12 =
  {-10.0, -8.0, -7.0, -6.0, -5.0, -4.0, -3.0, -2.5, -2.0, -1.7, -1.5, -1.2, -1.0,
     1.0,  1.2,  1.5,  1.7,  2.0,  2.5,  3.0,  4.0,  5.0,  6.0,  7.0,  8.0, 10.0};


ggFloat ggRoundTo125(ggFloat aValue, ggRoundType aRoundType)
{
  return static_cast<ggFloat>(ggRoundTo125(static_cast<ggDouble>(aValue), aRoundType));
}


ggDouble ggRoundTo125(ggDouble aValue, ggRoundType aRoundType)
{
  // nothing to do, if value is 0
  if (aValue == 0.0) return aValue;

  // preliminary result
  ggDouble vResult = aValue;

  // calculate the order of magnitude (negative if value below 1)
  ggInt32 vDecimals = static_cast<ggInt32>(log10(fabs(aValue)));
  vDecimals = fabs(aValue) < 1.0 ? vDecimals - 1 : vDecimals;

  // select proper factors for rounding
  const std::vector<ggDouble>* vFactors = &ggRoundFactors20;
  switch (aRoundType) {
    case ggRoundType::eFactor20: vFactors = &ggRoundFactors20; break;
    case ggRoundType::eFactor15: vFactors = &ggRoundFactors15; break;
    case ggRoundType::eFactor12: vFactors = &ggRoundFactors12; break;
  }

  // search the value, which is as close as possible to the value
  ggDouble vErrorMin = std::numeric_limits<ggDouble>::max();
  ggWalkerT<std::vector<ggDouble>::const_iterator> vFactorsWalker(*vFactors);
  while (vFactorsWalker) {
    ggDouble vValue = *vFactorsWalker * pow(10.0, vDecimals);
    if (vErrorMin > fabs(vValue - aValue)) {
      vErrorMin = fabs(vValue - aValue);
      vResult = vValue;
    }
  }

  // done! :-)
  return vResult;
}


ggFloat ggRoundToSD(ggFloat aValue, ggUInt16 aSignificantDigits)
{
  return static_cast<ggFloat>(ggRoundToSD(static_cast<ggDouble>(aValue), aSignificantDigits));
}


ggDouble ggRoundToSD(ggDouble aValue, ggUInt16 aSignificantDigits)
{
  // nothing to do, if value is 0
  if (aValue == 0.0) return aValue;

  // calculate the order of magnitude / position of decimal point (negative if value below 1)
  ggInt32 vDecimals = static_cast<ggInt32>(log10(fabs(aValue)));
  vDecimals = fabs(aValue) < 1.0 ? vDecimals - 1 : vDecimals;

  // compute a factor, which is a power of 10
  ggDouble vFactor = pow(10.0, vDecimals - aSignificantDigits + 1);

  // divide, round, and multiply
  ggDouble vRoundOffset = (aValue > 0.0) ? 0.5 : -0.5;
  ggDouble vResult = static_cast<ggInt64>(aValue / vFactor + vRoundOffset) * vFactor;

  return vResult;
}
