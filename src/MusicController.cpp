#include "../Headers/MusicController.hpp"

MusicController::MusicController(bool* downloading) : player(winrt::Windows::Media::Playback::MediaPlayer()), tempPlayer(winrt::Windows::Media::Playback::MediaPlayer()), 
													queue(), isDownloadingSong(downloading) {}

void MusicController::addToQueue(std::string& songUrl, std::string songTitle, bool isLocalSong) {
	Song s;
	s.songTitle = songTitle;
	s.isLocal = isLocalSong;
	s.url = songUrl;
	s.path = std::filesystem::current_path() / ("ytdlp\\temp\\" + s.songTitle + ".mp3");
	queue.push_back(s);

	checkQueue();
}

void MusicController::forcePlay(std::string& songUrl, std::string songTitle, bool isLocalSong) {
	queue.clear();
	Song s;
	s.isLocal = isLocalSong;
	s.songTitle = songTitle;
	s.url = songUrl;
	s.path = std::filesystem::current_path() / ("ytdlp\\temp\\" + s.songTitle + ".wav");
	queue.push_back(s);

	currentQueueIndex = -1;
	skipForward();
}

void MusicController::playSong(Song& s) {
	std::filesystem::path path = s.path;
	winrt::Windows::Foundation::Uri uri(path.c_str());
	winrt::Windows::Media::Core::MediaSource source = winrt::Windows::Media::Core::MediaSource::CreateFromUri(uri);

	player.Source(source);
	player.Play();
	playerActive = true;
}

void MusicController::pauseSong() {
	if (player.CanPause()) {
		player.Pause();
		pauseState = true;
	}
}

void MusicController::resumeSong() {
	isSongDone();
	player.Play();
	pauseState = false;
}

void MusicController::closePlayer() {
	player.Source(nullptr);
	player.Close();
	queue.clear();
}

void MusicController::skipForward() {
	if (currentQueueIndex < static_cast<int>(queue.size() - 1)) {
		++currentQueueIndex;
		playSong(queue[currentQueueIndex]);
	}
	checkQueue();
}

void MusicController::skipBackward() {
	if (currentQueueIndex > 0) {
		--currentQueueIndex;
		playSong(queue[currentQueueIndex]);
	}
	checkQueue();
}

void MusicController::checkQueue() {
	for (int i = currentQueueIndex; i < static_cast<int>(queue.size()); ++i) {
		if (i > 0) {
			if (i - 3 <= currentQueueIndex && !std::filesystem::exists(queue[i].path)) {
				std::string cmd = " -i \"" + queue[i].url + "\" \"ytdlp\\temp\\" + queue[i].songTitle + ".mp3\"";
				std::string cmdWav = "-i \"" + queue[i].url + "\" \"temp\\" + queue[i].songTitle + ".wav\"";
				ShellExecuteA(NULL, NULL, "ytdlp\\ffmpeg.exe", cmdWav.c_str(), NULL, SW_HIDE);
				ShellExecuteA(NULL, NULL, "ytdlp\\ffmpeg.exe", cmd.c_str(), NULL, SW_HIDE);
				ShellExecuteA(NULL, NULL, "cmd.exe", ("/c cd ytdlp\\temp & del /Q \"" + queue[i].songTitle + ".wav\"").c_str(), NULL, SW_HIDE);
			}
			else if (i - 3 > currentQueueIndex) {
				if (!queue[i].isLocal) {
					ShellExecuteA(NULL, NULL, "cmd.exe", ("/c cd ytdlp\\temp & del /Q \"" + queue[i].songTitle + ".mp3\"").c_str(), NULL, SW_HIDE);
				}
			}
		}
	}
	for (int i = currentQueueIndex - 1; i > 0; --i) {
		if (i + 3 >= currentQueueIndex && !std::filesystem::exists(queue[i].path)) {
			std::string cmd = " -i \"" + queue[i].url + "\" \"ytdlp\\temp\\" + queue[i].songTitle + ".mp3\"";
			std::string cmdWav = "-i \"" + queue[i].url + "\" \"temp\\" + queue[i].songTitle + ".wav\"";
			ShellExecuteA(NULL, NULL, "ytdlp\\ffmpeg.exe", cmdWav.c_str(), NULL, SW_HIDE);
			ShellExecuteA(NULL, NULL, "ytdlp\\ffmpeg.exe", cmd.c_str(), NULL, SW_HIDE);
			ShellExecuteA(NULL, NULL, "cmd.exe", ("/c cd ytdlp\\temp & del /Q \"" + queue[i].songTitle + ".wav\"").c_str(), NULL, SW_HIDE);
		}
		else if(i + 3 < currentQueueIndex) {
			if (!queue[i].isLocal) {
				ShellExecuteA(NULL, NULL, "cmd.exe", ("/c cd ytdlp\\temp & del /Q \"" + queue[i].songTitle + ".mp3\"").c_str(), NULL, SW_HIDE);
			}
		}
	}
}

bool MusicController::isPaused() {
	return pauseState;
}

bool MusicController::isActive() {
	return playerActive;
}

bool MusicController::isSongDone() {
	if (playerActive) {
		auto y = player.PlaybackSession().Position();
		if(y >= duration){
			return true;
		}
	}
	return false;
}

void MusicController::setDuration(std::string& song) {
	std::filesystem::path path = std::filesystem::current_path() / ("ytdlp\\temp\\" + song + ".wav");
	winrt::Windows::Foundation::Uri uri(path.c_str());
	winrt::Windows::Media::Core::MediaSource source = winrt::Windows::Media::Core::MediaSource::CreateFromUri(uri);

	player.Source(source);
	Sleep(250);
	duration = player.PlaybackSession().NaturalDuration();
}