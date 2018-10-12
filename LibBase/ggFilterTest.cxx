#include <random>

#include "LibBase/ggUnitTest.h"
#include "LibBase/ggFilterMedianT.h"
#include "LibBase/ggFilterCenterT.h"
#include "LibBase/ggFilterOutlierT.h"
#include "LibBase/ggFilterMeanT.h"
#include "LibBase/ggNumberTypes.h"
#include "LibBase/ggVectorTypes.h"
#include "LibBase/ggAveragesT.h"


template <typename TValueType>
TValueType ggRand(TValueType aSigma)
{
  // set up random generator
  static std::random_device vRandomDevice;
  static std::mt19937 vEngine(vRandomDevice());
  std::normal_distribution<TValueType> vRandom(0, aSigma);
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
  const ggUSize vOrder = 3;

  auto vLess = [] (const ggVector2Float& aVectorA, const ggVector2Float& aVectorB) {
    return aVectorA.IsShorter(aVectorB);
  };
  auto vDistance = [] (const ggVector2Float& aVectorA, const ggVector2Float& aVectorB) {
    return (aVectorA - aVectorB).Length();
  };

  ggFilterMedianT<ggVector2Float, decltype(vLess)> vFilterMedian(vOrder, vLess);
  ggFilterCenterT<ggVector2Float, ggFloat, decltype(vDistance)> vFilterCenter(vOrder, vDistance);
  ggFilterOutlierT<ggVector2Float, ggFloat, decltype(vDistance)> vFilterOutlier(vOrder, 1, vDistance);
  ggFilterMeanT<ggVector2Float> vFilterMean(vOrder);

  std::vector<ggVector2Float> vTruth;
  std::vector<ggVector2Float> vMeasured;
  std::vector<ggVector2Float> vMedian;
  std::vector<ggVector2Float> vCenter;
  std::vector<ggVector2Float> vOutlier;
  std::vector<ggVector2Float> vMean;

  ggAveragesT<ggFloat> vDeviationMeasured;
  ggAveragesT<ggFloat> vDeviationMedian;
  ggAveragesT<ggFloat> vDeviationCenter;
  ggAveragesT<ggFloat> vDeviationOutlier;
  ggAveragesT<ggFloat> vDeviationMean;

  for (ggUSize vCount = 0; vCount < aCountMax; vCount++) {

    ggVector2Float vPositionTrue = vCount * vDrift;

    ggVector2Float vPositionMeasured(vPositionTrue);

    const ggUSize vCountOutlier = 10;
    if (vCount % vCountOutlier == vCountOutlier - 1) {
      ggRandomize(vPositionMeasured, 10*vSigma);
    }
    else {
      ggRandomize(vPositionMeasured, vSigma);
    }

    vFilterMedian.Filter(vPositionMeasured);
    vFilterCenter.Filter(vPositionMeasured);
    vFilterOutlier.Filter(vPositionMeasured);
    vFilterMean.Filter(vPositionMeasured);

    vTruth.push_back(vPositionTrue);
    vMeasured.push_back(vPositionMeasured);
    vMedian.push_back(vFilterMedian.GetOut());
    vCenter.push_back(vFilterCenter.GetOut());
    vOutlier.push_back(vFilterOutlier.GetOut());
    vMean.push_back(vFilterMean.GetOut());

    vDeviationMeasured.AddSample((vPositionTrue - vPositionMeasured).Length());
    vDeviationMedian.AddSample((vPositionTrue - vFilterMedian.GetOut()).Length());
    vDeviationCenter.AddSample((vPositionTrue - vFilterCenter.GetOut()).Length());
    vDeviationOutlier.AddSample((vPositionTrue - vFilterOutlier.GetOut()).Length());
    vDeviationMean.AddSample((vPositionTrue - vFilterMean.GetOut()).Length());
  }

  if (aCountMax > 0) {
    std::cout << "X" << "\t" << "Measured" << "\t" << "Median" << "\t" << "Center" << "\t" << "Outlier" << "\t" << "Truth" << std::endl;
    for (ggUSize vIndex = 0; vIndex < vMeasured.size(); vIndex++) std::cout << vMeasured[vIndex].X() << "\t" << vMeasured[vIndex].Y() << "\t" << "\t" << "\t" << "\t" << std::endl;
    for (ggUSize vIndex = 0; vIndex < vMedian.size(); vIndex++)   std::cout << vMedian[vIndex].X()   << "\t" << "\t" << vMedian[vIndex].Y()   << "\t" << "\t" << "\t" << std::endl;
    for (ggUSize vIndex = 0; vIndex < vCenter.size(); vIndex++)   std::cout << vCenter[vIndex].X()   << "\t" << "\t" << "\t" << vCenter[vIndex].Y()   << "\t" << "\t" << std::endl;
    for (ggUSize vIndex = 0; vIndex < vOutlier.size(); vIndex++)  std::cout << vOutlier[vIndex].X()  << "\t" << "\t" << "\t" << "\t" << vOutlier[vIndex].Y()  << "\t" << std::endl;
    for (ggUSize vIndex = 0; vIndex < vTruth.size(); vIndex++)    std::cout << vTruth[vIndex].X()    << "\t" << "\t" << "\t" << "\t" << "\t" << vTruth[vIndex].Y()    << std::endl;

    std::cout << "Deviation" << "\t" << "Mean" << "\t" << "StdDev" << "\t" << "Min" << "\t" << "Max" << std::endl;
    std::cout << "Measured" << "\t" << vDeviationMeasured.GetMean() << "\t" << vDeviationMeasured.GetStdDev() << "\t" << vDeviationMeasured.GetMin() << "\t" << vDeviationMeasured.GetMax() << std::endl;
    std::cout << "Median"   << "\t" << vDeviationMedian.GetMean()   << "\t" << vDeviationMedian.GetStdDev()   << "\t" << vDeviationMedian.GetMin()   << "\t" << vDeviationMedian.GetMax()   << std::endl;
    std::cout << "Center"   << "\t" << vDeviationCenter.GetMean()   << "\t" << vDeviationCenter.GetStdDev()   << "\t" << vDeviationCenter.GetMin()   << "\t" << vDeviationCenter.GetMax()   << std::endl;
    std::cout << "Outlier"  << "\t" << vDeviationOutlier.GetMean()  << "\t" << vDeviationOutlier.GetStdDev()  << "\t" << vDeviationOutlier.GetMin()  << "\t" << vDeviationOutlier.GetMax()  << std::endl;
    std::cout << "Mean"     << "\t" << vDeviationMean.GetMean()     << "\t" << vDeviationMean.GetStdDev()     << "\t" << vDeviationMean.GetMin()     << "\t" << vDeviationMean.GetMax()     << std::endl;
  }
}


static void ggFilterTest()
{
  // change number of samples in order to generate some values
  ggFilterCompareMedianAndCenter(0);
}


GG_TEST_REGISTER(ggFilterTest);
