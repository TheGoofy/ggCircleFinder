#ifndef GGFILTERMEDIANT_H
#define GGFILTERMEDIANT_H

#include <algorithm>
#include "LibBase/ggFilterFirT.h"

/**
 * Filters a specific number of samples (aOrder), and returns the median value.
 *
 * Requires that TValueType has implemented the "<" operator. Alternatively a custom
 * "less" comparator function can be provided.
 *
 * Calculation effort is O(n).
 */
template <class TValueType,
          class TLessFunc = std::less<TValueType>>

class ggFilterMedianT : public ggFilterFirT<TValueType> {

public:

  // base filter type (shortcut)
  typedef ggFilterFirT<TValueType> tFilterFir;

  // construct filter with default "less" function (TValueType needs to have "<" operator implemented)
  ggFilterMedianT(ggUSize aOrder)
  : tFilterFir(aOrder),
    mLessFunc(mLessFuncDefault) {
  }

  // construct filter with custom "less" function
  ggFilterMedianT(ggUSize aOrder, const TLessFunc& aLessFunc)
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
  const TLessFunc& mLessFunc;

  // default less function
  static TLessFunc mLessFuncDefault;

};

// static member instance for default compare function
template <class TValueType, class TLessFunc>
TLessFunc ggFilterMedianT<TValueType, TLessFunc>::mLessFuncDefault;

#endif // GGFILTERMEDIANT_H
