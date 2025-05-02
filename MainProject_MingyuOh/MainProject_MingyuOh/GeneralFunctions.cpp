#include "pch.h"
#include "GeneralFunctions.h"
#include <fstream>
#include <io.h>

GeneralFunctions::GeneralFunctions()
{
	InitPath();
}


GeneralFunctions::~GeneralFunctions()
{
}

void GeneralFunctions::InitPath()
{
	// Exe path
	char chFilePath[SIZE_DEFAULT_PATH] = { 0 };
	GetModuleFileNameA(NULL, chFilePath, SIZE_DEFAULT_PATH);

	std::string fullPath(chFilePath);

	size_t pos = fullPath.find_last_of('\\');
	g_EXEPath = (pos != std::string::npos) ? fullPath.substr(0, pos) : "";

	// Setting path
	g_SettingDirPath = g_EXEPath + "\\Setting\\";
	g_SettingFilePath = g_SettingDirPath + "Setting.ini";

	// Load image path
	std::string loadImagePath = g_EXEPath;
	pos = loadImagePath.find_last_of("\\/");
	loadImagePath = loadImagePath.substr(0, pos);

	pos = loadImagePath.find_last_of("\\/");
	loadImagePath = loadImagePath.substr(0, pos);

	g_LoadImageDirPath = loadImagePath + "\\Images\\";

	// Save image path
	g_SaveImageDirPath = g_EXEPath + "\\SaveImages\\";

	// Save result path
	g_ResultFilePath = g_EXEPath + "\\result.csv";
}

void GeneralFunctions::AddLog(const std::string& fileName, const std::string& msg, const eLogFileExtension& logFileExtension)
{
	std::string path, fullPath, fileExtension;

	path = g_EXEPath + "\\";

	if (DoesFolderExist(path) == false)
	{
		CreateAllFolder(path);
	}

	fileExtension = GetLogFileExtension(logFileExtension);
	fullPath = path + fileName + fileExtension;

	if (DoesFileExist(fullPath) == false)
	{
		CreateCustomFile(fullPath, msg);
	}
	else
	{
		std::ofstream logfile(fullPath, std::ios::app);
		if (logfile.is_open() == true)
		{
			logfile << msg;
			logfile.close();
		}
	}
}

void GeneralFunctions::CreateAllFolder(const std::string& csPath)
{
	std::vector<std::string> vResult;
	GetTokenListForPath(csPath, vResult);

	if (vResult.size() < 2)
		return;

	std::string strMakeTargetPath = vResult[0];
	for (int i = 1; i < vResult.size(); i++)
	{
		strMakeTargetPath += "\\" + vResult[i];
		std::wstring wstrMakeTargetPath(strMakeTargetPath.begin(), strMakeTargetPath.end());
		::CreateDirectory(wstrMakeTargetPath.c_str(), NULL);
	}
}

void GeneralFunctions::CreateCustomFile(const std::string& filePath, const std::string& msg)
{
	CStdioFile file;

	if (DoesFileExist(filePath) == false)
	{
		std::string directoryPath = GetFolderPathFromFilePath(filePath);
		if (DoesFolderExist(directoryPath) == false)
		{
			CreateAllFolder(directoryPath);
		}

		std::ofstream file(filePath, std::ios::app);
		if (file.is_open() == true)
		{
			file << msg;
			file.close();
		}
	}
}

bool GeneralFunctions::CreateIniFile(const std::string& filePath, const std::string& msg)
{
	std::string fileExtension = GetFileExtension(filePath);
	if (fileExtension.find("ini") == std::string::npos)
	{
		return false;
	}

	if (DoesFileExist(filePath) == false)
	{
		std::string folderPath = GetFolderPathFromFilePath(filePath);
		if (DoesFolderExist(folderPath) == false)
		{
			CreateAllFolder(folderPath);
		}
		CreateCustomFile(filePath, msg);
		return true;
	}
	else
	{
		return false;
	}
}

std::string GeneralFunctions::CreateDataLine(const std::vector<std::string>& headList)
{
	std::string headData;
	for (const auto& head : headList)
	{
		headData += head + ',';
	}

	ReplaceLastCharacter(headData, ',', '\n');
	return headData;
}

bool GeneralFunctions::DoesFileExist(const std::string& filePath)
{
	std::wstring wsFilePath(filePath.begin(), filePath.end());
	DWORD fileAttributes = GetFileAttributes(wsFilePath.c_str());
	return (fileAttributes != INVALID_FILE_ATTRIBUTES) && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

bool GeneralFunctions::DoesFolderExist(const std::string& folderPath)
{
	std::wstring wsFolderPath(folderPath.begin(), folderPath.end());
	DWORD fileAttributes = GetFileAttributes(wsFolderPath.c_str());

	if (fileAttributes != INVALID_FILE_ATTRIBUTES
		&& (fileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return true;
	}
	return false;
}

std::string GeneralFunctions::GetFolderPathFromFilePath(const std::string& filePath)
{
	size_t lastBackSlash = filePath.find_last_of('\\');

	if (lastBackSlash != std::string::npos)
	{
		return filePath.substr(0, lastBackSlash);
	}
	else
	{
		return filePath;
	}
}

std::string GeneralFunctions::GetFileExtension(const std::string& filePath)
{
	std::wstring wsFilePath(filePath.begin(), filePath.end());
	LPCWSTR wideExtension = PathFindExtension(wsFilePath.c_str());
	std::wstring wsExtension(wideExtension);

	return std::string(wsExtension.begin(), wsExtension.end());
}

void GeneralFunctions::WriteIniValue(const std::string& section, const std::string& key, const std::string& value, const std::string& filePath)
{
	std::wstring wsSection(section.begin(), section.end());
	std::wstring wsKey(key.begin(), key.end());
	std::wstring wsValue(value.begin(), value.end());
	std::wstring wsFilePath(filePath.begin(), filePath.end());

	std::string fileExtension = GetFileExtension(filePath);
	if (fileExtension.find("ini") == std::string::npos)
	{
		return;
	}
	WritePrivateProfileString(wsSection.c_str(), wsKey.c_str(), wsValue.c_str(), wsFilePath.c_str());
}

bool GeneralFunctions::WriteDataInFile(const std::string& filePath, const std::string& data)
{
	std::string folderPath = GetFolderPathFromFilePath(filePath);
	if (folderPath.empty() == false)
	{
		if (DoesFolderExist(folderPath) == false)
		{
			CreateAllFolder(folderPath);
		}
	}

	std::ofstream file(filePath, std::ios::app);
	if (file.is_open() == true)
	{
		file << data;
		file.close();
	}
	return false;
}

std::string GeneralFunctions::ReadIniValue(const std::string& section, const std::string& key, const std::string& filePath)
{
	std::wstring wsSection(section.begin(), section.end());
	std::wstring wsKey(key.begin(), key.end());
	std::wstring wsFilePath(filePath.begin(), filePath.end());

	std::string fileExtension = GetFileExtension(filePath);
	if (fileExtension.find(("ini")) == std::string::npos)
	{
		return "";
	}

	TCHAR buffer[MAX_PATH];
	GetPrivateProfileString(wsSection.c_str(), wsKey.c_str(), _T(""), buffer, MAX_PATH, wsFilePath.c_str());
	std::wstring wsBuffer(buffer);
	std::string value = std::string(wsBuffer.begin(), wsBuffer.end());

	if (value.empty())
	{
		value = "0";
		std::wstring wsValue(value.begin(), value.end());
		WritePrivateProfileString(wsSection.c_str(), wsKey.c_str(), wsValue.c_str(), wsFilePath.c_str());
	}

	return value;
}

std::string GeneralFunctions::GetLogFileExtension(const eLogFileExtension& logFileExtension)
{
	int index = static_cast<int>(logFileExtension);
	if (logFileExtension >= eLogFileExtension::TOTAL_COUNT)
	{
		index = static_cast<int>(eLogFileExtension::LOG);
	}
	return LogFileExtensionList[index];
}

bool GeneralFunctions::ExtractSubString(const std::string& fullString, std::string& subString, size_t& start, const std::string& delimiter)
{
	size_t end = fullString.find(delimiter, start);

	if (end != std::string::npos)
	{
		subString = fullString.substr(start, end - start);
		start = end + delimiter.length();
		return true;
	}
	else if (start < fullString.length())
	{
		subString = fullString.substr(start);
		start = fullString.length();
		return true;
	}

	return false;
}


void GeneralFunctions::GetTokenListForPath(const std::string& csTarget, std::vector<std::string>& vList)
{
	size_t start = 0;
	std::string strFull, strSub;

	strFull = csTarget;

	vList.clear();
	while (ExtractSubString(strFull, strSub, start, "\\") == true)
	{
		if (strSub.empty() == false)
		{
			vList.push_back(strSub);
		}
	}
}

SYSTEMTIME GeneralFunctions::GetDateTime(std::string& sTime)
{
	SYSTEMTIME cur_time;
	GetLocalTime(&cur_time);

	char dateBuffer[SIZE_DEFAULT_BUFFER];
	sprintf(dateBuffer, "%04d-%02d-%02d %02d:%02d:%02d(%03d)",
		cur_time.wYear, cur_time.wMonth, cur_time.wDay,
		cur_time.wHour, cur_time.wMinute, cur_time.wSecond, cur_time.wMilliseconds);
	sTime += dateBuffer;

	return cur_time;
}

void GeneralFunctions::GetDateTimePath(std::string& sTime)
{
	SYSTEMTIME cur_time;
	GetLocalTime(&cur_time);

	char dateBuffer[SIZE_DEFAULT_BUFFER];
	sprintf(dateBuffer, "[%04d-%02d-%02d %02d:%02d:%02d.%03d]",
		cur_time.wYear, cur_time.wMonth, cur_time.wDay,
		cur_time.wHour, cur_time.wMinute, cur_time.wSecond, cur_time.wMilliseconds);
	sTime += dateBuffer;
}

void GeneralFunctions::GetTimePath(std::string& sTime)
{
	SYSTEMTIME cur_time;
	GetLocalTime(&cur_time);

	char dateBuffer[SIZE_DEFAULT_BUFFER];
	sprintf(dateBuffer, "%02d-%02d-%02d(%03d)", cur_time.wHour, cur_time.wMinute, cur_time.wSecond, cur_time.wMilliseconds);
	sTime += dateBuffer;
}

bool GeneralFunctions::CheckAndAddLastCharacter(std::string& refData, const char& ch)
{
	if (refData.empty() == false && refData.back() != ch)
	{
		refData.push_back(ch);
		return true;
	}
	return false;
}

bool GeneralFunctions::ReplaceLastCharacter(std::string& refData, char targetChar, char replaceChar)
{
	if (refData.empty() == false && refData.back() == targetChar)
	{
		refData.back() = replaceChar;
		return true;
	}
	return false;
}

std::string GeneralFunctions::StringFormat(const char* format, ...)
{
	va_list args;
	char buffer[SIZE_DEFAULT_BUFFER];

	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	return std::string(buffer);
}