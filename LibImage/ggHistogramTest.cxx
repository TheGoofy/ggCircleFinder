#include <iostream>

#include "LibBase/ggUnitTest.h"
#include "LibImage/ggHistogramIntegerT.h"


static void ggPrint(const ggHistogram& aHistogram)
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


static void ggPrintBinIndex(const ggHistogram& aHistogram, ggDouble aValueStepF)
{
  for (ggDouble vValueF = aHistogram.GetValueMinF(); vValueF <= aHistogram.GetValueMaxF(); vValueF += aValueStepF) {
    std::cout << "value=" << vValueF << " index=" << aHistogram.GetBinIndexF(vValueF) << std::endl;
  }
}


static void ggTestConsistency(const ggHistogram& aHistogram)
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


template <class TValueType, ggInt64 TCountBinCapacity>
static void ggTestHistogramIntegerT(bool aPrint, bool aPrintBinIndex)
{
  if (aPrint) std::cout << __PRETTY_FUNCTION__ << " ====================" << std::endl;

  ggHistogramIntegerT<TValueType, TCountBinCapacity> vHistogram;

  if (aPrint) ggPrint(vHistogram);
  if (aPrintBinIndex) ggPrintBinIndex(vHistogram, 0.1);
  ggTestConsistency(vHistogram);

  vHistogram.Add(3);
  vHistogram.Add(4);

  if (aPrint) ggPrint(vHistogram);
  if (aPrintBinIndex) ggPrintBinIndex(vHistogram, 0.2);
  ggTestConsistency(vHistogram);

  vHistogram.Add(5, 4);
  vHistogram.Add(6);
  vHistogram.Add(7, 5);
  vHistogram.Add(8);

  if (aPrint) ggPrint(vHistogram);
  if (aPrintBinIndex) ggPrintBinIndex(vHistogram, 0.2);
  ggTestConsistency(vHistogram);

  vHistogram.Add(5, 1);
  vHistogram.Add(9, 3);
  vHistogram.Add(10, 1);
  vHistogram.Add(11, 7);
  vHistogram.Add(12, 1);
  vHistogram.Add(13, 10);

  if (aPrint) ggPrint(vHistogram);
  if (aPrintBinIndex) ggPrintBinIndex(vHistogram, 0.5);
  ggTestConsistency(vHistogram);

  vHistogram.Reset();

  if (aPrint) ggPrint(vHistogram);
  if (aPrintBinIndex) ggPrintBinIndex(vHistogram, 0.1);
  ggTestConsistency(vHistogram);
}


static void ggHistogramTest()
{
  bool vPrint = false;
  bool vPrintBinIndex = false;
  ggTestHistogramIntegerT<ggUInt8, 6>(vPrint, vPrintBinIndex);
  ggTestHistogramIntegerT<ggUInt8, 8>(vPrint, vPrintBinIndex);
  ggTestHistogramIntegerT<ggUInt8, 12>(vPrint, vPrintBinIndex);
  ggTestHistogramIntegerT<ggUInt8, 16>(vPrint, vPrintBinIndex);
  ggTestHistogramIntegerT<ggUInt8, 256>(vPrint, vPrintBinIndex);
  ggTestHistogramIntegerT<ggInt8, 16>(vPrint, vPrintBinIndex);
  ggTestHistogramIntegerT<ggInt8, 64>(vPrint, vPrintBinIndex);
  ggTestHistogramIntegerT<ggInt8, 256>(vPrint, vPrintBinIndex);
  ggTestHistogramIntegerT<ggUInt16, 8>(vPrint, vPrintBinIndex);
  ggTestHistogramIntegerT<ggInt16, 16>(vPrint, vPrintBinIndex);
}


static bool ggRegistered = ggUnitTest::Register(ggHistogramTest);
