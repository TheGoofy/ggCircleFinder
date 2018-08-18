#include "LibBase/ggUnitTest.h"
#include "LibBase/ggVectorT.h"


static void ggVectorTest()
{
  ggVectorT<ggUInt32, 3> vVector3UInt32;
  GG_TEST_EQUAL(vVector3UInt32[0], 0);
  GG_TEST_EQUAL(vVector3UInt32[1], 0);
  GG_TEST_EQUAL(vVector3UInt32[2], 0);

  ggVectorT<ggUInt16, 4> vVector4UInt16;
  vVector4UInt16[0] = 2;
  vVector4UInt16[1] = 3;
  vVector4UInt16[2] = 4;
  vVector4UInt16[3] = 1;
  GG_TEST_EQUAL(vVector4UInt16.Min(), 1);
  GG_TEST_EQUAL(vVector4UInt16.Max(), 4);
  GG_TEST_EQUAL(vVector4UInt16.MinDimension(), 3);
  GG_TEST_EQUAL(vVector4UInt16.MaxDimension(), 2);

  ggVectorT<ggInt32, 2> vVector2Int32;
  vVector2Int32[0] = 3;
  vVector2Int32[1] = 4;
  GG_TEST_EQUAL(vVector2Int32[0], 3);
  GG_TEST_EQUAL(vVector2Int32[1], 4);
  GG_TEST_EQUAL(vVector2Int32.Length(), 5);
  vVector2Int32 *= -3;
  GG_TEST_EQUAL(vVector2Int32[0], -9);
  GG_TEST_EQUAL(vVector2Int32[1], -12);
  GG_TEST_EQUAL(vVector2Int32.Length(), 15);
}


GG_TEST_REGISTER(ggVectorTest);
