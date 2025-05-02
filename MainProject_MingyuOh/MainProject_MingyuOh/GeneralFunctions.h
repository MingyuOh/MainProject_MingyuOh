#pragma once

#include <vector>
#include <stdexcept>
#include <filesystem>

#include "Singleton.h"

enum
{
	SIZE_DEFAULT_PATH = 256,
	SIZE_DEFAULT_BUFFER = 1024
};

class GeneralFunctions :public Singleton<GeneralFunctions>
{
public:
	std::string g_EXEPath;
	std::string g_SettingDirPath;
	std::string g_SettingFilePath;
	std::string g_LoadImageDirPath;
	std::string g_SaveImageDirPath;
	std::string g_ResultFilePath;
public:
	//Init
	void InitPath();

	// Add
	void AddLog(const std::string& fileName, const std::string& msg, const eLogFileExtension& logFileExtension = eLogFileExtension::LOG);

	// Create
	void CreateAllFolder(const std::string& csPath);
	std::string CreateDataLine(const std::vector<std::string>& headList);
	void CreateCustomFile(const std::string& filePath, const std::string& msg);
	bool CreateIniFile(const std::string& filePath, const std::string& msg = "");

	bool DoesFileExist(const std::string& filePath);
	bool DoesFolderExist(const std::string& folderPath);

	std::string GetFolderPathFromFilePath(const std::string& filePath);
	std::string GetFileExtension(const std::string& filePath);

	// Write
	void WriteIniValue(const std::string& section, const std::string& key, const std::string& value, const std::string& filePath);
	bool WriteDataInFile(const std::string& filePath, const std::string& data);

	// Read
	std::string ReadIniValue(const std::string& section, const std::string& key, const std::string& filePath);

	// Util
	std::string GetLogFileExtension(const eLogFileExtension& logFileExtension);
	bool ExtractSubString(const std::string& fullString, std::string& subString, size_t& start, const std::string& delimiter);
	void GetTokenListForPath(const std::string& csTarget, std::vector<std::string>& vList);
	SYSTEMTIME GetDateTime(std::string& sTime);
	void GetDateTimePath(std::string& sTime);
	void GetTimePath(std::string& sTime);
	bool CheckAndAddLastCharacter(std::string& refData, const char& ch);
	bool ReplaceLastCharacter(std::string& refData, char targetChar, char replaceChar);
	std::string StringFormat(const char* format, ...);

public:
	GeneralFunctions();
	virtual ~GeneralFunctions();

	// Templete area
public:
	template<class T>
	std::string GetTextForEnum(const std::vector<T>& elements, int enumVal)
	{
		return elements[enumVal];
	}

	template<class T>
	int GetEnumForText(const std::vector<T>& elements, T enumText)
	{
		for (int i = 0; i < elements.size(); i++)
		{
			if (elements[i] == enumText)
			{
				return i;
			}
		}
		return 0;
	}
};

