#ifndef GGFILTERFIRINPUTBUFFERT_H
#define GGFILTERFIRINPUTBUFFERT_H

#include <vector>
#include <algorithm>
#include "LibBase/ggFilterT.h"
#include "LibBase/ggAssert.h"

/**
 * General FIR Filter (finite impulse response).
 *
 * This class provides a framework for a common FIR filter containing the storage for
 * the input values and triggering (re)calculation only when needed. The specific filter
 * behavior needs to be implemented by a deriveved class (overriding "Calculate").
 */
template <class TValueType>
class ggFilterFirInputBufferT : public ggFilterT<TValueType> {

public:

  // base filter type (shortcut)
  typedef ggFilterT<TValueType> tFilter;

  // creates a FIR filter with specified order
  ggFilterFirInputBufferT(ggUSize aOrder)
  : mInputIndex(static_cast<ggUSize>(-1)),
    mInputValues(),
    mOrder(aOrder),
    mCalculateOutput(true),
    mOutputValue() {
    mInputValues.reserve(mOrder);
  }

  // resets the filter to its initial state
  virtual void Reset() override {
    mInputIndex = static_cast<ggUSize>(-1);
    mInputValues.clear();
    mInputValues.reserve(mOrder);
    mCalculateOutput = true;
    mOutputValue = TValueType();
  }

  // returns the filter order
  ggUSize GetOrder() const {
    return mOrder;
  }

  // inputs a new value
  virtual void PushIn(const TValueType& aInputValue) override {

    // store input values in ring-buffer
    mInputIndex = (mInputIndex + 1) % mOrder;
    mInputValues.resize(std::max(mInputValues.size(), mInputIndex + 1));
    mInputValues[mInputIndex] = aInputValue;

    // output needs to be recalculated
    mCalculateOutput = true;
  }

  // returns the most recent input value (or default value of TValueType)
  virtual const TValueType& GetIn() const override {

    // output has still its default value, if there was not yet an input value added
    if (mInputValues.empty()) return mOutputValue;

    // if container is not empty, this is our last input:
    return mInputValues[mInputIndex];
  }

  // returns output value (triggers re-calculation if needed)
  virtual const TValueType& GetOut() const override {

    // re-calculate if needed
    if (mCalculateOutput && !mInputValues.empty()) {
      const_cast<ggFilterFirInputBufferT<TValueType>*>(this)->Calculate(mOutputValue);
      mCalculateOutput = false;
    }

    // return output
    return mOutputValue;
  }

protected:

  // (re)calculates output. only called, when output required after input was changed
  virtual void Calculate(TValueType& aOutputValue) = 0;

  // returns the index to the nth-last input value (0 for the most recent). The returned index is
  // truncated to the oldest value, if there are less input values.
  ggUSize GetInputIndex(ggUSize aLastIndex) const {

    // can't do anything useful, if there are no input values
    if (mInputValues.size() == 0) return mInputIndex;

    // truncate to the number of values
    aLastIndex = std::min(aLastIndex, mInputValues.size() - 1);

    // properly wrap around ring-buffer
    return (mInputIndex + mInputValues.size() - aLastIndex) % mInputValues.size();
  }

  // index of most recent input value
  ggUSize mInputIndex;

  // ring-buffer for input values (initially empty)
  std::vector<TValueType> mInputValues;

private:

  // filter order (maximum number of input values)
  const ggUSize mOrder;

  // flags the need for (re)calculation
  mutable bool mCalculateOutput;

  // calculated output value
  mutable TValueType mOutputValue;

};

#endif // GGFILTERFIRINPUTBUFFERT_H
