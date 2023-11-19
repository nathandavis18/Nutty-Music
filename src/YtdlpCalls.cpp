#include "../Headers/YtdlpCalls.hpp"

void YtdlpCalls::searchResults(const std::wstring search, bool& isDone, bool& isSearching, custom::myVector<std::string>& outputVec) {
	isSearching = true;

	std::wstring song = L"\"" + search + L"\"";
	std::wstring cmd = L"--flat-playlist --playlist-items 1:15 --print-to-file \"%(title)s BREAKPOINT %(id)s : %(original_url)s\" ytdlp\\outputs.txt --skip-download --default-search https://music.youtube.com/search?q= " + song;

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

	custom::myVector<std::string> result;
	std::string line;
	std::fstream stream{ "ytdlp/outputs.txt", stream.in };
	if (!stream.is_open())
		return;

	while (std::getline(stream, line)) {
		result.push_back(line);
	}

	std::string sub = "";
	std::string urlSub = "";
	std::string shortsUrl = "";
	std::string vidID = "";
	int index = 0;
	for (int i = 0; i < result.size(); ++i) {
		if (index == 5) break;

		sub = result[i].substr(0, result[i].find("BREAKPOINT") - 1);
		if (!outputVec.find(sub)) {
			urlSub = result[i].substr(result[i].find("BREAKPOINT") + 11, result[i].length() - 1);
			shortsUrl = urlSub.substr(urlSub.find(":") + 1, urlSub.length() - 1);
			if (shortsUrl.find("browse") != sub.npos) continue;


			outputVec.push_back(sub);
			vidID = urlSub.substr(0, urlSub.find(":"));
			//std::string cmd2 = "--skip-download --print-to-file \"%(urls)s ||||||BREAKER||||||\" ytdlp\\urlOutput.txt --default-search gvsearch " + vidID;
			//ShellExecuteA(NULL, NULL, "ytdlp\\yt-dlp.exe", cmd2.c_str(), NULL, SW_HIDE);
			//createButton(index, vidID);
			//createLabels(index, sub);
			++index;
		}
	}
	isSearching = false;
	isDone = true;
	wxWakeUpIdle(); //If the app is idle, this calls the idle function
	return;
}