#include "LibBase/ggUnitTest.h"
#include "LibBase/ggVector2T.h"


static void ggVector2Test()
{
  ggVector2T<ggDouble> vVector2Double(3.0, -5.0);
  ggVector2T<ggSize> vVector2Size(vVector2Double.GetConverted<ggSize>());
  GG_TEST_EQUAL(vVector2Size.X(), 3);
  GG_TEST_EQUAL(vVector2Size.Y(), -5);
}


GG_TEST_REGISTER(ggVector2Test);
