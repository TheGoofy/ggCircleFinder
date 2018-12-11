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
 * => -1 means one step into the pasr
 *
 * If prediction-index is half of the filter order, the returned value
 * is the same as the mean value.
 *
 * Effort is O(n).
 */
template <class TValueType>
class ggFilterLinearPredictionT : public ggFilterFirInputBufferT<TValueType> {

public:

  // construct filter with specific order and desired index of prediction (look ahead 0 = current, 1 = 1 step ahead)
  ggFilterLinearPredictionT(ggUSize aOrder, ggInt32 aIndexPrediction)
  : tFilterFir(aOrder),
    mIndexPrediction(aIndexPrediction) {
  }

protected:

  // base filter type (shortcut)
  typedef ggFilterFirInputBufferT<TValueType> tFilterFir;

  // cumputes oldest of all input values
  virtual void Calculate(TValueType& aOutputValue) override {

    // compile all the values for the linear regression
    ggAverages2T<ggDouble> vAverages2;
    for (ggUSize vIndex = 0; vIndex < tFilterFir::mInputValues.size(); vIndex++) {
      ggDouble vStep = -ggRound<ggDouble>(vIndex);
      ggDouble vValue = tFilterFir::mInputValues[tFilterFir::GetInputIndex(vIndex)];
      vAverages2.AddSample(vStep, vValue);
    }

    // calculate the regression
    ggDouble vA, vB;
    if (vAverages2.GetRegressionX(vA, vB)) {
      const ggDouble vStepPrediction = ggRound<ggDouble>(mIndexPrediction);
      aOutputValue = ggRound<TValueType>(vA * vStepPrediction + vB);
    }
    else {
      aOutputValue = tFilterFir::GetIn();
    }
  }

  ggInt32 mIndexPrediction;

};

#endif // GGFILTERLINEARPREDICTIONT_H
