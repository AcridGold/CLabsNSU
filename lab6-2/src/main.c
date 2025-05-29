#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct BTreeNode {
    int *key_values;
    struct BTreeNode **child_nodes;
    int key_count;
    int is_leaf_node;
} BTreeNode;

typedef struct BTree {
    BTreeNode *root;
    int order;
} BTree;

BTreeNode* create_node(int order, int is_leaf_node) {
    BTreeNode *btree_node = malloc(sizeof(BTreeNode));
    if (!btree_node)
    {
        return NULL;
    }
    btree_node->key_values = malloc((2 * order - 1) * sizeof(int));
    if (!btree_node->key_values) {
        free(btree_node);
        return NULL;
    }
    btree_node->child_nodes = calloc(2 * order, sizeof(BTreeNode*));
    if (!btree_node->child_nodes) {
        free(btree_node->key_values);
        free(btree_node);
        return NULL;
    }
    btree_node->key_count = 0;
    btree_node->is_leaf_node = is_leaf_node;
    return btree_node;
}

int binary_search(int *key_array, int count, int key) {
    int left = 0, right = count;
    while (left < right) {
        int mid = (left + right) >> 1;
        if (key_array[mid] <= key) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}

// Разделяет переполненный дочерний узел
void split_child(BTreeNode *parent, int index, int order)
{
    BTreeNode *child_to_split = parent->child_nodes[index];
    BTreeNode *new_node = create_node(order, child_to_split->is_leaf_node);
    if (!new_node)
    {
        return;
    }

    new_node->key_count = order - 1;
    memcpy(new_node->key_values, child_to_split->key_values + order, (order - 1) * sizeof(int));
    if (!child_to_split->is_leaf_node) {
        memcpy(new_node->child_nodes, child_to_split->child_nodes + order, order * sizeof(BTreeNode*));
        memset(child_to_split->child_nodes + order, 0, order * sizeof(BTreeNode*));
    }
    child_to_split->key_count = order - 1;

    memmove(parent->child_nodes + index + 2, parent->child_nodes + index + 1,
            (parent->key_count - index) * sizeof(BTreeNode*));
    parent->child_nodes[index + 1] = new_node;

    memmove(parent->key_values + index + 1, parent->key_values + index,
            (parent->key_count - index) * sizeof(int));
    parent->key_values[index] = child_to_split->key_values[order - 1];
    parent->key_count++;
}

// Вставка ключа в непереполненный узел
void insert_non_full(BTreeNode *btree_node, int key, int order) {
    if (btree_node->is_leaf_node) {
        int index = binary_search(btree_node->key_values, btree_node->key_count, key);
        memmove(btree_node->key_values + index + 1, btree_node->key_values + index,
                (btree_node->key_count - index) * sizeof(int));
        btree_node->key_values[index] = key;
        btree_node->key_count++;
    } else {
        int index = binary_search(btree_node->key_values, btree_node->key_count, key);
        if (btree_node->child_nodes[index]->key_count == 2 * order - 1) {
            split_child(btree_node, index, order);
            if (key > btree_node->key_values[index]) {
                index++;
            }
        }
        insert_non_full(btree_node->child_nodes[index], key, order);
    }
}

// Вставка ключа в дерево
void insert(BTree *btree, int key) {
    if (!btree->root) {
        btree->root = create_node(btree->order, 1);
        if (!btree->root)
        {
            return;
        }
        btree->root->key_values[0] = key;
        btree->root->key_count = 1;
    } else if (btree->root->key_count == 2 * btree->order - 1) {
        BTreeNode *new_root = create_node(btree->order, 0);
        if (!new_root)
        {
            return;
        }
        new_root->child_nodes[0] = btree->root;
        split_child(new_root, 0, btree->order);
        insert_non_full(new_root, key, btree->order);
        btree->root = new_root;
    } else {
        insert_non_full(btree->root, key, btree->order);
    }
}

int height(BTreeNode *btree_node) {
    if (!btree_node)
    {
        return 0;
    }
    if (btree_node->is_leaf_node)
    {
        return 1;
    }
    return height(btree_node->child_nodes[0]) + 1;
}

BTree* create_tree(int order) {
    BTree *btree = malloc(sizeof(BTree));
    if (!btree)
    {
        return NULL;
    }
    btree->order = order;
    btree->root = NULL;
    return btree;
}

void free_node(BTreeNode *btree_node, int order) {
    if (!btree_node)
    {
        return;
    }
    if (!btree_node->is_leaf_node) {
        for (int i = 0; i <= btree_node->key_count; i++) {
            free_node(btree_node->child_nodes[i], order);
        }
    }
    free(btree_node->key_values);
    free(btree_node->child_nodes);
    free(btree_node);
}

void free_tree(BTree *btree) {
    if (btree) {
        free_node(btree->root, btree->order);
        free(btree);
    }
}

int main()
{
    int order, key_count, key;
    if (scanf("%d", &order) != 1)
    {
        return 0;
    }
    if (scanf("%d", &key_count) != 1){
        return 0;
    }
    BTree *btree = create_tree(order);
    if (!btree)
    {
        return 0;
    }
    for (int i = 0; i < key_count; i++) {
        if (scanf("%d", &key) != 1) {
            free_tree(btree);
            return 0;
        }
        insert(btree, key);
    }
    printf("%d\n", height(btree->root));
    free_tree(btree);
    return 0;
}
