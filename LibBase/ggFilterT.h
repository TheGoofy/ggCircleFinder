#ifndef GGFILTERT_H
#define GGFILTERT_H

#include "LibBase/ggNumberTypes.h"

template <class TValueType>
class ggFilterT {

public:

  virtual void Reset() = 0;
  virtual const TValueType& Filter(const TValueType& aInputValue) = 0;
  virtual const TValueType& GetIn() const = 0;
  virtual const TValueType& GetOut() const = 0;

};

typedef ggFilterT<ggInt32> ggFilterInt32;
typedef ggFilterT<ggFloat> ggFilterFloat;
typedef ggFilterT<ggDouble> ggFilterDouble;

#endif // GGFILTERT_H
