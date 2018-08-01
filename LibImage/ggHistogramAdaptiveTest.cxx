#include <iostream>

#include "LibBase/ggUnitTest.h"
#include "LibImage/ggHistogramTest.h"
#include "LibImage/ggHistogramAdaptiveT.h"


template <class TValueType, ggInt64 TCountBinCapacity>
static void ggHistogramAdaptiveTestT(bool aPrint, bool aPrintBinIndex)
{
  if (aPrint) std::cout << __PRETTY_FUNCTION__ << " ====================" << std::endl;

  /*
  ggHistogramAdaptiveT<TValueType, TCountBinCapacity> vHistogram;

  if (aPrint) ggHistogramTest::Print(vHistogram);
  if (aPrintBinIndex) ggHistogramTest::PrintBinIndex(vHistogram, 0.1);
  ggHistogramTest::TestConsistency(vHistogram);
  */
}


static void ggHistogramAdaptiveTest()
{
  bool vPrint = false;
  bool vPrintBinIndex = false;

  ggHistogramAdaptiveTestT<ggFloat, 8>(true, vPrintBinIndex);
}


static bool ggRegistered = ggUnitTest::Register(ggHistogramAdaptiveTest);
