
// MainProject_MingyuOhDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MainProject_MingyuOh.h"
#include "MainProject_MingyuOhDlg.h"
#include "afxdialogex.h"

#include <psapi.h>

#include "../../output/Include/OpencvImageProcessing.h"
#include "../../output/Include/CustomImageProcessing.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainProjectMingyuOhDlg 대화 상자



CMainProjectMingyuOhDlg::CMainProjectMingyuOhDlg(CWnd* pParent /*=nullptr*/)
	: m_nKernelSize(21),
	m_LoadImagePath(_T("")),
	m_SaveImagePath(_T("")),
	m_nCurrentLoadImageCount(0),
	CDialogEx(IDD_MAINPROJECT_MINGYUOH_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMainProjectMingyuOhDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMainProjectMingyuOhDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CMainProjectMingyuOhDlg::OnBnClickedButtonLoad)
END_MESSAGE_MAP()


// CMainProjectMingyuOhDlg 메시지 처리기

void CMainProjectMingyuOhDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CMainProjectMingyuOhDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMainProjectMingyuOhDlg::OnDestroy()
{
	for (int i = 0; i < MAX_IMAGE_COUNT; i++)
	{
		m_SrcImageObjects[i].ReleaseObjects();
		m_DstOpencvImageObjects[i].ReleaseObjects();
		m_DstCustomImageObjects[i].ReleaseObjects();
	}
	m_ImageFilePaths.clear();
	m_hashPictureControlID.clear();
	m_ResultData.Release();
	ClearDisplayImages();

	ReleaseLog();

	m_ThreadPool.StopThreads();

	CDialogEx::OnDestroy();
}

BOOL CMainProjectMingyuOhDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	InitGeneralFunctions();

	LoadSettingIniFileData();

	InitObject();
	InitLog();
	InitThreadPool();

	return TRUE;
}

void CMainProjectMingyuOhDlg::OnBnClickedButtonLoad()
{
	LoadOrgImage();
	StartImageProcessing();
}

void CMainProjectMingyuOhDlg::InitObject()
{
	m_SrcImageObjects.resize(MAX_IMAGE_COUNT);
	m_DstOpencvImageObjects.resize(MAX_IMAGE_COUNT);
	m_DstCustomImageObjects.resize(MAX_IMAGE_COUNT);
	m_ImageFilePaths.resize(MAX_IMAGE_COUNT);

	for (int i = 0; i < MAX_IMAGE_COUNT; i++)
	{
		m_hashDisplayImages.insert(std::make_pair(i, std::vector<MatEx>()));
	}

	SetPictureControlIDInfo();
}

void CMainProjectMingyuOhDlg::InitLog()
{
	m_pLogBuffer = LogBuffer::GetInstance();

	m_pLogBuffer->Init();
	m_pLogBuffer->Run();

	WriteLog(LogFileName_ImageBlur, "Program start!", eLogInfoType::INFO_TYPE);
}

void CMainProjectMingyuOhDlg::InitThreadPool()
{
	m_ThreadPool.InitThread(MAX_IMAGE_COUNT);
	m_ThreadPool.StartThreads();
}

void CMainProjectMingyuOhDlg::InitGeneralFunctions()
{
	m_pGeneralFunctions = GeneralFunctions::GetInstance();
}

void CMainProjectMingyuOhDlg::LoadSettingIniFileData()
{
	if (m_pGeneralFunctions)
	{
		std::string settingDirPath = m_pGeneralFunctions->g_SettingDirPath;
		std::string settingFilePath = m_pGeneralFunctions->g_SettingFilePath;

		if (m_pGeneralFunctions->DoesFolderExist(settingDirPath) == false)
		{
			m_pGeneralFunctions->CreateAllFolder(settingDirPath);
			if (m_pGeneralFunctions->DoesFileExist(settingFilePath) == false)
			{
				m_pGeneralFunctions->CreateIniFile(settingFilePath);
				SaveSettingIniFileData();
			}
		}

		std::string value;
		value = m_pGeneralFunctions->ReadIniValue(InitIniSection, KeyKernelValue, settingFilePath);
		m_nKernelSize = std::stoi(value);
		if (m_nKernelSize < MIN_KERNEL_SIZE)
		{
			m_nKernelSize = MIN_KERNEL_SIZE;
		}

		value = m_pGeneralFunctions->ReadIniValue(InitIniSection, KeyLoadImagePath, settingFilePath);
		m_pGeneralFunctions->g_LoadImageDirPath = value;
		m_LoadImagePath = CString(value.c_str());

		value = m_pGeneralFunctions->ReadIniValue(InitIniSection, KeySaveImagePath, settingFilePath);
		m_pGeneralFunctions->g_SaveImageDirPath = value;
		m_SaveImagePath = CString(value.c_str());
		if (m_pGeneralFunctions->DoesFolderExist(value) == false)
		{
			m_pGeneralFunctions->CreateAllFolder(value);
		}
	}
}

void CMainProjectMingyuOhDlg::SaveSettingIniFileData()
{
	if (m_pGeneralFunctions)
	{
		std::string settingDirPath = m_pGeneralFunctions->g_SettingDirPath;
		std::string settingFilePath = m_pGeneralFunctions->g_SettingFilePath;
		std::string loadImageDirPath = m_pGeneralFunctions->g_LoadImageDirPath;
		std::string saveImageDirPath = m_pGeneralFunctions->g_SaveImageDirPath;

		if (m_pGeneralFunctions->DoesFolderExist(settingDirPath) == false)
		{
			m_pGeneralFunctions->CreateAllFolder(settingDirPath);
			if (m_pGeneralFunctions->DoesFileExist(settingFilePath) == false)
			{
				m_pGeneralFunctions->CreateIniFile(settingFilePath);
			}
		}

		std::string value;
		value = std::to_string(m_nKernelSize);
		m_pGeneralFunctions->WriteIniValue(InitIniSection, KeyKernelValue, value, settingFilePath);

		value = loadImageDirPath;
		m_LoadImagePath = loadImageDirPath.c_str();
		m_pGeneralFunctions->WriteIniValue(InitIniSection, KeyLoadImagePath, value, settingFilePath);

		value = saveImageDirPath;
		m_SaveImagePath = CString(saveImageDirPath.c_str());
		m_pGeneralFunctions->WriteIniValue(InitIniSection, KeySaveImagePath, value, settingFilePath);
		if (m_pGeneralFunctions->DoesFolderExist(saveImageDirPath) == false)
		{
			m_pGeneralFunctions->CreateAllFolder(saveImageDirPath);
		}
	}
}

void CMainProjectMingyuOhDlg::LoadOrgImage()
{
	ClearData();

	CFileDialog fileDlg(TRUE,
		_T("Open Image"),
		NULL,
		OFN_ALLOWMULTISELECT | OFN_EXPLORER,
		_T("Image Files (*.bmp;*.jpg;*.png)|*.bmp;*.jpg;*.png|All Files (*.*)|*.*||"));

	fileDlg.m_ofn.lpstrInitialDir = static_cast<LPCWSTR>(m_LoadImagePath);

	if (fileDlg.DoModal() == IDOK)
	{
		std::string logMsg;
		logMsg = "LoadOrgImage()! -- Start";
		WriteLog(LogFileName_ImageBlur, logMsg, eLogInfoType::INFO_TYPE);
		logMsg.clear();

		int index = 0;
		POSITION pos = fileDlg.GetStartPosition();

		// 파일 경로
		while (pos)
		{
			if (index >= MAX_IMAGE_COUNT)
			{
				break;
			}

			m_ImageFilePaths[index].Format(_T("%s"), fileDlg.GetNextPathName(pos));
			index++;
		}
		m_nCurrentLoadImageCount = index;

		// 이미지 설정
		for (int i = 0; i < m_nCurrentLoadImageCount; i++)
		{
			if (m_ImageFilePaths[i].IsEmpty() == true)
			{
				continue;
			}

			CT2CA tempPath(m_ImageFilePaths[i]);
			std::string convertPath(tempPath);
			cv::Mat loadImage = cv::imread(convertPath, cv::IMREAD_UNCHANGED);

			// 이미지 로드 성공 여부
			if (loadImage.empty() == true)
			{
				logMsg = m_pGeneralFunctions->StringFormat("Failed load image(Index: %d): %s", i, convertPath.c_str());
				WriteLog(LogFileName_ImageBlur, logMsg, eLogInfoType::ERROR_TYPE);
				logMsg.clear();
				continue;
			}

			logMsg = m_pGeneralFunctions->StringFormat("Success load image(Index: %d): %s", i, convertPath.c_str());
			WriteLog(LogFileName_ImageBlur, logMsg, eLogInfoType::INFO_TYPE);
			logMsg.clear();

			// 흑백 이미지 체크
			if (loadImage.channels() == 1)
			{
				// Source 이미지 오브젝트 저장
				if (m_SrcImageObjects[i].SetImage(loadImage.data, loadImage.cols, loadImage.rows) == false)
				{
					logMsg = m_pGeneralFunctions->StringFormat("Failed set image(Index: %d): %s", i, convertPath.c_str());
					WriteLog(LogFileName_ImageBlur, logMsg, eLogInfoType::ERROR_TYPE);
					logMsg.clear();
					continue;
				}
				logMsg = m_pGeneralFunctions->StringFormat("Success set image(Index: %d): %s", i, convertPath.c_str());
				WriteLog(LogFileName_ImageBlur, logMsg, eLogInfoType::INFO_TYPE);
				logMsg.clear();
			}
			else
			{
				logMsg = m_pGeneralFunctions->StringFormat("The loaded image is not a gray image(Index: %d): %s", i, convertPath.c_str());
				WriteLog(LogFileName_ImageBlur, logMsg, eLogInfoType::ERROR_TYPE);
				logMsg.clear();
				AfxMessageBox(CString(logMsg.c_str()));
				continue;
			}
		}

		logMsg = "LoadOrgImage() -- End";
		WriteLog(LogFileName_ImageBlur, logMsg, eLogInfoType::INFO_TYPE);
		logMsg.clear();
	}
}

void CMainProjectMingyuOhDlg::StartImageProcessing()
{
	WriteLog(LogFileName_ImageBlur, "StartImageProcessing() -- Start", eLogInfoType::INFO_TYPE);
	for (int i = 0; i < m_nCurrentLoadImageCount; i++)
	{
		if (!m_SrcImageObjects[i].m_pBuffer)
		{
			continue;
		}

		m_ThreadPool.EnqueueTask([this, index = i]
			{
				std::string logMsg = this->m_pGeneralFunctions->StringFormat("Opencv dll image blur(Index: %d) -- Start", index);
				this->WriteLog(LogFileName_ImageBlur, logMsg, eLogInfoType::INFO_TYPE);
				StartImageProcessTime(index, eImageProcessingType::OPENCV);
				StartMemoryUsage(index, eImageProcessingType::OPENCV);
				if (MGO_OPENCV::ImageBlur(&this->m_SrcImageObjects[index], &this->m_DstOpencvImageObjects[index], this->m_nKernelSize) == false)
				{
					this->WriteLog(LogFileName_ImageBlur, "Failed Opencv dll version ImageBlur()", eLogInfoType::ERROR_TYPE);
				}
				EndMemoryUsage(index, eImageProcessingType::OPENCV);
				EndImageProcessTime(index, eImageProcessingType::OPENCV);
				logMsg = this->m_pGeneralFunctions->StringFormat("Opencv dll image blur(Index: %d) -- End", index);
				this->WriteLog(LogFileName_ImageBlur, logMsg, eLogInfoType::INFO_TYPE);


				logMsg = this->m_pGeneralFunctions->StringFormat("Custom dll image blur(Index: %d) -- Start", index);
				this->WriteLog(LogFileName_ImageBlur, logMsg, eLogInfoType::INFO_TYPE);
				StartImageProcessTime(index, eImageProcessingType::CUSTOM);
				StartMemoryUsage(index, eImageProcessingType::CUSTOM);
				if (MGO_CUSTOM::ImageBlur(&this->m_SrcImageObjects[index], &this->m_DstCustomImageObjects[index], this->m_nKernelSize) == false)
				{
					this->WriteLog(LogFileName_ImageBlur, "Failed Custom dll version ImageBlur()", eLogInfoType::ERROR_TYPE);
				}
				EndMemoryUsage(index, eImageProcessingType::CUSTOM);
				EndImageProcessTime(index, eImageProcessingType::CUSTOM);
				logMsg = this->m_pGeneralFunctions->StringFormat("Custom dll image blur(Index: %d) -- End", index);
				this->WriteLog(LogFileName_ImageBlur, logMsg, eLogInfoType::INFO_TYPE);

				this->DisplayProcessingImage(index);

				this->CompareResultOfBlurAlogrithm(index);

				this->SaveImageProcessingResult(index);
			});
	}
	WriteLog(LogFileName_ImageBlur, "StartImageProcessing() -- End", eLogInfoType::INFO_TYPE);
}

void CMainProjectMingyuOhDlg::DisplayProcessingImage(const int& index)
{
	SetImageToPictureControl(&m_SrcImageObjects[index], index, ORG_TEXT);
	SetImageToPictureControl(&m_DstOpencvImageObjects[index], index, OPENCV_TEXT);
	SetImageToPictureControl(&m_DstCustomImageObjects[index], index, CUSTOM_TEXT);
}

void CMainProjectMingyuOhDlg::SetImageToPictureControl(const ImageObject* imageObject, const int& index, const std::string& textType)
{
	if (!imageObject)
	{
		return;
	}

	if (!imageObject->m_pBuffer)
	{
		return;
	}

	const int nWidth = imageObject->GetWidth();
	const int nHeight = imageObject->GetHeight();

	MatEx matImageEx;
	matImageEx.matImage = cv::Mat(nHeight, nWidth, CV_8UC1, imageObject->m_pBuffer);

	if (m_pGeneralFunctions)
	{
		int nImageProcessingType = m_pGeneralFunctions->GetEnumForText(ImageProcessingTypeList, textType);
		matImageEx.imageProcessingType = static_cast<eImageProcessingType>(nImageProcessingType);
	}

	std::unordered_map<int, std::vector<MatEx>>::iterator iterImage = m_hashDisplayImages.find(index);
	if (iterImage != m_hashDisplayImages.end())
	{
		iterImage->second.push_back(matImageEx);
	}

	CImage image;
	image.Create(matImageEx.matImage.cols, matImageEx.matImage.rows, 8);

	RGBQUAD palette[SIZE_DEFUALT];
	for (int i = 0; i < SIZE_DEFUALT; ++i)
	{
		palette[i].rgbRed = i;
		palette[i].rgbGreen = i;
		palette[i].rgbBlue = i;
		palette[i].rgbReserved = 0;
	}
	SetDIBColorTable(image.GetDC(), 0, SIZE_DEFUALT, palette);
	image.ReleaseDC();

	for (int row = 0; row < matImageEx.matImage.rows; row++)
	{
		memcpy((BYTE*)image.GetBits() + (row * image.GetPitch()), matImageEx.matImage.ptr(row), matImageEx.matImage.cols);
	}

	int nID = -1;
	std::unordered_map<std::string, std::vector<int>>::iterator iter = m_hashPictureControlID.find(textType);
	if (iter != m_hashPictureControlID.end())
	{
		nID = iter->second[index];
		if (nID != -1)
		{
			CStatic* pStatic = (CStatic*)GetDlgItem(nID);
			if (pStatic)
			{
				CClientDC dc(pStatic);
				CRect rect;
				pStatic->GetClientRect(&rect);
				image.StretchBlt(dc.GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), SRCCOPY);
			}
		}
	}
}

void CMainProjectMingyuOhDlg::CompareResultOfBlurAlogrithm(const int& index)
{
	std::vector<MatEx> opencvMatImages, customMatImages;
	std::unordered_map<int, std::vector<MatEx>>::iterator iter = m_hashDisplayImages.find(index);
	if (iter != m_hashDisplayImages.end())
	{
		opencvMatImages = iter->second;
	}

	iter = m_hashDisplayImages.find(static_cast<int>(index));
	if (iter != m_hashDisplayImages.end())
	{
		customMatImages = iter->second;
	}

	cv::Mat matOpencvHist, matCustomHist;

	int nHistSize = 256;
	float fRange[] = { 0, 256 };
	const float* fHistRanges = { fRange };

	cv::calcHist(&opencvMatImages[index].matImage, 1, 0, cv::Mat(), matOpencvHist, 1, &nHistSize, &fHistRanges);
	cv::calcHist(&customMatImages[index].matImage, 1, 0, cv::Mat(), matCustomHist, 1, &nHistSize, &fHistRanges);

	cv::normalize(matOpencvHist, matOpencvHist, 0, 1, cv::NORM_MINMAX);
	cv::normalize(matCustomHist, matCustomHist, 0, 1, cv::NORM_MINMAX);

	double fMatchPoint = cv::compareHist(matOpencvHist, matCustomHist, cv::HISTCMP_CORREL);
	m_ResultData.matchList[index] = (fMatchPoint > 0.9f) ? true : false;
}

void CMainProjectMingyuOhDlg::SaveImageProcessingResult(const int& index)
{
	SaveProcessingImage(index);
	SaveResultData(index);
}

void CMainProjectMingyuOhDlg::SaveProcessingImage(const int& index)
{
	if (m_hashDisplayImages.empty() == true)
	{
		return;
	}

	if (m_hashDisplayImages[index].empty() == true)
	{
		return;
	}

	for (int i = 0; i < m_hashDisplayImages[index].size(); i++)
	{
		MatEx matImageEx = m_hashDisplayImages[index][i];
		if (matImageEx.matImage.empty() == true)
		{
			return;
		}

		if (matImageEx.imageProcessingType == eImageProcessingType::ORG)
		{
			continue;
		}

		std::string saveImageDirPath = CT2CA(m_SaveImagePath);
		if (m_pGeneralFunctions->DoesFolderExist(saveImageDirPath) == false)
		{
			m_pGeneralFunctions->CreateAllFolder(saveImageDirPath);
		}

		std::string imageFileName = CreateSaveImageFileName(matImageEx.imageProcessingType, index);
		std::string imageFilePath = saveImageDirPath + imageFileName;
		if (cv::imwrite(imageFilePath, matImageEx.matImage) == false)
		{
			std::string errorMsg = m_pGeneralFunctions->StringFormat("Failed save image(Index: %d): %s", index, imageFilePath.c_str());
			WriteLog(LogFileName_ImageBlur, errorMsg, eLogInfoType::ERROR_TYPE);
		}

		std::string logMsg = m_pGeneralFunctions->StringFormat("Success save image(Index: %d): %s", index, imageFilePath.c_str());
		WriteLog(LogFileName_ImageBlur, logMsg, eLogInfoType::INFO_TYPE);
	}
}

void CMainProjectMingyuOhDlg::SaveResultData(const int& index)
{
	if (m_pGeneralFunctions)
	{
		std::string filePath = m_pGeneralFunctions->g_ResultFilePath;
		if (m_pGeneralFunctions->DoesFileExist(filePath) == false)
		{
			std::string resultHeader = m_pGeneralFunctions->CreateDataLine(ResultDataHeaderList);
			m_pGeneralFunctions->CreateCustomFile(filePath, resultHeader);
		}
		
		std::string msg, time, opencvProcessingTime, customProcessingTime, opencvMemoryUsage, customMemoryUsage, match;
		std::vector<std::string> dataList;
		
		m_pGeneralFunctions->GetDateTime(time);
		dataList.push_back(time);

		opencvProcessingTime = std::to_string(m_ResultData.opencvImageBlurProcessTimes[index].elapsedTime.count()) + "ms";
		dataList.push_back(opencvProcessingTime);

		customProcessingTime = std::to_string(m_ResultData.customImageBlurProcessTimes[index].elapsedTime.count()) + "ms";
		dataList.push_back(customProcessingTime);

		opencvMemoryUsage = std::to_string(m_ResultData.opencvImageBlurMemoryUsages[index].elapsedMemoryUsage) + "kb";
		dataList.push_back(opencvMemoryUsage);

		customMemoryUsage = std::to_string(m_ResultData.customImageBlurMemoryUsages[index].elapsedMemoryUsage) + "kb";
		dataList.push_back(customMemoryUsage);

		match = (m_ResultData.matchList[index] == true) ? "Correct" : "Incorrect";
		dataList.push_back(match);

		std::string data = m_pGeneralFunctions->CreateDataLine(dataList);
		m_pGeneralFunctions->WriteDataInFile(filePath, data);
	}
}

void CMainProjectMingyuOhDlg::SetPictureControlIDInfo()
{
	std::vector<int> ORG_PICTURE_ID_LIST = { IDC_STATIC_PICTURE_ORIGIN1, IDC_STATIC_PICTURE_ORIGIN2, IDC_STATIC_PICTURE_ORIGIN3 };
	m_hashPictureControlID.insert(std::make_pair(ORG_TEXT, ORG_PICTURE_ID_LIST));

	std::vector<int> OPENCV_PICTURE_ID_LIST = { IDC_STATIC_PICTURE_OPENCV1, IDC_STATIC_PICTURE_OPENCV2, IDC_STATIC_PICTURE_OPENCV3 };
	m_hashPictureControlID.insert(std::make_pair(OPENCV_TEXT, OPENCV_PICTURE_ID_LIST));

	std::vector<int> CUSTOM_PICTURE_ID_LIST = { IDC_STATIC_PICTURE_CUSTOM1, IDC_STATIC_PICTURE_CUSTOM2, IDC_STATIC_PICTURE_CUSTOM3 };
	m_hashPictureControlID.insert(std::make_pair(CUSTOM_TEXT, CUSTOM_PICTURE_ID_LIST));
}

void CMainProjectMingyuOhDlg::ClearData()
{
	ClearResultData();
	ClearImages();
	ClearPictureControls();
	ClearDisplayImages();
}

void CMainProjectMingyuOhDlg::ClearResultData()
{
	m_ResultData.Clear();
}

void CMainProjectMingyuOhDlg::ClearPictureControls()
{
	std::unordered_map<std::string, std::vector<int>>::iterator iter = m_hashPictureControlID.find(ORG_TEXT);
	if (iter != m_hashPictureControlID.end())
	{
		for (int i = 0; i < iter->second.size(); i++)
		{
			int nID = iter->second[i];
			CStatic* pStatic = (CStatic*)GetDlgItem(nID);
			if (pStatic)
			{
				CClientDC dc(pStatic);
				CRect rect;
				pStatic->GetClientRect(&rect);

				dc.FillSolidRect(&rect, RGB(MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE));
			}
		}
	}

	iter = m_hashPictureControlID.find(OPENCV_TEXT);
	if (iter != m_hashPictureControlID.end())
	{
		for (int i = 0; i < iter->second.size(); i++)
		{
			int nID = iter->second[i];
			CStatic* pStatic = (CStatic*)GetDlgItem(nID);
			if (pStatic)
			{
				CClientDC dc(pStatic);
				CRect rect;
				pStatic->GetClientRect(&rect);

				dc.FillSolidRect(&rect, RGB(MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE));
			}
		}
	}

	iter = m_hashPictureControlID.find(CUSTOM_TEXT);
	if (iter != m_hashPictureControlID.end())
	{
		for (int i = 0; i < iter->second.size(); i++)
		{
			int nID = iter->second[i];
			CStatic* pStatic = (CStatic*)GetDlgItem(nID);
			if (pStatic)
			{
				CClientDC dc(pStatic);
				CRect rect;
				pStatic->GetClientRect(&rect);

				dc.FillSolidRect(&rect, RGB(MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE));
			}
		}
	}
}

void CMainProjectMingyuOhDlg::ClearDisplayImages()
{
	for (auto& data : m_hashDisplayImages)
	{
		data.second.clear();
	}
}

void CMainProjectMingyuOhDlg::ClearImages()
{
	m_nCurrentLoadImageCount = 0;
	
	for (int i = 0; i < MAX_IMAGE_COUNT; i++)
	{
		if (m_SrcImageObjects[i].m_pBuffer)
		{
			delete[] m_SrcImageObjects[i].m_pBuffer;
			m_SrcImageObjects[i].m_pBuffer = nullptr;
		}

		if (m_DstOpencvImageObjects[i].m_pBuffer)
		{
			delete[] m_DstOpencvImageObjects[i].m_pBuffer;
			m_DstOpencvImageObjects[i].m_pBuffer = nullptr;
		}

		if (m_DstCustomImageObjects[i].m_pBuffer)
		{
			delete[] m_DstCustomImageObjects[i].m_pBuffer;
			m_DstCustomImageObjects[i].m_pBuffer = nullptr;
		}
	}
}

std::string CMainProjectMingyuOhDlg::CreateSaveImageFileName(const eImageProcessingType& imageProcessingType, const int& index, const std::string& ext)
{
	std::string fileName;
	if (m_pGeneralFunctions)
	{
		fileName = std::to_string(index) + UNDER_BAR;

		switch (imageProcessingType)
		{
			case eImageProcessingType::ORG:
			{
				fileName += ORG_TEXT + UNDER_BAR;
			}break;
			case eImageProcessingType::OPENCV:
			{
				fileName += OPENCV_TEXT + UNDER_BAR;
			}break;
			case eImageProcessingType::CUSTOM:
			{
				fileName += CUSTOM_TEXT + UNDER_BAR;
			}break;
		}

		std::string time;
		m_pGeneralFunctions->GetTimePath(time);

		fileName += time + ext;
	}

	return fileName;
}

void CMainProjectMingyuOhDlg::WriteLog(const std::string& logFileName, const std::string& msg, const eLogInfoType& logInfoType)
{
	if (m_pGeneralFunctions)
	{
		std::string time, data, logInfoTypeText;

		m_pGeneralFunctions->GetDateTimePath(time);
		logInfoTypeText = m_pGeneralFunctions->GetTextForEnum(logInfoTypeList, static_cast<int>(logInfoType));

		data = time + SPACE + logInfoTypeText + msg;
		m_pGeneralFunctions->CheckAndAddLastCharacter(data, LINE_BREAK);

		m_pLogBuffer->Producer(logFileName, data);
	}
}

void CMainProjectMingyuOhDlg::StartImageProcessTime(const int& index, const eImageProcessingType& imageProcessingType)
{
	if (imageProcessingType == eImageProcessingType::OPENCV)
	{
		m_ResultData.opencvImageBlurProcessTimes[index].startTime = std::chrono::high_resolution_clock::now();
	}
	else if (imageProcessingType == eImageProcessingType::CUSTOM)
	{
		m_ResultData.customImageBlurProcessTimes[index].startTime = std::chrono::high_resolution_clock::now();
	}
}

void CMainProjectMingyuOhDlg::EndImageProcessTime(const int& index, const eImageProcessingType& imageProcessingType)
{
	if (imageProcessingType == eImageProcessingType::OPENCV)
	{
		m_ResultData.opencvImageBlurProcessTimes[index].endTime = std::chrono::high_resolution_clock::now();
		m_ResultData.opencvImageBlurProcessTimes[index].elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>
			(m_ResultData.opencvImageBlurProcessTimes[index].endTime - m_ResultData.opencvImageBlurProcessTimes[index].startTime);
	}
	else if (imageProcessingType == eImageProcessingType::CUSTOM)
	{
		m_ResultData.customImageBlurProcessTimes[index].endTime = std::chrono::high_resolution_clock::now();
		m_ResultData.customImageBlurProcessTimes[index].elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>
			(m_ResultData.customImageBlurProcessTimes[index].endTime - m_ResultData.customImageBlurProcessTimes[index].startTime);
	}
}

void CMainProjectMingyuOhDlg::StartMemoryUsage(const int& index, const eImageProcessingType& imageProcessingType)
{
	if (imageProcessingType == eImageProcessingType::OPENCV)
	{
		m_ResultData.opencvImageBlurMemoryUsages[index].startMemoryUsage = GetMemoryUsage();
	}
	else if (imageProcessingType == eImageProcessingType::CUSTOM)
	{
		m_ResultData.customImageBlurMemoryUsages[index].startMemoryUsage = GetMemoryUsage();
	}
}

void CMainProjectMingyuOhDlg::EndMemoryUsage(const int& index, const eImageProcessingType& imageProcessingType)
{
	if (imageProcessingType == eImageProcessingType::OPENCV)
	{
		m_ResultData.opencvImageBlurMemoryUsages[index].endMemoryUsage = GetMemoryUsage();
		m_ResultData.opencvImageBlurMemoryUsages[index].endMemoryUsage = 
			(m_ResultData.opencvImageBlurMemoryUsages[index].endMemoryUsage - m_ResultData.opencvImageBlurMemoryUsages[index].startMemoryUsage) / 1024;
	}
	else if (imageProcessingType == eImageProcessingType::CUSTOM)
	{
		m_ResultData.customImageBlurMemoryUsages[index].endMemoryUsage = GetMemoryUsage();
		m_ResultData.customImageBlurMemoryUsages[index].endMemoryUsage =
			(m_ResultData.customImageBlurMemoryUsages[index].endMemoryUsage - m_ResultData.customImageBlurMemoryUsages[index].startMemoryUsage) / 1024;
	}
}

size_t CMainProjectMingyuOhDlg::GetMemoryUsage()
{
	PROCESS_MEMORY_COUNTERS_EX pmc;
	if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) 
	{
		return pmc.PrivateUsage;
	}
	return 0;
}

void CMainProjectMingyuOhDlg::ReleaseLog()
{
	m_pLogBuffer->StopThreads();
}
