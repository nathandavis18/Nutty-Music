#include <filesystem>
#include <fstream>
#include <array>
#include <algorithm>

#include "../Headers/mainwindow.hpp"
#include "../Headers/MusicController.hpp"

wxIMPLEMENT_APP(MyApp);

MyWindow::MyWindow() : myFrame(new MyFrame()), music(MusicController()), queueSongID(-500), playSongID(-500), tempQueueList(0) {
	myFrame->Show(true);
	wxStaticText* searchTextLabel = new wxStaticText(myFrame, wxID_ANY, "Search for a Song: ", wxPoint(64, 565), wxSize(125, 25));
	wxStaticText* localSongLabel = new wxStaticText(myFrame, wxID_ANY, "Browse for Local Files: ", wxPoint(25, 600), wxSize(125, 25));
	wxFont font = searchTextLabel->GetFont();
	font.SetPointSize(15);
	font.SetWeight(wxFONTWEIGHT_BOLD);
	searchTextLabel->SetFont(font);
	localSongLabel->SetFont(font);
	text = new wxTextCtrl(myFrame, SEARCH_ID, wxEmptyString, wxPoint(275, 565), wxSize(400, 25), wxTE_PROCESS_ENTER);
	wxButton* playLocalFilesButton = new wxButton(myFrame, wxID_ANY, "Play Local Song", wxPoint(275, 600), wxSize(100, 25));
	wxButton* addQueueLocalFilesButton = new wxButton(myFrame, wxID_ANY, "Add To Queue", wxPoint(380, 600), wxSize(100, 25));

	font.SetWeight(wxFONTWEIGHT_NORMAL);
	nowPlayingLabel = new wxStaticText(myFrame, wxID_ANY, "Now Playing:   ", wxPoint(25, 25), wxSize(300, 25));
	upNextLabel = new wxStaticText(myFrame, wxID_ANY, "       Up Next:   ", wxPoint(26, 80), wxSize(300, 25));
	nowPlayingLabel->SetFont(font);
	upNextLabel->SetFont(font);

	wxButton* playPauseButton = new wxButton(myFrame, wxID_ANY, "|> ||", wxPoint(625, 500), wxSize(50, 50));
	wxButton* forwardSkipButton = new wxButton(myFrame, wxID_ANY, ">|", wxPoint(685, 507), wxSize(35, 35));
	wxButton* reverseSkipButton = new wxButton(myFrame, wxID_ANY, "|<", wxPoint(580, 507), wxSize(35, 35));
	searchingLabel = new wxStaticText(myFrame, LABEL - 1 + songTitleLabelIndexOffset, "Searching . . .", wxPoint(820, 50 + (50 * 0)), wxSize(300, 50));
	searchingLabel->Show(false);

	text->SetHint("Enter a song title/artist");
	text->Bind(wxEVT_TEXT_ENTER, &MyWindow::PressedEnter, this);
	playPauseButton->Bind(wxEVT_BUTTON, &MyWindow::PlayPauseBtnClick, this);
	forwardSkipButton->Bind(wxEVT_BUTTON, &MyWindow::ForwardSkipBtnClick, this);
	reverseSkipButton->Bind(wxEVT_BUTTON, &MyWindow::ReverseSkipBtnClick, this);
	playLocalFilesButton->Bind(wxEVT_BUTTON, &MyWindow::PlayLocalFilesBtnClick, this);
	addQueueLocalFilesButton->Bind(wxEVT_BUTTON, &MyWindow::AddQueueLocalFilesBtnClick, this);

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
		CreateButtons(results);
		CreateLabels(results);
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
	if (music.IsSongDone()) {
		music.SkipForward();
		UpdateNowPlayingUpNext();
	}
}

/// <summary>
/// When the user enters a search keyword, start the search in a new thread
/// </summary>
/// <param name=""></param>
void MyWindow::PressedEnter(wxCommandEvent&) {
	ClearPrevSearch();
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
void MyWindow::CreateButtons(custom::myVector<std::string>& searchResults) {
	for (int i = 0; i < searchResults.size(); ++i) {
		wxButton* playButton = new wxButton(myFrame, BUTTON + i + playButtonIndexOffset, "Play", wxPoint(1000, 50 + (50 * i)), wxSize(75, 25));
		playButton->Bind(wxEVT_BUTTON, &MyWindow::PlayBtnClick, this);
		playButtons.push_back(playButton);

		wxButton* addQueueButton = new wxButton(myFrame, BUTTON + i + addQueueButtonIndexOffset, "Add to Queue", wxPoint(1100, 50 + (50 * i)), wxSize(100, 25));
		addQueueButton->Bind(wxEVT_BUTTON, &MyWindow::QueueBtnClick, this);
		addQueueButtons.push_back(addQueueButton);
	}
}

/// <summary>
/// Creates the labels for each song in the results list
/// </summary>
/// <param name="index"></param>
/// <param name="title"></param>
void MyWindow::CreateLabels(custom::myVector<std::string>& searchResults) {
	for (int i = 0; i < searchResults.size(); ++i) {
		std::string song = searchResults[i].substr(0, searchResults[i].find("BREAKPOINT"));
		wxStaticText* st = new wxStaticText(myFrame, LABEL + i + songTitleLabelIndexOffset, song, wxPoint(600, 50 + (50 * i)), wxSize(300, 50));
		labels.push_back(st);
	}
}

/// <summary>
/// When the play button is clicked, download the song and start it
/// </summary>
/// <param name="event"></param>
void MyWindow::PlayBtnClick(wxCommandEvent& event) {
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
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	music.ForcePlay(urls[playSongID], results[playSongID]);
	waitingSongStart = false;
	UpdateNowPlayingUpNext();
}

/// <summary>
/// Add song to queue. Song gets downloaded depending on position in queue
/// </summary>
/// <param name="event"></param>
void MyWindow::QueueBtnClick(wxCommandEvent& event) {
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
		music.AddToQueue(urls[x], results[x]);
	}
	tempQueueList.clear();
	UpdateNowPlayingUpNext();
}

void MyWindow::UpdateNowPlayingUpNext() {
	std::string npl = "Now Playing:   ";
	std::string unl = "       Up Next:   ";

	npl += music.GetCurrentSongTitle();
	unl += music.GetNextSongTitle();

	nowPlayingLabel->SetLabelText(npl);
	upNextLabel->SetLabelText(unl);
}

/// <summary>
/// Clears the labels, buttons, and vectors from the last search
/// </summary>
void MyWindow::ClearPrevSearch() {
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
void MyWindow::PlayPauseBtnClick(wxCommandEvent&) {
	if (music.IsActive()) {
		if (music.IsPaused()) {
			music.ResumeSong();
		}
		else {
			music.PauseSong();
		}
	}
}

/// <summary>
/// Forward skip button handler
/// </summary>
/// <param name=""></param>
void MyWindow::ForwardSkipBtnClick(wxCommandEvent&) {
	std::this_thread::sleep_for(std::chrono::milliseconds(50)); //Just so you can't go crazy on the skip button and break the app
	music.SkipForward();
	UpdateNowPlayingUpNext();
}

/// <summary>
/// Reverse Skip button handler
/// </summary>
/// <param name=""></param>
void MyWindow::ReverseSkipBtnClick(wxCommandEvent&) {
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	music.SkipBackward();
	UpdateNowPlayingUpNext();
}

void MyWindow::PlayLocalFilesBtnClick(wxCommandEvent&) {
	std::filesystem::path songPath = processData.BrowseFiles();

	std::string songTitle = songPath.string().substr(songPath.string().find_last_of("\\") + 1, songPath.string().find(".") - songPath.string().find_last_of("\\") - 1);
	music.ForcePlay(songPath.string(), songTitle, isLocalFile);
	UpdateNowPlayingUpNext();
}

void MyWindow::AddQueueLocalFilesBtnClick(wxCommandEvent&) {
	std::filesystem::path songPath = processData.BrowseFiles();

	std::string songTitle = songPath.string().substr(songPath.string().find_last_of("\\") + 1, songPath.string().find(".") - songPath.string().find_last_of("\\") - 1);
	music.AddToQueue(songPath.string(), songTitle, isLocalFile);
	UpdateNowPlayingUpNext();
}
/// <summary>
/// When the program is closed, make sure everything is properly destroyed and all temp files are cleaned up.
/// Makes sure the music player is properly closed as well.
/// </summary>
/// <param name=""></param>
void MyWindow::OnClose(wxCloseEvent&) {
	myFrame->Show(false);
	music.ClosePlayer();
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	ShellExecuteA(NULL, NULL, "cmd.exe", "/c cd ytdlp\\temp & del /Q *.wav", NULL, SW_HIDE);
	ShellExecuteA(NULL, NULL, "cmd.exe", "/c cd ytdlp\\temp & del /Q *.mp3", NULL, SW_HIDE);
	while (isSearching || gettingUrls) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10)); //Give the thread time to finish before fully closing
	}
	ShellExecuteA(NULL, NULL, "cmd.exe", "/c cd ytdlp & del /Q *.txt", NULL, SW_HIDE);
	myFrame->Destroy();
}