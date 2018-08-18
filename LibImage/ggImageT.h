#ifndef GGIMAGET_H
#define GGIMAGET_H


#include "LibBase/ggNumberTypes.h"
#include "LibBase/ggNumerics.h"
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
    memcpy(mValues, aOther.mValues, GetSizeTotal() * sizeof(TValueType));
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
    return (mSize.X() == 0) || (mSize.Y() == 0);
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

  inline const TValueType& GetClamp(ggSize aIndexX, ggSize aIndexY) const {
    return Get(ggClamp<ggSize>(aIndexX, 0, GetSizeX() - 1),
               ggClamp<ggSize>(aIndexY, 0, GetSizeY() - 1));
  }

  template <typename TIndexType>
  inline const TValueType& GetClamp(const ggVector2T<TIndexType>& aIndex) const {
    return Get(ggClamp<ggSize>(aIndex.X(), 0, GetSizeX() - 1),
               ggClamp<ggSize>(aIndex.Y(), 0, GetSizeY() - 1));
  }

  inline ggImageT<TValueType>& operator = (const ggImageT<TValueType>& aOther) {
    Resize(aOther.mSize);
    memcpy(mValues, aOther.mValues, aOther.GetSizeTotal() * sizeof(TValueType));
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
    const ggSize vSrcX0 = ggClamp<ggSize>(aPositionSrcX0, 0, GetSizeX());
    const ggSize vSrcY0 = ggClamp<ggSize>(aPositionSrcY0, 0, GetSizeY());
    const ggSize vSrcX1 = ggClamp<ggSize>(aPositionSrcX0 + aImageDst.GetSizeX(), 0, GetSizeX());
    const ggSize vSrcY1 = ggClamp<ggSize>(aPositionSrcY0 + aImageDst.GetSizeY(), 0, GetSizeY());
    for (ggSize vSrcY = vSrcY0; vSrcY < vSrcY1; vSrcY++) {
      ggSize vDstY = vSrcY - aPositionSrcY0;
      for (ggSize vSrcX = vSrcX0; vSrcX < vSrcX1; vSrcX++) {
        ggSize vDstX = vSrcX - aPositionSrcX0;
        aImageDst(vDstX, vDstY) = (*this)(vSrcX, vSrcY);
      }
    }
  }

  template <typename TValueProcessor>
  inline void ProcessValues(TValueProcessor aValueProcessor) {
    TValueType* vValuesIterator = mValues;
    TValueType* vValuesEnd = mValues + GetSizeTotal();
    while (vValuesIterator != vValuesEnd) {
      aValueProcessor(*vValuesIterator);
      ++vValuesIterator;
    }
  }

  template <typename TValueProcessor>
  inline void ProcessValues(TValueProcessor aValueProcessor) const {
    const TValueType* vValuesIterator = mValues;
    const TValueType* vValuesEnd = mValues + GetSizeTotal();
    while (vValuesIterator != vValuesEnd) {
      aValueProcessor(*vValuesIterator);
      ++vValuesIterator;
    }
  }

  template <typename TIndexProcessor>
  inline void ProcessIndex(TIndexProcessor aIndexProcessor) const {
    for (ggSize vIndexY = 0; vIndexY < GetSizeY(); vIndexY++) {
      for (ggSize vIndexX = 0; vIndexX < GetSizeX(); vIndexX++) {
        aIndexProcessor(vIndexX, vIndexY);
      }
    }
  }

  template <typename TIndexProcessor>
  inline void ProcessIndexReverse(TIndexProcessor aIndexProcessor) const {
    for (ggSize vIndexY = GetSizeY() - 1; vIndexY >= 0; vIndexY--) {
      for (ggSize vIndexX = GetSizeX() - 1; vIndexX >= 0; vIndexX--) {
        aIndexProcessor(vIndexX, vIndexY);
      }
    }
  }

  template <typename TIndexProcessor>
  inline void ProcessIndexRangeInside(ggSize aIndexBeginX, ggSize aIndexBeginY,
                                      ggSize aIndexEndX, ggSize aIndexEndY,
                                      TIndexProcessor aIndexProcessor) const {
    aIndexBeginX = ggClamp<ggSize>(aIndexBeginX, 0, GetSizeX());
    aIndexBeginY = ggClamp<ggSize>(aIndexBeginY, 0, GetSizeY());
    aIndexEndX = ggClamp<ggSize>(aIndexEndX, 0, GetSizeX());
    aIndexEndY = ggClamp<ggSize>(aIndexEndY, 0, GetSizeY());
    for (ggSize vIndexY = aIndexBeginY; vIndexY < aIndexEndY; vIndexY++) {
      for (ggSize vIndexX = aIndexBeginX; vIndexX < aIndexEndX; vIndexX++) {
        aIndexProcessor(vIndexX, vIndexY);
      }
    }
  }

  template <typename TIndexType, typename TIndexProcessor>
  inline void ProcessIndexRangeInside(const ggVector2T<TIndexType>& aIndexBegin,
                                      const ggVector2T<TIndexType>& aIndexEnd,
                                      TIndexProcessor aIndexProcessor) const {
    ProcessIndexRangeInside(aIndexBegin.X(), aIndexBegin.Y(),
                            aIndexEnd.X(), aIndexEnd.Y(),
                            aIndexProcessor);
  }

  template <typename TIndexProcessor>
  inline void ProcessIndexRangeOutside(ggSize aIndexBeginX, ggSize aIndexBeginY,
                                       ggSize aIndexEndX, ggSize aIndexEndY,
                                       TIndexProcessor aIndexProcessor) const {
    aIndexBeginX = ggClamp<ggSize>(aIndexBeginX, 0, GetSizeX());
    aIndexBeginY = ggClamp<ggSize>(aIndexBeginY, 0, GetSizeY());
    aIndexEndX = ggClamp<ggSize>(aIndexEndX, 0, GetSizeX());
    aIndexEndY = ggClamp<ggSize>(aIndexEndY, 0, GetSizeY());
    for (ggSize vIndexY = 0; vIndexY < aIndexBeginY; vIndexY++) {
      for (ggSize vIndexX = 0; vIndexX < GetSizeX(); vIndexX++) {
        aIndexProcessor(vIndexX, vIndexY);
      }
    }
    for (ggSize vIndexY = aIndexBeginY; vIndexY < aIndexEndY; vIndexY++) {
      for (ggSize vIndexX = 0; vIndexX < aIndexBeginX; vIndexX++) {
        aIndexProcessor(vIndexX, vIndexY);
      }
      for (ggSize vIndexX = aIndexEndX; vIndexX < GetSizeX(); vIndexX++) {
        aIndexProcessor(vIndexX, vIndexY);
      }
    }
    for (ggSize vIndexY = aIndexEndY; vIndexY < GetSizeY(); vIndexY++) {
      for (ggSize vIndexX = 0; vIndexX < GetSizeX(); vIndexX++) {
        aIndexProcessor(vIndexX, vIndexY);
      }
    }
  }

  template <typename TIndexType, typename TIndexProcessor>
  inline void ProcessIndexRangeOutside(const ggVector2T<TIndexType>& aIndexBegin,
                                       const ggVector2T<TIndexType>& aIndexEnd,
                                       TIndexProcessor aIndexProcessor) const {
    ProcessIndexRangeOutside(aIndexBegin.X(), aIndexBegin.Y(),
                             aIndexEnd.X(), aIndexEnd.Y(),
                             aIndexProcessor);
  }

  template <typename TIndexProcessorInside, typename TIndexProcessorOutside>
  inline void ProcessIndexRange(ggSize aIndexBeginX, ggSize aIndexBeginY,
                                ggSize aIndexEndX, ggSize aIndexEndY,
                                TIndexProcessorInside aIndexProcessorInside,
                                TIndexProcessorOutside aIndexProcessorOutside) const {
    aIndexBeginX = ggClamp<ggSize>(aIndexBeginX, 0, GetSizeX());
    aIndexBeginY = ggClamp<ggSize>(aIndexBeginY, 0, GetSizeY());
    aIndexEndX = ggClamp<ggSize>(aIndexEndX, 0, GetSizeX());
    aIndexEndY = ggClamp<ggSize>(aIndexEndY, 0, GetSizeY());
    for (ggSize vIndexY = 0; vIndexY < aIndexBeginY; vIndexY++) {
      for (ggSize vIndexX = 0; vIndexX < GetSizeX(); vIndexX++) {
        aIndexProcessorOutside(vIndexX, vIndexY);
      }
    }
    for (ggSize vIndexY = aIndexBeginY; vIndexY < aIndexEndY; vIndexY++) {
      for (ggSize vIndexX = 0; vIndexX < aIndexBeginX; vIndexX++) {
        aIndexProcessorOutside(vIndexX, vIndexY);
      }
      for (ggSize vIndexX = aIndexBeginX; vIndexX < aIndexEndX; vIndexX++) {
        aIndexProcessorInside(vIndexX, vIndexY);
      }
      for (ggSize vIndexX = aIndexEndX; vIndexX < GetSizeX(); vIndexX++) {
        aIndexProcessorOutside(vIndexX, vIndexY);
      }
    }
    for (ggSize vIndexY = aIndexEndY; vIndexY < GetSizeY(); vIndexY++) {
      for (ggSize vIndexX = 0; vIndexX < GetSizeX(); vIndexX++) {
        aIndexProcessorOutside(vIndexX, vIndexY);
      }
    }
  }

  template <typename TIndexType, typename TIndexProcessorInside, typename TIndexProcessorOutside>
  inline void ProcessIndexRange(const ggVector2T<TIndexType>& aIndexBegin,
                                const ggVector2T<TIndexType>& aIndexEnd,
                                TIndexProcessorInside aIndexProcessorInside,
                                TIndexProcessorOutside aIndexProcessorOutside) const {

    ProcessIndexRange(aIndexBegin.X(), aIndexBegin.Y(),
                      aIndexEnd.X(), aIndexEnd.Y(),
                      aIndexProcessorInside,
                      aIndexProcessorOutside);
  }

  template <typename TIndexProcessor>
  inline void ProcessIndexBorderInside(ggSize aBorderSize,
                                       TIndexProcessor aIndexProcessor) const {
    ProcessIndexRangeInside(aBorderSize, aBorderSize,
                            GetSizeX() - aBorderSize, GetSizeY() - aBorderSize,
                            aIndexProcessor);
  }

  template <typename TIndexProcessor>
  inline void ProcessIndexBorderOutside(ggSize aBorderSize,
                                        TIndexProcessor aIndexProcessor) const {
    ProcessIndexRangeOutside(aBorderSize, aBorderSize,
                             GetSizeX() - aBorderSize, GetSizeY() - aBorderSize,
                             aIndexProcessor);
  }

  template <typename TIndexProcessorInside, typename TIndexProcessorOutside>
  inline void ProcessIndexBorder(ggSize aBorderSize,
                                 TIndexProcessorInside aIndexProcessorInside,
                                 TIndexProcessorOutside aIndexProcessorOutside) const {
    ProcessIndexRange(aBorderSize, aBorderSize,
                      GetSizeX() - aBorderSize, GetSizeY() - aBorderSize,
                      aIndexProcessorInside,
                      aIndexProcessorOutside);
  }

  TValueType GetMin() const {
    GG_ASSERT(!Empty());
    TValueType vMin = mValues[0];
    ProcessValues([&vMin] (const TValueType& aValue) {
      if (aValue < vMin) vMin = aValue;
    });
    return vMin;
  }

  TValueType GetMax() const {
    GG_ASSERT(!Empty());
    TValueType vMax = mValues[0];
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
  inline ggImageT<TValueTypeConverted> GetConverted() const {
    return GetProcessed<TValueTypeConverted>([] (const TValueType& aValue) {
      return static_cast<TValueTypeConverted>(aValue);
    });
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

  template <typename TOtherValueType, typename TValueProcessor>
  inline void ProcessValues(const ggImageT<TOtherValueType>& aOther, TValueProcessor aValueProcessor) {
    GG_ASSERT(GetSize() == aOther.GetSize());
    TValueType* vValuesIterator = mValues;
    TValueType* vValuesEnd = mValues + GetSizeTotal();
    const TOtherValueType* vOtherValuesIterator = aOther.GetValues();
    while (vValuesIterator != vValuesEnd) {
      aValueProcessor(*vValuesIterator++, *vOtherValuesIterator++);
    }
  }

  inline ggImageT<TValueType>& operator += (const TValueType& aValue) {
    ProcessValues([aValue] (TValueType& aImageValue) {
      aImageValue += aValue;
    });
    return *this;
  }

  inline ggImageT<TValueType>& operator -= (const TValueType& aValue) {
    ProcessValues([aValue] (TValueType& aImageValue) {
      aImageValue -= aValue;
    });
    return *this;
  }

  template <typename TScalarValueType>
  inline ggImageT<TValueType>& operator *= (const TScalarValueType& aScalarValue) {
    ProcessValues([aScalarValue] (TValueType& aImageValue) {
      aImageValue *= aScalarValue;
    });
    return *this;
  }

  template <typename TScalarValueType>
  inline ggImageT<TValueType>& operator /= (const TScalarValueType& aScalarValue) {
    ProcessValues([aScalarValue] (TValueType& aImageValue) {
      aImageValue /= aScalarValue;
    });
    return *this;
  }

  inline ggImageT<TValueType>& operator += (const ggImageT<TValueType>& aOther) {
    ProcessValues(aOther, [] (TValueType& aImageValue, const TValueType& aOtherImageValue) {
      aImageValue += aOtherImageValue;
    });
    return *this;
  }

  inline ggImageT<TValueType>& operator -= (const ggImageT<TValueType>& aOther) {
    ProcessValues(aOther, [] (TValueType& aImageValue, const TValueType& aOtherImageValue) {
      aImageValue -= aOtherImageValue;
    });
    return *this;
  }

private:

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
  return 0.0L;
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
  return 1.0L;
}

#endif // GGIMAGET_H
