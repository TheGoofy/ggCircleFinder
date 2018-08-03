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


void ggHistogramTest::PrintBinIndex(const ggHistogram& aHistogram,
                                    const ggDouble& aValueStepF)
{
  const ggDouble vValueMinF = aHistogram.GetBinValueF(0);
  const ggDouble vValueMaxF = aHistogram.GetBinValueF(aHistogram.GetNumberOfBins() - 1);
  PrintBinIndex(aHistogram, vValueMinF, vValueMaxF, aValueStepF);
}


void ggHistogramTest::PrintBinIndex(const ggHistogram& aHistogram,
                                    const ggDouble& aValueMinF,
                                    const ggDouble& aValueMaxF,
                                    const ggDouble& aValueStepF)
{
  std::cout << "value=" << aValueMinF << " index=" << aHistogram.GetBinIndexF(aValueMinF) << std::endl;
  for (ggDouble vValueF = aValueMinF + aValueStepF; vValueF < aValueMaxF; vValueF += aValueStepF) {
    std::cout << "value=" << vValueF << " index=" << aHistogram.GetBinIndexF(vValueF) << std::endl;
  }
  std::cout << "value=" << aValueMaxF << " index=" << aHistogram.GetBinIndexF(aValueMaxF) << std::endl;
}


bool ggHistogramTest::TestConsistency(const ggHistogram& aHistogram,
                                      bool aBinIndexAdaptiv,
                                      const char* aReference)
{
  bool vSucceeded = true;
  ggInt64 vCountTotal = 0;

  if (aBinIndexAdaptiv) {
    GG_TEST_EQUAL2(aHistogram.GetBinIndexF(aHistogram.GetValueMinF()), 0, vSucceeded);
    GG_TEST_EQUAL2(aHistogram.GetBinIndexF(aHistogram.GetValueMaxF()), aHistogram.GetNumberOfBins()-1, vSucceeded);
  }

  bool vBinIndexFromValueIsCorrect = true;
  bool vBinCountFromValueIsCorrect = true;
  for (ggInt64 vBinIndex = 0; vBinIndex < aHistogram.GetNumberOfBins(); vBinIndex++) {
    vCountTotal += aHistogram.GetCount(vBinIndex);
    ggDouble vBinValueF = aHistogram.GetBinValueF(vBinIndex);
    if (vBinIndex != aHistogram.GetBinIndexF(vBinValueF)) vBinIndexFromValueIsCorrect = false;
    if (aHistogram.GetCount(vBinIndex) != aHistogram.GetCountF(vBinValueF)) vBinCountFromValueIsCorrect = false;
  }

  GG_TEST2(vBinIndexFromValueIsCorrect, vSucceeded);
  GG_TEST2(vBinCountFromValueIsCorrect, vSucceeded);

  GG_TEST_EQUAL2(aHistogram.GetCountTotal(), vCountTotal, vSucceeded);

  if (!vSucceeded) {
    std::cout << "Failed in: " << __PRETTY_FUNCTION__;
    if (aReference != nullptr) std::cout << " reference => " << aReference;
    std::cout << std::endl;
  }

  return vSucceeded;
}
