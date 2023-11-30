#include <filesystem>
#include <fstream>
#include <array>
#include <algorithm>

#include "../Headers/mainwindow.hpp"
#include "../Headers/MusicController.hpp"

wxIMPLEMENT_APP(MyApp);

MyWindow::MyWindow() : myFrame(new MyFrame()), music(MusicController(&gettingUrls)), queueSongID(-500), playSongID(-500), tempQueueList(0) {
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
	wxWakeUpIdle();
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
	if (!gettingUrls) {
		if (waitingPlaySong) {
			waitingSongStart = true;
			waitingPlaySong = false;
			PlaySong();
		}
		if (waitingAddQueue && !waitingSongStart) {
			AddToQueue();
		}
	}
	if (music.isSongDone()) {
		music.skipForward();
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
		std::thread t(&MyWindow::StartSearch, this, search, std::ref(results), std::ref(doneSearching), std::ref(isSearching), std::ref(gettingUrls));
		t.detach();
	}
}

void MyWindow::StartSearch(const std::string searchString, custom::myVector<std::string>& vecResults, bool& doneWithSearch, bool& searching, bool& gettingDownloadUrls) {
	m.lock();
	vecResults = processData.GetSearchResults(searchString, doneWithSearch, searching);//results contains all the data, now we need to parse it
	urls = processData.GetDownloadUrl(vecResults, gettingDownloadUrls);
	for (int i = 0; i < vecResults.size(); ++i) {
		vecResults[i] = vecResults[i].substr(0, vecResults[i].find("BREAKPOINT") - 1);
	}
	m.unlock();
	std::this_thread::sleep_for(std::chrono::milliseconds(50)); //Makes sure ytdlp has had enough time to grab the urls.
	wxWakeUpIdle(); //Wakes the app up if it is idling
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
	playSongID = event.GetId() - BUTTON - playButtonIndexOffset;
	if (gettingUrls) {
		waitingPlaySong = true;
	}
	else {
		PlaySong();
	}
}

void MyWindow::PlaySong() {
	if (!std::filesystem::is_directory("ytdlp\\temp")) {
		std::filesystem::create_directory("ytdlp\\temp");
	}
	else {
		wxExecute("cmd.exe /c cd ytdlp\\temp & del /Q *.wav & del /Q *.mp3", output, errors, wxEXEC_ASYNC);
	}

	wxExecute("cmd.exe /c cd ytdlp & ffmpeg -i \"" + urls[playSongID] + "\" \"temp\\" + results[playSongID] + ".wav\"", output, errors, wxEXEC_ASYNC);
	music.setDuration(results[playSongID]);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	music.forcePlay(urls[playSongID], results[playSongID]);
	waitingSongStart = false;
}

/// <summary>
/// Add song to queue. Song gets downloaded depending on position in queue
/// </summary>
/// <param name="event"></param>
void MyWindow::queueBtnClick(wxCommandEvent& event) {
	queueSongID = event.GetId() - BUTTON - addQueueButtonIndexOffset;
	tempQueueList.push_back(queueSongID);
	if (gettingUrls) {
		waitingAddQueue = true;
	}
	else {
		AddToQueue();
	}
}

void MyWindow::AddToQueue() {
	waitingAddQueue = false;
	if (!std::filesystem::is_directory("ytdlp\\temp")) {
		std::filesystem::create_directory("ytdlp\\temp");
	}
	for (const int& x : tempQueueList) {
		music.addToQueue(urls[x], results[x]);
	}
	tempQueueList.clear();
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
	std::this_thread::sleep_for(std::chrono::milliseconds(50)); //Just so you can't go crazy on the skip button and break the app
	music.skipForward();
}

/// <summary>
/// Reverse Skip button handler
/// </summary>
/// <param name=""></param>
void MyWindow::reverseSkipBtnClick(wxCommandEvent&) {
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	music.skipBackward();
}

/// <summary>
/// When the program is closed, make sure everything is properly destroyed and all temp files are cleaned up.
/// Makes sure the music player is properly closed as well.
/// </summary>
/// <param name=""></param>
void MyWindow::OnClose(wxCloseEvent&) {
	myFrame->Show(false);
	music.closePlayer();
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	ShellExecuteA(NULL, NULL, "cmd.exe", "/c cd ytdlp\\temp & del /Q *.wav", NULL, SW_HIDE);
	ShellExecuteA(NULL, NULL, "cmd.exe", "/c cd ytdlp\\temp & del /Q *.mp3", NULL, SW_HIDE);
	while (isSearching || gettingUrls) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10)); //Give the thread time to finish before fully closing
	}
	ShellExecuteA(NULL, NULL, "cmd.exe", "/c cd ytdlp & del /Q *.txt", NULL, SW_HIDE);
	myFrame->Destroy();
}