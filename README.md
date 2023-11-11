# Nutty Music

This is a GUI build surrounding yt-dlp and ffmpeg, with wxWidgets as the base.
This project will allow the user to search for local music files, or search
youtube music and download/play songs from there. Still a heavy WIP, 
and many featuers are not yet available. 

Current Features:
  Search for Youtube Song and download a temp.wav file, which is deleted on close of the program.

Features to be added:
  Permanent download option for songs.
  Playing songs through the GUI
  Non-Blocking GUI features.
  Local File search for .wav file
  Queue Feature
  Play/Pause feature
  Prettier GUI

This is a Windows-Only software. A Visual Studio solution and vcxproj file are included.
To build, simply open in Visual Studio, and build, either in release or debug mode.
All dependencies and includes are included.

Requires VC++ 20+.
