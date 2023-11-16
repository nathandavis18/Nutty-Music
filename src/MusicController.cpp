#include "../Headers/MusicController.hpp"

MusicController::MusicController() : player(winrt::Windows::Media::Playback::MediaPlayer()), queue(0) {}

void MusicController::addToQueue(std::string songPath, bool isLocalSong) {
	Song s;
	s.isLocal = isLocalSong;
	s.path = songPath;
	queue.push_back(s);

	checkQueue();
}

size_t MusicController::getQueueSize() {
	return queue.size();
}

void MusicController::playSong(std::string &sPath) {
	std::filesystem::path path = sPath;
	winrt::Windows::Foundation::Uri uri(path.c_str());
	winrt::Windows::Media::Core::MediaSource source = winrt::Windows::Media::Core::MediaSource::CreateFromUri(uri);

	player.Source(source);
	player.Play();
	isSongLoaded = true;

	Sleep(500);
	if(sPath.find("ytdlp\\temp") != sPath.npos)
		std::filesystem::remove(sPath);
}

void MusicController::pauseSong() {
	if (player.CanPause()) {
		player.Pause();
		isPaused = true;
	}
}

void MusicController::resumeSong() {
	player.Play();
	isPaused = false;
}

void MusicController::stopSong() {
	player.Pause();
	player.Position(winrt::Windows::Foundation::TimeSpan());
	isSongLoaded = false;
}

void MusicController::skipForward() {
	if (currentQueueIndex + 1 < queue.size()) {
		playSong(queue[++currentQueueIndex].path);
	}
	if (!queue[currentQueueIndex].isLocal) {
		--countDownloaded;
	}
	checkQueue();
}

void MusicController::checkQueue() {
	for (int i = lastDownloadedIndex + 1; i < queue.size(); ++i) {
		if (countDownloaded == 3) break;
		if (!queue[i].isLocal) {
			std::string cmd = "-x --audio-format wav -o ytdlp\\temp" + std::to_string((currentDownload % 3) + 1) + ".wav --use-extractors youtube --downloader ffmpeg -N 4 --throttled-rate 100000K " + queue[i].path;
			ShellExecuteA(NULL, NULL, "ytdlp\\yt-dlp.exe", cmd.c_str(), NULL, SW_HIDE);
			std::string file = "ytdlp\\temp" + std::to_string((currentDownload % 3) + 1) + ".wav";
			std::filesystem::path tmpPath = std::filesystem::current_path() / file;
			queue[i].path = tmpPath.string();
			lastDownloadedIndex = i;
			++countDownloaded;
			++currentDownload;
		}
	}
}

bool MusicController::getPauseState() {
	return isPaused;
}

bool MusicController::isSongPlaying() {
	return isSongLoaded;
}