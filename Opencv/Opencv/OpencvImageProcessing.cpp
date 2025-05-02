#include "pch.h"
#include "OpencvImageProcessing.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

extern bool MGO_OPENCV::ImageBlur(const ImageObject* src, ImageObject* dst, const int kenelSize)
{
	if (src == nullptr)
	{
		return false;
	}

	if (src->m_pBuffer == nullptr)
	{
		return false;
	}

	int nWidth = src->GetWidth();
	int nHeight = src->GetHeight();

	cv::Mat matImage(nHeight, nWidth, CV_8UC1, src->m_pBuffer);
	cv::Mat matBlurImage;
	cv::blur(matImage, matBlurImage, cv::Size(kenelSize, kenelSize));

	dst->SetWidth(nWidth);
	dst->SetHeight(nHeight);

	if (!dst->m_pBuffer)
	{
		delete[] dst->m_pBuffer;
	}
	dst->m_pBuffer = new unsigned char[nWidth * nHeight];

	memcpy(dst->m_pBuffer, matBlurImage.data, nWidth * nHeight);

	return true;
}
