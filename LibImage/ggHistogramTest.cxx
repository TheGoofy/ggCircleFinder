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

  std::cout << "Histogram"
            << " number_of_bins=" << aHistogram.GetNumberOfBins()
            << " range=" << aHistogram.GetValueMinF() << ".." << aHistogram.GetValueMaxF()
            << " width=" << aHistogram.GetBinWidthF()
            << " count_total=" << aHistogram.GetCountTotal()
            << std::endl;

  for (ggInt64 vBinIndex = 0; vBinIndex < aHistogram.GetNumberOfBins(); vBinIndex++) {
    std::cout << "Bin[" << vBinIndex << "]"
              << " value=" << aHistogram.GetBinValueF(vBinIndex)
              << " count=" << aHistogram.GetCount(vBinIndex)
              << " indexF=" << aHistogram.GetBinIndexF(aHistogram.GetBinValueF(vBinIndex))
              << " countF=" << aHistogram.GetCountF(aHistogram.GetBinValueF(vBinIndex))
              << std::endl;
  }
}


void ggHistogramTest::PrintBinIndex(const ggHistogram& aHistogram, ggDouble aValueStepF)
{
  for (ggDouble vValueF = aHistogram.GetValueMinF(); vValueF <= aHistogram.GetValueMaxF(); vValueF += aValueStepF) {
    std::cout << "value=" << vValueF << " index=" << aHistogram.GetBinIndexF(vValueF) << std::endl;
  }
}


void ggHistogramTest::TestConsistency(const ggHistogram& aHistogram)
{
  ggInt64 vCountTotal = 0;
  GG_TEST_EQUAL(aHistogram.GetBinIndexF(aHistogram.GetValueMinF()), 0);
  GG_TEST_EQUAL(aHistogram.GetBinIndexF(aHistogram.GetValueMaxF()), aHistogram.GetNumberOfBins()-1);
  for (ggInt64 vBinIndex = 0; vBinIndex < aHistogram.GetNumberOfBins(); vBinIndex++) {
    vCountTotal += aHistogram.GetCount(vBinIndex);
    GG_TEST_EQUAL(vBinIndex, aHistogram.GetBinIndexF(aHistogram.GetBinValueF(vBinIndex)));
    GG_TEST_EQUAL(aHistogram.GetCount(vBinIndex), aHistogram.GetCountF(aHistogram.GetBinValueF(vBinIndex)));
  }
  GG_TEST_EQUAL(aHistogram.GetCountTotal(), vCountTotal);
}
