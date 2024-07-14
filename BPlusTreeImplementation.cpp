#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <fstream>

using namespace std;

static int BUCKET_SIZE = 3;

class BPlusTreeNode
{
public:
    BPlusTreeNode **nodes = NULL;
    int *keys;

    int size = 0;
    bool isLeaf = false;

    BPlusTreeNode()
    {
        keys = new int[BUCKET_SIZE];
        nodes = new BPlusTreeNode *[BUCKET_SIZE + 1];

        for (int i = 0; i < BUCKET_SIZE + 1; i++)
        {
            nodes[i] = NULL;
        }
    }

    bool isFull()
    {
        return this->size >= BUCKET_SIZE;
    }
};

class BPlusTree
{
public:
    BPlusTreeNode *root;

    BPlusTree()
    {
        root = nullptr;
    }

    bool exists(int value)
    {
        // We just return a boolean if the int exists in the tree

        BPlusTreeNode *whereToInsert = root;

        // Find if there is a leaf node that could POSSIBLY contain the value
        while (!whereToInsert->isLeaf)
        {
            for (int i = 0; i < whereToInsert->size; i++)
            {
                if (whereToInsert->keys[i] > value)
                {
                    whereToInsert = whereToInsert->nodes[i];

                    break;
                }

                if (whereToInsert->size - 1 == i)
                {
                    whereToInsert = whereToInsert->nodes[i + 1];

                    break;
                }
            }
        }

        // Check if the possible node contains the value
        for (int i = 0; i < whereToInsert->size; i++)
        {
            if (whereToInsert->keys[i] == value)
            {
                return true;
            }
        }

        return false;
    }

    void insert(int value)
    {
        // The easiest case, the first entry
        if (this->root == nullptr)
        {
            this->root = new BPlusTreeNode();

            this->root->keys[0] = value;
            this->root->isLeaf = true;
            this->root->size = 1;

            return;
        }

        BPlusTreeNode *whereToInsert = this->root;
        BPlusTreeNode *parent = NULL;

        // Find the leaf node where we need to insert based on the B-TREE logic - this is the same code as the one above
        while (!whereToInsert->isLeaf)
        {
            parent = whereToInsert;

            for (int i = 0; i < whereToInsert->size; i++)
            {
                if (whereToInsert->keys[i] > value)
                {
                    whereToInsert = whereToInsert->nodes[i];

                    break;
                }

                if (whereToInsert->size - 1 == i)
                {
                    whereToInsert = whereToInsert->nodes[i + 1];

                    break;
                }
            }
        }

        // If it's not full we are in luck, we can just easily insert without spliting
        if (!whereToInsert->isFull())
        {

            int positionAtWhichWeInsert = 0;

            while (value > whereToInsert->keys[positionAtWhichWeInsert] && positionAtWhichWeInsert < whereToInsert->size)
            {
                positionAtWhichWeInsert++;
            }

            // shift all by one to right
            for (int i = whereToInsert->size; i > positionAtWhichWeInsert; i--)
            {
                whereToInsert->keys[i] = whereToInsert->keys[i - 1];
            }

            whereToInsert->keys[positionAtWhichWeInsert] = value;
            whereToInsert->size++;

            whereToInsert->nodes[whereToInsert->size] = whereToInsert->nodes[whereToInsert->size - 1];
            whereToInsert->nodes[whereToInsert->size - 1] = NULL;

            return;
        }

        // Now the hard part, spliting the pages
        BPlusTreeNode *newLeafNode = new BPlusTreeNode;
        int tempNodeValues[BUCKET_SIZE + 1];

        for (int i = 0; i < BUCKET_SIZE; i++)
        {
            tempNodeValues[i] = whereToInsert->keys[i];
        }

        int positionToInsertValue = 0;

        // find position where you need to insert the val
        while (value > whereToInsert->keys[positionToInsertValue] && positionToInsertValue < whereToInsert->size)
        {
            positionToInsertValue++;
        }

        // shift all by one to right
        for (int i = BUCKET_SIZE + 1; i > positionToInsertValue; i--)
        {
            tempNodeValues[i] = tempNodeValues[i - 1];
        }

        tempNodeValues[positionToInsertValue] = value;

        newLeafNode->isLeaf = true;

        whereToInsert->size = (BUCKET_SIZE + 1) / 2;
        newLeafNode->size = (BUCKET_SIZE + 1) - (BUCKET_SIZE + 1) / 2;

        for (int i = 0; i < whereToInsert->size; i++)
        {
            whereToInsert->keys[i] = tempNodeValues[i];
        }

        for (int i = 0, j = whereToInsert->size; i < newLeafNode->size; i++, j++)
        {
            newLeafNode->keys[i] = tempNodeValues[j];
        }

        if (whereToInsert == root)
        {
            BPlusTreeNode *newRootNode = new BPlusTreeNode;
            newRootNode->isLeaf = false;
            newRootNode->keys[0] = newLeafNode->keys[0];
            newRootNode->nodes[0] = whereToInsert;
            newRootNode->nodes[1] = newLeafNode;
            newRootNode->size = 1;

            root = newRootNode;
        }
        else
        {
            shiftTheTree(newLeafNode->keys[0], parent, newLeafNode);
        }
    }

    void shiftTheTree(int valueToInsert, BPlusTreeNode *current, BPlusTreeNode *child)
    {
        if (!current->isFull())
        {
            int positionToInsertValue = 0;

            // find position where you need to insert the val
            while (valueToInsert > current->keys[positionToInsertValue] && positionToInsertValue < current->size)
            {
                positionToInsertValue++;
            }

            // shift all by one to right
            for (int i = current->size; i > positionToInsertValue; i--)
            {
                current->keys[i] = current->keys[i - 1];
            }

            for (int i = current->size + 1; i > positionToInsertValue + 1; i--)
            {
                current->nodes[i] = current->nodes[i - 1];
            }

            current->keys[positionToInsertValue] = valueToInsert;
            current->size++;
            current->nodes[positionToInsertValue + 1] = child;

            return;
        }

        BPlusTreeNode *newInternal = new BPlusTreeNode;
        int tempKey[BUCKET_SIZE + 1];
        BPlusTreeNode *tempPtr[BUCKET_SIZE + 2];

        for (int i = 0; i < BUCKET_SIZE; i++)
            tempKey[i] = current->keys[i];

        for (int i = 0; i < BUCKET_SIZE + 1; i++)
            tempPtr[i] = current->nodes[i];

        int i = 0, j;
        while (valueToInsert > tempKey[i] && i < BUCKET_SIZE)
        {
            i++;
        }

        for (int j = BUCKET_SIZE + 1; j > i; j--)
        {
            tempKey[j] = tempKey[j - 1];
        }

        tempKey[i] = valueToInsert;
        for (int j = BUCKET_SIZE + 2; j > i + 1; j--)
        {
            tempPtr[j] = tempPtr[j - 1];
        }

        tempPtr[i + 1] = child;
        newInternal->isLeaf = false;
        current->size = (BUCKET_SIZE + 1) / 2;

        newInternal->size = BUCKET_SIZE - (BUCKET_SIZE + 1) / 2;

        for (int i = 0, j = current->size + 1;
             i < newInternal->size; i++, j++)
        {

            newInternal->keys[i] = tempKey[j];
        }

        for (int i = 0, j = current->size + 1;
             i < newInternal->size + 1; i++, j++)

        {
            newInternal->nodes[i] = tempPtr[j];
        }

        if (current == root)
        {
            BPlusTreeNode *newRoot = new BPlusTreeNode;
            newRoot->keys[0] = current->keys[current->size];
            newRoot->nodes[0] = current;
            newRoot->nodes[1] = newInternal;
            newRoot->isLeaf = false;
            newRoot->size = 1;
            root = newRoot;
        }
        else
        {
            shiftTheTree(current->keys[current->size], findParent(root, current), newInternal);
        }
    }

    BPlusTreeNode *findParent(BPlusTreeNode *current, BPlusTreeNode *child)
    {
        BPlusTreeNode *parent;

        if (current->isLeaf || (current->nodes[0])->isLeaf)
        {
            return NULL;
        }

        for (int i = 0; i < current->size + 1; i++)
        {
            if (current->nodes[i] == child)
            {
                parent = current;
                return parent;
            }
            else
            {
                parent = findParent(current->nodes[i], child);
                if (parent != NULL)
                {
                    return parent;
                }
            }
        }
        return parent;
    }

    void generateDot(BPlusTreeNode *current, ofstream &file)
    {
        if (current == nullptr)
            return;

        file << "  node" << current << " [label=\"";
        for (int i = 0; i < current->size; i++)
        {
            file << "<f" << i << "> |" << current->keys[i] << "| ";
        }
        file << "<f" << current->size << ">\"];" << endl;

        for (int i = 0; i <= current->size; i++)
        {
            if (current->nodes[i])
            {
                file << "  node" << current << ":f" << i << " -> node" << current->nodes[i] << ";" << endl;
                generateDot(current->nodes[i], file);
            }
        }
    }

    void exportToDot(const string &filename)
    {
        ofstream file(filename);
        file << "digraph BPlusTree {" << endl;
        file << "  node [shape=record];" << endl;
        generateDot(root, file);
        file << "}" << endl;
        file.close();
    }
};

int main()
{
    BPlusTree node;

    node.insert(8);
    node.insert(2);
    node.insert(5);

    node.insert(3);
    node.insert(6);
    node.insert(13);
    node.insert(7);
    node.insert(9);
    node.insert(10);
    node.insert(11);
    node.insert(15);
    node.insert(4);
    node.insert(12);
    node.insert(1);
    node.insert(14);

    cout << "Exists " << node.exists(3) << endl;
    cout << "Exists " << node.exists(6) << endl;
    cout << "Exists " << node.exists(13) << endl;
    cout << "Exists " << node.exists(6) << endl;
    cout << "Exists " << node.exists(1) << endl;
    cout << "Exists " << node.exists(4) << endl;
    cout << "Exists " << node.exists(12) << endl;

    cout << "YT" << endl;

    cout << "Exists " << node.exists(55) << endl;
    cout << "Exists " << node.exists(56) << endl;
    cout << "Exists " << node.exists(57) << endl;
    cout << "Exists " << node.exists(58) << endl;
    cout << "Exists " << node.exists(59) << endl;

    node.exportToDot("bplustree.dot");

    return 0;
}
