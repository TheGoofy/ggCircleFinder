#ifndef GGFILTERMEDIANT_H
#define GGFILTERMEDIANT_H

#include <algorithm>
#include <functional>
#include "LibBase/ggFilterFirT.h"

/**
 * Filters a specific number of samples (aOrder), and returns the median value.
 *
 * Requires that TValueType has implemented the "<" operator. Alternatively a custom
 * "less" comparator function can be provided.
 *
 * Calculation effort is O(n).
 */
template <class TValueType>

class ggFilterMedianT : public ggFilterFirT<TValueType> {

public:

  // base filter type (shortcut)
  typedef ggFilterFirT<TValueType> tFilterFir;

  // function returning A < B
  typedef std::function<bool (const TValueType& aValueA, const TValueType& aValueB)> tLessFunc;

  // construct filter with custom "less" function
  ggFilterMedianT(ggUSize aOrder, const tLessFunc aLessFunc = std::less<TValueType>())
  : tFilterFir(aOrder),
    mLessFunc(aLessFunc) {
  }

protected:

  // copies all input values, partially sorts, and picks middle element
  virtual void Calculate(TValueType& aOutputValue) override {

    // early return, if there are no input values
    if (tFilterFir::mInputValues.empty()) return;

    // copy buffer
    std::vector<TValueType> vValuesSorted(tFilterFir::mInputValues);

    // partially sort middle element (median)
    std::nth_element(vValuesSorted.begin(),
                     vValuesSorted.begin() + vValuesSorted.size() / 2,
                     vValuesSorted.end(),
                     mLessFunc);

    // assign the median
    aOutputValue = vValuesSorted[vValuesSorted.size() / 2];
  }

private:

  // function for comparing two elements
  const tLessFunc mLessFunc;

};

#endif // GGFILTERMEDIANT_H
