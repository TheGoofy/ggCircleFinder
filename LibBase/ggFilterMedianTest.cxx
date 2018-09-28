#include <vector>

#include "LibBase/ggUnitTest.h"
#include "LibBase/ggFilterMedianT.h"
#include "LibBase/ggNumberTypes.h"
#include "LibBase/ggVectorTypes.h"


static void ggFilterMedianTestInt32()
{
  ggFilterMedianT<ggInt32> vFilterMedian(5);
  ggFilterT<ggInt32>& vFilter = vFilterMedian;

  for (ggInt32 vCount = 0; vCount < 2; vCount++) {

    vFilter.Reset();

    GG_TEST_EQUAL(vFilter.Filter(6), 6);
    GG_TEST_EQUAL(vFilter.GetIn(), 6);
    GG_TEST_EQUAL(vFilter.GetOut(), 6);

    GG_TEST_EQUAL(vFilter.Filter(3), 3);
    GG_TEST_EQUAL(vFilter.GetIn(), 3);
    GG_TEST_EQUAL(vFilter.GetOut(), 6);

    GG_TEST_EQUAL(vFilter.Filter(5), 5);
    GG_TEST_EQUAL(vFilter.GetIn(), 5);
    GG_TEST_EQUAL(vFilter.GetOut(), 5);

    GG_TEST_EQUAL(vFilter.Filter(2), 2);
    GG_TEST_EQUAL(vFilter.GetIn(), 2);
    GG_TEST_EQUAL(vFilter.GetOut(), 5);

    GG_TEST_EQUAL(vFilter.Filter(7), 7);
    GG_TEST_EQUAL(vFilter.GetIn(), 7);
    GG_TEST_EQUAL(vFilter.GetOut(), 5);

    GG_TEST_EQUAL(vFilter.Filter(50), 50);
    GG_TEST_EQUAL(vFilter.GetIn(), 50);
    GG_TEST_EQUAL(vFilter.GetOut(), 5);

    GG_TEST_EQUAL(vFilter.Filter(6), 6);
    GG_TEST_EQUAL(vFilter.GetIn(), 6);
    GG_TEST_EQUAL(vFilter.GetOut(), 6);
  }
}


static void ggFilterMedianTestVector2Float()
{
  auto vLess = [] (const ggVector2Float& aVectorA, const ggVector2Float& aVectorB) {
    return aVectorA.IsShorter(aVectorB);
  };

  ggFilterMedianT<ggVector2Float, decltype(vLess)> vFilterMedian(3, vLess);
  ggFilterT<ggVector2Float>& vFilter = vFilterMedian;

  vFilter.Filter(ggVector2Float(3,2));
  GG_TEST_EQUAL(vFilter.GetIn(), ggVector2Float(3,2));
  GG_TEST_EQUAL(vFilter.GetOut(), ggVector2Float(3,2));

  vFilter.Filter(ggVector2Float(5,4));
  GG_TEST_EQUAL(vFilter.GetIn(), ggVector2Float(5,4));
  GG_TEST_EQUAL(vFilter.GetOut(), ggVector2Float(5,4));

  vFilter.Filter(ggVector2Float(20,1));
  GG_TEST_EQUAL(vFilter.GetIn(), ggVector2Float(20,1));
  GG_TEST_EQUAL(vFilter.GetOut(), ggVector2Float(5,4));

  vFilter.Filter(ggVector2Float(4,3));
  GG_TEST_EQUAL(vFilter.GetIn(), ggVector2Float(4,3));
  GG_TEST_EQUAL(vFilter.GetOut(), ggVector2Float(5,4));

  vFilter.Filter(ggVector2Float(2,2));
  GG_TEST_EQUAL(vFilter.GetIn(), ggVector2Float(2,2));
  GG_TEST_EQUAL(vFilter.GetOut(), ggVector2Float(4,3));
}


static void ggFilterMedianTest()
{
  ggFilterMedianTestInt32();
  ggFilterMedianTestVector2Float();
}


GG_TEST_REGISTER(ggFilterMedianTest);
