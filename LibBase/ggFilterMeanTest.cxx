#include <vector>

#include "LibBase/ggUnitTest.h"
#include "LibBase/ggFilterMeanT.h"
#include "LibBase/ggNumberTypes.h"
#include "LibBase/ggVectorTypes.h"


static void ggFilterMeanTestInt32()
{
  ggFilterMeanT<ggInt32> vFilterMean(3);
  ggFilterT<ggInt32>& vFilter = vFilterMean;

  for (ggInt32 vCount = 0; vCount < 2; vCount++) {

    vFilter.Reset();

    GG_TEST_EQUAL(vFilter.Filter(5), 5);
    GG_TEST_EQUAL(vFilter.GetIn(), 5);
    GG_TEST_EQUAL(vFilter.GetOut(), 5);

    GG_TEST_EQUAL(vFilter.Filter(3), 4);
    GG_TEST_EQUAL(vFilter.GetIn(), 3);
    GG_TEST_EQUAL(vFilter.GetOut(), 4);

    GG_TEST_EQUAL(vFilter.Filter(1), 3);
    GG_TEST_EQUAL(vFilter.GetIn(), 1);
    GG_TEST_EQUAL(vFilter.GetOut(), 3);

    GG_TEST_EQUAL(vFilter.Filter(7), 3);
    GG_TEST_EQUAL(vFilter.GetIn(), 7);
    GG_TEST_EQUAL(vFilter.GetOut(), 3);

    GG_TEST_EQUAL(vFilter.Filter(-3), 1);
    GG_TEST_EQUAL(vFilter.GetIn(), -3);
    GG_TEST_EQUAL(vFilter.GetOut(), 1);

    GG_TEST_EQUAL(vFilter.Filter(-10), -2);
    GG_TEST_EQUAL(vFilter.GetIn(), -10);
    GG_TEST_EQUAL(vFilter.GetOut(), -2);

    GG_TEST_EQUAL(vFilter.Filter(1), -4);
    GG_TEST_EQUAL(vFilter.GetIn(), 1);
    GG_TEST_EQUAL(vFilter.GetOut(), -4);
  }
}


static void ggFilterMeanTestVector2Float()
{
  ggFilterMeanT<ggVector2Float> vFilterMean(3);
  ggFilterT<ggVector2Float>& vFilter = vFilterMean;

  vFilter.Filter(ggVector2Float(3.0f,-1.0f));
  GG_TEST_EQUAL(vFilter.GetIn(), ggVector2Float(3.0f,-1.0f));
  GG_TEST_EQUAL(vFilter.GetOut(), ggVector2Float(3.0f,-1.0f));

  vFilter.Filter(ggVector2Float(-4.0f,0.0f));
  GG_TEST_EQUAL(vFilter.GetIn(), ggVector2Float(-4.0f,0.0f));
  GG_TEST_EQUAL(vFilter.GetOut(), ggVector2Float((3.0f-4.0f)/2,(-1.0f+0.0f)/2));

  vFilter.Filter(ggVector2Float(5.0f,1.0f));
  GG_TEST_EQUAL(vFilter.GetIn(), ggVector2Float(5.0f,1.0f));
  GG_TEST_EQUAL(vFilter.GetOut(), ggVector2Float((3.0f-4.0f+5.0f)/3,(-1.0f+0.0f+1.0f)/3));

  vFilter.Filter(ggVector2Float(4.0f,2.0f));
  GG_TEST_EQUAL(vFilter.GetIn(), ggVector2Float(4.0f,2.0f));
  GG_TEST_EQUAL(vFilter.GetOut(), ggVector2Float((-4.0f+5.0f+4.0f)/3,(0.0f+1.0f+2.0f)/3));

  vFilter.Filter(ggVector2Float(6.0f,2.0f));
  GG_TEST_EQUAL(vFilter.GetIn(), ggVector2Float(6.0f,2.0f));
  GG_TEST_EQUAL(vFilter.GetOut(), ggVector2Float((5.0f+4.0f+6.0f)/3,(1.0f+2.0f+2.0f)/3));
}


static void ggFilterMeanTest()
{
  ggFilterMeanTestInt32();
  ggFilterMeanTestVector2Float();
}


GG_TEST_REGISTER(ggFilterMeanTest);
