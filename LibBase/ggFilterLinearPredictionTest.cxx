#include <vector>

#include "LibBase/ggUnitTest.h"
#include "LibBase/ggFilterLinearPredictionT.h"
#include "LibBase/ggNumberTypes.h"
#include "LibBase/ggVectorTypes.h"


static void ggFilterLinearPredictionTestDouble()
{
  // filter order 3, look ahead 1
  ggFilterLinearPredictionT<ggDouble> vFilterLinearPrediction(3, 1);
  ggFilterT<ggDouble>& vFilter = vFilterLinearPrediction;

  // run twice in order to test "Reset()"
  for (ggInt32 vCount = 0; vCount < 2; vCount++) {

    vFilter.Reset();

    GG_TEST_EQUAL_FLOAT(vFilter.GetOut(), 0.0);
    GG_TEST_EQUAL_FLOAT(vFilter.Filter(11.0), 11.0);
    GG_TEST_EQUAL_FLOAT(vFilter.Filter( 9.0),  7.0);
    GG_TEST_EQUAL_FLOAT(vFilter.Filter( 7.0),  5.0);
    GG_TEST_EQUAL_FLOAT(vFilter.Filter( 8.0),  7.0);
    GG_TEST_EQUAL_FLOAT(vFilter.Filter( 9.0), 10.0);
    GG_TEST_EQUAL_FLOAT(vFilter.Filter(10.0), 11.0);
    GG_TEST_EQUAL_FLOAT(vFilter.Filter(10.0), 10.667);
    GG_TEST_EQUAL_FLOAT(vFilter.Filter(10.0), 10.0);
  }
}


static void ggFilterLinearPredictionTest()
{
  ggFilterLinearPredictionTestDouble();
}


GG_TEST_REGISTER(ggFilterLinearPredictionTest);
