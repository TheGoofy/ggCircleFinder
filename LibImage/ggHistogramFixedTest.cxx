#include <iostream>

#include "LibBase/ggUnitTest.h"
#include "LibImage/ggHistogramTest.h"
#include "LibImage/ggHistogramFixedT.h"


template <class TValueType>
static void ggHistogramFixedTestT(ggInt64 aNumberOfBins, bool aPrint, bool aPrintBinIndex)
{
  if (aPrint) std::cout << "========== " <<  GG_FUNCTION << " ==========" << std::endl;
  bool vSucceeded = true;

  ggHistogramFixedT<TValueType> vHistogram(aNumberOfBins, ggRound<TValueType>(-2), ggRound<TValueType>(2));
  // ggHistogramFixedT<TValueType> vHistogram(aNumberOfBins);

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, false, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.2);

  vHistogram.Add(ggRound<TValueType>(-0.3));

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, false, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.1);

  vHistogram.Add(ggRound<TValueType>(-2.0));
  vHistogram.Add(ggRound<TValueType>(-1.5));
  vHistogram.Add(ggRound<TValueType>(-1.0));
  vHistogram.Add(ggRound<TValueType>(1.2), 10);
  vHistogram.Add(ggRound<TValueType>(1.3), 10);
  vHistogram.Add(ggRound<TValueType>(1.4), 10);
  vHistogram.Add(ggRound<TValueType>(1.5), 10);
  vHistogram.Add(ggRound<TValueType>(1.6), 10);

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, false, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.1);

  vHistogram.Add(ggRound<TValueType>(-3.0), 100);
  vHistogram.Add(ggRound<TValueType>(3.0), 100);

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, false, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.1);

  vHistogram.Reset();

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, false, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.1);

  // fit range with number of bins
  TValueType vMin = ggRound<TValueType>(10);
  TValueType vMax = ggRound<TValueType>(10 + aNumberOfBins - 1);
  vHistogram.Reset(vMin, vMax);

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, false, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);

  // add values within the fitted range
  for (TValueType vValue = vMin; vValue <= vMax; vValue += 1) {
    vHistogram.Add(static_cast<TValueType>(vValue));
  }

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, false, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);

  // with the fitted range, each bin should now contain exactly one sample
  vSucceeded = true;
  bool vEachBinHasOneCount = true;
  for (ggInt64 vBinIndex = 0; vBinIndex < vHistogram.GetNumberOfBins(); vBinIndex++) {
    if (vHistogram.GetCount(vBinIndex) != 1) vEachBinHasOneCount = false;
  }
  GG_TEST2(vEachBinHasOneCount, vSucceeded);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
}


void ggHistogramFixedTestIndex()
{
  bool vSucceeded = true;
  ggHistogramFixedT<ggFloat> vHistogram(10);
  GG_TEST_EQUAL2(vHistogram.GetBinIndexF(-0.1), 0, vSucceeded);
  GG_TEST_EQUAL2(vHistogram.GetBinIndexF(0.0), 0, vSucceeded);
  GG_TEST_EQUAL2(vHistogram.GetBinIndexF(0.09), 0, vSucceeded);
  GG_TEST_EQUAL2(vHistogram.GetBinIndexF(0.1), 1, vSucceeded);
  GG_TEST_EQUAL2(vHistogram.GetBinIndexF(0.11), 1, vSucceeded);
  GG_TEST_EQUAL2(vHistogram.GetBinIndexF(0.99), 9, vSucceeded);
  GG_TEST_EQUAL2(vHistogram.GetBinIndexF(1.0), 9, vSucceeded);
  GG_TEST_EQUAL2(vHistogram.GetBinIndexF(1.1), 9, vSucceeded);
  if (!vSucceeded) ggHistogramTest::PrintBinIndex(vHistogram, -0.1, 1.1, 0.05);
}


static void ggHistogramFixedTest()
{
  bool vPrint = false;
  bool vPrintBinIndex = false;

  ggHistogramFixedTestT<ggInt32>(8, vPrint, vPrintBinIndex);
  ggHistogramFixedTestT<ggInt32>(32, vPrint, vPrintBinIndex);
  ggHistogramFixedTestT<ggInt8>(8, vPrint, vPrintBinIndex);
  ggHistogramFixedTestT<ggInt8>(32, vPrint, vPrintBinIndex);
  ggHistogramFixedTestT<ggUInt8>(8, vPrint, vPrintBinIndex);
  ggHistogramFixedTestT<ggUInt8>(32, vPrint, vPrintBinIndex);
  ggHistogramFixedTestT<ggDouble>(6, vPrint, vPrintBinIndex);
  ggHistogramFixedTestT<ggDouble>(8, vPrint, vPrintBinIndex);
  ggHistogramFixedTestT<ggDouble>(32, vPrint, vPrintBinIndex);

  ggHistogramFixedTestIndex();
}


GG_TEST_REGISTER(ggHistogramFixedTest);
