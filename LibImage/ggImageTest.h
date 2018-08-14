#ifndef GGIMAGETEST_H
#define GGIMAGETEST_H

#include <iostream>
#include "LibBase/ggUtility.h"
#include "LibImage/ggImageT.h"

namespace ggImageTest
{

  template <typename TValueType>
  void Print(const ggImageT<TValueType>& aImage) {
    std::cout << "size x = " << aImage.GetSizeX() << std::endl;
    std::cout << "size y = " << aImage.GetSizeY() << std::endl;
    std::cout << "number type = " << ggUtility::ToString(ggGetNumberType<TValueType>()) << std::endl;
    for (ggSize vIndexY = 0; vIndexY < aImage.GetSizeY(); vIndexY++) {
      for (ggSize vIndexX = 0; vIndexX < aImage.GetSizeX(); vIndexX++) {
        std::cout << ggUtility::ToString(aImage(vIndexX, vIndexY));
        std::cout << (vIndexX + 1 < aImage.GetSizeX() ? " " : "\n");
      }
    }
    std::cout << std::flush;
  }

}

#endif // GGIMAGETEST_H
