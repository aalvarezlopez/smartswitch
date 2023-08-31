/// make_wav.c
///
/// Copyright (C) 2013 INGEN10 Ingenieria SL
/// http://www.ingen10.com
///
/// LEGAL NOTICE:
/// All information contained herein is, and remains property of INGEN10 Ingenieria SL.
/// Dissemination of this information or reproduction of this material is strictly
/// forbidden unless prior written permission is obtained from its owner.
/// ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC PERFORMANCE, OR PUBLIC DISPLAY
/// OF, OR THROUGH USE OF THIS SOURCE CODE IS STRICTLY PROHIBITED, AND IT IS A VIOLATION
/// OF INTERNATIONAL TRADE TREATIES AND LAWS.
/// THE RECEIPT OR POSSESSION OF THIS DOCUMENT DOES NOT CONVEY OR IMPLY ANY RIGHTS.
///
/// Authored by:   AV (27/11/2013)
/// Revised by:
/// Last Version:
///
/// FILE CONTENTS:
/// wave.h functions to create a complete wav file, including metadata

#ifndef MAKE_WAV_H_
#define MAKE_WAV_H_

#define NUM_CHANNELS 1
#define BYTES_PER_SAMPLE 2
#define BIG_SIZE 12
#define STANDARD_SIZE 4
#define LITTLE_SIZE 2
#define NUM_METADATA_BIG 1
#define NUM_METADATA_STANDARD 3
#define SUBCHUNK1_SIZE 16
#define CHUNK_BASE_SIZE 36
#define AUDIO_FORMAT 1 /* 1 = PCM */
#define BYTE_SIZE 8

void WriteWav(FIL* wave_file, unsigned long num_samples, short int* data,
              int s_rate);
void WriteWavHeader(FIL* wav_file, unsigned long num_samples, int s_rate);

unsigned int WriteWavData(FIL* wav_file, unsigned long num_samples,
                          short int* data);
void WriteWavMetadata(FIL* wav_file, int gain, int cal, int id);
#endif /* MAKE_WAV_H_ */
