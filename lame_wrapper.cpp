/*
 *      Get Audio routines source file
 *
 *      Copyright (c) 1999 Albert L Faber
 *                    2008-2012 Robert Hegemann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */


#include <stdio.h>
#include <string>
#include <lame.h>
#include <iostream>

static int const WAV_ID_RIFF = 0x52494646; /* "RIFF" */
static int const WAV_ID_WAVE = 0x57415645; /* "WAVE" */
static int const WAV_ID_FMT = 0x666d7420; /* "fmt " */
static int const WAV_ID_DATA = 0x64617461; /* "data" */
static short const WAVE_FORMAT_EXTENSIBLE = 0xFFFE;

static long make_even_number_of_bytes_in_length(long x)
{
  if ((x & 0x01) != 0) {
        return x + 1;
  }
    return x;
}

static int read_16_bits_low_high(FILE * fp)
{
  unsigned char bytes[2] = { 0, 0 };
  fread(bytes, 1, 2, fp);
  {
    int32_t const low = bytes[0];
    int32_t const high = (signed char) (bytes[1]);
    return (high << 8) | low;
  }
}


static int read_32_bits_low_high(FILE * fp)
{
  unsigned char bytes[4] = { 0, 0, 0, 0 };
  fread(bytes, 1, 4, fp);
  {
    int32_t const low = bytes[0];
    int32_t const medl = bytes[1];
    int32_t const medh = bytes[2];
    int32_t const high = (signed char) (bytes[3]);
    return (high << 24) | (medh << 16) | (medl << 8) | low;
  }
}

static int read_16_bits_high_low(FILE * fp)
{
  unsigned char bytes[2] = { 0, 0 };
  fread(bytes, 1, 2, fp);
  {
    int32_t const low = bytes[1];
    int32_t const high = (signed char) (bytes[0]);
    return (high << 8) | low;
  }
}

static int read_32_bits_high_low(FILE * fp)
{
    unsigned char bytes[4] = { 0, 0, 0, 0 };
    fread(bytes, 1, 4, fp);
    {
        int32_t const low = bytes[3];
        int32_t const medl = bytes[2];
        int32_t const medh = bytes[1];
        int32_t const high = (signed char) (bytes[0]);
        return (high << 24) | (medh << 16) | (medl << 8) | low;
    }
}

/* global data for get_audio.c. */
typedef struct get_audio_data_struct {
  int     pcmbitwidth;
  int     pcmswapbytes;
} get_audio_data;


static int parse_wave_header(lame_global_flags * gfp, FILE * sf, get_audio_data& global)
{
  int     format_tag = 0;
  int     channels = 0;
  int     block_align = 0;
  int     bits_per_sample = 0;
  int     samples_per_sec = 0;
  int     avg_bytes_per_sec = 0;
  
  
  int     is_wav = 0;
  long    data_length = 0, file_length, subSize = 0;
  int     loop_sanity = 0;

  int     type = read_32_bits_high_low(sf);

  if (type != WAV_ID_RIFF) {
    std::cout << "File is not wave"  << std::endl;
    return -1;
  }

  file_length = read_32_bits_high_low(sf);
  if (read_32_bits_high_low(sf) != WAV_ID_WAVE)
    return -1;
  
  for (loop_sanity = 0; loop_sanity < 20; ++loop_sanity) {
    int     type = read_32_bits_high_low(sf);
    
    if (type == WAV_ID_FMT) {
      subSize = read_32_bits_low_high(sf);
      subSize = make_even_number_of_bytes_in_length(subSize);
      if (subSize < 16) {
	/*DEBUGF(
	  "'fmt' chunk too short (only %ld bytes)!", subSize);  */
	return -1;
      }
      
      format_tag = read_16_bits_low_high(sf);
      subSize -= 2;
      channels = read_16_bits_low_high(sf);
      subSize -= 2;
      samples_per_sec = read_32_bits_low_high(sf);
      subSize -= 4;
      avg_bytes_per_sec = read_32_bits_low_high(sf);
      subSize -= 4;
      block_align = read_16_bits_low_high(sf);
      subSize -= 2;
      bits_per_sample = read_16_bits_low_high(sf);
      subSize -= 2;
      
      /* WAVE_FORMAT_EXTENSIBLE support */
      if ((subSize > 9) && (format_tag == WAVE_FORMAT_EXTENSIBLE)) {
	read_16_bits_low_high(sf); /* cbSize */
	read_16_bits_low_high(sf); /* ValidBitsPerSample */
	read_32_bits_low_high(sf); /* ChannelMask */
	/* SubType coincident with format_tag for PCM int or float */
	format_tag = read_16_bits_low_high(sf);
	subSize -= 10;
      }

      /* DEBUGF("   skipping %d bytes\n", subSize); */
      
      if (subSize > 0) {
	if (fseek(sf, (long) subSize, SEEK_CUR) != 0)
	  return -1;
      };
      
    }
    else if (type == WAV_ID_DATA) {
      subSize = read_32_bits_low_high(sf);
      data_length = subSize;
      is_wav = 1;
      /* We've found the audio data. Read no further! */
      break;
      
    }
    else {
      subSize = read_32_bits_low_high(sf);
      subSize = make_even_number_of_bytes_in_length(subSize);
      if (fseek(sf, (long) subSize, SEEK_CUR) != 0) {
	return -1;
      }
    }
  }
  if (is_wav) {

    /* make sure the header is sane */
    if (-1 == lame_set_num_channels(gfp, channels)) {
      std::cout << "Unsupported number of channels" << channels << std::endl;
      return -1;
    }
    lame_set_in_samplerate(gfp, samples_per_sec);
    
    global. pcmbitwidth = bits_per_sample;
    lame_set_num_samples(gfp, data_length / (channels * ((bits_per_sample + 7) / 8)));
    return 1;
  }
  return -1;
}


static int
unpack_read_samples(const int samples_to_read, const int bytes_per_sample,
                    const int swap_order, int *sample_buffer, FILE * pcm_in)
{
  size_t  samples_read;
  int     i;
  int    *op;              /* output pointer */
  unsigned char *ip = (unsigned char *) sample_buffer; /* input pointer */
  const int b = sizeof(int) * 8;
  
#define GA_URS_IFLOOP( ga_urs_bps )					\
  if( bytes_per_sample == ga_urs_bps )					\
      for( i = samples_read * bytes_per_sample; (i -= bytes_per_sample) >=0;)
  
    samples_read = fread(sample_buffer, bytes_per_sample, samples_to_read, pcm_in);
    op = sample_buffer + samples_read;

    if (swap_order == 0) {
        GA_URS_IFLOOP(1)
            * --op = ip[i] << (b - 8);
        GA_URS_IFLOOP(2)
            * --op = ip[i] << (b - 16) | ip[i + 1] << (b - 8);
        GA_URS_IFLOOP(3)
            * --op = ip[i] << (b - 24) | ip[i + 1] << (b - 16) | ip[i + 2] << (b - 8);
        GA_URS_IFLOOP(4)
            * --op =
            ip[i] << (b - 32) | ip[i + 1] << (b - 24) | ip[i + 2] << (b - 16) | ip[i + 3] << (b -
                                                                                              8);
    }
    else {
        GA_URS_IFLOOP(1)
            * --op = (ip[i] ^ 0x80) << (b - 8) | 0x7f << (b - 16); /* convert from unsigned */
        GA_URS_IFLOOP(2)
            * --op = ip[i] << (b - 8) | ip[i + 1] << (b - 16);
        GA_URS_IFLOOP(3)
            * --op = ip[i] << (b - 8) | ip[i + 1] << (b - 16) | ip[i + 2] << (b - 24);
        GA_URS_IFLOOP(4)
            * --op =
            ip[i] << (b - 8) | ip[i + 1] << (b - 16) | ip[i + 2] << (b - 24) | ip[i + 3] << (b -
                                                                                             32);
    }
#undef GA_URS_IFLOOP

    return (samples_read);
}


static void splitChannels(int* all, int* left, int* right, int samples) {
  for (int i = 0, j = 0; i < samples; i+=2, j++) {
    left[j] = all[i];
    right[j] = all[i+1];
  }
}


int wavToMp3(const char* in, const char* out) {

  std::string fileIn(in);
  std::string fileOut(out);
  
  int read, write;


  FILE *pcm = fopen(fileIn.c_str(), "rb");  //source    
  if (pcm <= 0) {
    std::cerr << "Cannot open " << fileIn << std::endl;
    return -1;
  } 
  
  FILE *mp3 = fopen(fileOut.c_str(), "wb");  //output
  if (pcm <= 0) {
    std::cerr << "Cannot open " << fileOut << std::endl;
    fclose(pcm); 
    return -1;
  } 

  
  const int PCM_SIZE = 1152;
  const int MP3_SIZE = PCM_SIZE * 2;
  
  int pcm_buffer_l[PCM_SIZE>>1];
  int pcm_buffer_r[PCM_SIZE>>1];

  int pcm_buffer[PCM_SIZE];

  unsigned char mp3_buffer[MP3_SIZE];
 
  lame_t lame = lame_init();

  lame_set_VBR(lame, vbr_default);
  lame_set_quality(lame, 2);
  
  get_audio_data global;
  if (parse_wave_header(lame, pcm, global) < 0) {
    std::cerr << "Cannot parse header " << fileOut << std::endl;
    fclose(pcm); 
    fclose(mp3); 
    return -1;
  }   
  lame_init_params(lame);

  do {
    
    read = unpack_read_samples(PCM_SIZE, global.pcmbitwidth/8, 
			       global.pcmbitwidth == 8 ? 1 : 0, 
			       pcm_buffer, pcm);
    
    if (read == 0) {

      write = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);

    } else {
      
      if (lame_get_num_channels(lame) == 2) {

	splitChannels(pcm_buffer, pcm_buffer_l, pcm_buffer_r, read);	
	write = lame_encode_buffer_int (lame,pcm_buffer_l, pcm_buffer_r, read/2, mp3_buffer, MP3_SIZE);
	
      } else {

	write = lame_encode_buffer_int (lame, pcm_buffer, 0, read, mp3_buffer, MP3_SIZE);
	
      }
    }

    fwrite(mp3_buffer, write, 1, mp3);

  } while (read != 0);

  
  lame_close(lame);
  fclose(mp3);
  fclose(pcm);
  
  return 1;
}
