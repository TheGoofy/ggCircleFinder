#include <vector>

#include "LibBase/ggUnitTest.h"
#include "LibBase/ggFilterOutlierT.h"
#include "LibBase/ggNumberTypes.h"
#include "LibBase/ggVectorTypes.h"


static void ggFilterOutlierTestInt32()
{
  ggFilterOutlierT<ggInt32> vFilter3(3, 1);
  GG_TEST_EQUAL(vFilter3.Filter(20), 20);
  GG_TEST_EQUAL(vFilter3.Filter(30), 30);
  GG_TEST_EQUAL(vFilter3.Filter(50), 30);
  GG_TEST_EQUAL(vFilter3.Filter(40), 40);
  GG_TEST_EQUAL(vFilter3.Filter(10), 40); // outlier
  GG_TEST_EQUAL(vFilter3.Filter(50), 50);
  GG_TEST_EQUAL(vFilter3.Filter(30), 30);

  ggFilterOutlierT<ggInt32> vFilter5(5, 2);
  GG_TEST_EQUAL(vFilter5.Filter(10), 10);
  GG_TEST_EQUAL(vFilter5.Filter(10), 10);
  GG_TEST_EQUAL(vFilter5.Filter(10), 10);
  GG_TEST_EQUAL(vFilter5.Filter(20), 10);
  GG_TEST_EQUAL(vFilter5.Filter(90), 20);
  GG_TEST_EQUAL(vFilter5.Filter(10), 20);
  GG_TEST_EQUAL(vFilter5.Filter(10), 10);

  vFilter5.Reset();
  GG_TEST_EQUAL(vFilter5.Filter(0), 0);
  GG_TEST_EQUAL(vFilter5.Filter(0), 0);
  GG_TEST_EQUAL(vFilter5.Filter(0), 0); // (*)
  GG_TEST_EQUAL(vFilter5.Filter(10), 0);
  GG_TEST_EQUAL(vFilter5.Filter(-10), 0); // mean is 0 => take value from (*)

  vFilter5.Reset();
  GG_TEST_EQUAL(vFilter5.Filter(-6), -6);
  GG_TEST_EQUAL(vFilter5.Filter(5), 5);
  GG_TEST_EQUAL(vFilter5.Filter(-5), -5); // mean is -2
  GG_TEST_EQUAL(vFilter5.Filter(6), -5); // mean is 0
}


static void ggFilterOutlierTestVector2Float()
{
  // function to calculate distance between two vectors
  auto vDistanceFunc = [] (const ggVector2Float& aVectorA, const ggVector2Float& aVectorB) -> ggFloat {
    return (aVectorB - aVectorA).Length();
  };

  // filter with custom distance function
  ggFilterOutlierT<ggVector2Float, ggFloat> vFilter3(3, 1, vDistanceFunc);

  // playground    ^ y
  //   .   .   .   |   .   .   .
  //   .  (F)  .   |   .  (C)  .
  //   .   .   .   |   .  (D)  .
  // --+---+---+---+---+--(A)--+--> x
  //   .  (B)  .   |   .   .   .
  //   .  (E) (G)  |   .   .   .
  //   .   .  (H)  |   .   .   .
  //               |
  ggVector2Float vVectorA( 2.0f,  0.0f);
  ggVector2Float vVectorB(-2.0f, -1.0f);
  ggVector2Float vVectorC( 2.0f,  2.0f);
  ggVector2Float vVectorD( 2.0f,  1.0f);
  ggVector2Float vVectorE(-2.0f, -2.0f);
  ggVector2Float vVectorF(-2.0f,  2.0f);
  ggVector2Float vVectorG(-1.0f, -2.0f);
  ggVector2Float vVectorH(-1.0f, -3.0f);

  // tests ...
  GG_TEST_EQUAL(vFilter3.Filter(vVectorA), vVectorA);
  GG_TEST_EQUAL(vFilter3.Filter(vVectorB), vVectorB);
  GG_TEST_EQUAL(vFilter3.Filter(vVectorC), vVectorC);
  GG_TEST_EQUAL(vFilter3.Filter(vVectorD), vVectorD);
  GG_TEST_EQUAL(vFilter3.Filter(vVectorE), vVectorD);
  GG_TEST_EQUAL(vFilter3.Filter(vVectorF), vVectorF);
  GG_TEST_EQUAL(vFilter3.Filter(vVectorG), vVectorG);
  GG_TEST_EQUAL(vFilter3.Filter(vVectorH), vVectorG);
}


static void ggFilterOutlierTest()
{
  ggFilterOutlierTestInt32();
  ggFilterOutlierTestVector2Float();
}


GG_TEST_REGISTER(ggFilterOutlierTest);
