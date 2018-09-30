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
 * Initially (or after "Reset()") the values of Input or Output are potentially
 * undefined (depends on actual filter implementation).
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

  // resets filter to its initial state
  virtual void Reset() = 0;

  // inputs a new value to be filtered
  virtual void PushIn(const TValueType& aInputValue) = 0;

  // returns the most recent input value
  virtual const TValueType& GetIn() const = 0;

  // returns the current output value
  virtual const TValueType& GetOut() const = 0;

  // does a filter cycle (input new value, return output)
  virtual const TValueType& Filter(const TValueType& aInputValue) {
    PushIn(aInputValue);
    return GetOut();
  }

};

#endif // GGFILTERT_H
