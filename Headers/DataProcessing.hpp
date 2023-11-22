#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <fstream>
#include <vector>
#include "custom/myVector.hpp"


class DataProcessing {
public:
	/// <summary>
	/// Takes in a wstring, a bool which controls if the search function is running, 
	/// a bool that shows when it is completed, and an output vector.
	/// </summary>
	custom::myVector<std::string> GetSearchResults(const std::string&, bool&, bool&);
	custom::myVector<std::string> GetTop5(custom::myVector<std::string>&);
	void GetDownloadUrl(custom::myVector<std::string>&);
private:
	//custom::myVector<std::wstring> temp;
};