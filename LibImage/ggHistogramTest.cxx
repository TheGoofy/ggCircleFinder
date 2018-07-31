#include "LibImage/ggHistogramTest.h"

#include <iostream>

#include "LibBase/ggUnitTest.h"
#include "LibImage/ggHistogramIntegerT.h"


static void Print(const ggHistogram& aHistogram)
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


static void PrintBinIndex(const ggHistogram& aHistogram, ggDouble aValueStepF)
{
  for (ggDouble vValueF = aHistogram.GetValueMinF(); vValueF <= aHistogram.GetValueMaxF(); vValueF += aValueStepF) {
    std::cout << "value=" << vValueF << " index=" << aHistogram.GetBinIndexF(vValueF) << std::endl;
  }
}


static void TestConsistency(const ggHistogram& aHistogram)
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
void TestHistogramIntegerT(bool aPrint, bool aPrintBinIndex)
{
  if (aPrint) std::cout << __PRETTY_FUNCTION__ << " ====================" << std::endl;

  ggHistogramIntegerT<TValueType, TCountBinCapacity> vHistogram;

  if (aPrint) Print(vHistogram);
  if (aPrintBinIndex) PrintBinIndex(vHistogram, 0.1);
  TestConsistency(vHistogram);

  vHistogram.Add(3);
  vHistogram.Add(4);

  if (aPrint) Print(vHistogram);
  if (aPrintBinIndex) PrintBinIndex(vHistogram, 0.2);
  TestConsistency(vHistogram);

  vHistogram.Add(5, 4);
  vHistogram.Add(6);
  vHistogram.Add(7, 5);
  vHistogram.Add(8);

  if (aPrint) Print(vHistogram);
  if (aPrintBinIndex) PrintBinIndex(vHistogram, 0.2);
  TestConsistency(vHistogram);

  vHistogram.Add(5, 1);
  vHistogram.Add(9, 3);
  vHistogram.Add(10, 1);
  vHistogram.Add(11, 7);
  vHistogram.Add(12, 1);
  vHistogram.Add(13, 10);

  if (aPrint) Print(vHistogram);
  if (aPrintBinIndex) PrintBinIndex(vHistogram, 0.5);
  TestConsistency(vHistogram);

  vHistogram.Reset();

  if (aPrint) Print(vHistogram);
  if (aPrintBinIndex) PrintBinIndex(vHistogram, 0.1);
  TestConsistency(vHistogram);
}


void ggHistogramTest::Execute()
{
  bool vPrint = false;
  bool vPrintBinIndex = false;
  TestHistogramIntegerT<ggUInt8, 6>(vPrint, vPrintBinIndex);
  TestHistogramIntegerT<ggUInt8, 8>(vPrint, vPrintBinIndex);
  TestHistogramIntegerT<ggUInt8, 12>(vPrint, vPrintBinIndex);
  TestHistogramIntegerT<ggUInt8, 16>(vPrint, vPrintBinIndex);
  TestHistogramIntegerT<ggUInt8, 256>(vPrint, vPrintBinIndex);
  TestHistogramIntegerT<ggInt8, 16>(vPrint, vPrintBinIndex);
  TestHistogramIntegerT<ggInt8, 64>(vPrint, vPrintBinIndex);
  TestHistogramIntegerT<ggInt8, 256>(vPrint, vPrintBinIndex);
  TestHistogramIntegerT<ggUInt16, 8>(vPrint, vPrintBinIndex);
  TestHistogramIntegerT<ggInt16, 16>(vPrint, vPrintBinIndex);
}
