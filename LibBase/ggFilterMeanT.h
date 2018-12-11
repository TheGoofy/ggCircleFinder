#ifndef GGFILTERMEANT_H
#define GGFILTERMEANT_H

#include <algorithm>
#include "LibBase/ggFilterFirInputBufferT.h"

/**
 * Calculates mean (average).
 * Effort is O(n).
 */
template <class TValueType>
class ggFilterMeanT : public ggFilterFirInputBufferT<TValueType> {

public:

  // construct filter with specific order
  ggFilterMeanT(ggUSize aOrder)
  : tFilterFir(aOrder) {
  }

protected:

  // base filter type (shortcut)
  typedef ggFilterFirInputBufferT<TValueType> tFilterFir;

  // cumputes average of all input values
  virtual void Calculate(TValueType& aOutputValue) override {

    // early return, if there are no input values
    if (tFilterFir::mInputValues.empty()) return;

    // calculate mean value
    TValueType vSum = tFilterFir::mInputValues.front();
    std::for_each(tFilterFir::mInputValues.begin() + 1, tFilterFir::mInputValues.end(), [&vSum] (const TValueType& aValue) {
      vSum += aValue;
    });

    // assign the mean
    aOutputValue = vSum / static_cast<ggSize>(tFilterFir::mInputValues.size());
  }

};

#endif // GGFILTERMEANT_H
