#include <vector>

#include "LibBase/ggUnitTest.h"
#include "LibBase/ggFilterDelayT.h"
#include "LibBase/ggNumberTypes.h"
#include "LibBase/ggVectorTypes.h"


static void ggFilterDelayTestInt32()
{
  ggFilterDelayT<ggInt32> vFilterDelay4(4);
  ggFilterT<ggInt32>* vFilter = &vFilterDelay4;

  for (ggInt32 vCount = 0; vCount < 2; vCount++) {

    vFilter->Reset();
    GG_TEST_EQUAL(vFilter->GetIn(), 0);
    GG_TEST_EQUAL(vFilter->GetOut(), 0);

    vFilter->PushIn(3);
    GG_TEST_EQUAL(vFilter->GetIn(), 3);
    GG_TEST_EQUAL(vFilter->GetOut(), 0);

    vFilter->PushIn(4);
    GG_TEST_EQUAL(vFilter->GetIn(), 4);
    GG_TEST_EQUAL(vFilter->GetOut(), 0);

    vFilter->PushIn(5);
    GG_TEST_EQUAL(vFilter->GetIn(), 5);
    GG_TEST_EQUAL(vFilter->GetOut(), 0);

    vFilter->PushIn(6);
    GG_TEST_EQUAL(vFilter->GetIn(), 6);
    GG_TEST_EQUAL(vFilter->GetOut(), 0);

    vFilter->PushIn(7);
    GG_TEST_EQUAL(vFilter->GetIn(), 7);
    GG_TEST_EQUAL(vFilter->GetOut(), 3);

    vFilter->PushIn(8);
    GG_TEST_EQUAL(vFilter->GetIn(), 8);
    GG_TEST_EQUAL(vFilter->GetOut(), 4);

  }

  ggFilterDelayT<ggInt32> vFilterDelay0(0);
  vFilter = &vFilterDelay0;

  GG_TEST_EQUAL(vFilter->GetIn(), 0);
  GG_TEST_EQUAL(vFilter->GetOut(), 0);

  vFilter->PushIn(-3);
  GG_TEST_EQUAL(vFilter->GetIn(), -3);
  GG_TEST_EQUAL(vFilter->GetOut(), -3);

  vFilter->PushIn(-4);
  GG_TEST_EQUAL(vFilter->GetIn(), -4);
  GG_TEST_EQUAL(vFilter->GetOut(), -4);
}


static void ggFilterDelayTestVector2Float()
{
  ggFilterDelayT<ggVector2Float> vFilterDelay(2);
  ggFilterT<ggVector2Float>& vFilter = vFilterDelay;

  vFilter.PushIn(ggVector2Float(-1,-1));
  GG_TEST_EQUAL(vFilter.GetIn(), ggVector2Float(-1,-1));
  GG_TEST_EQUAL(vFilter.GetOut(), ggVector2Float(0,0));

  vFilter.PushIn(ggVector2Float(-2,-2));
  GG_TEST_EQUAL(vFilter.GetIn(), ggVector2Float(-2,-2));
  GG_TEST_EQUAL(vFilter.GetOut(), ggVector2Float(0,0));

  vFilter.PushIn(ggVector2Float(-3,-3));
  GG_TEST_EQUAL(vFilter.GetIn(), ggVector2Float(-3,-3));
  GG_TEST_EQUAL(vFilter.GetOut(), ggVector2Float(-1,-1));

  vFilter.PushIn(ggVector2Float(-4,-4));
  GG_TEST_EQUAL(vFilter.GetIn(), ggVector2Float(-4,-4));
  GG_TEST_EQUAL(vFilter.GetOut(), ggVector2Float(-2,-2));
}


static void ggFilterDelayTest()
{
  ggFilterDelayTestInt32();
  ggFilterDelayTestVector2Float();
}


GG_TEST_REGISTER(ggFilterDelayTest);
