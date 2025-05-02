#pragma once

#include <unordered_map>

#include "LogBuffer.h"
#include "ImageObject.h"
#include "GeneralFunctions.h"
#include "CustomThreadPool.h"

enum
{
	MAX_IMAGE_COUNT = 3,
	SIZE_DEFUALT = 256
};

class CMainProjectMingyuOhDlg : public CDialogEx
{
public:
	CMainProjectMingyuOhDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAINPROJECT_MINGYUOH_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);


protected:
	HICON m_hIcon;

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonLoad();

public:
	int m_nKernelSize;
	CString m_LoadImagePath;
	CString m_SaveImagePath;

	int m_nCurrentLoadImageCount;
	CustomThreadPool m_ThreadPool;
	ResultData m_ResultData;

	std::shared_ptr<LogBuffer> m_pLogBuffer;
	std::shared_ptr<GeneralFunctions> m_pGeneralFunctions;

	std::vector<ImageObject> m_SrcImageObjects;
	std::vector<ImageObject> m_DstOpencvImageObjects;
	std::vector<ImageObject> m_DstCustomImageObjects;
	std::vector<CString> m_ImageFilePaths;

	std::unordered_map<int, std::vector<MatEx>> m_hashDisplayImages;
	std::unordered_map<std::string, std::vector<int>> m_hashPictureControlID;
public:
	void InitObject();
	void InitLog();
	void InitThreadPool();
	void InitGeneralFunctions();

	void LoadSettingIniFileData();
	void SaveSettingIniFileData();

	void LoadOrgImage();
	void StartImageProcessing();

	void DisplayProcessingImage(const int& index);
	void SetImageToPictureControl(const ImageObject* imageObject, const int& index, const std::string& textType);

	void CompareResultOfBlurAlogrithm(const int& index);
	void SaveImageProcessingResult(const int& index);
	void SaveProcessingImage(const int& index);
	void SaveResultData(const int& index);
	void SetPictureControlIDInfo();

	void ClearData();
	void ClearResultData();
	void ClearPictureControls();
	void ClearDisplayImages();
	void ClearImages();

	std::string CreateSaveImageFileName(const eImageProcessingType& imageProcessingType, const int& index, const std::string& ext = ".jpg");

	void WriteLog(const std::string& logFileName, const std::string& msg, const eLogInfoType& logInfoType);

	void StartImageProcessTime(const int& index, const eImageProcessingType& imageProcessingType);
	void EndImageProcessTime(const int& index, const eImageProcessingType& imageProcessingType);

	void StartMemoryUsage(const int& index, const eImageProcessingType& imageProcessingType);
	void EndMemoryUsage(const int& index, const eImageProcessingType& imageProcessingType);
	size_t GetMemoryUsage();

	void ReleaseLog();
};
