#include "../Headers/MusicController.hpp"

MusicController::MusicController() : player(winrt::Windows::Media::Playback::MediaPlayer()), queue(), 
					duration(winrt::Windows::Foundation::TimeSpan(0)) {}

void MusicController::AddToQueue(const std::string& songUrl, const std::string& songTitle, bool isLocalSong) {
	Song s;
	s.songTitle = songTitle;
	s.isLocal = isLocalSong;
	if (isLocalSong) {
		s.path = songUrl;
	}
	else {
		s.url = songUrl;
		s.path = std::filesystem::current_path() / ("ytdlp\\temp\\" + s.songTitle + ".mp3");
	}
	queue.push_back(s);

	CheckQueue();
}

void MusicController::ForcePlay(const std::string& songUrl, const std::string& songTitle, bool isLocalSong) {
	queue.clear();
	Song s;
	s.isLocal = isLocalSong;
	s.songTitle = songTitle;
	if (isLocalSong) {
		s.path = songUrl;
	}
	else {
		s.url = songUrl;
		s.path = std::filesystem::current_path() / ("ytdlp\\temp\\" + s.songTitle + ".wav");
	}
	queue.push_back(s);

	currentQueueIndex = -1;
	SkipForward();
}

void MusicController::PlaySong() {
	player.Play();
	playerActive = true;
}

void MusicController::PauseSong() {
	if (player.CanPause()) {
		player.Pause();
		pauseState = true;
	}
}

void MusicController::ResumeSong() {
	IsSongDone();
	player.Play();
	pauseState = false;
}

void MusicController::ClosePlayer() {
	player.Source(nullptr);
	player.Close();
	queue.clear();
}

void MusicController::SkipForward() {
	if (currentQueueIndex < static_cast<int>(queue.size() - 1)) {
		++currentQueueIndex;
		SetDuration(queue[currentQueueIndex].path);
		PlaySong();
	}
	else if (currentQueueIndex == queue.size() - 1) {
		currentQueueIndex = 0;
		SetDuration(queue[currentQueueIndex].path);
		PlaySong();
	}
	CheckQueue();
}

void MusicController::SkipBackward() {
	if (player.PlaybackSession().Position() > winrt::Windows::Foundation::TimeSpan(fiveSeconds)) {
		RestartSong();
	}
	else{
		--currentQueueIndex;
		if (currentQueueIndex < 0) {
			currentQueueIndex = queue.size() - 1;
		}
		SetDuration(queue[currentQueueIndex].path);
		PlaySong();
	}
	CheckQueue();
}

void MusicController::CheckQueue() {
	for (int i = currentQueueIndex; i < static_cast<int>(queue.size()); ++i) {
		if (i > 0) {
			if (i - 3 <= currentQueueIndex && !std::filesystem::exists(queue[i].path) && !queue[i].isLocal) {
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
		if (i + 3 >= currentQueueIndex && !std::filesystem::exists(queue[i].path) && !queue[i].isLocal) {
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

bool MusicController::IsPaused() {
	return pauseState;
}

bool MusicController::IsActive() {
	return playerActive;
}

bool MusicController::IsSongDone() {
	if (playerActive) {
		auto y = player.PlaybackSession().Position();
		if(y >= duration){
			return true;
		}
	}
	return false;
}

void MusicController::SetDuration(const std::wstring& song) {
	winrt::Windows::Foundation::Uri uri(song.c_str());
	winrt::Windows::Media::Core::MediaSource source = winrt::Windows::Media::Core::MediaSource::CreateFromUri(uri);

	player.Source(source);
	Sleep(250);
	duration = player.PlaybackSession().NaturalDuration();
}

void MusicController::RestartSong() {
	player.PlaybackSession().Position(winrt::Windows::Foundation::TimeSpan(0));
}

void MusicController::ChangeVolume(const double change) {
	auto x = player.Volume();
	x += change;
	if (x > 1.0) x = 1.0;
	if (x < 0.0) x = 0.0;
	player.Volume(x);
}

const std::string& MusicController::GetCurrentSongTitle() const& {
	if (queue.size() > 0) {
		return queue[currentQueueIndex].songTitle;
	}
	return "";
}

const std::string& MusicController::GetNextSongTitle() const& {
	if (queue.size() > 0) {
		return queue[(currentQueueIndex + 1) % queue.size()].songTitle;
	}
	return "";
}