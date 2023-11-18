#include "../Headers/MusicController.hpp"

MusicController::MusicController() : player(winrt::Windows::Media::Playback::MediaPlayer()), queue(0) {}

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
	s.path = std::filesystem::current_path() / ("ytdlp\\temp\\" + s.songTitle + ".mp3");
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
}

void MusicController::pauseSong() {
	if (player.CanPause()) {
		player.Pause();
		isPaused = true;
	}
}

void MusicController::resumeSong() {
	if (player.CanPause()) {
		player.Play();
		isPaused = false;
	}
}

void MusicController::closePlayer() {
	player.Close();
	queue.clear();
}

void MusicController::skipForward() {
	if (currentQueueIndex + 1 < queue.size()) {
		playSong(queue[++currentQueueIndex]);
	}
	checkQueue();
}

void MusicController::skipBackward() {
	if (currentQueueIndex > 0) {
		playSong(queue[--currentQueueIndex]);
	}
	checkQueue();
}

void MusicController::checkQueue() {
	for (int i = currentQueueIndex; i < queue.size(); ++i) {
		if (i - 3 <= currentQueueIndex && !std::filesystem::exists(queue[i].path)) {
			std::string cmd = "-x --audio-format mp3 -o ytdlp\\temp\\%(title)s.mp3 -f ba --use-extractors youtube --downloader ffmpeg -N 4 --throttled-rate 100000K " + queue[i].url;
			ShellExecuteA(NULL, NULL, "ytdlp\\yt-dlp.exe", cmd.c_str(), NULL, SW_HIDE);
		}
		else if(i - 3 > currentQueueIndex) {
			if (!queue[i].isLocal) std::filesystem::remove(queue[i].path);
		}
	}
	for (int i = currentQueueIndex - 1; i >= 0; --i) {
		if (i + 3 >= currentQueueIndex && !std::filesystem::exists(queue[i].path)) {
			std::string cmd = "-x --audio-format mp3 -o ytdlp\\temp\\%(title)s.mp3 -f ba --use-extractors youtube --downloader ffmpeg -N 4 --throttled-rate 100000K " + queue[i].url;
			ShellExecuteA(NULL, NULL, "ytdlp\\yt-dlp.exe", cmd.c_str(), NULL, SW_HIDE);
		}
		else if(i + 3 < currentQueueIndex) {
			if (!queue[i].isLocal) std::filesystem::remove(queue[i].path);
		}
	}
}

bool MusicController::getPauseState() {
	return isPaused;
}