#ifndef GGIMAGET_H
#define GGIMAGET_H

#include "ggTypes.h"
#include "ggAssert.h"
#include "ggVectorTypes.h"


template <class TValueType>
class ggImageT
{

public:

  typedef TValueType tValueType;

  ggImageT()
  : mSize(0, 0),
    mValues(0) {
  }

  ggImageT(const ggImageT<TValueType>& aOther)
  : mSize(aOther.mSize) {
    mValues = new TValueType[GetSizeTotal()];
    std::memcpy(mValues, aOther.mValues, GetSizeTotal() * sizeof(TValueType));
  }

  ggImageT(ggSize aSizeX, ggSize aSizeY)
  : mSize(aSizeX, aSizeY) {
    mValues = new TValueType[GetSizeTotal()];
  }

  ggImageT(ggSize aSizeX, ggSize aSizeY, const TValueType& aValue)
  : mSize(aSizeX, aSizeY) {
    mValues = new TValueType[GetSizeTotal()];
    SetValues(aValue);
  }

  ggImageT(const ggVector2Size& aSize)
  : mSize(aSize) {
    mValues = new TValueType[GetSizeTotal()];
  }

  ggImageT(const ggVector2Size& aSize, const TValueType& aValue)
  : mSize(aSize) {
    mValues = new TValueType[GetSizeTotal()];
    SetValues(aValue);
  }

  virtual ~ggImageT() {
    delete[] mValues;
  }

  inline ggSize GetSizeX() const {
    return mSize.X();
  }

  inline ggSize GetSizeY() const {
    return mSize.Y();
  }

  inline const ggVector2Size& GetSize() const {
    return mSize;
  }

  inline ggSize GetSizeTotal() const {
    return mSize.X() * mSize.Y();
  }

  inline void Resize(ggSize aSizeX, ggSize aSizeY) {
    Resize(ggVector2Size(aSizeX, aSizeY));
  }

  inline void Resize(const ggVector2Size& aSize) {
    const ggSize vOldSize = GetSizeTotal();
    const ggSize vNewSize = aSize.X() * aSize.Y();
    if (vNewSize != vOldSize) {
      mSize = aSize;
      delete[] mValues;
      mValues = new TValueType[vNewSize];
    }
  }

  inline bool IsInside(ggSize aIndexX, ggSize aIndexY) const {
    return (aIndexX < GetSizeX()) && (aIndexY < GetSizeY());
  }

  template <typename TIndexType>
  inline bool IsInside(const ggVector2T<TIndexType>& aIndex) const {
    return IsInside(aIndex.X(), aIndex.Y());
  }

  inline TValueType& Get(ggSize aIndexX, ggSize aIndexY) {
    GG_ASSERT(IsInside(aIndexX, aIndexY));
    return mValues[aIndexX + aIndexY * GetSizeX()];
  }

  template <typename TIndexType>
  inline TValueType& Get(const ggVector2T<TIndexType>& aIndex) {
    return Get(aIndex.X(), aIndex.Y());
  }

  inline const TValueType& Get(ggSize aIndexX, ggSize aIndexY) const {
    return const_cast<ggImageT<TValueType>*>(this)->Get(aIndexX, aIndexY);
  }

  template <typename TIndexType>
  inline const TValueType& Get(const ggVector2T<TIndexType>& aIndex) const {
    return Get(aIndex.X(), aIndex.Y());
  }

  inline TValueType& operator () (ggSize aIndexX, ggSize aIndexY) {
    return Get(aIndexX, aIndexY);
  }

  template <typename TIndexType>
  inline TValueType& operator () (const ggVector2T<TIndexType>& aIndex) {
    return Get(aIndex.X(), aIndex.Y());
  }

  inline const TValueType& operator () (ggSize aIndexX, ggSize aIndexY) const {
    return Get(aIndexX, aIndexY);
  }

  template <typename TIndexType>
  inline const TValueType& operator () (const ggVector2T<TIndexType>& aIndex) const {
    return Get(aIndex.X(), aIndex.Y());
  }

  inline ggImageT<TValueType>& operator = (const ggImageT<TValueType>& aOther) {
    Resize(aOther.mSizeX, aOther.mSizeY);
    std::memcpy(mValues, aOther.mValues, GetSizeTotal() * sizeof(TValueType));
    return *this;
  }

  inline void Set(ggSize aIndexX, ggSize aIndexY, const TValueType& aValue) {
    Get(aIndexX, aIndexY) = aValue;
  }

  template <typename TIndexType>
  inline void Set(const ggVector2T<TIndexType>& aIndex, const TValueType& aValue) {
    Set(aIndex.X(), aIndex.Y(), aValue);
  }

  inline void SetValues(const TValueType& aValue) {
    TValueType* vValueIterator = mValues;
    const TValueType* vValuesEnd = mValues + GetSizeTotal();
    while (vValueIterator != vValuesEnd) *vValueIterator++ = aValue;
  }

  inline ggImageT<TValueType>& operator = (const TValueType& aValue) {
    SetValues(aValue);
    return *this;
  }

  inline const TValueType* GetValues() const {
    return mValues;
  }

  inline TValueType* GetValues() {
    return mValues;
  }

private:

  ggVector2Size mSize;
  TValueType* mValues;

};

#endif // GGIMAGET_H
