#pragma once

#include <string>
#include <atomic>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>


#ifdef _DEBUG
#pragma comment(lib, "opencv_world410d.lib")
#else
#pragma comment(lib, "opencv_world410.lib")
#endif

#pragma comment(lib, "ImageObjectLib.lib")
#pragma comment(lib, "Opencv.lib")
#pragma comment(lib, "Custom.lib")


// Enum
enum class eLogFileExtension : int
{
	LOG = 0,
	CSV,

	TOTAL_COUNT = 2
};

enum class eImageProcessingType : int
{
	ORG = 0,
	OPENCV,
	CUSTOM,

	TOTAL_COUNT = 3
};

enum class eLogInfoType : int
{
	INFO_TYPE = 0,
	WARNING_TYPE,
	ERROR_TYPE,

	TOTAL_COUNT = 3
};

// const
const int MIN_KERNEL_SIZE = 21;
const int MAX_COLOR_VALUE = 255;

const std::vector<std::string> LogFileExtensionList =
{
	".log",
	".csv"
};

const std::vector<std::string> ImageProcessingTypeList =
{
	"ORG",
	"OPENCV",
	"CUSTOM"
};

const std::vector<std::string> logInfoTypeList =
{
	"[Info]",
	"[Warning]",
	"[Error]"
};

const std::vector<std::string> ResultDataHeaderList =
{
	"LOGTIME",
	"OPENCV_PROCESSING_TIME",
	"CUSTOM_PROCESSING_TIME",
	"OPENCV_MEMORY_USAGE",
	"CUSTOM_MEMORY_USAGE",
	"MATCH"
};

const char SPACE = ' ';
const char UNDER_BAR = '_';
const char LINE_BREAK = '\n';

const std::string LogFileName_ImageBlur = "image_blur";

const std::string ORG_TEXT = "ORG";
const std::string OPENCV_TEXT = "OPENCV";
const std::string CUSTOM_TEXT = "CUSTOM";

// const - ini header
const std::string InitIniSection = "Init data";

// const - ini key
const std::string KeyKernelValue = "KERNEL_VALUE";
const std::string KeyLoadImagePath = "LOAD_IMAGE_PATH";
const std::string KeySaveImagePath = "SAVE_IMAGE_PATH";

// Struct
typedef struct _AtomicBool
{
	std::atomic<bool> value;

	_AtomicBool() : value(false) 
	{

	}

	_AtomicBool(_AtomicBool&& refAutomicBool) noexcept : value(refAutomicBool.value.load())
	{

	}

	_AtomicBool& operator=(_AtomicBool&& refAutomicBool) noexcept
	{
		if (this != &refAutomicBool)
		{
			value.store(refAutomicBool.value.load());
		}
		return *this;
	}

	_AtomicBool(const _AtomicBool&) = delete;
	_AtomicBool& operator=(const _AtomicBool&) = delete;

	void store(bool desired, std::memory_order order = std::memory_order_release)
	{
		value.store(desired, order);
	}

	bool load(std::memory_order order = std::memory_order_acquire) const
	{
		return value.load(order);
	}

}AtomicBool;

typedef struct _LogData
{
	std::string fileName;
	std::string data;
	eLogFileExtension logFileExtension;

	_LogData() : fileName(""), data(""), logFileExtension(eLogFileExtension::LOG)
	{

	}

	_LogData(const std::string& refFileName, const std::string& refData, const eLogFileExtension& refLogFileExtension) : fileName(refFileName), data(refData), logFileExtension(refLogFileExtension)
	{

	}

	_LogData& operator=(const _LogData& refLogData)
	{
		if (this != &refLogData)
		{
			fileName = refLogData.fileName;
			data = refLogData.data;
			logFileExtension = refLogData.logFileExtension;
		}
		return *this;
	}

	~_LogData() = default;

	void Clear()
	{
		fileName = "";
		data = "";
		logFileExtension = eLogFileExtension::LOG;
	}
}LogData;

typedef struct _MatEx
{
	eImageProcessingType imageProcessingType;
	cv::Mat matImage;

	_MatEx() : imageProcessingType(eImageProcessingType::ORG)
	{

	}

	_MatEx(const eImageProcessingType& refImageProcessingType, cv::Mat refMatImage) : imageProcessingType(refImageProcessingType)
	{
		matImage = refMatImage.clone();
	}

	_MatEx(const _MatEx& refMatEx)
	{
		imageProcessingType = refMatEx.imageProcessingType;
		matImage = refMatEx.matImage.clone();
	}
}MatEx;

typedef struct _ProcessTime
{
	std::chrono::high_resolution_clock::time_point startTime;
	std::chrono::high_resolution_clock::time_point endTime;
	std::chrono::milliseconds elapsedTime;
}ProcessTime;

typedef struct _MemoryUsage
{
	size_t startMemoryUsage;
	size_t endMemoryUsage;
	size_t elapsedMemoryUsage;

	_MemoryUsage() : startMemoryUsage(0), endMemoryUsage(0), elapsedMemoryUsage(0)
	{

	}

	void Clear()
	{
		startMemoryUsage = 0;
		endMemoryUsage = 0;
		elapsedMemoryUsage = 0;
	}
}MemoryUsage;

typedef struct _ResultData
{
	enum
	{
		MAX_IMAGE_COUNT = 3
	};
	std::vector<bool> matchList;
	std::vector<MemoryUsage> opencvImageBlurMemoryUsages;
	std::vector<MemoryUsage> customImageBlurMemoryUsages;
	std::vector<ProcessTime> opencvImageBlurProcessTimes;
	std::vector<ProcessTime> customImageBlurProcessTimes;

	_ResultData()
	{
		matchList.resize(MAX_IMAGE_COUNT, false);
		opencvImageBlurMemoryUsages.resize(MAX_IMAGE_COUNT);
		customImageBlurMemoryUsages.resize(MAX_IMAGE_COUNT);
		opencvImageBlurProcessTimes.resize(MAX_IMAGE_COUNT);
		customImageBlurProcessTimes.resize(MAX_IMAGE_COUNT);
	}

	void Clear()
	{
		for (int i = 0; i < MAX_IMAGE_COUNT; i++)
		{
			matchList[i] = false;
			opencvImageBlurMemoryUsages[i].Clear();
			customImageBlurMemoryUsages[i].Clear();
		}
	}

	void Release()
	{
		matchList.clear();
		opencvImageBlurMemoryUsages.clear();
		customImageBlurMemoryUsages.clear();
		opencvImageBlurProcessTimes.clear();
		customImageBlurProcessTimes.clear();
	}
}ResultData;