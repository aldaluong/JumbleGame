#include <iostream>
#include <unordered_set>
#include <fstream>
#include <algorithm>

/***************************************************************************************
    Jumble Game
    
    Takes a string word as an input and outputs a text file with its possible "jumbled" words.
    The "5desk.txt" is used as the dictionary source and is taken from SCOWL 
    (http://wordlist.sourceforge.net).
    The output text file is named after the word: <word>.txt
 
    The algorithm is as follows:
        - Find all combinations of a word (avoiding duplicates).
        - For all the word combinations, permute the input word (avoiding duplicates again).
        - Look up each word permutation in the dictionary.
 ****************************************************************************************/

static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

class Jumble {
    
    std::ofstream outputFile;
    
    struct Dictionary
    {
        Dictionary()
        {
            std::ifstream inputDictionary("5desk.txt");
            std::string line;
            if (inputDictionary.is_open()) {
                while (getline(inputDictionary,line)) {
                    rtrim(line);
                    words.insert(line);
                }
                inputDictionary.close();
            }
        }
        
        bool contains(std::string const& word) const { return words.count(word); }
        
        std::unordered_set<std::string> words;
    };
    
    Dictionary dict;

    void permuteWordAndCheckDictionary(std::string &word, int startIndex);
    void generateWordCombinationPermuteAndCheckDictionary(std::string &inputWord, std::string outputWord, int startIndex);
    
public:
    void jumbleWord(std::string const& inputWord);
};

// --- permutes all letters of a word and looks up the word in the dictionary ---
void Jumble::permuteWordAndCheckDictionary(std::string &word, int startIndex) {
    
    int length = word.length();
    
    // tracks previous characters that were used for permutation
    char previousCharacters[256];
    memset(previousCharacters, 0, 256);
    
    if (startIndex >= length) {
        // check if the word exists in the dictionary
        if (dict.contains(word)) {
            outputFile << word << "\n";
            std::cout << "word is in dictionary: " << word << "\n";
        }
        return;
    }
    
    for (int i = startIndex; i < length; i++) {
        // avoid permuting duplicate characters
        if (!previousCharacters[word[i]]) {
            std::swap(word[startIndex], word[i]);
            permuteWordAndCheckDictionary(word, startIndex+1);
            std::swap(word[startIndex], word[i]);
            previousCharacters[word[i]] = 1;
        }
    }
}

// --- Finds all combinations of a word then permutes and looks up the word in the dictionary ---
void Jumble::generateWordCombinationPermuteAndCheckDictionary(std::string &inputWord, std::string outputWord, int startIndex) {
    
    for (int i = startIndex; i < inputWord.length(); ++i) {
        outputWord.push_back(inputWord[i]);
        
        // permute the outputWord and check if it exists in the dictionary
        std::sort(outputWord.begin(), outputWord.end());
        permuteWordAndCheckDictionary(outputWord, 0);
        generateWordCombinationPermuteAndCheckDictionary(inputWord, outputWord, i+1);
        
        outputWord.erase(outputWord.length()-1, 1);
        
        // avoid duplicate combinations
        if (inputWord[i] == inputWord[i+1]) {
            i++;
        }
    }
}

// --- Jumble words ---
void Jumble::jumbleWord(std::string const& inputWord) {
    
    // create the output file
    std::string outputFileName(inputWord + ".txt");
    outputFile.open(outputFileName);
    
    // need output word buffer for generating word combinations
    std::string outputWord;
    
    // input word needs to be copied since it'll be mutated
    std::string inputWordCopy(inputWord);
    // input word needs to be sorted to help filter duplicate combinations
    std::sort(inputWordCopy.begin(), inputWordCopy.end());
    
    generateWordCombinationPermuteAndCheckDictionary(inputWordCopy, outputWord, 0);
    
    outputFile.close();
}

int main() {
    
    std::string word;
    getline(std::cin, word);
    Jumble jumbleGame;
    jumbleGame.jumbleWord(word);
    
    return 0;
}