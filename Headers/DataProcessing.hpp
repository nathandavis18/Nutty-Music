#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <shobjidl.h>
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include "custom/myVector.hpp"


class DataProcessing {
public:
	custom::myVector<std::string> GetSearchResults(const std::string&, bool&, bool&);
	custom::myVector<std::string> GetDownloadUrl(custom::myVector<std::string>&, bool&);
	bool YtdlpDone();
	std::filesystem::path BrowseFiles();
private:
	//custom::myVector<std::wstring> temp;
};