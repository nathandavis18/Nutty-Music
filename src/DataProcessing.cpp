#include "../Headers/DataProcessing.hpp"
static std::string ytdlpPath = "../../../../ytdlp";

/// <summary>
/// Gets the song title, artist, download url, and any other available information and stores it in a vector.
/// </summary>
/// <param name="search">The search string from the user's input</param>
/// <param name="isDone">A bool to track the status of this function</param>
/// <param name="isSearching">A bool to know if the searching label should be displayed</param>
/// <returns>Returns a vector of the search results</returns>
std::vector<std::string> DataProcessing::GetSearchResults(const std::string& search, bool& isDone, bool& isSearching){
	isSearching = true;
	std::vector<std::string> temp;

	std::string song = "\"" + search + "\"";
	std::string cmd = "/c cd " + ytdlpPath + " & python ytsearch.py " + song;

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
	std::ifstream stream(ytdlpPath + "/searchresults.txt");
	if (!stream.is_open()) {
		return std::vector<std::string>();
	}

	while (std::getline(stream, line)) {
		temp.push_back(line);
	}
	isSearching = false;
	isDone = true;
	return temp;
}

/// <summary>
/// Gets the download URLs for the songs and stores them in a vector.
/// </summary>
/// <param name="data">The list of strings containing all the song information</param>
/// <param name="gettingUrls">A boolean used to track the status of the urls</param>
/// <returns>Returns a vector of the download urls</returns>
std::vector<std::string> DataProcessing::GetDownloadUrl(std::vector<std::string>& data, bool& gettingUrls) {
	gettingUrls = true;
	for (int i = 0; i < data.size(); ++i) {
		std::string url = "\"" + data[i].substr(data[i].find("BREAKPOINT") + 11, 12) + "\"";
		std::string cmd = "/c cd " + ytdlpPath + "& yt-dlp --flat-playlist --no-warnings --default-search gvsearch --print-to-file \"%(urls)s\" temp" + std::to_string(i) + ".txt --skip-download -- " + url;
		ShellExecuteA(NULL, NULL, "cmd.exe", cmd.c_str(), NULL, SW_HIDE);
	}

	while (!YtdlpDone()) {
		Sleep(50);
	}

	std::vector<std::string> temp;
	for (int i = 0; i < 5; ++i) {
		int lineNumber = 0;
		std::string file = ytdlpPath + "/temp" + std::to_string(i) + ".txt";
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
	ShellExecuteA(NULL, NULL, "cmd.exe", "/c cd ../../../../ytdlp & del /Q *.txt", NULL, SW_HIDE);
	gettingUrls = false;
	return temp;
}

/// <summary>
/// Determines if all the downloads have finished. Continuously called until done.
/// </summary>
/// <returns></returns>
bool DataProcessing::YtdlpDone() {
	
	bool a = std::filesystem::exists(ytdlpPath + "\\temp0.txt");
	bool b = std::filesystem::exists(ytdlpPath + "\\temp1.txt");
	bool c = std::filesystem::exists(ytdlpPath + "\\temp2.txt");
	bool d = std::filesystem::exists(ytdlpPath + "\\temp3.txt");
	bool e = std::filesystem::exists(ytdlpPath + "\\temp4.txt");

	bool z = (a && b && c && d && e); //If all the files exist
	return z;
}

/// <summary>
/// Reference for opening a file explorer dialog box in windows API
/// https://learn.microsoft.com/en-us/windows/win32/learnwin32/example--the-open-dialog-box
/// </summary>
/// <returns></returns>
std::filesystem::path DataProcessing::BrowseFiles() {
	std::filesystem::path returnPath;
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr)) {
		IFileDialog* fileOpen;

		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&fileOpen));
		if (SUCCEEDED(hr)) {
			hr = fileOpen->Show(NULL);
			if (SUCCEEDED(hr)) {
				IShellItem* item;
				hr = fileOpen->GetResult(&item);
				if (SUCCEEDED(hr)) {
					PWSTR filePath;
					hr = item->GetDisplayName(SIGDN_FILESYSPATH, &filePath);

					if (SUCCEEDED(hr)) {
						returnPath = filePath;
						CoTaskMemFree(filePath);
					}
					item->Release();
				}
			}
			fileOpen->Release();
		}
		CoUninitialize();
	}
	return returnPath;
}