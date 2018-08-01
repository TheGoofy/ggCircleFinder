#include <iostream>

#include "LibBase/ggUnitTest.h"
#include "LibImage/ggHistogramTest.h"
#include "LibImage/ggHistogramIntegerT.h"


template <class TValueType, ggInt64 TCountBinCapacity>
static void ggHistogramIntegerTestT(bool aPrint, bool aPrintBinIndex)
{
  if (aPrint) std::cout << __PRETTY_FUNCTION__ << " ====================" << std::endl;

  ggHistogramIntegerT<TValueType, TCountBinCapacity> vHistogram;

  if (aPrint) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.1);
  ggHistogramTest::TestConsistency(vHistogram);

  vHistogram.Add(3);
  vHistogram.Add(4);

  if (aPrint) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.2);
  ggHistogramTest::TestConsistency(vHistogram);

  vHistogram.Add(5, 4);
  vHistogram.Add(6);
  vHistogram.Add(7, 5);
  vHistogram.Add(8);

  if (aPrint) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.2);
  ggHistogramTest::TestConsistency(vHistogram);

  vHistogram.Add(5, 1);
  vHistogram.Add(9, 3);
  vHistogram.Add(10, 1);
  vHistogram.Add(11, 7);
  vHistogram.Add(12, 1);
  vHistogram.Add(13, 10);

  if (aPrint) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.5);
  ggHistogramTest::TestConsistency(vHistogram);

  vHistogram.Reset();

  if (aPrint) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.1);
  ggHistogramTest::TestConsistency(vHistogram);
}


static void ggHistogramIntegerTest()
{
  bool vPrint = false;
  bool vPrintBinIndex = false;

  ggHistogramIntegerTestT<ggUInt8, 6>(vPrint, vPrintBinIndex);
  ggHistogramIntegerTestT<ggUInt8, 8>(vPrint, vPrintBinIndex);
  ggHistogramIntegerTestT<ggUInt8, 12>(vPrint, vPrintBinIndex);
  ggHistogramIntegerTestT<ggUInt8, 16>(vPrint, vPrintBinIndex);
  ggHistogramIntegerTestT<ggUInt8, 256>(vPrint, vPrintBinIndex);
  ggHistogramIntegerTestT<ggInt8, 16>(vPrint, vPrintBinIndex);
  ggHistogramIntegerTestT<ggInt8, 64>(vPrint, vPrintBinIndex);
  ggHistogramIntegerTestT<ggInt8, 256>(vPrint, vPrintBinIndex);
  ggHistogramIntegerTestT<ggUInt16, 8>(vPrint, vPrintBinIndex);
  ggHistogramIntegerTestT<ggInt16, 16>(vPrint, vPrintBinIndex);
}


static bool ggRegistered = ggUnitTest::Register(ggHistogramIntegerTest);
