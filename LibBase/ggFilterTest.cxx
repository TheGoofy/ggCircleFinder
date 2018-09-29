#include <random>

#include "LibBase/ggUnitTest.h"
#include "LibBase/ggFilterMedianT.h"
#include "LibBase/ggFilterCenterT.h"
#include "LibBase/ggNumberTypes.h"
#include "LibBase/ggVectorTypes.h"


template <typename TValueType>
TValueType ggRand(TValueType aSigma)
{
  // set up random generator
  static std::random_device vRandomDevice;
  static std::mt19937 vEngine(vRandomDevice());
  static std::normal_distribution<TValueType> vRandom(0, aSigma);
  return vRandom(vEngine);
}


template <typename TValueType, ggUSize TDimensions>
void ggRandomize(ggVectorT<TValueType, TDimensions>& aVector, TValueType aSigma)
{
  aVector.ProcessValues([&] (TValueType& aValue) {
    aValue += ggRand<TValueType>(aSigma);
  });
}


static void ggFilterCompareMedianAndCenter(const ggUSize aCountMax)
{
  const ggFloat vSigma = 1.0f;
  const ggVector2Float vDrift(10.0f*vSigma/aCountMax, 5.0f*vSigma/aCountMax);
  const ggUSize vFilterLength = 21;

  auto vLess = [] (const ggVector2Float& aVectorA, const ggVector2Float& aVectorB) {
    return aVectorA.IsShorter(aVectorB);
  };
  auto vDistance = [] (const ggVector2Float& aVectorA, const ggVector2Float& aVectorB) {
    return (aVectorA - aVectorB).Length();
  };

  ggFilterMedianT<ggVector2Float, decltype(vLess)> vFilterMedian(vFilterLength, vLess);
  ggFilterCenterT<ggVector2Float, ggFloat, decltype(vDistance)> vFilterCenter(vFilterLength, vDistance);

  std::vector<ggVector2Float> vMeasured;
  std::vector<ggVector2Float> vMedian;
  std::vector<ggVector2Float> vCenter;

  for (ggUSize vCount = 0; vCount < aCountMax; vCount++) {

    ggVector2Float vVector = vCount * vDrift;
    ggRandomize(vVector, vSigma);

    vFilterMedian.Filter(vVector);
    vFilterCenter.Filter(vVector);

    vMeasured.push_back(vVector);
    vMedian.push_back(vFilterMedian.GetOut());
    vCenter.push_back(vFilterCenter.GetOut());
  }

  if (aCountMax > 0) {
    std::cout << "X" << "\t" << "Measured" << "\t" << "Median" << "\t" << "Center" << std::endl;
    for (ggUSize vIndex = 0; vIndex < vMeasured.size(); vIndex++) std::cout << vMeasured[vIndex].X() << "\t" << vMeasured[vIndex].Y() << "\t" << "\t" << std::endl;
    for (ggUSize vIndex = 0; vIndex < vMedian.size(); vIndex++) std::cout << vMedian[vIndex].X() << "\t" << "\t" << vMedian[vIndex].Y() << "\t" << std::endl;
    for (ggUSize vIndex = 0; vIndex < vCenter.size(); vIndex++) std::cout << vCenter[vIndex].X() << "\t" << "\t" << "\t" << vCenter[vIndex].Y() << std::endl;
  }
}


static void ggFilterTest()
{
  // change number of samples in order to generate some values
  ggFilterCompareMedianAndCenter(0);
}


GG_TEST_REGISTER(ggFilterTest);
