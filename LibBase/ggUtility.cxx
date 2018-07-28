// 0) include own header
#include "ggUtility.h"

// 1) include system
#include <math.h>
#include <vector>

// 2) include own project-related (sort by component dependency)
#include "LibBase/ggWalkerT.h"


std::vector<ggColorUInt8> ggUtility::ColorTableHot()
{
  std::vector<ggColorUInt8> vColorTable(256);
  for (ggUSize vIndex = 0; vIndex < 256; vIndex++) {
    ggUInt8 vI = static_cast<ggUInt8>(vIndex);
    if      (vIndex <   4) vColorTable[vIndex].Set(         2*vI,         0,       8*vI, 64*vI); //  4 steps: black => dark blue (transparent => opaque)
    else if (vIndex <  32) vColorTable[vIndex].Set(         2*vI,         0,       8*vI,   255); // 28 steps: dark blue => blue
    else if (vIndex <  64) vColorTable[vIndex].Set(         2*vI,         0,        255,   255); // 32 steps: blue => blue purple
    else if (vIndex <  96) vColorTable[vIndex].Set(3*(vI-64)+128,         0, 4*(127-vI),   255); // 32 steps: blue purple => magenta
    else if (vIndex < 128) vColorTable[vIndex].Set(1*(vI-96)+224, 2*(vI-96), 4*(127-vI),   255); // 32 steps: magenta => light red
    else if (vIndex < 160) vColorTable[vIndex].Set(          255, 2*(vI-96),          0,   255); // 32 steps: light red => orange
    else if (vIndex < 192) vColorTable[vIndex].Set(          255, 2*(vI-96),          0,   255); // 32 steps: orange => orange yellow
    else if (vIndex < 224) vColorTable[vIndex].Set(          255, 2*(vI-96), 4*(vI-192),   255); // 32 steps: orange yellow => light yellow
    else                   vColorTable[vIndex].Set(          255,       255, 4*(vI-192),   255); // 32 steps: light yellow => white
  }
  return vColorTable;
}


std::vector<ggColorUInt8> ggUtility::ColorTableRainbow(bool aIndexZeroTransparent)
{
  std::vector<ggColorUInt8> vColorTable(256);
  for (ggUSize vIndex = 0; vIndex < 256; vIndex++) {
    ggUInt8 vI = static_cast<ggUInt8>(vIndex);
    if      (vIndex <  50) vColorTable[vIndex].Set(           255,              0, 250-5*(vI-0), 255); // 50 steps (250..005): purple => red
    else if (vIndex < 100) vColorTable[vIndex].Set(           255,    5+5*(vI-50),            0, 255); // 50 steps (005..250): red => yellow
    else if (vIndex < 128) vColorTable[vIndex].Set(249-9*(vI-100),            255,            0, 255); // 28 steps (249..006): yellow => green
    else if (vIndex < 156) vColorTable[vIndex].Set(             0,            255, 6+9*(vI-128), 255); // 28 steps (006..249): green => cyan
    else if (vIndex < 206) vColorTable[vIndex].Set(             0, 250-5*(vI-156),          255, 255); // 50 steps (250..005): cyan => blue
    else                   vColorTable[vIndex].Set(  5+5*(vI-206),              0,          255, 255); // 50 steps (005..250): blue => purple
  }
  if (aIndexZeroTransparent) vColorTable[0].SetA(0);
  return vColorTable;
}


std::vector<ggColorUInt8> ggUtility::ColorTableRandomRainbow(bool aIndexZeroTransparent)
{
  // use rainbow table, pick random color and swap it with the last color
  std::vector<ggColorUInt8> vColorTable(ColorTableRainbow(false));
  for (ggUSize vIndex = 1; vIndex < vColorTable.size(); vIndex++) {
    ggUSize vIndexLast = vColorTable.size() - vIndex;
    ggUSize vIndexRand = static_cast<ggUSize>(rand()) % vIndexLast;
    std::swap(vColorTable[vIndexRand], vColorTable[vIndexLast]);
  }
  if (aIndexZeroTransparent) vColorTable[0].SetA(0);
  return vColorTable;
}


std::vector<ggColorUInt8> ggUtility::ColorTableRandomHot(bool aIndexZeroTransparent)
{
  std::vector<ggColorUInt8> vColorTable(256);
  for (ggUSize vIndex = 0; vIndex < 256; vIndex++) {
    vColorTable[vIndex].Set(155 + rand() % 100,
                            100 + rand() % 100,
                            0 + rand() % 100,
                            255);
  }
  if (aIndexZeroTransparent) vColorTable[0].SetA(0);
  return vColorTable;
}


std::vector<ggColorUInt8> ggUtility::ColorTableRandomCold(bool aIndexZeroTransparent)
{
  std::vector<ggColorUInt8> vColorTable(256);
  for (ggUSize vIndex = 0; vIndex < 256; vIndex++) {
    vColorTable[vIndex].Set(0 + rand() % 100,
                            100 + rand() % 100,
                            155 + rand() % 100,
                            255);
  }
  if (aIndexZeroTransparent) vColorTable[0].SetA(0);
  return vColorTable;
}
