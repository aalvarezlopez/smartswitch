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
/// Revised by:     AAL (16/12/2013)
/// Last Version:   JRB (21/12/2013)
///
/// FILE CONTENTS:
/// wave.c functions to create a complete wav file, including metadata

#include <stdio.h>
#include "ff.h"
#include "usart.h"
#include "configuration.h"

#include "make_wav.h"

#define NUM_METADATA 6

/** \brief Write data in Little Endian format
 *
 *  \param word: data to wrtie
 *         num_bytes: size of data to write
 *         wave_file: pointer to the destination wave_file
 *
 * \return bytes_written: number of bytes written
 */
unsigned int WriteLittleEndian(unsigned int word, int num_bytes, FIL* wav_file)
{
    unsigned buf;
    unsigned int bytes_written;

    while (num_bytes > 0) {
        buf = word & 0xff;
        f_write(wav_file, &buf, 1, &bytes_written);
        num_bytes--;
        word >>= 8;
    }

    return bytes_written;
}

/** \brief Creates a complete wav file
 *
 *  \param wave_file: pointer to the destination wave_file
 *         num_samples: number of samples
 *         data: array containing points to create the wave file
 *         s_rate: sample rate
 */
void WriteWav(FIL* wav_file, unsigned long num_samples, short int* data,
              int s_rate)
{
    unsigned int sample_rate;
    unsigned int byte_rate;
    unsigned int bytes_written;
    unsigned long i;

    if (s_rate <= 0) { sample_rate = BASE_SAMPLE_RATE; }
    else { sample_rate = (unsigned int) s_rate; }

    byte_rate = sample_rate * NUM_CHANNELS * BYTES_PER_SAMPLE;
    // write RIFF header
    f_write(wav_file, "RIFF", STANDARD_SIZE, &bytes_written);
    WriteLittleEndian(
        CHUNK_BASE_SIZE + BYTES_PER_SAMPLE * num_samples * NUM_CHANNELS,
        STANDARD_SIZE, wav_file);
    f_write(wav_file, "WAVE", STANDARD_SIZE, &bytes_written);
    // write fmt  subchunk
    f_write(wav_file, "fmt ", STANDARD_SIZE, &bytes_written);
    WriteLittleEndian(SUBCHUNK1_SIZE, STANDARD_SIZE,
                      wav_file);          // sub chunk 1 size
    WriteLittleEndian(AUDIO_FORMAT, LITTLE_SIZE, wav_file);
    WriteLittleEndian(NUM_CHANNELS, LITTLE_SIZE, wav_file);
    WriteLittleEndian(sample_rate, STANDARD_SIZE, wav_file);
    WriteLittleEndian(byte_rate, STANDARD_SIZE, wav_file);
    WriteLittleEndian(NUM_CHANNELS * BYTES_PER_SAMPLE, LITTLE_SIZE,
                      wav_file);          // block align
    WriteLittleEndian(BYTE_SIZE * BYTES_PER_SAMPLE, LITTLE_SIZE, wav_file);
    // write subchunk data
    f_write(wav_file, "data", STANDARD_SIZE, &bytes_written);
    WriteLittleEndian(BYTES_PER_SAMPLE * num_samples * NUM_CHANNELS,
                      STANDARD_SIZE, wav_file);

    for (i = 0; i < num_samples; i++) {
        WriteLittleEndian((unsigned int) (data[i]), BYTES_PER_SAMPLE, wav_file);
    }
}

/** \brief Creates the header of a wav file
 *
 *  \param wave_file: pointer to the destination wave_file
 *         num_samples: number of samples
 *         s_rate: sample rate
 */
void WriteWavHeader(FIL* wav_file, unsigned long num_samples, int s_rate)
{
    unsigned int sample_rate;
    unsigned int byte_rate;
    unsigned int bytes_written;
    unsigned int size;

    if (s_rate <= 0) { sample_rate = BASE_SAMPLE_RATE; }
    else { sample_rate = (unsigned int) s_rate; }

    byte_rate = sample_rate * NUM_CHANNELS * BYTES_PER_SAMPLE;
    // write RIFF header
    f_write(wav_file, "RIFF", STANDARD_SIZE, &bytes_written);
    size = CHUNK_BASE_SIZE + BYTES_PER_SAMPLE * num_samples * NUM_CHANNELS + 8
           + STANDARD_SIZE * NUM_METADATA;          //8 = Subchunk3ID + Subchunk3Size (2*4)
    WriteLittleEndian(size, STANDARD_SIZE, wav_file);
    f_write(wav_file, "WAVE", STANDARD_SIZE, &bytes_written);
    // write fmt  subchunk
    f_write(wav_file, "fmt ", STANDARD_SIZE, &bytes_written);
    WriteLittleEndian(SUBCHUNK1_SIZE, STANDARD_SIZE,
                      wav_file);          // sub chunk 1 size
    WriteLittleEndian(AUDIO_FORMAT, LITTLE_SIZE, wav_file);
    WriteLittleEndian(NUM_CHANNELS, LITTLE_SIZE, wav_file);
    WriteLittleEndian(sample_rate, STANDARD_SIZE, wav_file);
    WriteLittleEndian(byte_rate, STANDARD_SIZE, wav_file);
    WriteLittleEndian(NUM_CHANNELS * BYTES_PER_SAMPLE, LITTLE_SIZE,
                      wav_file);          // block align
    WriteLittleEndian(BYTE_SIZE * BYTES_PER_SAMPLE, LITTLE_SIZE, wav_file);
    // write data subchunk
    f_write(wav_file, "data", STANDARD_SIZE, &bytes_written);
    WriteLittleEndian(BYTES_PER_SAMPLE * num_samples * NUM_CHANNELS,
                      STANDARD_SIZE, wav_file);
}

/** \brief Creates the metadata of a wav file
 *
 *  \param wave_file: pointer to the destination wave_file
 */
void WriteWavMetadata(FIL* wav_file, int gain, int cal, int id)
{
    unsigned int bytes_written;
    f_write(wav_file, "META", STANDARD_SIZE, &bytes_written);
    WriteLittleEndian(STANDARD_SIZE * NUM_METADATA, STANDARD_SIZE,
                      wav_file);          // subchunk size = 8 (2 * 4)
    WriteLittleEndian(gain, STANDARD_SIZE, wav_file);          //gain
    WriteLittleEndian(cal, STANDARD_SIZE, wav_file);          //cal
    WriteLittleEndian(id, STANDARD_SIZE, wav_file);          //id
    f_write(wav_file, "viblog_proto", 12, &bytes_written);
}

/** \brief Creates the data of a wav file
 *
 *  \param wave_file: pointer to the destination wave_file
 *         num_samples: number of samples
 *         data: array containing points to create the wave file
 *
 * \return bytes_written: number of bytes written
 */
unsigned int WriteWavData(FIL* wav_file, unsigned long num_samples,
                          short int* data)
{
    unsigned int bytes_written;
    f_write(wav_file, data, num_samples * 2, &bytes_written);
    bytes_written /= 2;
    return bytes_written;
}
