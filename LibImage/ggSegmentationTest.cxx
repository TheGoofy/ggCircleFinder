#include "LibBase/ggUnitTest.h"
#include "LibImage/ggSegmentation.h"
#include "LibImage/ggImageFilter.h"
#include "LibImage/ggHistogramTest.h"


static void ggSegmentationTest()
{
  bool vPrint = false;

  // generate a histogram from normal distributed noise
  ggImageT<ggFloat> vImage(32, 32, 0.0f);
  ggImageFilter::AddNoise(vImage, 1.0f);
  ggHistogramFixedT<ggFloat> vHistogram = ggImageFilter::GetHistogram(vImage, 8);
  if (vPrint) ggHistogramTest::Print(vHistogram);

  // calculate two means threshold
  ggDouble vThreshold = 0;
  ggSegmentation::CalculateThresholdTwoMeans(vHistogram, vThreshold);
  if (vPrint) std::cout << "vThreshold = " << vThreshold << std::endl;

  // threshold is expected to be located in the middle
  GG_TEST(vHistogram.GetValueMinF() <= vThreshold);
  GG_TEST(vHistogram.GetValueMaxF() >= vThreshold);
  GG_TEST(vHistogram.GetBinValueF(3) < vThreshold);
  GG_TEST(vHistogram.GetBinValueF(4) > vThreshold);

  // calculate three means threshold
  ggDouble vThresholdA = 0;
  ggDouble vThresholdB = 0;
  ggSegmentation::CalculateThresholdThreeMeans(vHistogram, vThresholdA, vThresholdB);

  // thresholds are expected to be located in the middle
  GG_TEST(vHistogram.GetValueMinF() <= vThresholdA);
  GG_TEST(vHistogram.GetValueMaxF() >= vThresholdA);
  GG_TEST(vHistogram.GetValueMinF() <= vThresholdB);
  GG_TEST(vHistogram.GetValueMaxF() >= vThresholdB);
  GG_TEST(vThresholdA <= vThresholdB);
}


GG_TEST_REGISTER(ggSegmentationTest);
