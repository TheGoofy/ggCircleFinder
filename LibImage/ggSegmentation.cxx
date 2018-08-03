#include "LibImage/ggSegmentation.h"

#include "LibBase/ggAveragesT.h"
#include "LibImage/ggHistogram.h"


#include <iostream>


static void ggCalculateThresholdTwoMeans(const ggHistogram& aHistogram,
                                         ggInt64 aBinIndexRangeMin,
                                         ggInt64 aBinIndexRangeMax,
                                         ggInt64& aBinIndexThreshold,
                                         ggDouble& aSumOfSquareErrors)
{
  // averages for the two groups: A (lower), B (upper)
  ggAveragesT<ggDouble> vAveragesA;
  ggAveragesT<ggDouble> vAveragesB;

  // calculate averages for the entire range with group B (upper)
  for (ggInt64 vBinIndex = aBinIndexRangeMin; vBinIndex <= aBinIndexRangeMax; vBinIndex++) {
    ggDouble vValue = aHistogram.GetBinValueF(vBinIndex);
    ggInt64 vCount = aHistogram.GetCount(vBinIndex);
    // accumulate all samples in group B
    vAveragesB.AddSample(vValue, vCount);
  }

  // index with of minimal sum of squared errors of both groups
  aSumOfSquareErrors = vAveragesB.GetSumOfSquaredErrors();
  aBinIndexThreshold = aBinIndexRangeMin;

  // loop again: increase range of group A, decrease range of group B
  for (ggInt64 vBinIndex = aBinIndexRangeMin; vBinIndex < aBinIndexRangeMax; vBinIndex++) {
    ggDouble vValue = aHistogram.GetBinValueF(vBinIndex);
    ggInt64 vCount = aHistogram.GetCount(vBinIndex);
    // "swap" a sample (current bin) from group B to group A
    vAveragesA.AddSample(vValue, vCount);
    vAveragesB.RemoveSample(vValue, vCount);
    // add up total sum of squared errors
    ggDouble vErrors = vAveragesA.GetSumOfSquaredErrors() + vAveragesB.GetSumOfSquaredErrors();
    if (vErrors < aSumOfSquareErrors) {
      aSumOfSquareErrors = vErrors;
      aBinIndexThreshold = vBinIndex;
    }
  }
}


static void ggCalculateThresholdThreeMeans(const ggHistogram& aHistogram,
                                           ggInt64 aBinIndexRangeMin,
                                           ggInt64 aBinIndexRangeMax,
                                           ggInt64& aBinIndexThresholdA,
                                           ggInt64& aBinIndexThresholdB,
                                           ggDouble& aSumOfSquareErrors)
{
  // averages for the three groups: A (lower), B and C are handled as 2-means
  ggAveragesT<ggDouble> vAveragesA;

  // index with of minimal sum of squared errors of both groups
  aSumOfSquareErrors = std::numeric_limits<ggDouble>::max();
  aBinIndexThresholdA = aBinIndexRangeMin;
  aBinIndexThresholdB = aBinIndexRangeMin;

  // increase range for A, check threshold and error for remaining range BC
  for (ggInt64 vBinIndex = aBinIndexRangeMin; vBinIndex + 1 < aBinIndexRangeMax; vBinIndex++) {
    ggDouble vValue = aHistogram.GetBinValueF(vBinIndex);
    ggInt64 vCount = aHistogram.GetCount(vBinIndex);
    // add sample (current bin) to group A
    vAveragesA.AddSample(vValue, vCount);
    // find sum of errors and threshold for upper groups B and C
    ggInt64 vBinIndexThresholdB = vBinIndex + 1;
    ggDouble vSumOfSquareErrorsBC = std::numeric_limits<ggDouble>::max();
    ggCalculateThresholdTwoMeans(aHistogram, vBinIndex + 1, aBinIndexRangeMax, vBinIndexThresholdB, vSumOfSquareErrorsBC);
    // add up total sum of squared errors
    ggDouble vErrors = vAveragesA.GetSumOfSquaredErrors() + vSumOfSquareErrorsBC;
    if (vErrors < aSumOfSquareErrors) {
      aSumOfSquareErrors = vErrors;
      aBinIndexThresholdA = vBinIndex;
      aBinIndexThresholdB = vBinIndexThresholdB;
    }
  }
}


void ggSegmentation::CalculateThresholdTwoMeans(const ggHistogram& aHistogram, ggDouble& aThreshold)
{
  // early return if there are not enough bins
  if (aHistogram.GetNumberOfBins() < 2) return;

  // index with of minimal sum of squared errors of both groups
  ggDouble vSumOfSquareErrors = std::numeric_limits<ggDouble>::max();;
  ggInt64 vBinIndexThreshold = 0;

  // calculate the bin-index with the lowes sum of square errors over the whole histogram range
  ggCalculateThresholdTwoMeans(aHistogram, 0, aHistogram.GetNumberOfBins() - 1,
                               vBinIndexThreshold, vSumOfSquareErrors);

  // threshold is located between left and right group
  aThreshold = aHistogram.GetBinValueF(vBinIndexThreshold);
  if (vBinIndexThreshold + 1 < aHistogram.GetNumberOfBins()) {
    aThreshold = (aThreshold + aHistogram.GetBinValueF(vBinIndexThreshold + 1)) / 2.0;
  }
}


void ggSegmentation::CalculateThresholdThreeMeans(const ggHistogram& aHistogram,
                                                  ggDouble& aThresholdA,
                                                  ggDouble& aThresholdB)
{
  // early return if there are not enough bins
  if (aHistogram.GetNumberOfBins() < 3) return;

  // index with of minimal sum of squared errors of both groups
  ggDouble vSumOfSquareErrors = std::numeric_limits<ggDouble>::max();;
  ggInt64 vBinIndexThresholdA = 0;
  ggInt64 vBinIndexThresholdB = 0;

  // calculate the bin-index with the lowes sum of square errors over the whole histogram range
  ggCalculateThresholdThreeMeans(aHistogram, 0, aHistogram.GetNumberOfBins() - 1,
                                 vBinIndexThresholdA, vBinIndexThresholdB, vSumOfSquareErrors);

  // threshold A is located between groups A and B
  aThresholdA = aHistogram.GetBinValueF(vBinIndexThresholdA);
  if (vBinIndexThresholdA + 1 < aHistogram.GetNumberOfBins()) {
    aThresholdA = (aThresholdA + aHistogram.GetBinValueF(vBinIndexThresholdA + 1)) / 2.0;
  }

  // threshold B is located between groups B and C
  aThresholdB = aHistogram.GetBinValueF(vBinIndexThresholdB);
  if (vBinIndexThresholdB + 1 < aHistogram.GetNumberOfBins()) {
    aThresholdB = (aThresholdB + aHistogram.GetBinValueF(vBinIndexThresholdB + 1)) / 2.0;
  }
}
