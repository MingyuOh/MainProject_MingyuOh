#pragma once
#include "ImageObject.h"

namespace MGO_OPENCV
{
	extern "C++" __declspec(dllexport) bool ImageBlur(const ImageObject * src, ImageObject * dst, const int kenelSize);
}