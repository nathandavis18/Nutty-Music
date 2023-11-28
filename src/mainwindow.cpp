#include <filesystem>
#include <fstream>
#include <array>
#include <algorithm>

#include "../Headers/mainwindow.hpp"
#include "../Headers/MusicController.hpp"

wxIMPLEMENT_APP(MyApp);

MyWindow::MyWindow() : myFrame(new MyFrame()) {
	myFrame->Show(true);
	text = new wxTextCtrl(myFrame, SEARCH_ID, wxEmptyString, wxPoint(300, 540), wxSize(600, 100), wxTE_PROCESS_ENTER);
	wxButton* playPauseButton = new wxButton(myFrame, wxID_ANY, "|> ||", wxPoint(100, 100), wxSize(200, 200));
	wxButton* forwardSkipButton = new wxButton(myFrame, wxID_ANY, ">|", wxPoint(100, 300), wxSize(200, 200));
	wxButton* reverseSkipButton = new wxButton(myFrame, wxID_ANY, "|<", wxPoint(100, 500), wxSize(200, 200));
	searchingLabel = new wxStaticText(myFrame, LABEL - 1 + songTitleLabelIndexOffset, "Searching rn", wxPoint(500, 50 + (50 * 0)), wxSize(300, 50));
	searchingLabel->Show(false);

	text->SetHint("Enter a song title/artist");
	text->Bind(wxEVT_TEXT_ENTER, &MyWindow::PressedEnter, this);
	playPauseButton->Bind(wxEVT_BUTTON, &MyWindow::playPauseBtnClick, this);
	forwardSkipButton->Bind(wxEVT_BUTTON, &MyWindow::forwardSkipBtnClick, this);
	reverseSkipButton->Bind(wxEVT_BUTTON, &MyWindow::reverseSkipBtnClick, this);

	myFrame->Bind(wxEVT_CLOSE_WINDOW, &MyWindow::OnClose, this);
	myFrame->Bind(wxEVT_IDLE, &MyWindow::OnIdle, this);
}

/// <summary>
/// Controls what happens while the app is idle.
/// Used to handle the search happening in a secondary thread
/// </summary>
/// <param name="evt"></param>
void MyWindow::OnIdle(wxIdleEvent&) {
	if (doneSearching) {
		std::filesystem::remove("ytdlp\\searchResults.txt");
		searchingLabel->Show(false);
		doneSearching = false;
		createButtons(results);
		createLabels(results);
		text->Enable();
		//wxMessageBox(results[0]);
		return;
	}
	else if (isSearching) {
		text->Disable();
		searchingLabel->Show(true);
		return;
	}
}

/// <summary>
/// When the user enters a search keyword, start the search in a new thread
/// </summary>
/// <param name=""></param>
void MyWindow::PressedEnter(wxCommandEvent&) {
	clearPrevSearch();
	std::string search = text->GetValue().ToStdString();
	if (search.find_first_not_of(' ') != search.npos) {
		std::thread t(&MyWindow::StartSearch, this, search, std::ref(results), std::ref(doneSearching), std::ref(isSearching));
		t.detach();
	}
}

void MyWindow::StartSearch(const std::string searchString, custom::myVector<std::string>& vecResults, bool& doneWithSearch, bool& searching) {
	m.lock();
	bool x = true;
	vecResults = processData.GetSearchResults(searchString, doneWithSearch, searching);//results contains all the data, now we need to parse it
	urls = processData.GetDownloadUrl(vecResults, x);
	m.unlock();
	ShellExecuteA(NULL, NULL, "cmd.exe", "/c cd ytdlp & del /Q *.txt", NULL, SW_HIDE);
	wxWakeUpIdle(); //If the app is idle, this calls the idle function
}

/// <summary>
/// Creates the play and add to queue buttons for each song in the results list
/// </summary>
/// <param name="index"></param>
/// <param name="url"></param>
void MyWindow::createButtons(custom::myVector<std::string>& searchResults) {
	for (int i = 0; i < searchResults.size(); ++i) {
		wxButton* playButton = new wxButton(myFrame, BUTTON + i + playButtonIndexOffset, "Play", wxPoint(900, 50 + (50 * i)), wxSize(75, 25));
		playButton->Bind(wxEVT_BUTTON, &MyWindow::playBtnClick, this);
		playButtons.push_back(playButton);

		wxButton* addQueueButton = new wxButton(myFrame, BUTTON + i + addQueueButtonIndexOffset, "Add to Queue", wxPoint(1000, 50 + (50 * i)), wxSize(100, 25));
		addQueueButton->Bind(wxEVT_BUTTON, &MyWindow::queueBtnClick, this);
		addQueueButtons.push_back(addQueueButton);
	}
}

/// <summary>
/// Creates the labels for each song in the results list
/// </summary>
/// <param name="index"></param>
/// <param name="title"></param>
void MyWindow::createLabels(custom::myVector<std::string>& searchResults) {
	for (int i = 0; i < searchResults.size(); ++i) {
		std::string song = searchResults[i].substr(0, searchResults[i].find("BREAKPOINT"));
		wxStaticText* st = new wxStaticText(myFrame, LABEL + i + songTitleLabelIndexOffset, song, wxPoint(500, 50 + (50 * i)), wxSize(300, 50));
		labels.push_back(st);
	}
}

/// <summary>
/// When the play button is clicked, download the song and start it
/// </summary>
/// <param name="event"></param>
void MyWindow::playBtnClick(wxCommandEvent& event) {
	wxExecute("cmd.exe /c cd ytdlp\\temp & del /Q *.mp3", output, errors, wxEXEC_ASYNC);
	int id = event.GetId() - wxID_HIGHEST - 1;
	//std::string cmd = "-x --audio-format mp3 -o temp\\%(title)s.mp3 -f ba --use-extractors youtube --downloader ffmpeg -N 4 --throttled-rate 100000K " + urls[id];
	//wxExecute("cmd.exe /c cd ytdlp & yt-dlp " + cmd, output, errors, wxEXEC_ASYNC);
	//music.forcePlay(urls[id], songTitles[id]);
}

/// <summary>
/// Add song to queue. Song gets downloaded depending on position in queue
/// </summary>
/// <param name="event"></param>
void MyWindow::queueBtnClick(wxCommandEvent& event) {
	int id = event.GetId() - wxID_HIGHEST - 15;
	//music.addToQueue(urls[id], songTitles[id]);
}

/// <summary>
/// Clears the labels, buttons, and vectors from the last search
/// </summary>
void MyWindow::clearPrevSearch() {
	for (int i = 0; i < labels.size(); ++i) {
		labels[i]->Destroy();
		playButtons[i]->Destroy();
		addQueueButtons[i]->Destroy();
	}
	labels.clear();
	playButtons.clear();
	addQueueButtons.clear();
	//urls.clear();
	//songTitles.clear();
	results.clear();
}

/// <summary>
/// Resuming and Pausing music button handler
/// </summary>
/// <param name=""></param>
void MyWindow::playPauseBtnClick(wxCommandEvent&) {
	if (music.isActive()) {
		if (music.isPaused()) {
			music.resumeSong();
		}
		else {
			music.pauseSong();
		}
	}
}

/// <summary>
/// Forward skip button handler
/// </summary>
/// <param name=""></param>
void MyWindow::forwardSkipBtnClick(wxCommandEvent&) {
	music.skipForward();
}

/// <summary>
/// Reverse Skip button handler
/// </summary>
/// <param name=""></param>
void MyWindow::reverseSkipBtnClick(wxCommandEvent&) {
	music.skipBackward();
}

/// <summary>
/// When the program is closed, make sure everything is properly destroyed and all temp files are cleaned up.
/// Makes sure the music player is properly closed as well.
/// </summary>
/// <param name=""></param>
void MyWindow::OnClose(wxCloseEvent&) {
	music.closePlayer();
	wxExecute("cmd.exe /c cd ytdlp\\temp & del /Q *.mp3", output, errors, wxEXEC_ASYNC);
	myFrame->Destroy();
}