#include "huffman.h"
#include "Bits.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define SIZE (1024 * 1024) // 1 МБ

void serialize_tree(Node* node, BitWriter* writer)
{
    if (!node) return;
    if (node->left == NULL && node->right == NULL)
    {
        BitWriter_WriteBit(writer, 1);
        BitWriter_WriteByte(writer, node->ch);
    }
    else
    {
        BitWriter_WriteBit(writer, 0);
        serialize_tree(node->left, writer);
        serialize_tree(node->right, writer);
    }
}

void compress(FILE* in, FILE* out)
{
    long start_pos = ftell(in);
    uint32_t freq[256] = {0};
    uint32_t total_chars = 0;
    uint8_t* buffer = malloc(SIZE);
    if (!buffer)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, SIZE, in)) > 0)
    {
        for (size_t i = 0; i < bytes_read; i++)
        {
            freq[buffer[i]]++;
            total_chars++;
        }
    }

    // Один символ
    int unique_symbols = 0;
    uint8_t single_symbol = 0;
    for (int i = 0; i < 256; i++)
    {
        if (freq[i] > 0)
        {
            unique_symbols++;
            single_symbol = i;
            if (unique_symbols > 1) break;
        }
    }

    fseek(in, start_pos, SEEK_SET);

    fwrite(&total_chars, sizeof(uint32_t), 1, out);

    if (total_chars == 0)
    {
        free(buffer);
        return;
    }

    BitWriter writer;
    BitWriter_Init(&writer, out);

    if (unique_symbols == 1)
    {
        // Один символ
        BitWriter_WriteBit(&writer, 1);
        BitWriter_WriteByte(&writer, single_symbol);
    }
    else
    {
        size_t freq_tree[256];
        for (int i = 0; i < 256; i++)
        {
            freq_tree[i] = freq[i];
        }
        Node* root = build_tree(freq_tree);
        if (!root)
        {
            fprintf(stderr, "Failed to build Huffman tree\n");
            free(buffer);
            return;
        }

        Code codes[256] = {{0}};
        byte current_code[32] = {0};
        generate_codes(root, codes, current_code, 0);

        serialize_tree(root, &writer);

        while ((bytes_read = fread(buffer, 1, SIZE, in)) > 0)
        {
            for (size_t i = 0; i < bytes_read; i++)
            {
                uint8_t ch = buffer[i];
                if (codes[ch].bit_length > 0)
                {
                    BitWriter_WriteBits(&writer, codes[ch].code, codes[ch].bit_length);
                }
            }
        }

        free_tree(root);
        free_codes(codes);
    }

    BitWriter_Flush(&writer);
    free(buffer);
}

void decompress(FILE* in, FILE* out)
{
    uint32_t total_chars;
    if (fread(&total_chars, sizeof(uint32_t), 1, in) != 1)
    {
        fprintf(stderr, "Failed to read total_chars\n");
        return;
    }

    if (total_chars == 0)
    {
        return;
    }

    BitReader reader;
    BitReader_Init(&reader, in);

    Node* root = deserialize_tree(&reader);
    if (!root)
    {
        fprintf(stderr, "Failed to deserialize Huffman tree\n");
        return;
    }

    if (root->left == NULL && root->right == NULL)
    {
        // Один символ
        for (uint32_t i = 0; i < total_chars; i++)
        {
            fputc(root->ch, out);
        }
    }
    else
    {
        uint32_t chars_decoded = 0;
        while (chars_decoded < total_chars)
        {
            Node* current = root;
            while (current->left != NULL && current->right != NULL)
            {
                int bit = BitReader_ReadBit(&reader);
                if (bit < 0)
                {
                    fprintf(stderr, "Unexpected end of data\n");
                    free_tree(root);
                    return;
                }
                current = bit == 0 ? current->left : current->right;
            }
            if (current->left == NULL && current->right == NULL)
            {
                fputc(current->ch, out);
                chars_decoded++;
            }
            else
            {
                fprintf(stderr, "Invalid Huffman tree\n");
                free_tree(root);
                return;
            }
        }
    }

    free_tree(root);
}

int main(int argc, char* argv[])
{
    // .exe -c/-d in.txt out.txt
    if (argc == 4 && (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "-d") == 0))
    {
        char* input_file = argv[2];
        char* output_file = argv[3];
        char mode = argv[1][1];

        FILE* in = fopen(input_file, "rb");
        if (!in)
        {
            fprintf(stderr, "Cannot open %s\n", input_file);
            return 0;
        }
        FILE* out = fopen(output_file, "wb");
        if (!out)
        {
            fprintf(stderr, "Cannot open %s\n", output_file);
            fclose(in);
            return 0;
        }
        if (mode == 'c')
        {
            compress(in, out);
        }
        else
        {
            decompress(in, out);
        }
        fclose(in);
        fclose(out);
    }
    else
    {
        // Тестировщик
        FILE* in = fopen("in.txt", "rb");
        if (!in)
        {
            fprintf(stderr, "Cannot open in.txt\n");
            return 0;
        }
        int mode = fgetc(in);
        if (mode == EOF || (mode != 'c' && mode != 'd'))
        {
            fprintf(stderr, "Invalid mode or empty file\n");
            fclose(in);
            return 0;
        }
        FILE* out = fopen("out.txt", "wb");
        if (!out)
        {
            fprintf(stderr, "Cannot open out.txt\n");
            fclose(in);
            return 0;
        }
        if (mode == 'c')
        {
            compress(in, out);
        }
        else
        {
            decompress(in, out);
        }
        fclose(in);
        fclose(out);
    }
    return 0;
}
