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

Features to be added:

    Permanent download option for songs.
    Prettier GUI

This is a Windows-Only software. A Visual Studio solution and vcxproj file are included.
To build, simply open in Visual Studio, and build, either in release or debug mode.

For the development build, you will need to have your own copy of ffmpeg.exe and yt-dlp.exe.
These executables need to be placed in the root of the ytdlp folder.

For the release version, these executables will be provided for you (once I am satisfied with the result and upload a release version).

Requires VC++ and Python 3.7+.
