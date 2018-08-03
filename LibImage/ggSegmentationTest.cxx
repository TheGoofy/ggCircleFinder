#include "LibBase/ggUnitTest.h"
#include "LibImage/ggSegmentation.h"
#include "LibImage/ggImageFilter.h"
#include "LibImage/ggHistogramTest.h"


static void ggSegmentationTest()
{
  bool vPrint = false;

  // generate a histogram from normal distributed noise
  ggImageT<ggFloat> vImage(16, 16, 0.0f);
  ggImageFilter::AddNoise(vImage, 1.0f);
  ggHistogramFixedT<ggFloat> vHistogram = ggImageFilter::GetHistogram(vImage, 8);
  if (vPrint) ggHistogramTest::Print(vHistogram);

  // calculate two means threshold
  ggDouble vThreshold = ggSegmentation::CalculateThresholdTwoMeans(vHistogram);

  // threshold is expected to be located in the middle
  GG_TEST(vHistogram.GetValueMinF() <= vThreshold);
  GG_TEST(vHistogram.GetValueMaxF() >= vThreshold);
  GG_TEST(vHistogram.GetBinValueF(3) < vThreshold);
  GG_TEST(vHistogram.GetBinValueF(4) > vThreshold);
}


static bool ggRegistered = ggUnitTest::Register(ggSegmentationTest);
