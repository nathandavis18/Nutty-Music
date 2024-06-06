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

	wxButton* volumeUpButton = new wxButton(myFrame, VOLUME_UP, "Volume Up", wxPoint(1100, 350), wxSize(100, 100));
	wxButton* volumeDownButton = new wxButton(myFrame, VOLUME_DOWN, "Volume Down", wxPoint(1100, 525), wxSize(100, 100));
	currentVolumeLabel = new wxStaticText(myFrame, wxID_ANY, "Volume: 50%", wxPoint(1085, 470), wxSize(100, 50));
	currentVolumeLabel->SetFont(font);
	currentVolumeLabel->Show(true);

	font.SetPointSize(15);
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
	volumeUpButton->Bind(wxEVT_BUTTON, &MyWindow::ChangeVolume, this);
	volumeDownButton->Bind(wxEVT_BUTTON, &MyWindow::ChangeVolume, this);

	myFrame->Bind(wxEVT_CLOSE_WINDOW, &MyWindow::OnClose, this);
	myFrame->Bind(wxEVT_IDLE, &MyWindow::OnIdle, this);
}

/// <summary>
/// Controls what happens while the app is idling
/// </summary>
/// <param name="">Empty parameter, required for wxWidgets</param>
void MyWindow::OnIdle(wxIdleEvent&) {
	wxWakeUpIdle();
	if (doneSearching) {
		std::filesystem::remove("../../../../ytdlp\\searchResults.txt");
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
/// <param name="">Empty parameter, required for wxWidgets</param>
void MyWindow::PressedEnter(wxCommandEvent&) {
	ClearPrevSearch();
	std::string search = text->GetValue().ToStdString();
	if (search.find_first_not_of(' ') != search.npos) {
		std::thread t(&MyWindow::StartSearch, this, search, std::ref(results), std::ref(doneSearching), std::ref(isSearching), std::ref(gettingUrls));
		t.detach();
	}
}

/// <summary>
/// This function runs on a separate thread as to not lock the GUI.
/// It searches for songs using yt-dlp and returns the result to the user.
/// </summary>
/// <param name="searchString">The string the user enters</param>
/// <param name="vecResults">The search results</param>
/// <param name="doneWithSearch">A boolean value that is checked to see if this function is done running</param>
/// <param name="searching">A boolean to know if the 'is searching' text should be displayed</param>
/// <param name="gettingDownloadUrls">A boolean to know if the song can be played or not</param>
void MyWindow::StartSearch(const std::string searchString, std::vector<std::string>& vecResults, bool& doneWithSearch, bool& searching, bool& gettingDownloadUrls) {
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
/// Creates the play and add to queue buttons for each search result returned.
/// </summary>
/// <param name="searchResults">The strings of search results</param>
void MyWindow::CreateButtons(std::vector<std::string>& searchResults) {
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
/// Creates the song lables so the user knows what each song is, based on a predetermined format
/// </summary>
/// <param name="searchResults">The strings of all search results</param>
void MyWindow::CreateLabels(std::vector<std::string>& searchResults) {
	for (int i = 0; i < searchResults.size(); ++i) {
		std::string song = searchResults[i].substr(0, searchResults[i].find("BREAKPOINT"));
		wxStaticText* st = new wxStaticText(myFrame, LABEL + i + songTitleLabelIndexOffset, song, wxPoint(600, 50 + (50 * i)), wxSize(300, 50));
		labels.push_back(st);
	}
}

/// <summary>
/// When the play button is clicked, download the song and start it
/// </summary>
/// <param name="event">Button event to get the button's unique ID</param>
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
	if (!std::filesystem::is_directory("../../../../ytdlp\\temp")) {
		std::filesystem::create_directory("../../../../ytdlp\\temp");
	}
	else {
		wxExecute("cmd.exe /c cd ../../../../ytdlp\\temp & del /Q *.wav & del /Q *.mp3", output, errors, wxEXEC_ASYNC);
	}

	wxExecute("cmd.exe /c cd ../../../../ytdlp & ffmpeg -i \"" + urls[playSongID] + "\" \"temp\\" + results[playSongID] + ".wav\"", output, errors, wxEXEC_ASYNC);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	music.ForcePlay(urls[playSongID], results[playSongID]);
	waitingSongStart = false;
	UpdateNowPlayingUpNext();
}

/// <summary>
/// Button controller for changing the player volume.
/// Increments by 10%.
/// </summary>
/// <param name="event">Empty parameter, required for wxWidgets</param>
void MyWindow::ChangeVolume(wxCommandEvent& event)
{
	bool volumeUp = event.GetId() == VOLUME_UP;
	if (volumeUp) {
		music.ChangeVolume(true);
	}
	else {
		music.ChangeVolume(false);
	}

	int volume = music.GetCurrentVolume() * 100;
	std::string lblText = "Volume: " + std::to_string(volume) + "%";
	currentVolumeLabel->SetLabelText(lblText);
}

/// <summary>
/// Add song to queue. Song gets downloaded depending on position in queue
/// </summary>
/// <param name="event">The button click event, used to get the button's unique ID</param>
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

/// <summary>
/// Button Event Controller for adding a song to the queue list.
/// If the temp file directory for the music player doesn't exist, it creates it.
/// </summary>
void MyWindow::AddToQueue() {
	waitingAddQueue = false;
	if (!std::filesystem::is_directory("../../../../ytdlp\\temp")) {
		std::filesystem::create_directory("../../../../ytdlp\\temp");
	}
	for (const int& x : tempQueueList) {
		music.AddToQueue(urls[x], results[x]);
	}
	tempQueueList.clear();
	UpdateNowPlayingUpNext();
}

/// <summary>
/// Updates the displayed song titles every time a song finishes playing/is skipped.
/// </summary>
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
/// <param name="">Empty parameter, required for wxWidgets</param>
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
/// <param name="">Empty parameter, required for wxWidgets</param>
void MyWindow::ForwardSkipBtnClick(wxCommandEvent&) {
	std::this_thread::sleep_for(std::chrono::milliseconds(50)); //Just so you can't go crazy on the skip button and break the app
	music.SkipForward();
	UpdateNowPlayingUpNext();
}

/// <summary>
/// Reverse Skip button handler
/// </summary>
/// <param name="">Empty parameter, required for wxWidgets</param>
void MyWindow::ReverseSkipBtnClick(wxCommandEvent&) {
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	music.SkipBackward();
	UpdateNowPlayingUpNext();
}

/// <summary>
/// Opens a file dialog to allow choosing a local song.
/// </summary>
/// <param name="">Empty parameter, required for wxWidgets</param>
void MyWindow::PlayLocalFilesBtnClick(wxCommandEvent&) {
	std::filesystem::path songPath = processData.BrowseFiles();

	std::string songTitle = songPath.string().substr(songPath.string().find_last_of("\\") + 1, songPath.string().find(".") - songPath.string().find_last_of("\\") - 1);
	music.ForcePlay(songPath.string(), songTitle, isLocalFile);
	UpdateNowPlayingUpNext();
}

/// <summary>
/// Opens a file dialog to allow choosing a local song to add to queue.
/// </summary>
/// <param name="">Empty parameter, required for wxWidgets</param>
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
/// <param name="">Empty parameter, required for wxWidgets</param>
void MyWindow::OnClose(wxCloseEvent&) {
	myFrame->Show(false);
	music.ClosePlayer();
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	ShellExecuteA(NULL, NULL, "cmd.exe", "/c cd ../../../../ytdlp\\temp & del /Q *.wav", NULL, SW_HIDE);
	ShellExecuteA(NULL, NULL, "cmd.exe", "/c cd ../../../../ytdlp\\temp & del /Q *.mp3", NULL, SW_HIDE);
	while (isSearching || gettingUrls) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10)); //Give the thread time to finish before fully closing
	}
	ShellExecuteA(NULL, NULL, "cmd.exe", "/c cd ../../../../ytdlp & del /Q *.txt", NULL, SW_HIDE);
	myFrame->Destroy();
}