#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node* create_node(unsigned char ch, size_t freq, Node* left, Node* right)
{
    Node* node = malloc(sizeof(Node));
    if (node == NULL)
    {
        return NULL;
    }

    node->ch = ch;
    node->freq = freq;
    node->left = left;
    node->right = right;

    if (left != NULL && right != NULL)
    {
        node->height = 1 + (left->height > right->height ? left->height : right->height);
    }
    else if (left != NULL)
    {
        node->height = 1 + left->height;
    }
    else if (right != NULL)
    {
        node->height = 1 + right->height;
    }
    else
    {
        node->height = 1;
    }

    return node;
}

Node* extract_min(Node** heap, size_t* size)
{
    if (*size == 0) return NULL;
    size_t min_idx = 0;
    for (size_t i = 1; i < *size; i++)
    {
        if (heap[i]->freq < heap[min_idx]->freq)
        {
            min_idx = i;
        }
        else if (heap[i]->freq == heap[min_idx]->freq)
        {
            if (heap[i]->height < heap[min_idx]->height)
            {
                min_idx = i;
            }
            else if (heap[i]->height == heap[min_idx]->height)
            {
                int is_leaf_i = (heap[i]->left == NULL && heap[i]->right == NULL);
                int is_leaf_min = (heap[min_idx]->left == NULL && heap[min_idx]->right == NULL);
                if (!is_leaf_i && is_leaf_min)
                {
                    min_idx = i;
                }
                else if (is_leaf_i == is_leaf_min && is_leaf_i)
                {
                    if (heap[i]->ch < heap[min_idx]->ch)
                    {
                        min_idx = i;
                    }
                }
            }
        }
    }
    Node* min_node = heap[min_idx];
    heap[min_idx] = heap[*size - 1];
    (*size)--;

    return min_node;
}

Node* build_tree(size_t freq[256])
{
    Node* heap[256];
    size_t heap_size = 0;
    for (int i = 0; i < 256; i++)
    {
        if (freq[i] > 0)
        {
            Node* node = create_node((unsigned char)i, freq[i], NULL, NULL);
            if (!node)
            {
                while (heap_size > 0)
                {
                    free(heap[--heap_size]);
                }
                return NULL;
            }
            heap[heap_size++] = node;
        }
    }
    if (heap_size == 0) return NULL;
    while (heap_size > 1)
    {
        Node* left = extract_min(heap, &heap_size);
        Node* right = extract_min(heap, &heap_size);
        if (!left || !right)
        {
            while (heap_size > 0)
            {
                free(heap[--heap_size]);
            }
            free(left);
            free(right);
            return NULL;
        }
        Node* parent = create_node(0, left->freq + right->freq, left, right);
        if (!parent)
        {
            free_tree(left);
            free_tree(right);
            while (heap_size > 0)
            {
                free(heap[--heap_size]);
            }
            return NULL;
        }
        heap[heap_size++] = parent;
    }
    return heap[0];
}

void generate_codes(Node* node, Code codes[256], byte current_code[], size_t depth)
{
    if (!node) return;
    if (node->left == NULL && node->right == NULL)
    {
        size_t bytes = (depth + 7) / 8;
        codes[node->ch].code = malloc(bytes);
        if (codes[node->ch].code)
        {
            memset(codes[node->ch].code, 0, bytes);
            memcpy(codes[node->ch].code, current_code, bytes);
            codes[node->ch].bit_length = depth;
        }
        else
        {
            fprintf(stderr, "Failed to allocate code for symbol %u\n", node->ch);
        }
    }
    else
    {
        size_t byte_idx = depth / 8;
        size_t bit_pos = 7 - (depth % 8);
        if (byte_idx >= 32)
        {
            fprintf(stderr, "Depth %zu exceeds current_code size\n", depth);
            return;
        }
        if (node->left)
        {
            current_code[byte_idx] &= ~(1 << bit_pos);
            generate_codes(node->left, codes, current_code, depth + 1);
        }
        if (node->right)
        {
            current_code[byte_idx] |= (1 << bit_pos);
            generate_codes(node->right, codes, current_code, depth + 1);
        }
    }
}

void free_tree(Node* node)
{
    if (node)
    {
        free_tree(node->left);
        free_tree(node->right);
        free(node);
    }
}

void free_codes(Code codes[256])
{
    for (int i = 0; i < 256; i++)
    {
        if (codes[i].code)
        {
            free(codes[i].code);
            codes[i].code = NULL;
        }
    }
}

Node* deserialize_tree(BitReader* reader)
{
    int bit = BitReader_ReadBit(reader);
    if (bit < 0)
    {
        return NULL;
    }

    if (bit == 1)
    {
        uint8_t ch;
        int read_success = BitReader_ReadByte(reader, &ch);
        if (!read_success)
        {
            return NULL;
        }
        Node* leaf = create_node(ch, 0, NULL, NULL);
        if (leaf == NULL)
        {
            return NULL;
        }
        return leaf;
    }

    Node* left = deserialize_tree(reader);
    if (left == NULL)
    {
        return NULL;
    }

    Node* right = deserialize_tree(reader);
    if (right == NULL)
    {
        free_tree(left);
        return NULL;
    }

    Node* node = create_node(0, 0, left, right);
    if (node == NULL)
    {
        free_tree(left);
        free_tree(right);
        return NULL;
    }

    return node;
}
