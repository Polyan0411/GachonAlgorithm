#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum word length
#define MAX_WORD_LENGTH 50
// Maximum suggestions length
#define MAX_SUGGESTIONS 1000
char* suggestions[MAX_SUGGESTIONS];
int numSuggestions = 0;

// Check if a given word is in the dictionary (case-insensitive)
int isWordInDictionary(const char* word, const char** dictionary, int dictSize) {
    for (int i = 0; i < dictSize; ++i) {
        if (_stricmp(dictionary[i], word) == 0) {
            return 1; // Word is in the dictionary
        }
    }
    return 0; // Word is not in the dictionary
}

// Read dictionary.txt (10000 words)
int readDictionary(const char* filename, char*** dictionary) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1; // Return -1 to indicate an error
    }

    char word[MAX_WORD_LENGTH];
    int dictSize = 0;

    // Number of words in Dictionary
    while (fgets(word, sizeof(word), file) != NULL) {
        dictSize++;
    }

    // Allocate memory for Dictionary
    *dictionary = (char**)malloc(dictSize * sizeof(char*));
    if (*dictionary == NULL) {
        perror("Memory allocation error");
        fclose(file);
        return -1;
    }

    // Rewind the file to read words again
    rewind(file);

    // Read words into the dictionary
    for (int i = 0; i < dictSize; ++i) {
        if (fgets(word, sizeof(word), file) != NULL) {
            // Remove newline character from each word
            word[strcspn(word, "\n")] = '\0';

            // Allocate memory for each word
            (*dictionary)[i] = _strdup(word);
            if ((*dictionary)[i] == NULL) {
                perror("Memory allocation error");
                fclose(file);

                // Free allocated memory before returning
                for (int j = 0; j < i; ++j) {
                    free((*dictionary)[j]);
                }
                free(*dictionary);

                return -1; // Return -1 to indicate an error
            }
        }
    }

    fclose(file);
    return dictSize; // Return the size of the dictionary
}

// Free memory allocated for the dictionary
void freeDictionary(char** dictionary, int dictSize) {
    for (int i = 0; i < dictSize; ++i) {
        free(dictionary[i]);
    }
    free(dictionary);
}

// Recursive function to generate autocorrect suggestions
void autocorrect(char* input, int index, const char** dictionary, int dictSize, int maxErrors, int changed, char firstChar) {
    if (input[index] == '\0' || changed >= maxErrors) {
        if (isWordInDictionary(input, dictionary, dictSize) && numSuggestions < MAX_SUGGESTIONS) {
            suggestions[numSuggestions++] = _strdup(input);
        }
        return;
    }

    char original = input[index];

    for (int i = 0; i < 26; ++i) {
        input[index] = 'a' + i;
        if (original != 'a' + i) {
            if (index == 0 && input[index] != firstChar) {
                continue;
            }
            autocorrect(input, index + 1, dictionary, dictSize, maxErrors, changed + 1, firstChar);
        }
        else {
            autocorrect(input, index + 1, dictionary, dictSize, maxErrors, changed, firstChar);
        }
    }

    input[index] = original;
}

int main() {
    char input[100];
    char** dictionary;
    int dictSize = readDictionary("dictionary.txt", &dictionary);

    if (dictSize != -1) {
        while (1) {
            printf("Enter a word for autocorrection (or 'exit' to quit): ");
            scanf("%s", input);

            if (strcmp(input, "exit") == 0) {
                break;
            }

            int maxErrors = 1;
            int choice = 0;
            int inputLength = strlen(input);

            do {
                numSuggestions = 0;
                autocorrect(input, 0, (const char**)dictionary, dictSize, maxErrors, 0, input[0]);

                if (numSuggestions == 0) {
                    if (maxErrors >= inputLength) {
                        printf("No suggestions found after %d attempts. Please try another word.\n", inputLength);
                        break;
                    }
                    maxErrors++;
                    continue;
                }

                printf("Autocorrect suggestions:\n");
                for (int i = 0; i < numSuggestions; ++i) {
                    printf("%d. %s\n", i + 1, suggestions[i]);
                }

                printf("Enter the number of the correct word, or 0 if it's not listed: ");
                scanf("%d", &choice);

                if (choice > 0 && choice <= numSuggestions) {
                    printf("AutoCorrected Word: %s\n", suggestions[choice - 1]);
                    break;
                }

                if (maxErrors >= inputLength) {
                    printf("No suggestions found after %d attempts. Please try another word.\n", inputLength);
                    break;
                }

                maxErrors++;
            } while (choice == 0);
        }

        freeDictionary(dictionary, dictSize);
    }

    return 0;
}
