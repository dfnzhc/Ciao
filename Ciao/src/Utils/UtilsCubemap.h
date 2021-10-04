#pragma once

#include "Bitmap.h"

namespace Ciao
{
	Bitmap convertEquirectangularMapToVerticalCross(const Bitmap& b);
	Bitmap convertVerticalCrossToCubeMapFaces(const Bitmap& b);
}
