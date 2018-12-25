#include <iostream>

#include "LibBase/ggUnitTest.h"
#include "LibImage/ggHistogramTest.h"
#include "LibImage/ggHistogramAdaptiveIntT.h"


template <class TValueType>
static void ggHistogramAdaptiveIntTestT(ggInt64 aCountBinCapacity, bool aPrint, bool aPrintBinIndex)
{
  if (aPrint) std::cout << "========== " <<  GG_FUNCTION << " ==========" << std::endl;
  bool vSucceeded = true;

  ggHistogramAdaptiveIntT<TValueType> vHistogram(aCountBinCapacity);

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, true, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.1);

  vHistogram.Add(3);

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, true, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.2);

  vHistogram.Add(4);
  vHistogram.Add(5, 4);
  vHistogram.Add(6);
  vHistogram.Add(7, 5);
  vHistogram.Add(8);

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, true, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.2);

  vHistogram.Add(5, 1);
  vHistogram.Add(9, 3);
  vHistogram.Add(10, 1);
  vHistogram.Add(11, 7);
  vHistogram.Add(12, 1);
  vHistogram.Add(13, 10);

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, true, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.5);

  vHistogram.Reset();

  vSucceeded = ggHistogramTest::TestConsistency(vHistogram, true, GG_FILE_LINE);
  if (aPrint || !vSucceeded) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.1);
}


static void ggHistogramAdaptiveIntTest()
{
  bool vPrint = false;
  bool vPrintBinIndex = false;

  ggHistogramAdaptiveIntTestT<ggUInt8>(6, vPrint, vPrintBinIndex);
  ggHistogramAdaptiveIntTestT<ggUInt8>(8, vPrint, vPrintBinIndex);
  ggHistogramAdaptiveIntTestT<ggUInt8>(12, vPrint, vPrintBinIndex);
  ggHistogramAdaptiveIntTestT<ggUInt8>(16, vPrint, vPrintBinIndex);
  ggHistogramAdaptiveIntTestT<ggUInt8>(256, vPrint, vPrintBinIndex);
  ggHistogramAdaptiveIntTestT<ggInt8>(16, vPrint, vPrintBinIndex);
  ggHistogramAdaptiveIntTestT<ggInt8>(64, vPrint, vPrintBinIndex);
  ggHistogramAdaptiveIntTestT<ggInt8>(256, vPrint, vPrintBinIndex);
  ggHistogramAdaptiveIntTestT<ggUInt16>(8, vPrint, vPrintBinIndex);
  ggHistogramAdaptiveIntTestT<ggInt16>(16, vPrint, vPrintBinIndex);
}


GG_TEST_REGISTER(ggHistogramAdaptiveIntTest);
