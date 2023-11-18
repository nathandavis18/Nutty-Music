#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <mutex>
#include <fstream>
#include <array>
#include "custom/myVector.hpp"


class YtdlpCalls {
public:
	void searchResults(std::wstring);
private:
	std::mutex m;
};