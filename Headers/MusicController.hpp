#pragma once
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/windows.media.core.h>
#include <winrt/windows.media.playback.h>
#include <Windows.h>
#include <string>
#include <vector>
#include <filesystem>

class MusicController {
public:
	MusicController();
	void playSong(std::string&);
	void addToQueue(std::string songPath, bool = false);
	void pauseSong();
	void resumeSong();
	void stopSong();
	void skipForward();
	void checkQueue();
	size_t getQueueSize();
	bool getPauseState();
	bool isSongPlaying();
private:
	struct Song {
		std::string path;
		bool isLocal = false;
	};
	winrt::Windows::Media::Playback::MediaPlayer player;
	std::vector<Song> queue;
	bool isPaused = false;
	bool isSongLoaded = false;
	int currentQueueIndex = -1;
	int lastDownloadedIndex = currentQueueIndex;
	int countDownloaded = 0;
	int currentDownload = 0;
};