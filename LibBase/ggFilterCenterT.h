#ifndef GGFILTERCENTERT_H
#define GGFILTERCENTERT_H

#include <functional>
#include "LibBase/ggFilterFirInputBufferT.h"

/**
 * Default distance calculator for common scalar types (calculates absolute difference)
 */
template <typename TValueType>
TValueType ggFilterCenterDistanceFuncT(const TValueType& aValueA, const TValueType& aValueB) {
  return aValueA < aValueB ? aValueB - aValueA : aValueA - aValueB;
}

/**
 * Finds the sample where the total distance to all other samples is minimal.
 *
 * Related with "geometric median" (see: https://en.wikipedia.org/wiki/Geometric_median). But
 * this filter outputs a specific measurement (and not a synthetically generated new value)
 * For scalar value types like int or float, the filter behaves (almost) like the median (if
 * the number of samples is even it returns sometimes the upper and spmetimes the lower median).
 * Other value types may require an extra implementation for the distance calculation (some sort
 * of cost-function). Usually the euclidean distance works well for vector types.
 *
 * Note that the calculation effort is O(n^2).
 *
 * For filters with higher orders (over 20) a more sophisticated implementaion should be
 * considered (e.g. Weiszfeld's algorithm).
 */
template <class TValueType,
          class TDistanceValueType = TValueType>

class ggFilterCenterT : public ggFilterFirInputBufferT<TValueType> {

public:

  // distance function
  typedef std::function<TDistanceValueType (const TValueType& aValueA, const TValueType& aValueB)> tDistanceFunc;

  // construct filter with specific order and custom distance calculation
  ggFilterCenterT(ggUSize aOrder, const tDistanceFunc aDistanceFunc = ggFilterCenterDistanceFuncT<TValueType>)
  : tFilterFir(aOrder),
    mDistanceFunc(aDistanceFunc) {
  }

protected:

  // base filter type (shortcut)
  typedef ggFilterFirInputBufferT<TValueType> tFilterFir;

  // finds input-value with smallest sum of distances to all its neighbors
  virtual void Calculate(TValueType& aOutputValue) override {

    // early return, if there are no input values
    if (tFilterFir::mInputValues.empty()) return;

    // find smallest of all sums of distances
    ggUSize vIndexMin = 0;
    TDistanceValueType vDistanceTotalMin = 0;
    for (ggUSize vIndexA = 0; vIndexA < tFilterFir::mInputValues.size(); vIndexA++) {

      // calculate sum of all distances
      TDistanceValueType vDistanceTotal = 0;
      for (ggUSize vIndexB = 0; vIndexB < tFilterFir::mInputValues.size(); vIndexB++) {
        vDistanceTotal += mDistanceFunc(tFilterFir::mInputValues[vIndexA],
                                        tFilterFir::mInputValues[vIndexB]);
      }

      // use the first iteration for initialisation, later iterations will test if "less"
      if ((vIndexA == 0) || (vDistanceTotal < vDistanceTotalMin)) {
        vDistanceTotalMin = vDistanceTotal;
        vIndexMin = vIndexA;
      }
    }

    // assign center element as output
    aOutputValue = tFilterFir::mInputValues[vIndexMin];
  }

private:

  // distance function
  const tDistanceFunc mDistanceFunc;

};

#endif // GGFILTERCENTERT_H
