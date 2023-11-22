#include "../Headers/DataProcessing.hpp"

custom::myVector<std::string> DataProcessing::GetSearchResults(const std::string& search, bool& isDone, bool& isSearching){
	isSearching = true;
	custom::myVector<std::string> temp;

	std::string song = "\"" + search + "\"";
	std::string cmd = "--flat-playlist --playlist-items 1:15 --print-to-file \"%(title)s BREAKPOINT %(id)s : %(original_url)s\" ytdlp\\outputs.txt --skip-download --no-warnings --default-search https://music.youtube.com/search?q= " + song;

	SHELLEXECUTEINFOA sei = { 0 };
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.hwnd = NULL;
	sei.lpVerb = NULL;
	sei.lpFile = "ytdlp\\yt-dlp";
	sei.lpParameters = cmd.c_str();
	sei.nShow = SW_HIDE;
	sei.hInstApp = NULL;

	ShellExecuteExA(&sei);
	WaitForSingleObject(sei.hProcess, INFINITE);
	CloseHandle(sei.hProcess);

	std::string line;
	std::fstream stream{ "ytdlp/outputs.txt", stream.in };
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

custom::myVector<std::string> DataProcessing::GetTop5(custom::myVector<std::string>& searchResults)
{
	int count = 0;
	custom::myVector<std::string> temp;
	for (int i = 0; i < searchResults.size(); ++i) {
		if (count == 5) break;

		std::string url = searchResults[i].substr(searchResults[i].find(": http") + 2, searchResults[i].length());
		if (url.find("/browse/") != url.npos) continue;

		std::string tempStr = searchResults[i];
		temp.push_back(tempStr);
		++count;
	}
	return temp;
}
void DataProcessing::GetDownloadUrl(custom::myVector<std::string>& data) {
	std::string cmd = data[0];
	ShellExecuteA(NULL, NULL, cmd.c_str(), NULL, NULL, SW_HIDE);
}
