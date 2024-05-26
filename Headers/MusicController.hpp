#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/windows.media.core.h>
#include <winrt/windows.media.playback.h>
#include <string>
#include <filesystem>
#include <fstream>
#include "wx/wxprec.h"

#include "custom/myVector.hpp" //My own custom-built vector class

class MusicController {
	struct Song { //All the data about a song that needs to be kept track of
		std::string songTitle;
		std::filesystem::path path;
		std::string url;
		bool isLocal = false;
	};
public:
	MusicController(); //Default constructor to initialize the player and the queue
	void PlaySong(); //Takes in a Song and passes it to the player to play it
	void AddToQueue(const std::string& songUrl, const std::string&, bool = false); //Takes in the url and title of the song and adds it to the queue
	void ForcePlay(const std::string& songUrl, const std::string&, bool = false); //Resets queue, sets song to play to index 0 and plays it
	void PauseSong();
	void ResumeSong();
	void ClosePlayer(); //Called when program ends
	void SkipForward();
	void SkipBackward();
	void CheckQueue(); //Checks the queue to see what temporary songs should be downloaded (3 songs ahead and behind, if possible)
	bool IsPaused(); //Returns pauseState
	bool IsActive();
	bool IsSongDone();
	void SetDuration(const std::wstring&);
	void RestartSong();
	void ChangeVolume(const bool);
	double GetCurrentVolume();
	const std::string& GetCurrentSongTitle() const&;
	const std::string& GetNextSongTitle() const&;
private:
	winrt::Windows::Media::Playback::MediaPlayer player;
	winrt::Windows::Foundation::TimeSpan duration;
	custom::myVector<Song> queue;
	bool pauseState = false;
	bool playerActive = false;
	int currentQueueIndex = -1;
	double currentVolume = 0.5;

	static constexpr size_t fiveSeconds = 50000000;
};