#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <thread>
#include <chrono>
#include "custom/myVector.hpp" //My own custom-built vector class
#include "wx/wxprec.h"
#include "wx/utils.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "YtdlpCalls.hpp"
#include "mainapp.hpp"
#include "MusicController.hpp"

class MyWindow {
public:
	MyWindow();
private:
	enum {
		SEARCH_ID = 2, BUTTON = 3, LABEL = 4
	};
	std::thread t;
	wxArrayString output, errors;
	MyFrame* myFrame;
	wxTextCtrl* text;
	MusicController music;
	wxButton* test;
	wxButton* playPauseButton;
	wxButton* forwardSkipButton;
	wxButton* reverseSkipButton;
	YtdlpCalls calls;
	custom::myVector<std::string> urls;
	custom::myVector<std::string> songTitles;
	custom::myVector<wxStaticText*> labels;
	custom::myVector<wxButton*> playButtons;
	custom::myVector<wxButton*> addQueueButtons;
	wxStaticText* lbl;


	//Functions to control the actions of the UI
	void PressedEnter(wxCommandEvent& event);
	void createButton(int index, std::string url);
	void createLabels(int index, std::string title);
	void playBtnClick(wxCommandEvent& event);
	void queueBtnClick(wxCommandEvent& event);
	void playPauseBtnClick(wxCommandEvent& event);
	void forwardSkipBtnClick(wxCommandEvent& event);
	void reverseSkipBtnClick(wxCommandEvent& event);
	void clearPrevSearch();
	void OnClose(wxCloseEvent& event);
	void searchResults(std::wstring);
	void OnIdle(wxIdleEvent&);


	static constexpr int playButtonIndexOffset = 1;
	static constexpr int addQueueButtonIndexOffset = 15;
	static constexpr int songTitleLabelIndexOffset = 25;
	bool doneSearching = false;
	bool isSearching = false;
};