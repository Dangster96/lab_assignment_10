#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ALPHABET_SIZE 26
#define MAX_WORDS 256

// Trie node structure
struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    int count; // Used to count occurrences of a word
};

// Trie structure
struct Trie {
    struct TrieNode *root;
};

// Function to create a new trie node
struct TrieNode* createTrieNode() {
    struct TrieNode* pNode = malloc(sizeof(struct TrieNode));
    if (pNode) {
        int i;
        pNode->count = 0;
        for (i = 0; i < ALPHABET_SIZE; i++)
            pNode->children[i] = NULL;
    }
    return pNode;
}

// Initializes a trie structure
struct Trie* createTrie() {
    struct Trie* pTrie = malloc(sizeof(struct Trie));
    if (pTrie)
        pTrie->root = createTrieNode();
    return pTrie;
}

// Inserts the word into the trie structure
void insert(struct Trie *pTrie, char *word) {
    int level;
    int length = strlen(word);
    int index;

    struct TrieNode *pCrawl = pTrie->root;

    for (level = 0; level < length; level++) {
        index = word[level] - 'a';
        if (!pCrawl->children[index])
            pCrawl->children[index] = createTrieNode();
        pCrawl = pCrawl->children[index];
    }

    // Mark the end of word and increase count
    pCrawl->count++;
}

// Computes the number of occurrences of the word in the trie
int numberOfOccurances(struct Trie *pTrie, char *word) {
    int level;
    int length = strlen(word);
    int index;
    struct TrieNode *pCrawl = pTrie->root;

    for (level = 0; level < length; level++) {
        index = word[level] - 'a';
        if (!pCrawl->children[index])
            return 0; // Word not found

        pCrawl = pCrawl->children[index];
    }

    return pCrawl->count;
}

// Deallocate the trie structure
void deallocateTrieNode(struct TrieNode *pNode) {
    int i;
    for (i = 0; i < ALPHABET_SIZE; i++) {
        if (pNode->children[i] != NULL) {
            deallocateTrieNode(pNode->children[i]);
        }
    }
    free(pNode);
}

struct Trie* deallocateTrie(struct Trie *pTrie) {
    if (pTrie != NULL) {
        deallocateTrieNode(pTrie->root);
        free(pTrie);
        pTrie = NULL;
    }
    return pTrie;
}

// This function will read number of words in the dictionary,
// and read all the words in the dictionary to the structure words
int readDictionary(char *filename, char **pInWords) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error opening file '%s': %s\n", filename, strerror(errno));
        return -1;
    }

    char line[100];
    int count = 0;
    while (fgets(line, sizeof(line), file) && count < MAX_WORDS) {
        line[strcspn(line, "\n")] = 0; // Remove newline character
        pInWords[count++] = strdup(line);
    }

    if (ferror(file)) {
        fprintf(stderr, "Error reading file '%s'\n", filename);
        fclose(file);
        return -1;
    }

    fclose(file);
    return count;
}

int main(void) {
    char *inWords[MAX_WORDS];

    // Read the number of words in the dictionary
    int numWords = readDictionary("dictionary.txt", inWords);
    if (numWords < 0) {
        printf("Failed to open the dictionary file.\n");
        return 1;
    }

    struct Trie *pTrie = createTrie();
    for (int i = 0; i < numWords; i++) {
        insert(pTrie, inWords[i]);
    }

    // Test words to find their occurrences
    char *pWords[] = {"notaword", "ucf", "no", "note", "corg"};
    for (int i = 0; i < 5; i++) {
        printf("\t%s : %d\n", pWords[i], numberOfOccurances(pTrie, pWords[i]));
    }

    // Deallocate the trie
    pTrie = deallocateTrie(pTrie);
    if (pTrie != NULL)
        printf("There is an error in deallocating the trie\n");

    // Free the strings allocated for dictionary words
    for (int i = 0; i < numWords; i++) {
        free(inWords[i]);
    }

    return 0;
}

