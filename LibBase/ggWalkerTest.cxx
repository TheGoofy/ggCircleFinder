#include <vector>

#include "LibBase/ggUnitTest.h"
#include "LibBase/ggWalkerT.h"
#include "LibBase/ggNumberTypes.h"


static void ggWalkerTest()
{
  typedef std::vector<ggInt32> tVector;
  typedef ggWalkerT<tVector::const_iterator> tWalker;

  tVector vVector = {1, 2, 3, 4};
  tWalker vWalker(vVector);

  ggInt32 vSum = 10;
  vWalker.ForEach([&vSum] (ggInt32 aValue) {
    vSum += aValue;
  });

  GG_TEST_EQUAL(vSum, 10 + 1 + 2 + 3 + 4);

  ggInt32 vProduct = -5;
  while (vWalker) {
    vProduct *= *vWalker;
  }

  GG_TEST_EQUAL(vProduct, -5 * 1 * 2 * 3 * 4);
}


static bool ggRegistered = ggUnitTest::Register(ggWalkerTest);
