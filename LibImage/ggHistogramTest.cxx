#include "LibImage/ggHistogramTest.h"

#include <iostream>

#include "LibBase/ggUnitTest.h"
#include "LibImage/ggHistogram.h"


void ggHistogramTest::Print(const ggHistogram& aHistogram)
{
  /*
  - virtual ggInt64 GetNumberOfBins() const = 0;
  - virtual ggInt64 GetCountTotal() const = 0;
  - virtual ggInt64 GetCount(ggInt64 aBinIndex) const = 0;
  - virtual ggInt64 GetCountF(const ggDouble& aValueF) const = 0;
  - virtual ggInt64 GetBinIndexF(const ggDouble& aValueF) const = 0;
  - virtual ggDouble GetBinValueF(ggInt64 aBinIndex) const = 0;
  - virtual ggDouble GetBinWidthF() const = 0;
  - virtual ggDouble GetValueMinF() const = 0;
  - virtual ggDouble GetValueMaxF() const = 0;
  */

  std::cout << "==> Histogram"
            << " number_of_bins=" << aHistogram.GetNumberOfBins()
            << " range=" << aHistogram.GetValueMinF() << ".." << aHistogram.GetValueMaxF()
            << " count_total=" << aHistogram.GetCountTotal()
            << std::endl;

  for (ggInt64 vBinIndex = 0; vBinIndex < aHistogram.GetNumberOfBins(); vBinIndex++) {

    ggDouble vBinValue = aHistogram.GetBinValueF(vBinIndex);
    ggDouble vBinWidth = aHistogram.GetBinWidthF(vBinIndex);

    std::cout << "Bin[" << vBinIndex << "]"
              << " value=" << vBinValue-vBinWidth/2 << ".." << vBinValue+vBinWidth/2
              << " count=" << aHistogram.GetCount(vBinIndex)
              << " indexF=" << aHistogram.GetBinIndexF(vBinValue)
              << " countF=" << aHistogram.GetCountF(vBinValue)
              << std::endl;
  }
}


void ggHistogramTest::PrintBinIndex(const ggHistogram& aHistogram, ggDouble aValueStepF)
{
  for (ggDouble vValueF = aHistogram.GetValueMinF(); vValueF <= aHistogram.GetValueMaxF(); vValueF += aValueStepF) {
    std::cout << "value=" << vValueF << " index=" << aHistogram.GetBinIndexF(vValueF) << std::endl;
  }
}


bool ggHistogramTest::TestConsistency(const ggHistogram& aHistogram, const char* aReference)
{
  bool vSucceeded = true;
  ggInt64 vCountTotal = 0;

  GG_TEST_EQUAL2(aHistogram.GetBinIndexF(aHistogram.GetValueMinF()), 0, vSucceeded);
  GG_TEST_EQUAL2(aHistogram.GetBinIndexF(aHistogram.GetValueMaxF()), aHistogram.GetNumberOfBins()-1, vSucceeded);

  for (ggInt64 vBinIndex = 0; vBinIndex < aHistogram.GetNumberOfBins(); vBinIndex++) {
    vCountTotal += aHistogram.GetCount(vBinIndex);
    GG_TEST_EQUAL2(vBinIndex, aHistogram.GetBinIndexF(aHistogram.GetBinValueF(vBinIndex)), vSucceeded);
    GG_TEST_EQUAL2(aHistogram.GetCount(vBinIndex), aHistogram.GetCountF(aHistogram.GetBinValueF(vBinIndex)), vSucceeded);
  }

  GG_TEST_EQUAL2(aHistogram.GetCountTotal(), vCountTotal, vSucceeded);

  if (!vSucceeded) {
    std::cout << "Failed in: " << __PRETTY_FUNCTION__;
    if (aReference != nullptr) std::cout << " reference => " << aReference;
    std::cout << std::endl;
  }

  return vSucceeded;
}
