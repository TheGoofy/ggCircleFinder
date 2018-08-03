#ifndef GGSEGMENTATION_H
#define GGSEGMENTATION_H

#include "LibBase/ggNumberTypes.h"

class ggHistogram;

namespace ggSegmentation {

  /**
   * Calculates threshold based on k-means (k=2).
   * Bimodal distribution. Identical with Otsu-method.
   * Requires a histogram with at least 2 bins.
   */
  void CalculateThresholdTwoMeans(const ggHistogram& aHistogram,
                                  ggDouble& aThreshold);

  /**
   * Calculates two thresholds based on k-means (k=3).
   * Trimodal distributed values.
   * Requires a histogram with at least 2 bins.
   */
  void CalculateThresholdThreeMeans(const ggHistogram& aHistogram,
                                    ggDouble& aThresholdA,
                                    ggDouble& aThresholdB);

}

#endif // GGSEGMENTATION_H
