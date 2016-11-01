# wav2mp3

A command line tool that converts files from wav to mp3

# To install
1. Install lame library, Makefile is pointing to the standard lame install directory on linux, should be editted on windows.
2. run make

*Tested on linux and windows (Cygwin)

# Usage

wav2mp3 "wav-files-directory"

Result files will be placed in the same directory.

*Tested for wav files with 1,2 channels, 8,16,24,32 bits per sample.

*Uses VBR Encoding with quality 2.


# Using

1. POSIX thread pool library (https://github.com/Pithikos/C-Thread-Pool)
2. Lame project (http://lame.sourceforge.net/)


