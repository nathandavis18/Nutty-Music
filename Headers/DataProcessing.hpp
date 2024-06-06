#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <shobjidl.h>
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>


class DataProcessing {
public:
	std::vector<std::string> GetSearchResults(const std::string&, bool&, bool&);
	std::vector<std::string> GetDownloadUrl(std::vector<std::string>&, bool&);
	bool YtdlpDone();
	std::filesystem::path BrowseFiles();
};