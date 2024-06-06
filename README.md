# Nutty Music

This is a GUI build surrounding yt-dlp and ffmpeg, with wxWidgets as the base.
This project will allow the user to search for local music files, or search
youtube music and download/play songs from there. Still a heavy WIP, 
and many featuers are not yet available. 

Current Features:
  
    Search for Youtube Song and download a temp.wav file, which is deleted on close of the program.
    Playing Music either from Youtube or Local song
    GUI is (almost) never blocked
    Queue is implemented for both Youtube music and Local songs. Can be mixed together
    Able to play/pause the player
    Able to Skip forward/backward within the queue
        --If only 1 song is in queue, this just restarts the song

This is a Windows-Only software. A Visual Studio solution and vcxproj file are included.
To build, simply open in Visual Studio, and build, either in release or debug mode.

May need to add the following to the environment variable in project properties:

    PATH=$(ProjectDir)\dll;%PATH%

To use, you will need a copy of ffmpeg and yt-dlp stored in the root of the ytdlp folder.

[Link to yt-dlp download](https://github.com/yt-dlp/yt-dlp/releases/download/2024.05.27/yt-dlp.exe) | [Link to ffmpeg download](https://github.com/yt-dlp/FFmpeg-Builds/releases/download/latest/ffmpeg-master-latest-win64-gpl.zip)

For ffmpeg, you only need ffmpeg.exe. ffprobe and ffplay can be discarded.

Requires VC++ and Python 3.7+.
