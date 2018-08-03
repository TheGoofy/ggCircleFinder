#ifndef GGSEGMENTATION_H
#define GGSEGMENTATION_H

#include "LibBase/ggNumberTypes.h"

class ggHistogram;

namespace ggSegmentation {

  /**
   * Calculates threshold based on k-means (k=2).
   * Identical with Otsu-method.
   */
  ggDouble CalculateThresholdTwoMeans(const ggHistogram& aHistogram);

}

#endif // GGSEGMENTATION_H
