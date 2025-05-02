#pragma once

#include <string>
#include <vector>

class ImageObject
{
private:
	int m_nPrevWidth;
	int m_nWidth;
	int m_nPrevHeight;
	int m_nHeight;
public:
	unsigned char* m_pBuffer;
public:
	void ReAllocateByteImage(const unsigned char* pImage);

	bool SetImage(const int& nWidth, const int& nHeight);
	bool SetImage(const unsigned char* pImage, const int& nWidth, const int& nHeight);

	void SetWidth(const int& nWidth);
	int GetWidth() const;

	void SetHeight(const int& nHeight);
	int GetHeight() const;

	void ReleaseObjects();
public:
	ImageObject();
	~ImageObject();
};