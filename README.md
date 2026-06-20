# project-audio
A CLI-based audio playback program written in C++ which uses the [miniaudio library](https://github.com/mackron/miniaudio)

## Features:<br>
### Basic functions:<br>
Loading and playback of audio files<br>
Pausing<br>
Stopping playback<br>
Picking volume<br>
Seeking<br>
Checking elapsed time<br>
Checking song length<br>
### Queue functions:<br>
Playback of a queue of songs<br>
Adding and removing songs to queue<br>
Shuffling<br>
Moving songs around<br>
Listing songs in queue<br>
Skipping<br>
Playing previous track<br>
Selection of a specific song to play<br>

## Building:<br>
Build using CMake >=3.10

## To be added:<br>
Handle audio device change (plugging in other headphones, speakers, etc.)<br>
Make a modern GUI with FTXUI or Qt
