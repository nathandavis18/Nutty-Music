#include <filesystem>
#include <fstream>
#include <array>
#include <algorithm>

#include "../Headers/mainwindow.hpp"
#include "../Headers/MusicController.hpp"

wxIMPLEMENT_APP(MyApp);

MyWindow::MyWindow() {
	this->myFrame = new MyFrame();
	myFrame->Show(true);
	button = new wxButton(myFrame, wxID_ANY, "Test", wxPoint(300, 540), wxSize(300, 100));
	text = new wxTextCtrl(myFrame, wxID_ANY, "Enter a URL", wxPoint(700, 540), wxSize(100, 100), wxTE_PROCESS_ENTER);
	playPauseButton = new wxButton(myFrame, wxID_ANY, "|> ||", wxPoint(100, 100), wxSize(200, 200));
	forwardSkipButton = new wxButton(myFrame, wxID_ANY, ">|", wxPoint(100, 300), wxSize(200, 200));

	text->Bind(wxEVT_TEXT_ENTER, &MyWindow::PressedEnter, this);
	button->Bind(wxEVT_BUTTON, &MyWindow::OnBtnClick, this);
	playPauseButton->Bind(wxEVT_BUTTON, &MyWindow::playPauseBtnClick, this);
	forwardSkipButton->Bind(wxEVT_BUTTON, &MyWindow::forwardSkipBtnClick, this);
}

void MyWindow::OnBtnClick(wxCommandEvent& event) {
	PressedEnter(event);
}

void MyWindow::PressedEnter(wxEvent& event) {
	clearPrevSearch();
	std::string s = text->GetValue().ToStdString();
	s = "\"" + s + "\"";
	std::string cmd = "cmd.exe /c cd ytdlp & yt-dlp --skip-download --playlist-items 1:15 --flat-playlist --default-search https://music.youtube.com/search?q= ";
		cmd += "--print-to-file \"%(title)s BREAKPOINT %(id)s : %(original_url)s\" outputs.txt " + s;
	std::vector<std::string> songList = SearchResult(cmd.c_str());
	std::filesystem::remove("ytdlp/outputs.txt");

	songTitles.clear();
	std::string sub = "";
	std::string urlSub = "";
	std::string shortsUrl = "";
	std::string vidID = "";
	int index = 0;
	for (int i = 0; i < songList.size(); ++i) {
		sub = songList[i].substr(0, songList[i].find("BREAKPOINT"));
		if (sub.substr(0, 3) != "NA" && !(sub.find("loop") != sub.npos || sub.find("Loop") != sub.npos || sub.find("LOOP") != sub.npos)) {
			urlSub = songList[i].substr(songList[i].find("BREAKPOINT") + 11, songList[i].length() - 1);
			shortsUrl = urlSub.substr(urlSub.find(":") + 1, urlSub.length());
			if (shortsUrl.find("browse") != sub.npos) continue;

			songTitles.push_back(sub);
			vidID = urlSub.substr(0, urlSub.find(":"));
			createButton(index, vidID);
			createLabels(index, sub);
			++index;
		}
	}
}

std::vector<std::string> MyWindow::SearchResult(std::string cmd) {
	wxExecute(cmd, output, errors, wxEXEC_ASYNC);

	std::vector<std::string> result;
	std::string line;
	std::fstream stream{ "ytdlp/outputs.txt", stream.in };
	if (!stream.is_open())
		return result;

	while (std::getline(stream, line)) {
		result.push_back(line);
	}

	return result;
}

void MyWindow::createButton(int index, std::string url) {
	wxButton* playButton = new wxButton(myFrame, wxID_HIGHEST + index + playButtonIndexOffset, "Play", wxPoint(900, 50 + (50 * index)), wxSize(75, 25));
	playButton->Bind(wxEVT_BUTTON, &MyWindow::playBtnClick, this);
	playButtons.push_back(playButton);

	wxButton* addQueueButton = new wxButton(myFrame, wxID_HIGHEST + index + addQueueButtonIndexOffset, "Add to Queue", wxPoint(1000, 50 + (50 * index)), wxSize(100, 25));
	addQueueButton->Bind(wxEVT_BUTTON, &MyWindow::queueBtnClick, this);
	addQueueButtons.push_back(addQueueButton);
	urls.push_back(url);
}

void MyWindow::createLabels(int index, std::string title) {
	wxStaticText* st = new wxStaticText(myFrame, wxID_HIGHEST + index + songTitleLabelIndexOffset, title, wxPoint(500, 50 + (50 * index)), wxSize(300, 50));
	labels.push_back(st);
}

void MyWindow::playBtnClick(wxCommandEvent& event) {
	std::filesystem::remove("ytdlp\\temp.wav");
	int id = event.GetId() - wxID_HIGHEST - 1;
	std::string url = urls[id];
	std::string cmd = "-x --audio-format wav -o temp.wav -f ba --use-extractors youtube --downloader ffmpeg -N 4 --throttled-rate 100000K " + url;
	wxExecute("cmd.exe /c cd ytdlp & yt-dlp " + cmd, output, errors, wxEXEC_ASYNC);
	std::filesystem::path path = std::filesystem::current_path() / "ytdlp\\temp.wav";
	std::string sPath = path.string();
	music.playSong(sPath);
}

void MyWindow::queueBtnClick(wxCommandEvent& event) {
	int id = event.GetId() - wxID_HIGHEST - 15;
	std::string url = urls[id];
	music.addToQueue(url);
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
}

void MyWindow::playPauseBtnClick(wxCommandEvent& event) {
	if (music.isSongPlaying()) {
		if (music.getPauseState()) {
			music.resumeSong();
		}
		else {
			music.pauseSong();
		}
	}
}

void MyWindow::forwardSkipBtnClick(wxCommandEvent& event) {
	music.skipForward();
}