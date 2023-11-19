#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/windows.media.core.h>
#include <winrt/windows.media.playback.h>
#include <string>
#include <filesystem>

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
	void playSong(Song&); //Takes in a Song and passes it to the player to play it
	void addToQueue(std::string& songUrl, std::string = "", bool = false); //Takes in the url and title of the song and adds it to the queue
	void forcePlay(std::string& songUrl, std::string = "", bool = false); //Resets queue, sets song to play to index 0 and plays it
	void pauseSong();
	void resumeSong();
	void closePlayer(); //Called when program ends
	void skipForward();
	void skipBackward();
	void checkQueue(); //Checks the queue to see what temporary songs should be downloaded (3 songs ahead and behind, if possible)
	bool isPaused(); //Returns pauseState
	bool isActive();
private:
	winrt::Windows::Media::Playback::MediaPlayer player;
	custom::myVector<Song> queue;
	bool pauseState = false;
	bool playerActive = false;
	int currentQueueIndex = -1;
};