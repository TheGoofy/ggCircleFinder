#include "LibBase/ggUnitTest.h"
#include "LibBase/ggRound.h"


static void ggRoundTest()
{
  GG_TEST_EQUAL(ggRound<ggInt8>(-200), -128);
  GG_TEST_EQUAL(ggRound<ggInt8>(-63), -63);
  GG_TEST_EQUAL(ggRound<ggInt8>(63), 63);
  GG_TEST_EQUAL(ggRound<ggInt8>(200), 127);
  GG_TEST_EQUAL(ggRound<ggUInt8>(-100), 0);
  GG_TEST_EQUAL(ggRound<ggUInt8>(100), 100);
  GG_TEST_EQUAL(ggRound<ggUInt8>(300), 255);
  GG_TEST_EQUAL(ggRound<ggInt16>(2.4), 2);
  GG_TEST_EQUAL(ggRound<ggInt16>(1.5), 2);
  GG_TEST_EQUAL(ggRound<ggInt16>(1.1), 1);
  GG_TEST_EQUAL(ggRound<ggInt16>(0.6), 1);
  GG_TEST_EQUAL(ggRound<ggInt16>(0.5), 1);
  GG_TEST_EQUAL(ggRound<ggInt16>(0.4), 0);
  GG_TEST_EQUAL(ggRound<ggInt16>(0.1), 0);
  GG_TEST_EQUAL(ggRound<ggInt16>(0.0), 0);
  GG_TEST_EQUAL(ggRound<ggInt16>(-0.1), 0);
  GG_TEST_EQUAL(ggRound<ggInt16>(-0.4), 0);
  GG_TEST_EQUAL(ggRound<ggInt16>(-0.5), -1);
  GG_TEST_EQUAL(ggRound<ggInt16>(-0.6), -1);
  GG_TEST_EQUAL(ggRound<ggInt16>(-1.1), -1);
  GG_TEST_EQUAL(ggRound<ggInt16>(-1.5), -2);
  GG_TEST_EQUAL(ggRound<ggInt16>(-2.4), -2);
  GG_TEST_EQUAL(ggRound<ggInt16>(-33000.0f), -32768);
  GG_TEST_EQUAL(ggRound<ggInt16>(-33000), -32768);
  GG_TEST_EQUAL(ggRound<ggInt16>(-22000), -22000);
  GG_TEST_EQUAL(ggRound<ggInt16>(22000), 22000);
  GG_TEST_EQUAL(ggRound<ggInt16>(33000), 32767);
  GG_TEST_EQUAL(ggRound<ggInt16>(33000.0f), 32767);
  GG_TEST_EQUAL(ggRound<ggUInt16>(-33000), 0);
  GG_TEST_EQUAL(ggRound<ggUInt16>(33000), 33000);
  GG_TEST_EQUAL(ggRound<ggUInt16>(66000), 65535);
  GG_TEST_EQUAL(ggRound<ggUInt16>(-10.1f), 0);
  GG_TEST_EQUAL(ggRound<ggFloat>(-0.62), static_cast<ggFloat>(-0.62));
  GG_TEST_EQUAL(ggRound<ggDouble>(3.14f), static_cast<ggDouble>(3.14f));
}


static bool ggRegistered = ggUnitTest::Register(ggRoundTest);
