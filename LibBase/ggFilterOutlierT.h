#ifndef GGFILTEROUTLIERT_H
#define GGFILTEROUTLIERT_H

#include <algorithm>
#include <functional>
#include "LibBase/ggFilterFirInputBufferT.h"

/**
 * Default distance calculator for common scalar types (calculates absolute difference)
 */
template <typename TValueType>
TValueType ggFilterOutlierDistanceFuncT(const TValueType& aValueA, const TValueType& aValueB) {
  return aValueA < aValueB ? aValueB - aValueA : aValueA - aValueB;
}

/**
 * Finds the most recent sample, closest to the mean of all other samples. Rejects at maximum a
 * specified number of outliers.
 *
 * The filter is (almost) as good as the "ggFilterCenterT", but it's more efficient, and the
 * filter delay is less (defined by the maximum number of allowed outliers).
 *
 * Note that the calculation effort is O(n).
 */
template <class TValueType,
          class TDistanceValueType = TValueType>

class ggFilterOutlierT : public ggFilterFirInputBufferT<TValueType> {

public:

  // function returning the distance between A and B
  typedef std::function<TDistanceValueType (const TValueType& aValueA, const TValueType& aValueB)> tDistanceFunc;

  // construct filter with specific order and custom distance calculation
  ggFilterOutlierT(ggUSize aOrder, ggUSize aOutliers, const tDistanceFunc aDistanceFunc = ggFilterOutlierDistanceFuncT<TValueType>)
  : tFilterFir(aOrder),
    mOutliers(aOutliers),
    mDistanceFunc(aDistanceFunc) {
  }

protected:

  // base filter type (shortcut)
  typedef ggFilterFirInputBufferT<TValueType> tFilterFir;

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
  const tDistanceFunc mDistanceFunc;

};

#endif // GGFILTEROUTLIERT_H
