#include <filesystem>
#include <fstream>
#include <array>

#include "../Headers/mainwindow.hpp"
#include "../Headers/MusicController.hpp"

wxIMPLEMENT_APP(MyApp);

MyWindow::MyWindow() {
	this->myFrame = new MyFrame();
	myFrame->Show(true);
	button = new wxButton(myFrame, wxID_ANY, "Test", wxPoint(300, 540), wxSize(300, 100));
	text = new wxTextCtrl(myFrame, wxID_ANY, "Enter a URL", wxPoint(700, 540), wxSize(100, 100), wxTE_PROCESS_ENTER);

	text->Bind(wxEVT_TEXT_ENTER, &MyWindow::PressedEnter, this);
	button->Bind(wxEVT_BUTTON, &MyWindow::OnBtnClick, this);
}

void MyWindow::OnBtnClick(wxCommandEvent& event) {
	wxMessageBox("Button was clicked", "Testing Button", wxOK | wxICON_INFORMATION);

	std::string s = text->GetValue().ToStdString();
	wxMessageBox(s, "TEST", wxOK | wxICON_INFORMATION);
	if (s.substr(0, 4) == "http" || s.substr(0, 4) == "www.") {
		std::string cmd = "cd ytdlp & yt-dlp -x " + s;
		system(cmd.c_str());
	}
	else {
		s = "\"" + s + "\"";
		std::string cmd = "cd ytdlp & yt-dlp --default-search ytsearch ytsearch5:" + s + " --skip-download --get-title --get-duration --flat-playlist --quiet --ignore-errors > outputs.txt";
		//wxMessageBox(result, "S", wxOK | wxICON_INFORMATION);
	}
}

void MyWindow::PressedEnter(wxEvent& event) {
	std::string s = text->GetValue().ToStdString();
	if (s.substr(0, 4) == "http" || s.substr(0, 4) == "www.") {
		std::string cmd = "cd ytdlp & yt-dlp -x " + s;
		system(cmd.c_str());
	}
	else {
		s = "\"" + s + "\"";
		std::string cmd = "cmd.exe /c cd ytdlp & yt-dlp --skip-download --playlist-items 1:10 --flat-playlist --print-to-file \"%(title)s BREAKPOINT %(id)s\" outputs.txt --default-search https://music.youtube.com/search?q= " + s;
		std::vector<std::string> songList = SearchResult(cmd.c_str());
		std::filesystem::remove("ytdlp/outputs.txt");

		std::string sub = "";
		std::string urlSub = "";
		for (int i = 0; i < songList.size(); ++i) {
			sub = songList[i].substr(0, songList[i].find("BREAKPOINT"));
			if (sub != "NA ") {
				songTitles.push_back(sub);
				urlSub = songList[i].substr(songList[i].find("BREAKPOINT") + 11, songList[i].length() - 1);
				createButton(i, urlSub);
				createLabels(i, sub);
			}
		}
	}
}

std::vector<std::string> MyWindow::SearchResult(std::string cmd) {
	wxArrayString output,errors;
	wxExecute(cmd, output, errors, wxEXEC_SYNC);

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
	wxButton* button = new wxButton(myFrame, wxID_HIGHEST + index + 1, "Play", wxPoint(900, 50 + (50 * index)), wxSize(75, 25));
	btnUrl tmp;
	tmp.button = button;
	tmp.url = url;
	downloadButtons.push_back(tmp);
	button->Bind(wxEVT_BUTTON, &MyWindow::testBtn, this);
}

void MyWindow::createLabels(int index, std::string title) {
	wxStaticText* st = new wxStaticText(myFrame, wxID_ANY, title, wxPoint(500, 50 + (50 * index)), wxSize(300, 25));
}

void MyWindow::testBtn(wxCommandEvent& event) {
	int id = event.GetId() - wxID_HIGHEST - 1;
	std::string url = downloadButtons[id].url;
	std::string cmd = "cmd.exe /c cd ytdlp & yt-dlp -x --audio-format wav -o temp.wav --use-extractors youtube --downloader ffmpeg -N 4 --throttled-rate 100000K " + url;
	wxArrayString output, errors;
	wxExecute(cmd, output, errors, wxEXEC_SYNC);
}