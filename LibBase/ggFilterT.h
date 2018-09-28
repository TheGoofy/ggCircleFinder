#ifndef GGFILTERT_H
#define GGFILTERT_H

#include "LibBase/ggNumberTypes.h"

/**
 * Filter interface (blackbox).
 *
 * An input value is added by calling "Filter(...)".
 * Current input- or output-values are returned by "GetIn()" or "GetOut()".
 * Internal filter state is reset to initial state by calling "Reset()".
 *
 * Initially (or after "Reset()") the values of Input or Output are most likely undefined.
 *
 *        +---------------+
 *        |               |
 *  >>>>> | IN        OUT | >>>>>
 *        |               |
 *        +---------------+
 */
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
