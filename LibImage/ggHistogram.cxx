#include "LibImage/ggHistogram.h"
#include <algorithm>


ggHistogram::ggHistogram()
{
}


ggHistogram::~ggHistogram()
{
}


ggInt64 ggHistogram::GetCountMax() const
{
  return GetCount(GetBinIndexCountMax());
}


ggInt64 ggHistogram::GetBinIndexCountMax() const
{
  ggInt64 vCountMax = 0;
  ggInt64 vBinIndexCountMax = 0;
  for (ggInt64 vBinIndex = 0; vBinIndex < GetNumberOfBins(); vBinIndex++) {
    if (GetCount(vBinIndex) > vCountMax) {
      vCountMax = GetCount(vBinIndex);
      vBinIndexCountMax = vBinIndex;
    }
  }
  return vBinIndexCountMax;
}


ggInt64 ggHistogram::GetBinIndexMin() const
{
  return 0;
}


ggInt64 ggHistogram::GetBinIndexMax() const
{
  return std::max<ggInt64>(0, GetNumberOfBins() - 1);
}


ggDouble ggHistogram::GetBinValueMinF() const
{
  return GetBinValueF(GetBinIndexMin());
}


ggDouble ggHistogram::GetBinValueMaxF() const
{
  return GetBinValueF(GetBinIndexMax());
}


ggDouble ggHistogram::GetBinValueMinLowerF() const
{
  const ggInt64 vBinIndex = GetBinIndexMin();
  return GetBinValueF(vBinIndex) - GetBinWidthF(vBinIndex) / 2.0;
}


ggDouble ggHistogram::GetBinValueMaxUpperF() const
{
  const ggInt64 vBinIndex = GetBinIndexMax();
  return GetBinValueF(vBinIndex) + GetBinWidthF(vBinIndex) / 2.0;
}


ggDouble ggHistogram::GetBinValueRangeF() const
{
  return GetBinValueMaxF() - GetBinValueMinF();
}


ggDouble ggHistogram::GetBinValueRangeOuterF() const
{
  return GetBinValueMaxUpperF() - GetBinValueMinLowerF();
}
