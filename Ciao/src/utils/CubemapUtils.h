#pragma once

#include "Bitmap.h"

namespace Ciao
{
    Bitmap ConvertEquirectangularMapToVerticalCross(const Bitmap& b);
    Bitmap ConvertVerticalCrossToCubeMapFaces(const Bitmap& b);
}
