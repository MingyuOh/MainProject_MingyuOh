#include "pch.h"
#include "ImageObject.h"

void ImageObject::ReAllocateByteImage(const unsigned char* pImage)
{
	if (pImage == nullptr)
	{
		return;
	}

	if (m_pBuffer)
	{
		delete[] m_pBuffer;
		m_pBuffer = nullptr;
	}

	size_t nSize = m_nWidth * m_nHeight;

	m_pBuffer = new unsigned char[nSize];
	memset(m_pBuffer, 0, nSize);
	memcpy(m_pBuffer, pImage, nSize);
}

bool ImageObject::SetImage(const int& nWidth, const int& nHeight)
{
	return false;
}

bool ImageObject::SetImage(const unsigned char* pImage, const int& nWidth, const int& nHeight)
{
	if (pImage == nullptr)
	{
		return false;
	}

	SetWidth(nWidth);
	SetHeight(nHeight);

	size_t nPrevSize = m_nPrevWidth * m_nPrevHeight;
	size_t nSize = nWidth * nHeight;

	if (m_pBuffer)
	{
		if (nPrevSize != nSize)
		{
			ReAllocateByteImage(pImage);
		}
	}
	else
	{
		m_pBuffer = new unsigned char[nSize];
		memset(m_pBuffer, 0, nSize);

		memcpy(m_pBuffer, pImage, nSize);
	}

	return true;
}

void ImageObject::SetWidth(const int& nWidth)
{
	m_nPrevWidth = m_nWidth;
	if (nWidth >= 0)
	{
		m_nWidth = nWidth;
	}
}

int ImageObject::GetWidth() const
{
	return m_nWidth;
}

void ImageObject::SetHeight(const int& nHeight)
{
	m_nPrevHeight = m_nHeight;
	if (nHeight >= 0)
	{
		m_nHeight = nHeight;
	}
}

int ImageObject::GetHeight() const
{
	return m_nHeight;
}

void ImageObject::ReleaseObjects()
{
	if (m_pBuffer)
	{
		delete[] m_pBuffer;
	}
}

ImageObject::ImageObject(): m_nPrevWidth(0), m_nWidth(0), m_nPrevHeight(0), m_nHeight(0), m_pBuffer(nullptr)
{
}

ImageObject::~ImageObject()
{
}