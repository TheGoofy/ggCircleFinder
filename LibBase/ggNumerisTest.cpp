#include "LibBase/ggUnitTest.h"
#include "LibBase/ggNumerics.h"


namespace ggNumericsTest {

  void TestAbs()
  {
    GG_TEST_EQUAL(ggAbs(0), 0);
    GG_TEST_EQUAL(ggAbs(2), 2);
    GG_TEST_EQUAL(ggAbs(-2), 2);
    GG_TEST_EQUAL(-ggAbs(-2), -2);
    GG_TEST_EQUAL_FLOAT(ggAbs(0.0), 0.0);
    GG_TEST_EQUAL_FLOAT(ggAbs(2.1), 2.1);
    GG_TEST_EQUAL_FLOAT(ggAbs(-2.1), 2.1);
    GG_TEST_EQUAL_FLOAT(-ggAbs(-2.1), -2.1);
  }

  void TestEqual()
  {
    GG_TEST(ggEqual(0.0f, 0.0f, 0));
    GG_TEST(ggEqual(0.0f, 0.0f, 1));
    GG_TEST(ggEqual(0.0f, 0.0f, 2));
    GG_TEST(ggEqual(0.0f, 0.0f, 3));
    GG_TEST(ggEqual(0.0f, 0.0f, 4));
    GG_TEST(ggEqual(0.0f, 0.0f, 5));
    GG_TEST(ggEqual(0.0f, 0.0f, 6));

    GG_TEST(ggEqual(1.0f, 1.0f, 0));
    GG_TEST(ggEqual(1.0f, 1.0f, 1));
    GG_TEST(ggEqual(1.0f, 1.0f, 2));
    GG_TEST(ggEqual(1.0f, 1.0f, 3));
    GG_TEST(ggEqual(1.0f, 1.0f, 4));
    GG_TEST(ggEqual(1.0f, 1.0f, 5));
    GG_TEST(ggEqual(1.0f, 1.0f, 6));

    GG_TEST( ggEqual(-1.0f, -1.0f, 3));
    GG_TEST(!ggEqual( 1.0f, -1.0f, 3));
    GG_TEST(!ggEqual(-1.0f,  1.0f, 3));

    GG_TEST( ggEqual(1.0f, 0.9f, 0));
    GG_TEST(!ggEqual(1.0f, 0.9f, 1));
    GG_TEST( ggEqual(1.0f, 0.99f, 1));
    GG_TEST(!ggEqual(1.0f, 0.99f, 2));
    GG_TEST( ggEqual(1.0f, 0.999f, 2));
    GG_TEST(!ggEqual(1.0f, 0.999f, 3));
    GG_TEST( ggEqual(1.0f, 0.9999f, 3));
    GG_TEST(!ggEqual(1.0f, 0.9999f, 4));
    GG_TEST( ggEqual(1.0f, 0.99999f, 4));
    GG_TEST(!ggEqual(1.0f, 0.99999f, 5));
    GG_TEST( ggEqual(1.0f, 0.999999f, 5));
    GG_TEST(!ggEqual(1.0f, 0.999999f, 6));

    GG_TEST(!ggEqual(1.0f, 0.5f, 0));
    GG_TEST( ggEqual(1.0f, 0.9f, 0));
    GG_TEST(!ggEqual(1.0f, 0.90f, 1));
    GG_TEST( ggEqual(1.0f, 0.91f, 1));
    GG_TEST(!ggEqual(1.0f, 0.990f, 2));
    GG_TEST( ggEqual(1.0f, 0.991f, 2));
    GG_TEST(!ggEqual(1.0f, 0.9990f, 3));
    GG_TEST( ggEqual(1.0f, 0.9991f, 3));

    GG_TEST(!ggEqual(0.001f, 0.0009990f, 3));
    GG_TEST( ggEqual(0.001f, 0.0009991f, 3));
    GG_TEST(!ggEqual(0.01f, 0.009990f, 3));
    GG_TEST( ggEqual(0.01f, 0.009991f, 3));
    GG_TEST(!ggEqual(0.1f, 0.09990f, 3));
    GG_TEST( ggEqual(0.1f, 0.09991f, 3));
    GG_TEST(!ggEqual(1.0f, 0.9990f, 3));
    GG_TEST( ggEqual(1.0f, 0.9991f, 3));
    GG_TEST(!ggEqual(10.0f, 9.990f, 3));
    GG_TEST( ggEqual(10.0f, 9.991f, 3));
    GG_TEST(!ggEqual(100.0f, 99.90f, 3));
    GG_TEST( ggEqual(100.0f, 99.91f, 3));
    GG_TEST(!ggEqual(1000.0f, 999.0f, 3));
    GG_TEST( ggEqual(1000.0f, 999.1f, 3));

    // is this true or false? sqrt(5)*sqrt(5) == 5
    GG_TEST(sqrt(5.0)*sqrt(5.0) != 5.0); // theoretically this test fails, but thanks to numerical issues it succeeds
    GG_TEST( ggEqual(sqrt(5.0)*sqrt(5.0), 5.0,  6)); // comparing the 6 significant digits => the are the same
    GG_TEST( ggEqual(sqrt(5.0)*sqrt(5.0), 5.0, 15)); // 15 significant digits are accurate
    GG_TEST(!ggEqual(sqrt(5.0)*sqrt(5.0), 5.0, 16)); // the numbers differ at the 16th significat digit
  }

  void Execute()
  {
    TestAbs();
    TestEqual();
  }

}


static void ggNumericsTestExecute()
{
  ggNumericsTest::Execute();
}


GG_TEST_REGISTER(ggNumericsTestExecute);
