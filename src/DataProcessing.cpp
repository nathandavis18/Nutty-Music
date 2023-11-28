#include "../Headers/DataProcessing.hpp"

custom::myVector<std::string> DataProcessing::GetSearchResults(const std::string& search, bool& isDone, bool& isSearching){
	isSearching = true;
	custom::myVector<std::string> temp;

	std::string song = "\"" + search + "\"";
	std::string cmd = "/c cd ytdlp & python ytsearch.py " + song;

	SHELLEXECUTEINFOA sei = { 0 };
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.hwnd = NULL;
	sei.lpVerb = NULL;
	sei.lpFile = "cmd.exe";
	sei.lpParameters = cmd.c_str();
	sei.nShow = SW_HIDE;
	sei.hInstApp = NULL;

	ShellExecuteExA(&sei);
	WaitForSingleObject(sei.hProcess, INFINITE);
	CloseHandle(sei.hProcess);

	std::string line;
	std::fstream stream{ "ytdlp/searchresults.txt", stream.in };
	if (!stream.is_open()) {
		return custom::myVector<std::string>();
	}

	while (std::getline(stream, line)) {
		temp.push_back(line);
	}
	isSearching = false;
	isDone = true;
	return temp;
}

custom::myVector<std::string> DataProcessing::GetDownloadUrl(custom::myVector<std::string>& data, bool& gettingUrls) {
	gettingUrls = true;
	for (int i = 0; i < data.size(); ++i) {
		std::string url = "\"" + data[i].substr(data[i].find("BREAKPOINT") + 11, 12) + "\"";
		std::string cmd = "/c cd ytdlp & yt-dlp --flat-playlist --no-warnings --default-search gvsearch --print-to-file \"%(urls)s\" temp" + std::to_string(i) + ".txt --skip-download " + url;
		if (i + 1 >= data.size()) {
			SHELLEXECUTEINFOA sei = { 0 };
			sei.cbSize = sizeof(SHELLEXECUTEINFO);
			sei.fMask = SEE_MASK_NOCLOSEPROCESS;
			sei.hwnd = NULL;
			sei.lpVerb = NULL;
			sei.lpFile = "cmd.exe";
			sei.lpParameters = cmd.c_str();
			sei.nShow = SW_HIDE;
			sei.hInstApp = NULL;
			ShellExecuteExA(&sei);
			WaitForSingleObject(sei.hProcess, INFINITE);
			CloseHandle(sei.hProcess);
		}
		else {
			ShellExecuteA(NULL, NULL, "cmd.exe", cmd.c_str(), NULL, SW_HIDE);
		}
	}

	custom::myVector<std::string> temp;
	for (int i = 0; i < 5; ++i) {
		int lineNumber = 0;
		std::string file = "ytdlp/temp" + std::to_string(i) + ".txt";
		std::string line;
		std::fstream stream{ file, stream.in };
		if (!stream.is_open()) {
			return temp;
		}
		while (std::getline(stream, line)) {
			if (lineNumber == 1) { //We only want the second url from each set of urls
				temp.push_back(line);
			}
			++lineNumber;
		}
	}
	gettingUrls = false;
	return temp;
}
