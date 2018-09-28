#include <vector>

#include "LibBase/ggUnitTest.h"
#include "LibBase/ggFilterCenterT.h"
#include "LibBase/ggNumberTypes.h"
#include "LibBase/ggVectorTypes.h"


static void ggFilterCenterTestInt32()
{
  // for scalar value types, the filter should behave like the median

  ggFilterCenterT<ggInt32> vFilterCenter(5);
  ggFilterT<ggInt32>& vFilter = vFilterCenter;

  for (ggInt32 vCount = 0; vCount < 2; vCount++) {

    vFilter.Reset();

    GG_TEST_EQUAL(vFilter.Filter(6), 6);
    GG_TEST_EQUAL(vFilter.GetIn(), 6);
    GG_TEST_EQUAL(vFilter.GetOut(), 6);

    GG_TEST_EQUAL(vFilter.Filter(3), 6);
    GG_TEST_EQUAL(vFilter.GetIn(), 3);
    GG_TEST_EQUAL(vFilter.GetOut(), 6);

    GG_TEST_EQUAL(vFilter.Filter(5), 5);
    GG_TEST_EQUAL(vFilter.GetIn(), 5);
    GG_TEST_EQUAL(vFilter.GetOut(), 5);

    GG_TEST_EQUAL(vFilter.Filter(2), 3);
    GG_TEST_EQUAL(vFilter.GetIn(), 2);
    GG_TEST_EQUAL(vFilter.GetOut(), 3);

    GG_TEST_EQUAL(vFilter.Filter(7), 5);
    GG_TEST_EQUAL(vFilter.GetIn(), 7);
    GG_TEST_EQUAL(vFilter.GetOut(), 5);

    GG_TEST_EQUAL(vFilter.Filter(50), 5);
    GG_TEST_EQUAL(vFilter.GetIn(), 50);
    GG_TEST_EQUAL(vFilter.GetOut(), 5);

    GG_TEST_EQUAL(vFilter.Filter(6), 6);
    GG_TEST_EQUAL(vFilter.GetIn(), 6);
    GG_TEST_EQUAL(vFilter.GetOut(), 6);
  }
}


static void ggFilterCenterTestVector2Float()
{
  auto vDistance = [] (const ggVector2Float& aVectorA, const ggVector2Float& aVectorB) {
    return (aVectorA - aVectorB).Length();
  };

  ggFilterCenterT<ggVector2Float, ggFloat, decltype(vDistance)> vFilterCenter(3, vDistance);
  ggFilterT<ggVector2Float>& vFilter = vFilterCenter;

  vFilter.Filter(ggVector2Float(3,-1));
  GG_TEST_EQUAL(vFilter.GetIn(), ggVector2Float(3,-1));
  GG_TEST_EQUAL(vFilter.GetOut(), ggVector2Float(3,-1));

  vFilter.Filter(ggVector2Float(-4,0));
  GG_TEST_EQUAL(vFilter.GetIn(), ggVector2Float(-4,0));
  GG_TEST_EQUAL(vFilter.GetOut(), ggVector2Float(3,-1));

  vFilter.Filter(ggVector2Float(5,1));
  GG_TEST_EQUAL(vFilter.GetIn(), ggVector2Float(5,1));
  GG_TEST_EQUAL(vFilter.GetOut(), ggVector2Float(3,-1));

  vFilter.Filter(ggVector2Float(4,2));
  GG_TEST_EQUAL(vFilter.GetIn(), ggVector2Float(4,2));
  GG_TEST_EQUAL(vFilter.GetOut(), ggVector2Float(4,2));

  vFilter.Filter(ggVector2Float(6,2));
  GG_TEST_EQUAL(vFilter.GetIn(), ggVector2Float(6,2));
  GG_TEST_EQUAL(vFilter.GetOut(), ggVector2Float(5,1));
}


static void ggFilterCenterTest()
{
  ggFilterCenterTestInt32();
  ggFilterCenterTestVector2Float();
}


GG_TEST_REGISTER(ggFilterCenterTest);
