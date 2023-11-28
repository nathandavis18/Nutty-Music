#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include "custom/myVector.hpp" //My own custom-built vector class
#include "wx/wxprec.h"
#include "wx/utils.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "DataProcessing.hpp"
#include "mainapp.hpp"
#include "MusicController.hpp"

class MyWindow {
public:
	MyWindow();
private:
	enum {
		SEARCH_ID = 2, BUTTON = 3, LABEL = 4
	};

	/// <summary>
	/// ====================================================================
	/// A LOT OF THIS NEEDS TO BE CLEANED UP, THIS IS NEXT ON THE TO-DO LIST
	/// ====================================================================
	/// </summary>
	wxArrayString output, errors;
	MyFrame* myFrame;
	MusicController music;
	DataProcessing processData;
	custom::myVector<std::string> results;
	custom::myVector<std::string> urls;
	custom::myVector<wxStaticText*> labels;
	custom::myVector<wxButton*> playButtons;
	custom::myVector<wxButton*> addQueueButtons;

	wxTextCtrl* text;
	wxStaticText* searchingLabel;


	//Functions to control the actions of the UI
	void PressedEnter(wxCommandEvent& event);
	void createButtons(custom::myVector<std::string>& searchResults);
	void createLabels(custom::myVector<std::string>& searchResults);
	void playBtnClick(wxCommandEvent& event);
	void queueBtnClick(wxCommandEvent& event);
	void playPauseBtnClick(wxCommandEvent& event);
	void forwardSkipBtnClick(wxCommandEvent& event);
	void reverseSkipBtnClick(wxCommandEvent& event);
	void StartSearch(const std::string, custom::myVector<std::string>&, bool&, bool&);
	void clearPrevSearch();
	void OnClose(wxCloseEvent& event);
	void OnIdle(wxIdleEvent&);


	static constexpr int playButtonIndexOffset = 1;
	static constexpr int addQueueButtonIndexOffset = 15;
	static constexpr int songTitleLabelIndexOffset = 25;
	bool doneSearching = false;
	bool isSearching = false;
	bool gettingUrls = false;
	std::mutex m;
};