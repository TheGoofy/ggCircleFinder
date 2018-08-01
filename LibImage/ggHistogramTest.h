#ifndef GGHISTOGRAMTEST_H
#define GGHISTOGRAMTEST_H

#include "LibBase/ggNumberTypes.h"

class ggHistogram;

namespace ggHistogramTest
{
  void Print(const ggHistogram& aHistogram);
  void PrintBinIndex(const ggHistogram& aHistogram, ggDouble aValueStepF);
  void TestConsistency(const ggHistogram& aHistogram);
}

#endif // GGHISTOGRAMTEST_H
