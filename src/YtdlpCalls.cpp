#include "../Headers/YtdlpCalls.hpp"

void YtdlpCalls::searchResults(std::wstring search) {
	std::lock_guard<std::mutex> lg(m);

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

	return;
}