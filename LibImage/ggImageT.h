#ifndef GGIMAGET_H
#define GGIMAGET_H

#include <cstring>

#include "LibBase/ggTypes.h"
#include "LibBase/ggAssert.h"
#include "LibBase/ggVectorTypes.h"


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

  inline bool Empty() const {
    return (mSize.X() > 0) && (mSize.Y() > 0);
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
    return (0 <= aIndexX) && (aIndexX < GetSizeX()) &&
           (0 <= aIndexY) && (aIndexY < GetSizeY());
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
    ProcessValues([aValue] (TValueType& aValueProcessed) {
      aValueProcessed = aValue;
    });
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

  inline static TValueType GetMinLimit() {
    return std::numeric_limits<TValueType>::min();
  }

  inline static TValueType GetMaxLimit() {
    return std::numeric_limits<TValueType>::max();
  }

  template <typename TValueTypeDst>
  void Copy(ggImageT<TValueTypeDst>& aImageDst,
            const ggSize aPositionSrcX0,
            const ggSize aPositionSrcY0)
  {
    const ggSize vSrcX0 = Clamp<ggSize>(aPositionSrcX0, 0, GetSizeX());
    const ggSize vSrcY0 = Clamp<ggSize>(aPositionSrcY0, 0, GetSizeY());
    const ggSize vSrcX1 = Clamp<ggSize>(aPositionSrcX0 + aImageDst.GetSizeX(), 0, GetSizeX());
    const ggSize vSrcY1 = Clamp<ggSize>(aPositionSrcY0 + aImageDst.GetSizeY(), 0, GetSizeY());
    for (ggSize vSrcY = vSrcY0; vSrcY < vSrcY1; vSrcY++) {
      ggSize vDstY = vSrcY - aPositionSrcY0;
      for (ggSize vSrcX = vSrcX0; vSrcX < vSrcX1; vSrcX++) {
        ggSize vDstX = vSrcX - aPositionSrcX0;
        aImageDst(vDstX, vDstY) = (*this)(vSrcX, vSrcY);
      }
    }
  }

  template <typename TProcessor>
  inline void ProcessValues(TProcessor aProcessor) {
    TValueType* vValuesIterator = mValues;
    TValueType* vValuesEnd = mValues + GetSizeTotal();
    while (vValuesIterator != vValuesEnd) {
      aProcessor(*vValuesIterator);
      ++vValuesIterator;
    }
  }

  template <typename TProcessor>
  inline void ProcessValues(TProcessor aProcessor) const {
    const TValueType* vValuesIterator = mValues;
    const TValueType* vValuesEnd = mValues + GetSizeTotal();
    while (vValuesIterator != vValuesEnd) {
      aProcessor(*vValuesIterator);
      ++vValuesIterator;
    }
  }

  const TValueType& GetMin() const {
    GG_ASSERT(!Empty());
    const TValueType& vMin = mValues[0];
    ProcessValues([&vMin] (const TValueType& aValue) {
      if (aValue < vMin) vMin = aValue;
    });
    return vMin;
  }

  const TValueType& GetMax() const {
    GG_ASSERT(!Empty());
    const TValueType& vMax = mValues[0];
    ProcessValues([&vMax] (const TValueType& aValue) {
      if (aValue > vMax) vMax = aValue;
    });
    return vMax;
  }

  void GetMinMax(TValueType& aMin, TValueType& aMax) const {
    if (!Empty()) aMin = aMax = mValues[0];
    ProcessValues([&aMin, &aMax] (const TValueType& aValue) {
      if (aValue < aMin) aMin = aValue;
      if (aValue > aMax) aMax = aValue;
    });
  }

  ggVectorT<TValueType, 2> GetMinMax() const {
    ggVectorT<TValueType, 2> vMinMax;
    GetMinMax(vMinMax[0], vMinMax[1]);
    return vMinMax;
  }

  void Invert(const TValueType& aReference) {
    ProcessValues([aReference] (TValueType& aValue) {
      aValue = aReference - aValue;
    });
  }

  void Invert() {
    Invert(GetMinLimit() + GetMaxLimit());
  }

  ggImageT<TValueType> Inverted(const TValueType& aReference) {
    ggImageT<TValueType> vImageInverted(*this);
    vImageInverted.Invert(aReference);
    return vImageInverted;
  }

  ggImageT<TValueType> Inverted() {
    ggImageT<TValueType> vImageInverted(*this);
    vImageInverted.Invert();
    return vImageInverted;
  }

  template <typename TValueTypeProcessed, typename TProcessor>
  inline ggImageT<TValueTypeProcessed> GetProcessed(TProcessor aProcessor) const {
    ggImageT<TValueTypeProcessed> vImageProcessed(mSize);
    const TValueType* vValuesIterator = mValues;
    const TValueType* vValuesEnd = mValues + GetSizeTotal();
    TValueTypeProcessed* vValuesProcessedIterator = vImageProcessed.GetValues();
    while (vValuesIterator != vValuesEnd) {
      *vValuesProcessedIterator++ = aProcessor(*vValuesIterator++);
    }
    return vImageProcessed;
  }

  template <typename TValueTypeConverted>
  inline ggImageT<TValueTypeConverted> GetConvertedFitMinMax(const TValueTypeConverted& aMinConverted,
                                                             const TValueTypeConverted& aMaxConverted) const {
    ggVectorT<tValueType, 2> vMinMax(GetMinMax());
    ggDouble vDelta = vMinMax[1] - vMinMax[0];
    if (vDelta != 0.0) {
      ggDouble vDeltaConverted = aMaxConverted - aMinConverted;
      ggDouble vScale = vDeltaConverted / vDelta;
      ggDouble vOffset = aMinConverted - vMinMax[0] * vScale;
      return GetProcessed<TValueTypeConverted>([vScale, vOffset] (const TValueType& aValue) {
        return aValue * vScale + vOffset;
      });
    }
    else {
      return GetProcessed<TValueTypeConverted>([] (const TValueType& aValue) {
        return aValue;
      });
    }
  }

  template <typename TValueTypeConverted>
  inline ggImageT<TValueTypeConverted> GetConvertedFitMinMax() const {
    return GetConvertedFitMinMax<TValueTypeConverted>(ggImageT<TValueTypeConverted>::GetMinLimit(),
                                                      ggImageT<TValueTypeConverted>::GetMaxLimit());
  }

private:

  template <typename T>
  inline T Clamp(const T& aValue, const T& aValueMin, const T& aValueMax) {
    return aValue < aValueMin ? aValueMin : (aValue > aValueMax ? aValueMax : aValue);
  }

  ggVector2Size mSize;
  TValueType* mValues;

};

template <>
inline float ggImageT<float>::GetMinLimit() {
  return 0.0f;
}

template <>
inline double ggImageT<double>::GetMinLimit() {
  return 0.0;
}

template <>
inline long double ggImageT<long double>::GetMinLimit() {
  return 0.0;
}

template <>
inline float ggImageT<float>::GetMaxLimit() {
  return 1.0f;
}

template <>
inline double ggImageT<double>::GetMaxLimit() {
  return 1.0;
}

template <>
inline long double ggImageT<long double>::GetMaxLimit() {
  return 1.0;
}

#endif // GGIMAGET_H
