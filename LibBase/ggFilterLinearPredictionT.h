#ifndef GGFILTERLINEARPREDICTIONT_H
#define GGFILTERLINEARPREDICTIONT_H

#include "LibBase/ggFilterFirInputBufferT.h"
#include "LibBase/ggAverages2T.h"

/**
 * Extrapolate based on linear regression.
 *
 * Output is calculated for provided prediction-index:
 * => 0 means the current step (likely similar to current input)
 * => 1 means one step ahead (future)
 * => -1 means one step into the past
 *
 * If prediction-index is half of the filter order, the returned value
 * is the same as the mean value.
 *
 * Effort is O(n).
 */
template <class TValueType>
class ggFilterLinearPredictionT : public ggFilterFirInputBufferT<TValueType> {

public:

  // construct filter with specific order and desired index of prediction (look ahead 0 = current, 1 = one step ahead)
  ggFilterLinearPredictionT(ggUSize aOrder, ggDouble aPredictionStep)
  : tFilterFir(aOrder),
    mPredictionStep(aPredictionStep) {
  }

protected:

  // base filter type (shortcut)
  typedef ggFilterFirInputBufferT<TValueType> tFilterFir;

  // cumputes oldest of all input values
  virtual void Calculate(TValueType& aOutputValue) override {

    // compile all the values for the linear regression
    ggAverages2T<ggDouble> vAverages2;
    for (ggUSize vIndex = 0; vIndex < tFilterFir::mInputValues.size(); vIndex++) {
      const ggDouble vStep = -ggRound<ggDouble>(vIndex);
      const ggDouble vValue = tFilterFir::mInputValues[tFilterFir::GetInputIndex(vIndex)];
      vAverages2.AddSample(vStep, vValue);
    }

    // calculate the regression
    ggDouble vA = 1.0, vB = 0.0;
    if (vAverages2.GetRegressionX(vA, vB)) {
      aOutputValue = ggRound<TValueType>(vA * mPredictionStep + vB);
    }
    else {
      aOutputValue = tFilterFir::GetIn();
    }
  }

  ggDouble mPredictionStep;

};

#endif // GGFILTERLINEARPREDICTIONT_H
