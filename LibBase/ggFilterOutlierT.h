#ifndef GGFILTEROUTLIERT_H
#define GGFILTEROUTLIERT_H

#include "LibBase/ggFilterFirT.h"

/**
 * Default distance calculator for common scalar types (calculates absolute difference)
 */
template <class TValueType,
          class TDistanceValueType = TValueType>
struct ggFilterOutlierDistanceFuncT {
  TDistanceValueType operator () (const TValueType& aValueA, const TValueType& aValueB) const {
    return aValueA < aValueB ? aValueB - aValueA : aValueA - aValueB;
  }
};

/**
 * Finds the most recent sample, closest to the mean of all other samples. Rejects at maximum a
 * specified number of outliers.
 *
 * Note that the calculation effort is O(n).
 */
template <class TValueType,
          class TDistanceValueType = TValueType,
          class TDistanceFunc = ggFilterOutlierDistanceFuncT<TValueType>>

class ggFilterOutlierT : public ggFilterFirT<TValueType> {

public:

  // base filter type (shortcut)
  typedef ggFilterFirT<TValueType> tFilterFir;

  // construct filter with specific order and default distance calculation (difference)
  ggFilterOutlierT(ggUSize aOrder, ggUSize aOutliers)
  : tFilterFir(aOrder),
    mOutliers(aOutliers),
    mDistanceFunc(mDistanceFuncDefault) {
  }

  // construct filter with specific order and custom distance calculation
  ggFilterOutlierT(ggUSize aOrder, ggUSize aOutliers, const TDistanceFunc& aDistanceFunc)
  : tFilterFir(aOrder),
    mOutliers(aOutliers),
    mDistanceFunc(aDistanceFunc) {
  }

protected:

  // finds input-value which is closest to the current mean
  virtual void Calculate(TValueType& aOutputValue) override {

    // early return, if there are no input values
    if (tFilterFir::mInputValues.empty()) return;

    // calculate mean value (sum up)
    TValueType vValueMean = tFilterFir::mInputValues.front();
    std::for_each(tFilterFir::mInputValues.begin() + 1, tFilterFir::mInputValues.end(), [&vValueMean] (const TValueType& aValue) {
      vValueMean += aValue;
    });

    // calculate mean value (divide)
    vValueMean /= static_cast<ggSize>(tFilterFir::mInputValues.size());

    // calculate index of nth-last value
    const ggUSize vIndexStart = tFilterFir::GetInputIndex(mOutliers);

    // init index and shortest distance from last (newest) value
    ggUSize vIndexMin = tFilterFir::mInputIndex;
    TDistanceValueType vDistanceMin = mDistanceFunc(vValueMean, tFilterFir::mInputValues[vIndexMin]);

    // find recent value with smallest distance (in reverse order)
    ggUSize vIndex = tFilterFir::mInputIndex;
    while (vIndex != vIndexStart) {

      // iterate to previous element (last element already inspected)
      vIndex = (vIndex + tFilterFir::mInputValues.size() - 1) % tFilterFir::mInputValues.size();

      // check distance
      TDistanceValueType vDistance = mDistanceFunc(vValueMean, tFilterFir::mInputValues[vIndex]);
      if (vDistance < vDistanceMin) {
        vDistanceMin = vDistance;
        vIndexMin = vIndex;
      }
    }

    // assign output
    aOutputValue = tFilterFir::mInputValues[vIndexMin];
  }

private:

  // maximum number of outliers
  const ggUSize mOutliers;

  // distance function
  const TDistanceFunc& mDistanceFunc;

  // default distance function (static)
  static TDistanceFunc mDistanceFuncDefault;

};

// instance of static member (default distance function)
template <class TValueType, class TDistanceValueType, class TDistanceFunc>
TDistanceFunc ggFilterOutlierT<TValueType, TDistanceValueType, TDistanceFunc>::mDistanceFuncDefault;

#endif // GGFILTEROUTLIERT_H
