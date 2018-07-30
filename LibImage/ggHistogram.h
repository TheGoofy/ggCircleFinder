#ifndef GGHISTOGRAM_H
#define GGHISTOGRAM_H

#include "LibBase/ggNumberTypes.h"

class ggHistogram
{

public:

  ggHistogram() {}
  virtual ~ggHistogram() {}

  virtual ggInt64 GetNumberOfBins() const = 0;
  virtual ggInt64 GetCountTotal() const = 0;
  virtual ggInt64 GetCount(ggInt64 aBinIndex) const = 0;
  virtual ggInt64 GetCountF(const ggDouble& aValueF) const = 0;
  virtual ggInt64 GetBinIndexF(const ggDouble& aValueF) const = 0;
  virtual ggDouble GetBinValueF(ggInt64 aBinIndex) const = 0;
  virtual ggDouble GetBinDeltaF() const = 0;
  virtual ggDouble GetValueMinF() const = 0;
  virtual ggDouble GetValueMaxF() const = 0;

};

#endif // GGHISTOGRAM_H
