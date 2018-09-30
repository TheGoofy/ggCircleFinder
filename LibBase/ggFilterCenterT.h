#ifndef GGFILTERCENTERT_H
#define GGFILTERCENTERT_H

#include "LibBase/ggFilterFirT.h"

/**
 * Default distance calculator for common scalar types (calculates absolute difference)
 */
template <class TValueType,
          class TDistanceValueType = TValueType>
struct ggFilterCenterDistanceFuncT {
  TDistanceValueType operator () (const TValueType& aValueA, const TValueType& aValueB) const {
    return aValueA < aValueB ? aValueB - aValueA : aValueA - aValueB;
  }
};

/**
 * Finds the sample where the total distance to all other samples is minimal.
 *
 * Related with "geometric median" (see: https://en.wikipedia.org/wiki/Geometric_median). But
 * this filter outputs a specific measurement (and not a synthetically generated new value)
 * For scalar value types like int or float, the filter behaves (almost) like the median (if
 * the number of samples is even it returns sometimes the upper and spmetimes the lower median).
 * Other value types may require an extra implementat for the distance calculation (some sort
 * of cost-function). Usually the euclidean distance works well for vector types.
 *
 * Note that the calculation effort is O(n^2).
 */
template <class TValueType,
          class TDistanceValueType = TValueType,
          class TDistanceFunc = ggFilterCenterDistanceFuncT<TValueType>>

class ggFilterCenterT : public ggFilterFirT<TValueType> {

public:

  // base filter type (shortcut)
  typedef ggFilterFirT<TValueType> tFilterFir;

  // construct filter with specific order and default distance calculation (difference)
  ggFilterCenterT(ggUSize aOrder)
  : tFilterFir(aOrder),
    mDistanceFunc(mDistanceFuncDefault) {
  }

  // construct filter with specific order and custom distance calculation
  ggFilterCenterT(ggUSize aOrder, const TDistanceFunc& aDistanceFunc)
  : tFilterFir(aOrder),
    mDistanceFunc(aDistanceFunc) {
  }

protected:

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
  const TDistanceFunc& mDistanceFunc;

  // default distance function (static)
  static TDistanceFunc mDistanceFuncDefault;

};

// instance of static member (default distance function)
template <class TValueType, class TDistanceValueType, class TDistanceFunc>
TDistanceFunc ggFilterCenterT<TValueType, TDistanceValueType, TDistanceFunc>::mDistanceFuncDefault;

#endif // GGFILTERCENTERT_H
