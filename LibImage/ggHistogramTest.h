#ifndef GGHISTOGRAMTEST_H
#define GGHISTOGRAMTEST_H

#include "LibBase/ggNumberTypes.h"

class ggHistogram;

namespace ggHistogramTest
{
  void Print(const ggHistogram& aHistogram);
  void PrintBinIndex(const ggHistogram& aHistogram, const ggDouble& aValueStepF);
  void PrintBinIndex(const ggHistogram& aHistogram, const ggDouble& aValueMinF, const ggDouble& aValueMaxF, const ggDouble& aValueStepF);
  bool TestConsistency(const ggHistogram& aHistogram, bool aBinIndexAdaptiv, const char* aReference = nullptr);
}

#endif // GGHISTOGRAMTEST_H
