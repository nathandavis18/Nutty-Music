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
	custom::myVector<std::wstring> GetSearchResults(const std::wstring&, bool&, bool&);
	custom::myVector<std::wstring> GetTop5(custom::myVector<std::wstring>&);
	void GetDownloadUrl(custom::myVector<std::wstring>&);
private:
	//custom::myVector<std::wstring> temp;
};