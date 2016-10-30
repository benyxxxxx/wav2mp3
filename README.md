# wav2mp3

A command line tool that converts files from wav to mp3

# To install
1. Install lame library
2. Setup LAME_DIR variable in Makefile pointing to the lame install directory

run make

*Compiles on linux and windows (Cygwin)

# Usage

wav2mp3 "wav-files-directory"

Using Unix path separators.

Result files will be placed in the same directory.

Tested for wav files with 1-2 channels, 8-16 bits per sample.



# Using

1. POSIX thread pool library (https://github.com/Pithikos/C-Thread-Pool)
2. Lame project (http://lame.sourceforge.net/)


