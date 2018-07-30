#include "LibImage/ggHistogramTest.h"

#include <iostream>

#include "LibImage/ggHistogramIntegerT.h"


static void Print(const ggHistogram& aHistogram)
{
  /*
  - virtual ggInt64 GetNumberOfBins() const = 0;
  - virtual ggInt64 GetCountTotal() const = 0;
  - virtual ggInt64 GetCount(ggInt64 aBinIndex) const = 0;
  virtual ggInt64 GetCountF(const ggDouble& aValueF) const = 0;
  virtual ggInt64 GetBinIndexF(const ggDouble& aValueF) const = 0;
  - virtual ggDouble GetBinValueF(ggInt64 aBinIndex) const = 0;
  - virtual ggDouble GetBinDeltaF() const = 0;
  - virtual ggDouble GetValueMinF() const = 0;
  - virtual ggDouble GetValueMaxF() const = 0;
  */
  std::cout << "Histogram"
            << " number_of_bins=" << aHistogram.GetNumberOfBins()
            << " range=" << aHistogram.GetValueMinF() << ".." << aHistogram.GetValueMaxF()
            << " delta=" << aHistogram.GetBinDeltaF()
            << " count_total=" << aHistogram.GetCountTotal() << std::endl;
  for (ggInt64 vBinIndex = 0; vBinIndex < aHistogram.GetNumberOfBins(); vBinIndex++) {
    std::cout << "Bin[" << vBinIndex << "] value=" << aHistogram.GetBinValueF(vBinIndex) << " count=" << aHistogram.GetCount(vBinIndex) << std::endl;
  }
}


void ggHistogramTest::Execute()
{
  ggHistogramIntegerT<ggUInt8, 8> vHistogram;

  Print(vHistogram);

  vHistogram.Add(3);

  Print(vHistogram);

  vHistogram.Add(2, 4);
  vHistogram.Add(3);
  vHistogram.Add(5);

  Print(vHistogram);

  vHistogram.Add(8, 3);
  vHistogram.Add(10, 1);

  Print(vHistogram);
}
