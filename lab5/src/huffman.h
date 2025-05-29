#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <stdint.h>
#include <stdio.h>
#include "Bits.h"

typedef uint8_t byte;

typedef struct Node {
    unsigned char ch;
    size_t freq;
    size_t height;
    struct Node* left;
    struct Node* right;
} Node;

typedef struct {
    byte* code;
    size_t bit_length;
} Code;

Node* create_node(unsigned char ch, size_t freq, Node* left, Node* right);
Node* build_tree(size_t freq[256]);
void generate_codes(Node* node, Code codes[256], byte current_code[], size_t depth);
void free_tree(Node* node);
void free_codes(Code codes[256]);
Node* deserialize_tree(BitReader* reader);

#endif
