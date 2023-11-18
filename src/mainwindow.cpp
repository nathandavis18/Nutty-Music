#include <filesystem>
#include <fstream>
#include <array>
#include <algorithm>

#include "../Headers/mainwindow.hpp"
#include "../Headers/MusicController.hpp"

wxIMPLEMENT_APP(MyApp);

MyWindow::MyWindow() : myFrame(new MyFrame()) {
	myFrame->Show(true);
	text = new wxTextCtrl(myFrame, SEARCH_ID, "Enter a URL", wxPoint(700, 540), wxSize(100, 100), wxTE_PROCESS_ENTER);
	playPauseButton = new wxButton(myFrame, wxID_ANY, "|> ||", wxPoint(100, 100), wxSize(200, 200));
	forwardSkipButton = new wxButton(myFrame, wxID_ANY, ">|", wxPoint(100, 300), wxSize(200, 200));
	reverseSkipButton = new wxButton(myFrame, wxID_ANY, "|<", wxPoint(100, 500), wxSize(200, 200));
	lbl = new wxStaticText(myFrame, LABEL - 1 + songTitleLabelIndexOffset, "Searching rn", wxPoint(500, 50 + (50 * 0)), wxSize(300, 50));
	lbl->Show(false);

	text->Bind(wxEVT_TEXT_ENTER, &MyWindow::PressedEnter, this);
	playPauseButton->Bind(wxEVT_BUTTON, &MyWindow::playPauseBtnClick, this);
	forwardSkipButton->Bind(wxEVT_BUTTON, &MyWindow::forwardSkipBtnClick, this);
	reverseSkipButton->Bind(wxEVT_BUTTON, &MyWindow::reverseSkipBtnClick, this);

	myFrame->Bind(wxEVT_CLOSE_WINDOW, &MyWindow::OnClose, this);
	myFrame->Bind(wxEVT_IDLE, &MyWindow::OnIdle, this);
}

void MyWindow::OnIdle(wxIdleEvent& evt) {
	if (doneSearching) {
		std::filesystem::remove("ytdlp\\outputs.txt");
		lbl->Show(false);
		doneSearching = false;
		createButton(0, "vidID");
		createLabels(0, "sub");
		text->Enable();
		return;
	}
	if (isSearching) {
		text->Disable();
		lbl->Show(true);
		return;
	} 
	return;
}

void MyWindow::PressedEnter(wxCommandEvent& event) {
	clearPrevSearch();
	std::wstring s = text->GetValue().ToStdWstring();
	if (s.find_first_not_of(' ') != s.npos) {
		t = std::thread(&MyWindow::searchResults, this, s);
		t.detach();
	}
}

void MyWindow::createButton(int index, std::string url) {
	wxButton* playButton = new wxButton(myFrame, BUTTON + index + playButtonIndexOffset, "Play", wxPoint(900, 50 + (50 * index)), wxSize(75, 25));
	playButton->Bind(wxEVT_BUTTON, &MyWindow::playBtnClick, this);
	playButtons.push_back(playButton);
	test = playButton;
	test->Show(true);

	wxButton* addQueueButton = new wxButton(myFrame, BUTTON + index + addQueueButtonIndexOffset, "Add to Queue", wxPoint(1000, 50 + (50 * index)), wxSize(100, 25));
	addQueueButton->Bind(wxEVT_BUTTON, &MyWindow::queueBtnClick, this);
	addQueueButtons.push_back(addQueueButton);
	urls.push_back(url);
}

void MyWindow::createLabels(int index, std::string title) {
	wxStaticText* st = new wxStaticText(myFrame, LABEL + index + songTitleLabelIndexOffset, title, wxPoint(500, 50 + (50 * index)), wxSize(300, 50));
	labels.push_back(st);
}

void MyWindow::playBtnClick(wxCommandEvent& event) {
	wxExecute("cmd.exe /c cd ytdlp\\temp & del /Q *.mp3", output, errors, wxEXEC_ASYNC);
	int id = event.GetId() - wxID_HIGHEST - 1;
	std::string cmd = "-x --audio-format mp3 -o temp\\%(title)s.mp3 -f ba --use-extractors youtube --downloader ffmpeg -N 4 --throttled-rate 100000K " + urls[id];
	wxExecute("cmd.exe /c cd ytdlp & yt-dlp " + cmd, output, errors, wxEXEC_ASYNC);
	music.forcePlay(urls[id], songTitles[id]);
}

void MyWindow::queueBtnClick(wxCommandEvent& event) {
	int id = event.GetId() - wxID_HIGHEST - 15;
	music.addToQueue(urls[id], songTitles[id]);
}

void MyWindow::clearPrevSearch() {
	for (int i = 0; i < labels.size(); ++i) {
		labels[i]->Destroy();
		playButtons[i]->Destroy();
		addQueueButtons[i]->Destroy();
	}
	labels.clear();
	playButtons.clear();
	addQueueButtons.clear();
	urls.clear();
	songTitles.clear();
}

void MyWindow::playPauseBtnClick(wxCommandEvent& event) {
	if (music.getPauseState()) {
		music.resumeSong();
	}
	else {
		music.pauseSong();
	}
}

void MyWindow::forwardSkipBtnClick(wxCommandEvent& event) {
	music.skipForward();
}

void MyWindow::reverseSkipBtnClick(wxCommandEvent& event) {
	music.skipBackward();
}

void MyWindow::OnClose(wxCloseEvent& event) {
	music.closePlayer();
	wxExecute("cmd.exe /c cd ytdlp\\temp & del /Q *.mp3", output, errors, wxEXEC_ASYNC);
	myFrame->Destroy();
}

void MyWindow::searchResults(std::wstring search) {
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
		if (!songTitles.find(sub)) {
			urlSub = result[i].substr(result[i].find("BREAKPOINT") + 11, result[i].length() - 1);
			shortsUrl = urlSub.substr(urlSub.find(":") + 1, urlSub.length() - 1);
			if (shortsUrl.find("browse") != sub.npos) continue;


			songTitles.push_back(sub);
			vidID = urlSub.substr(0, urlSub.find(":"));
			//std::string cmd2 = "--skip-download --print-to-file \"%(urls)s ||||||BREAKER||||||\" ytdlp\\urlOutput.txt --default-search gvsearch " + vidID;
			//ShellExecuteA(NULL, NULL, "ytdlp\\yt-dlp.exe", cmd2.c_str(), NULL, SW_HIDE);
			//createButton(index, vidID);
			//createLabels(index, sub);
			++index;
		}
	} 
	isSearching = false;
	doneSearching = true;
	wxWakeUpIdle(); //If the app is idle, this calls the idle function
	return;
}