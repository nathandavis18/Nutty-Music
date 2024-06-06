#include "../Headers/MusicController.hpp"

MusicController::MusicController() : player(winrt::Windows::Media::Playback::MediaPlayer()), queue(), 
					duration(winrt::Windows::Foundation::TimeSpan(0)) {
	player.Volume(currentVolume);
}

/// <summary>
/// Creates an instance of Song and stores it in a vector to keep track of the queue.
/// </summary>
/// <param name="songUrl">The url of the song to be able to be downloaded</param>
/// <param name="songTitle">The title of the song</param>
/// <param name="isLocalSong">To know if the song is a temp file or permanent file</param>
void MusicController::AddToQueue(const std::string& songUrl, const std::string& songTitle, bool isLocalSong) {
	Song s;
	s.songTitle = songTitle;
	s.isLocal = isLocalSong;
	if (isLocalSong) {
		s.path = songUrl;
	}
	else {
		s.url = songUrl;
		s.path = std::filesystem::current_path() / ("../../../../ytdlp\\temp\\" + s.songTitle + ".mp3");
	}
	queue.push_back(s);

	CheckQueue();
}

/// <summary>
/// When the user clicks 'play' instead of 'add to queue'. Forces the chosen song to be played.
/// Creates an instance of Song and adds it to the queue to be played.
/// </summary>
/// <param name="songUrl">The url of the song to be able to be downloaded</param>
/// <param name="songTitle">The title of the song</param>
/// <param name="isLocalSong">To know if the song is a temp file or permanent file</param>
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
		s.path = std::filesystem::current_path() / ("../../../../ytdlp\\temp\\" + s.songTitle + ".wav");
	}
	queue.push_back(s);

	currentQueueIndex = -1;
	SkipForward();
}

/// <summary>
/// Starts the player.
/// </summary>
void MusicController::PlaySong() {
	player.Play();
	playerActive = true;
}

/// <summary>
/// Pauses the player if active and is in a state able to be paused.
/// </summary>
void MusicController::PauseSong() {
	if (player.CanPause()) {
		player.Pause();
		pauseState = true;
	}
}

/// <summary>
/// Checks to see if the song is done, and resumes the player.
/// </summary>
void MusicController::ResumeSong() {
	IsSongDone();
	player.Play();
	pauseState = false;
}

/// <summary>
/// Makes sure everything is cleaned up properly on close.
/// </summary>
void MusicController::ClosePlayer() {
	player.Source(nullptr);
	player.Close();
	queue.clear();
}

/// <summary>
/// Skips to the next song in the queue.
/// </summary>
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

/// <summary>
/// If the player is 5 seconds into the song, it restarts the song. Otherwise, skips backwards in the queue.
/// </summary>
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

/// <summary>
/// Checks the queue for songs within 3 positions of the current song to make sure each song is ready to be played. 
/// </summary>
void MusicController::CheckQueue() {
	for (int i = currentQueueIndex; i < static_cast<int>(queue.size()); ++i) {
		if (i > 0) {
			if (i - 3 <= currentQueueIndex && !std::filesystem::exists(queue[i].path) && !queue[i].isLocal) {
				std::string cmd = " -i \"" + queue[i].url + "\" \"../../../../ytdlp\\temp\\" + queue[i].songTitle + ".mp3\"";
				std::string cmdWav = "-i \"" + queue[i].url + "\" \"temp\\" + queue[i].songTitle + ".wav\"";
				ShellExecuteA(NULL, NULL, "../../../../ytdlp\\ffmpeg.exe", cmdWav.c_str(), NULL, SW_HIDE);
				ShellExecuteA(NULL, NULL, "../../../../ytdlp\\ffmpeg.exe", cmd.c_str(), NULL, SW_HIDE);
				ShellExecuteA(NULL, NULL, "cmd.exe", ("/c cd ../../../../ytdlp\\temp & del /Q \"" + queue[i].songTitle + ".wav\"").c_str(), NULL, SW_HIDE);
			}
			else if (i - 3 > currentQueueIndex) {
				if (!queue[i].isLocal) {
					ShellExecuteA(NULL, NULL, "cmd.exe", ("/c cd ../../../../ytdlp\\temp & del /Q \"" + queue[i].songTitle + ".mp3\"").c_str(), NULL, SW_HIDE);
				}
			}
		}
	}
	for (int i = currentQueueIndex - 1; i > 0; --i) {
		if (i + 3 >= currentQueueIndex && !std::filesystem::exists(queue[i].path) && !queue[i].isLocal) {
			std::string cmd = " -i \"" + queue[i].url + "\" \"../../../../ytdlp\\temp\\" + queue[i].songTitle + ".mp3\"";
			std::string cmdWav = "-i \"" + queue[i].url + "\" \"temp\\" + queue[i].songTitle + ".wav\"";
			ShellExecuteA(NULL, NULL, "../../../../ytdlp\\ffmpeg.exe", cmdWav.c_str(), NULL, SW_HIDE);
			ShellExecuteA(NULL, NULL, "../../../../ytdlp\\ffmpeg.exe", cmd.c_str(), NULL, SW_HIDE);
			ShellExecuteA(NULL, NULL, "cmd.exe", ("/c cd ../../../../ytdlp\\temp & del /Q \"" + queue[i].songTitle + ".wav\"").c_str(), NULL, SW_HIDE);
		}
		else if(i + 3 < currentQueueIndex) {
			if (!queue[i].isLocal) {
				ShellExecuteA(NULL, NULL, "cmd.exe", ("/c cd ../../../../ytdlp\\temp & del /Q \"" + queue[i].songTitle + ".mp3\"").c_str(), NULL, SW_HIDE);
			}
		}
	}
}

/// <summary>
/// Returns the pause state of the player
/// </summary>
/// <returns></returns>
bool MusicController::IsPaused() {
	return pauseState;
}

/// <summary>
/// Returns the active state of the player
/// </summary>
/// <returns></returns>
bool MusicController::IsActive() {
	return playerActive;
}

/// <summary>
/// Determines if a song has reached the end or not
/// </summary>
/// <returns></returns>
bool MusicController::IsSongDone() {
	if (playerActive) {
		auto y = player.PlaybackSession().Position();
		if(y >= duration){
			return true;
		}
	}
	return false;
}

/// <summary>
/// Sets the duration of the current song to determine when it will end. 
/// </summary>
/// <param name="song"></param>
void MusicController::SetDuration(const std::wstring& song) {
	winrt::Windows::Foundation::Uri uri(song.c_str());
	winrt::Windows::Media::Core::MediaSource source = winrt::Windows::Media::Core::MediaSource::CreateFromUri(uri);

	player.Source(source);
	Sleep(250);
	duration = player.PlaybackSession().NaturalDuration();
}

/// <summary>
/// Restarts the current playing song by setting the position to 0. 
/// </summary>
void MusicController::RestartSong() {
	player.PlaybackSession().Position(winrt::Windows::Foundation::TimeSpan(0));
}

/// <summary>
/// Changes the volume of the player. In increments of 10%
/// </summary>
/// <param name="volumeUp"></param>
void MusicController::ChangeVolume(const bool volumeUp) {
	if (volumeUp) {
		currentVolume += 0.1;
	}
	else {
		currentVolume -= 0.1;
	}
	if (currentVolume > 1.0) currentVolume = 1.0;
	if (currentVolume < 0.0) currentVolume = 0.0;
	player.Volume(currentVolume);
}

/// <summary>
/// Gets the current volume for displaying purposes.
/// </summary>
/// <returns></returns>
double MusicController::GetCurrentVolume()
{
	return currentVolume;
}

/// <summary>
/// Gets the current song title for displaying purposes
/// </summary>
/// <returns></returns>
const std::string& MusicController::GetCurrentSongTitle() const& {
	if (queue.size() > 0) {
		return queue[currentQueueIndex].songTitle;
	}
	return "";
}

/// <summary>
/// Gets the upcoming song for displaying purposes.
/// </summary>
/// <returns></returns>
const std::string& MusicController::GetNextSongTitle() const& {
	if (queue.size() > 0) {
		return queue[(currentQueueIndex + 1) % queue.size()].songTitle;
	}
	return "";
}