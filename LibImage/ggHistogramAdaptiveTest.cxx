#include <iostream>

#include "LibBase/ggUnitTest.h"
#include "LibImage/ggHistogramTest.h"
#include "LibImage/ggHistogramAdaptiveT.h"


template <class TValueType, ggInt64 TCountBinCapacity>
static void ggHistogramAdaptiveTestT(bool aPrint, bool aPrintBinIndex)
{
  if (aPrint) std::cout << "========== " <<  __PRETTY_FUNCTION__ << " ==========" << std::endl;
  bool vSucceeded = true;

  ggHistogramAdaptiveT<TValueType, TCountBinCapacity> vHistogram;

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.02);

  vHistogram.Add(ggRound<TValueType>(-0.3));

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.02);

  vHistogram.Add(ggRound<TValueType>(-0.5), 2);

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.1);

  vHistogram.Add(ggRound<TValueType>(0.5), 5);
  vHistogram.Add(ggRound<TValueType>(-0.6), 1);
  vHistogram.Add(ggRound<TValueType>(0.6), 1);

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.1);

  vHistogram.Add(ggRound<TValueType>(-20.0), 1);
  vHistogram.Add(ggRound<TValueType>(-17.0), 1);
  vHistogram.Add(ggRound<TValueType>(10.0), 3);
  vHistogram.Add(ggRound<TValueType>(12.0), 1);

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.5);

  vHistogram.Reset();

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.02);
}


static void ggHistogramAdaptiveTest()
{
  bool vPrint = false;
  bool vPrintBinIndex = false;

  ggHistogramAdaptiveTestT<ggInt32, 6>(vPrint, vPrintBinIndex);
  ggHistogramAdaptiveTestT<ggInt32, 32>(vPrint, vPrintBinIndex);
  ggHistogramAdaptiveTestT<ggUInt32, 6>(vPrint, vPrintBinIndex);
  ggHistogramAdaptiveTestT<ggUInt32, 32>(vPrint, vPrintBinIndex);
  ggHistogramAdaptiveTestT<ggFloat, 6>(vPrint, vPrintBinIndex);
  ggHistogramAdaptiveTestT<ggFloat, 32>(vPrint, vPrintBinIndex);
  ggHistogramAdaptiveTestT<ggDouble, 6>(vPrint, vPrintBinIndex);
  ggHistogramAdaptiveTestT<ggDouble, 32>(vPrint, vPrintBinIndex);
}


static bool ggRegistered = ggUnitTest::Register(ggHistogramAdaptiveTest);
