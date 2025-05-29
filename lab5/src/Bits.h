#ifndef BITS_H
#define BITS_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct
{
    uint8_t buffer;
    int bit_count;
    FILE* out;
    size_t total_bits;
} BitWriter;

typedef struct
{
    FILE* file;
    uint8_t buffer;
    int bit_pos;
    int eof;
} BitReader;

void BitWriter_Init(BitWriter* writer, FILE* out);
void BitWriter_WriteBit(BitWriter* writer, int bit);
void BitWriter_WriteBits(BitWriter* writer, uint8_t* data, size_t n_bits);
void BitWriter_WriteByte(BitWriter* writer, uint8_t byte);
void BitWriter_Flush(BitWriter* writer);
void BitReader_Init(BitReader* reader, FILE* file);
int BitReader_ReadBit(BitReader* reader);
int BitReader_ReadByte(BitReader* reader, uint8_t* byte);

#endif
