#include "Bits.h"


void BitWriter_Init(BitWriter* writer, FILE* out)
{
    writer->buffer = 0;
    writer->bit_count = 0;
    writer->out = out;
    writer->total_bits = 0;
}

void BitWriter_WriteBit(BitWriter* writer, int bit)
{
    writer->buffer = (writer->buffer << 1) | (bit & 1);
    writer->bit_count++;
    writer->total_bits++;
    if (writer->bit_count == 8)
    {
        fwrite(&writer->buffer, 1, 1, writer->out);
        writer->buffer = 0;
        writer->bit_count = 0;
    }
}

void BitWriter_WriteBits(BitWriter* writer, uint8_t* data, size_t n_bits)
{
    for (size_t i = 0; i < n_bits; i++)
    {
        size_t byte_pos = i / 8;
        size_t bit_pos = 7 - (i % 8);
        int bit = (data[byte_pos] >> bit_pos) & 1;
        BitWriter_WriteBit(writer, bit);
    }
}

void BitWriter_WriteByte(BitWriter* writer, uint8_t byte)
{
    for (int i = 7; i >= 0; i--)
    {
        BitWriter_WriteBit(writer, (byte >> i) & 1);
    }
}

void BitWriter_Flush(BitWriter* writer)
{
    if (writer->bit_count > 0)
    {
        writer->buffer <<= (8 - writer->bit_count);
        fwrite(&writer->buffer, 1, 1, writer->out);
        writer->total_bits += (8 - writer->bit_count);
        writer->buffer = 0;
        writer->bit_count = 0;
    }
}

void BitReader_Init(BitReader* reader, FILE* file)
{
    reader->file = file;
    reader->buffer = 0;
    reader->bit_pos = -1;
    reader->eof = 0;
}

int BitReader_ReadBit(BitReader* reader)
{
    if (reader->eof) return -1;
    if (reader->bit_pos < 0)
    {
        int byte = fgetc(reader->file);
        if (byte == EOF)
        {
            reader->eof = 1;
            return -1;
        }
        reader->buffer = (uint8_t)byte;
        reader->bit_pos = 7;
    }
    int bit = (reader->buffer >> reader->bit_pos) & 1;
    reader->bit_pos--;
    return bit;
}

int BitReader_ReadByte(BitReader* reader, uint8_t* byte)
{
    *byte = 0;
    for (int i = 7; i >= 0; i--)
    {
        int bit = BitReader_ReadBit(reader);
        if (bit < 0) return 0;
        *byte |= (bit << i);
    }
    return 1;
}
