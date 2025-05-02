#pragma once
#include "ImageObject.h"

namespace MGO_CUSTOM
{
	extern "C++" __declspec(dllexport) bool ImageBlur(const ImageObject * src, ImageObject * dst, const int kenelSize);
	extern "C++" __declspec(dllexport) void Blur(const ImageObject * src, ImageObject* dst, const int kenelSize);
}