#include <string>

#ifndef _WAVE2MP3_
#define _WAVE2MP3_

/**
 * Will convert an input file of wav format <in> to mp3 and wrire result to <out> fileout
 * Returns 1 on success, -1 on failure
 */

int wavToMp3(std::string& in, std::string& out);

#endif
