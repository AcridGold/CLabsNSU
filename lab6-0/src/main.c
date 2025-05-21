#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct Node
{
    int key;
    struct Node* left;
    struct Node* right;
    int height;
} Node;

int height(Node* node)
{
    return node ? node->height : 0;
}

int balanceFactor(Node* node)
{
    return node ? height(node->left) - height(node->right) : 0;
}

Node* Rotate_r(Node* y)
{
    Node* x = y->left;
    Node* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = 1 + (int)fmax(height(y->left), height(y->right));
    x->height = 1 + (int)fmax(height(x->left), height(x->right));

    return x;
}

Node* Rotate_l(Node* x)
{
    Node* y = x->right;
    Node* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = 1 + (int)fmax(height(x->left), height(x->right));
    y->height = 1 + (int)fmax(height(y->left), height(y->right));

    return y;
}

Node* insertion(Node* node, int key)
{
    if (!node)
    {
        Node* new = (Node*)malloc(sizeof(Node));
        if (!new)
        {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
        new->key = key;
        new->left = NULL;
        new->right = NULL;
        new->height = 1;
        return new;
    }

    if (key < node->key)
        node->left = insertion(node->left, key);
    else if (key > node->key)
        node->right = insertion(node->right, key);
    else
        return node;

    node->height = 1 + (int)fmax(height(node->left), height(node->right));
    int balance = balanceFactor(node);

    if (balance > 1 && key < node->left->key)
        return Rotate_r(node);
    if (balance < -1 && key > node->right->key)
        return Rotate_l(node);
    if (balance > 1 && key > node->left->key)
    {
        node->left = Rotate_l(node->left);
        return Rotate_r(node);
    }
    if (balance < -1 && key < node->right->key)
    {
        node->right = Rotate_r(node->right);
        return Rotate_l(node);
    }

    return node;
}

int getTreeHeight(Node* root)
{
    return height(root);
}

void inOrderTraversal(Node* root)
{
    if (root)
    {
        inOrderTraversal(root->left);
        printf("%d ", root->key);
        inOrderTraversal(root->right);
    }
}

void PrintPaths(Node* root, char* path, int level, int max_depth)
{
    if (!root) return;

    if (level >= max_depth)
    {
        printf("Path too long, truncated\n");
        return;
    }

    path[level] = '\0';

    if (!root->left && !root->right)
    {
        printf("%s %d\n", path, root->key);
        return;
    }

    path[level] = 'L';
    PrintPaths(root->left, path, level + 1, max_depth);

    path[level] = 'R';
    PrintPaths(root->right, path, level + 1, max_depth);
}

void freeTree(Node* node)
{
    if (node)
    {
        freeTree(node->left);
        freeTree(node->right);
        free(node);
    }
}

int main(int argc, char* argv[])
{
    int trace = 0;
    FILE* file = stdin;
    const int MAX_N = 2000000;

    if (argc == 3 && strcmp(argv[1], "-t") == 0)
    {
        trace = 1;
        file = fopen(argv[2], "r");
        if (!file)
        {
            printf("Cannot open file %s\n", argv[2]);
            return 1;
        }
    }
    else if (argc == 2 && strcmp(argv[1], "-t") == 0)
    {
        trace = 1;
    }
    else if (argc > 1)
    {
        printf("Usage: %s [-t] [filename]\n", argv[0]);
        return 1;
    }

    int N;
    if (fscanf(file, "%d", &N) != 1 || N < 0 || N > MAX_N)
    {
        printf("Bad input\n");
        if (file != stdin) fclose(file);
        return 1;
    }

    Node* root = NULL;
    for (int i = 0; i < N; i++)
    {
        int key;
        if (fscanf(file, "%d", &key) != 1)
        {
            printf("Bad input\n");
            freeTree(root);
            if (file != stdin) fclose(file);
            return 1;
        }
        root = insertion(root, key);
    }

    if (file != stdin)
        fclose(file);

    printf("%d\n", getTreeHeight(root));

    if (trace)
    {
        inOrderTraversal(root);
        printf("\n");

        char* path = (char*)malloc(MAX_N + 1);
        if (!path)
        {
            fprintf(stderr, "Memory allocation failed for path\n");
            freeTree(root);
            return 1;
        }
        PrintPaths(root, path, 0, MAX_N);
        free(path);
    }

    freeTree(root);
    return 0;
}
