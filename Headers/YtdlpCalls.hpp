#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <fstream>
#include "wx/wxprec.h"
#include "custom/myVector.hpp"


class YtdlpCalls {
public:
	void searchResults(const std::wstring, bool&, bool&, custom::myVector<std::string>&);
private:
	
};