#include <iostream>

#include "LibBase/ggUnitTest.h"
#include "LibImage/ggHistogramTest.h"
#include "LibImage/ggHistogramAdaptiveFloatT.h"


template <class TValueType>
static void ggHistogramAdaptiveFloatTestT(ggInt64 aCountBinCapacity, bool aPrint, bool aPrintBinIndex)
{
  if (aPrint) std::cout << "========== " <<  __PRETTY_FUNCTION__ << " ==========" << std::endl;
  bool vSucceeded = true;

  ggHistogramAdaptiveFloatT<TValueType> vHistogram(aCountBinCapacity);

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, true, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.02);

  vHistogram.Add(ggRound<TValueType>(-0.3));

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, true, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.02);

  vHistogram.Add(ggRound<TValueType>(-0.5), 2);

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, true, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.1);

  vHistogram.Add(ggRound<TValueType>(0.5), 5);
  vHistogram.Add(ggRound<TValueType>(-0.6), 1);
  vHistogram.Add(ggRound<TValueType>(0.6), 1);

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, true, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.1);

  vHistogram.Add(ggRound<TValueType>(-20.0), 1);
  vHistogram.Add(ggRound<TValueType>(-17.0), 1);
  vHistogram.Add(ggRound<TValueType>(10.0), 3);
  vHistogram.Add(ggRound<TValueType>(12.0), 1);

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, true, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.5);

  vHistogram.Reset();

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, true, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.02);

  // fit range with number of bins
  TValueType vMin = ggRound<TValueType>(10);
  TValueType vMax = ggRound<TValueType>(10 + aCountBinCapacity);
  vHistogram.Reset(vMin, vMax);

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, true, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);

  // add values within the fitted range
  for (TValueType vValue = vMin; vValue < vMax; vValue += 1) {
    vHistogram.Add(static_cast<TValueType>(vValue + 0.5));
  }

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, true, GG_FILE_LINE);
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


static void ggHistogramAdaptiveFloatTest()
{
  bool vPrint = false;
  bool vPrintBinIndex = false;

  ggHistogramAdaptiveFloatTestT<ggInt32>(6, vPrint, vPrintBinIndex);
  ggHistogramAdaptiveFloatTestT<ggInt32>(32, vPrint, vPrintBinIndex);
  ggHistogramAdaptiveFloatTestT<ggUInt32>(6, vPrint, vPrintBinIndex);
  ggHistogramAdaptiveFloatTestT<ggUInt32>(32, vPrint, vPrintBinIndex);
  ggHistogramAdaptiveFloatTestT<ggFloat>(6, vPrint, vPrintBinIndex);
  ggHistogramAdaptiveFloatTestT<ggFloat>(32, vPrint, vPrintBinIndex);
  ggHistogramAdaptiveFloatTestT<ggDouble>(6, vPrint, vPrintBinIndex);
  ggHistogramAdaptiveFloatTestT<ggDouble>(32, vPrint, vPrintBinIndex);
}


GG_TEST_REGISTER(ggHistogramAdaptiveFloatTest);
