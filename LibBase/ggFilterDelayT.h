#ifndef GGFILTERDELAYT_H
#define GGFILTERDELAYT_H

#include "LibBase/ggFilterFirInputBufferT.h"

/**
 * Delays input value by the order of the filter
 * Effort is O(1).
 */
template <class TValueType>
class ggFilterDelayT : public ggFilterFirInputBufferT<TValueType> {

public:

  // construct filter with specific order
  ggFilterDelayT(ggUSize aDelay)
  : tFilterFir(aDelay + 1) {
  }

protected:

  // base filter type (shortcut)
  typedef ggFilterFirInputBufferT<TValueType> tFilterFir;

  // cumputes oldest of all input values
  virtual void Calculate(TValueType& aOutputValue) override {

    // oldest element is the input value, which would be overwritten as next (ring-buffer)
    ggUSize vIndex = (tFilterFir::mInputIndex + 1) % tFilterFir::GetOrder();

    // assign output, if last element exists
    if (vIndex < tFilterFir::mInputValues.size()) {
      aOutputValue = tFilterFir::mInputValues[vIndex];
    }
  }

};

#endif // GGFILTERDELAYT_H
