#include "pch.h"
#include "CustomImageProcessing.h"

extern bool MGO_CUSTOM::ImageBlur(const ImageObject* src, ImageObject* dst, const int kenelSize)
{
	if (src == nullptr)
	{
		return false;
	}

	if (src->m_pBuffer == nullptr)
	{
		return false;
	}

	const int nWidth = src->GetWidth();
	const int nHeight = src->GetHeight();

	dst->SetWidth(nWidth);
	dst->SetHeight(nHeight);

	if (!dst->m_pBuffer)
	{
		delete[] dst->m_pBuffer;
	}
	dst->m_pBuffer = new unsigned char[nWidth * nHeight];
	memcpy(dst->m_pBuffer, src->m_pBuffer, nWidth * nHeight);

	Blur(src, dst, kenelSize);

	return true;
}

extern void MGO_CUSTOM::Blur(const ImageObject* src, ImageObject* dst, const int kenelSize)
{
	if (!src || !dst)
	{
		return;
	}

	const int nWidth = dst->GetWidth();
	const int nHeight = dst->GetHeight();
	const int nNormalize = static_cast<int>(kenelSize * 0.5f);

	// Seperable Filter Àû¿ë
	BYTE* temp = new BYTE[nWidth * nHeight];

	for (int row = 0; row < nHeight; row++)
	{
		for (int col = 0; col < nWidth; col++)
		{
			int nSum = 0, nCount = 0;

			for (int dCol = -nNormalize; dCol <= nNormalize; dCol++)
			{
				int nColValue = col + dCol;
				if (nColValue >= 0 && nColValue < nWidth)
				{
					nSum += src->m_pBuffer[row * nWidth + nColValue];
					nCount++;
				}
			}

			if (nCount != 0)
			{
				dst->m_pBuffer[row * nWidth + col] = static_cast<unsigned int>(nSum / nCount);
			}
		}
	}

	for (int row = 0; row < nHeight; row++)
	{
		for (int col = 0; col < nWidth; col++)
		{
			int nSum = 0, nCount = 0;

			for (int dRow = -nNormalize; dRow <= nNormalize; dRow++)
			{
				int nRowValue = row + dRow;
				if (nRowValue >= 0 && nRowValue < nHeight)
				{
					nSum += src->m_pBuffer[nRowValue * nWidth + col];
					nCount++;
				}
			}

			if (nCount != 0)
			{
				dst->m_pBuffer[row * nWidth + col] = static_cast<unsigned int>(nSum / nCount);
			}
		}
	}

	delete[] temp;
}