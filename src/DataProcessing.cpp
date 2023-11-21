#include "../Headers/DataProcessing.hpp"

custom::myVector<std::wstring> DataProcessing::GetSearchResults(const std::wstring& search, bool& isDone, bool& isSearching){
	isSearching = true;
	std::vector<std::wstring> temp;

	std::wstring song = L"\"" + search + L"\"";
	std::wstring cmd = L"--flat-playlist --playlist-items 1:15 --print-to-file \"%(title)s BREAKPOINT %(id)s : %(original_url)s\" ytdlp\\outputs.txt --skip-download --no-warnings --default-search https://music.youtube.com/search?q= " + song;

	SHELLEXECUTEINFO sei = { 0 };
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.hwnd = NULL;
	sei.lpVerb = NULL;
	sei.lpFile = L"ytdlp\\yt-dlp";
	sei.lpParameters = cmd.c_str();
	sei.nShow = SW_HIDE;
	sei.hInstApp = NULL;

	ShellExecuteEx(&sei);
	WaitForSingleObject(sei.hProcess, INFINITE);
	CloseHandle(sei.hProcess);

	std::wstring line;
	std::wfstream stream{ "ytdlp/outputs.txt", stream.in };
	if (!stream.is_open()) {
		return custom::myVector<std::wstring>();
	}

	while (std::getline(stream, line)) {
		temp.push_back(line);
	}
	isSearching = false;
	isDone = true;
	custom::myVector<std::wstring> temp2(temp.size());
	for (int i = 0; i < temp.size(); ++i) {
		temp2[i] = temp[i];
	}
	return temp2;
}

custom::myVector<std::wstring> DataProcessing::GetTop5(custom::myVector<std::wstring>& searchResults)
{
	int count = 0;
	custom::myVector<std::wstring> temp;
	for (int i = 0; i < searchResults.size(); ++i) {
		if (count == 5) break;

		std::wstring url = searchResults[i].substr(searchResults[i].find(L": http") + 2, searchResults[i].length());
		if (url.find(L"/browse/") != url.npos) continue;

		std::wstring tempStr = searchResults[i];
		temp.push_back(tempStr);
		++count;
	}
	return temp;
}
void DataProcessing::GetDownloadUrl(custom::myVector<std::wstring>& data) {
	std::wstring cmd = data[0];
	ShellExecute(NULL, NULL, cmd.c_str(), NULL, NULL, SW_HIDE);
}
