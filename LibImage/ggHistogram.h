#ifndef GGHISTOGRAM_H
#define GGHISTOGRAM_H

#include "LibBase/ggNumberTypes.h"

/**
 * Interface for a histogram. Individual bins contain the number of values (count).
 * Bins have a vertain width - i.e. all values within that width are collected in a bin.
 */
class ggHistogram
{

public:

  ggHistogram();
  virtual ~ggHistogram();

  virtual ggInt64 GetNumberOfBins() const = 0;
  virtual ggInt64 GetCountTotal() const = 0;
  virtual ggInt64 GetCount(ggInt64 aBinIndex) const = 0;
  virtual ggInt64 GetCountF(const ggDouble& aValueF) const = 0;
  virtual ggInt64 GetBinIndexF(const ggDouble& aValueF) const = 0;
  virtual ggDouble GetBinValueF(ggInt64 aBinIndex) const = 0;
  virtual ggDouble GetBinWidthF(ggInt64 aBinIndex) const = 0;
  virtual ggDouble GetValueMinF() const = 0;
  virtual ggDouble GetValueMaxF() const = 0;

  ggInt64 GetCountMax() const;
  ggInt64 GetBinIndexCountMax() const;
  ggInt64 GetBinIndexMin() const;
  ggInt64 GetBinIndexMax() const;
  ggDouble GetBinValueMinF() const;
  ggDouble GetBinValueMaxF() const;
  ggDouble GetBinValueMinLowerF() const;
  ggDouble GetBinValueMaxUpperF() const;
  ggDouble GetBinValueRangeF() const;
  ggDouble GetBinValueRangeOuterF() const;

};

#endif // GGHISTOGRAM_H
