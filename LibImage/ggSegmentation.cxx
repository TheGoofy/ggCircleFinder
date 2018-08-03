#include "LibImage/ggSegmentation.h"

#include "LibBase/ggAveragesT.h"
#include "LibImage/ggHistogram.h"


ggDouble ggSegmentation::CalculateThresholdTwoMeans(const ggHistogram& aHistogram)
{
  // averages for the two groups: A (lower), B (upper)
  ggAveragesT<ggDouble> vAveragesA;
  ggAveragesT<ggDouble> vAveragesB;

  // calculate averages for the entire range with group B (upper)
  for (ggInt64 vBinIndex = 0; vBinIndex < aHistogram.GetNumberOfBins(); vBinIndex++) {
    ggDouble vValue = aHistogram.GetBinValueF(vBinIndex);
    ggInt64 vCount = aHistogram.GetCount(vBinIndex);
    // accumulate all samples in group B
    vAveragesB.AddSample(vValue, vCount);
  }

  // index with of minimal sum of squared errors of both groups
  ggDouble vErrorsMin = vAveragesB.GetSumOfSquaredErrors();
  ggInt64 vBinIndexMin = 0;

  // loop again: increase range of group A, decrease range of group B
  for (ggInt64 vBinIndex = 0; vBinIndex < aHistogram.GetNumberOfBins(); vBinIndex++) {
    ggDouble vValue = aHistogram.GetBinValueF(vBinIndex);
    ggInt64 vCount = aHistogram.GetCount(vBinIndex);
    // "swap" a sample (current bin) from group B to group A
    vAveragesA.AddSample(vValue, vCount);
    vAveragesB.RemoveSample(vValue, vCount);
    // add up total sum of squared errors
    ggDouble vErrors = vAveragesA.GetSumOfSquaredErrors() + vAveragesB.GetSumOfSquaredErrors();
    if (vErrors < vErrorsMin) {
      vErrorsMin = vErrors;
      vBinIndexMin = vBinIndex;
    }
  }

  // threshold is located between left and right group
  ggDouble vThreshold = aHistogram.GetBinValueF(vBinIndexMin);
  if (vBinIndexMin + 1 < aHistogram.GetNumberOfBins()) {
    vThreshold = (vThreshold + aHistogram.GetBinValueF(vBinIndexMin + 1)) / 2.0;
  }

  return vThreshold;
}
