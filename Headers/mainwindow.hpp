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

	bool doneSearching = false;
	bool isSearching = false;
	bool gettingUrls = false;

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
	wxStaticText* nowPlayingLabel;
	wxStaticText* upNextLabel;


	//Functions to control the actions of the UI
	void PressedEnter(wxCommandEvent& event);
	void CreateButtons(custom::myVector<std::string>& searchResults);
	void CreateLabels(custom::myVector<std::string>& searchResults);
	void PlayBtnClick(wxCommandEvent& event);
	void QueueBtnClick(wxCommandEvent& event);
	void PlaySong();
	void AddToQueue();
	void PlayPauseBtnClick(wxCommandEvent& event);
	void ForwardSkipBtnClick(wxCommandEvent& event);
	void ReverseSkipBtnClick(wxCommandEvent& event);
	void StartSearch(const std::string, custom::myVector<std::string>&, bool&, bool&, bool&);
	void ClearPrevSearch();
	void OnClose(wxCloseEvent& event);
	void OnIdle(wxIdleEvent&);
	void PlayLocalFilesBtnClick(wxCommandEvent& event);
	void AddQueueLocalFilesBtnClick(wxCommandEvent& event);
	void UpdateNowPlayingUpNext();

	//If waiting on the songs to be downloaded, set to true
	bool waitingAddQueue = false;
	bool waitingPlaySong = false;
	bool waitingSongStart = false;

	int queueSongID; //The id of the song to be added to queue
	int playSongID; //The id of the song to be played
	
	custom::myVector<int> tempQueueList; //If waitingAddQueue is true, then queueSongID gets added to vector.
										 //Vector is cleared after being used

	static constexpr int playButtonIndexOffset = 1;
	static constexpr int addQueueButtonIndexOffset = 15;
	static constexpr int songTitleLabelIndexOffset = 25;
	static constexpr bool isLocalFile = true;

	std::mutex m;
};