#ifndef GGLINET_H
#define GGLINET_H


template <class TVector>
class ggLineT
{
public:

  typedef TVector tVector;

  inline ggLineT()
  : mPos0(),
    mPos1() {
  }

  inline ggLineT(const TVector& aPos0,
                 const TVector& aPos1)
  : mPos0(aPos0),
    mPos1(aPos1) {
  }

  inline ggLineT(const ggLineT& aOther)
  : mPos0(aOther.mPos0),
    mPos1(aOther.mPos1) {
  }

  inline static ggLineT FromDir(const TVector& aPos0,
                                const TVector& aDir) {
    return ggLineT(aPos0, aPos0 + aDir);
  }

  inline TVector& Pos0() {
    return mPos0;
  }

  inline const TVector& Pos0() const {
    return mPos0;
  }

  inline TVector& Pos1() {
    return mPos1;
  }

  inline const TVector& Pos1() const {
    return mPos1;
  }

  inline TVector Dir() const {
    return mPos1 - mPos0;
  }

  inline void SetDir(const TVector& aDir) {
    mPos1 = mPos0 + aDir;
  }

  inline void Translate(const TVector& aOffset) {
    mPos0 += aOffset;
    mPos1 += aOffset;
  }

  inline ggLineT Translated(const TVector& aOffset) const {
    return ggLineT(mPos0 + aOffset, mPos1 + aOffset);
  }

  inline typename TVector::tValueType Length() const {
    return Dir().Length();
  }

  inline void SetLength(const typename TVector::tValueType& aLength) {
    TVector vDirN(Dir().Normalized());
    mPos1 = mPos0 + aLength * vDirN;
  }

  template <typename TScalarType>
  inline TVector PosAt(const TScalarType& aScalar) const {
    TVector vDirectionN(Dir().Normalized());
    return mPos0 + aScalar * vDirectionN;
  }

private:

  TVector mPos0;
  TVector mPos1;

};


#endif // GGLINET_H
