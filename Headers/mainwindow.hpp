#pragma once
#include <vector>
#include <string>

#include "mainapp.hpp"

class MyWindow {
public:
	MyWindow();
private:
	struct btnUrl {
		wxButton* button;
		std::string url;
	};
	MyFrame* myFrame;
	wxTextCtrl* text;
	wxButton* button;
	std::vector<btnUrl> downloadButtons;
	std::vector<std::string> songTitles;
	void OnBtnClick(wxCommandEvent& event);
	void PressedEnter(wxEvent& event);
	std::vector<std::string> SearchResult(std::string cmd);
	void createButton(int index, std::string url);
	void createLabels(int index, std::string title);
	void testBtn(wxCommandEvent& event);
};