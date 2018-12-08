#ifndef GGAVERAGESTEST_H
#define GGAVERAGESTEST_H

#include "LibBase/ggAveragesT.h"

namespace ggAveragesTest
{
  template <typename TValueType>
  void Print(const ggAveragesT<TValueType>& aAverages,
             const char* aHeaderText = nullptr);

  void Execute();
}

#endif // GGAVERAGESTEST_H
