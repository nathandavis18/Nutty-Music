#pragma once
#include <vector>
#include <string>

#include "mainapp.hpp"
#include "MusicController.hpp"

class MyWindow {
public:
	MyWindow();
private:
	wxArrayString output, errors;
	MyFrame* myFrame;
	MusicController music;
	wxTextCtrl* text;
	wxButton* button;
	wxButton* playPauseButton;
	wxButton* forwardSkipButton;
	wxButton* reverseSkipButton;
	std::vector<std::string> urls;
	std::vector<std::string> songTitles;
	std::vector<wxStaticText*> labels;
	std::vector<wxButton*> playButtons;
	std::vector<wxButton*> addQueueButtons;


	//Functions to control the actions of the UI
	void OnBtnClick(wxCommandEvent& event);
	void PressedEnter(wxEvent& event);
	std::vector<std::string> SearchResult(std::string cmd);
	void createButton(int index, std::string url);
	void createLabels(int index, std::string title);
	void playBtnClick(wxCommandEvent& event);
	void queueBtnClick(wxCommandEvent& event);
	void playPauseBtnClick(wxCommandEvent& event);
	void forwardSkipBtnClick(wxCommandEvent& event);
	void clearPrevSearch();


	static constexpr int playButtonIndexOffset = 1;
	static constexpr int addQueueButtonIndexOffset = 15;
	static constexpr int songTitleLabelIndexOffset = 25;
};