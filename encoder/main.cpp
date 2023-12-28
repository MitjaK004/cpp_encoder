#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <vector>
using std::ifstream;
using std::ofstream;
using std::fstream;
using std::string;
using std::atoi;
using std::cout;
using std::endl;
using std::vector;
using std::streampos;

//https://macrosec.tech/index.php/2020/09/20/creating-a-fud-backdoor/

//make a zip file maker with better compression

int openFile(string filename, char*& buffer, std::streampos& fileSize) {
    ifstream inFile;

    inFile.open(filename, std::ios::in | std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "Error opening the file for reading!" << std::endl;
        return -1;
    }

    inFile.seekg(0, std::ios::end);
    fileSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    buffer = new char[fileSize];

    inFile.read(buffer, fileSize);

    if (!inFile) {
        std::cerr << "Error reading the file!" << std::endl;
        return -1;
    }

    std::cout << "Size of the file: " << fileSize << std::endl;
    //std::cout << "Content of the file:" << std::endl;
    //std::cout.write(buffer, fileSize);


    inFile.close();

    return 1;
}

int openFile(string filename, int*& buffer, std::streampos& fileSize) {
    ifstream inFile;

    inFile.open(filename, std::ios::in | std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "Error opening the file for reading!" << std::endl;
        return -1;
    }

    inFile.seekg(0, std::ios::end);
    fileSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    size_t numIntegers = fileSize / sizeof(int);

    buffer = new int[numIntegers];

    inFile.read(reinterpret_cast<char*>(buffer), fileSize);

    if (!inFile) {
        std::cerr << "Error reading the file!" << std::endl;
        return -1;
    }

    std::cout << "Size of the file: " << fileSize << std::endl;
    //std::cout << "Content of the file:" << std::endl;
    //std::cout.write(buffer, fileSize);


    inFile.close();

    return 1;
}

int saveFile(string filename, char*& buffer, std::streampos& fileSize) {
    std::ofstream outFile;

    outFile.open(filename, std::ios::out | std::ios::binary);

    if (!outFile.is_open()) {
        std::cerr << "Error opening the file!" << std::endl;
        return -1;
    }

    outFile.write(buffer, fileSize);

    if (!outFile) {
        std::cerr << "Error writing to the file!" << std::endl;
        outFile.close();
        return -1;
    }

    outFile.close();

    std::cout << "Data has been saved to the file '" << filename << "' successfully." << std::endl;

    return 1;
}

int saveFile(string filename, int*& buffer, std::streampos& fileSize) {
    std::ofstream outFile;

    outFile.open(filename, std::ios::out | std::ios::binary);

    if (!outFile.is_open()) {
        std::cerr << "Error opening the file!" << std::endl;
        return -1;
    }

    size_t totalSize = fileSize * 4;

    outFile.write(reinterpret_cast<const char*>(buffer), totalSize);

    if (!outFile) {
        std::cerr << "Error writing to the file!" << std::endl;
        outFile.close();
        return -1;
    }

    outFile.close();

    std::cout << "Data has been saved to the file '" << filename << "' successfully." << std::endl;

    return 1;
}

void encode(int*& buffer, char* data, std::streampos& fileSize, std::string key) {
    buffer = new int[fileSize];
    int keyc = 0;
    int keyl = key.length();
    for (int i = 0; i < fileSize; i++) {
        buffer[i] = data[i] * key.at(keyc);
        keyc++;
        if (keyc >= keyl) {
            keyc = 0;
        }
    }
}

void decode(char*& buffer, int* data, std::streampos& fileSize, std::string key) {
    fileSize = fileSize / 4;
    buffer = new char[fileSize];
    int keyc = 0;
    int keyl = key.length();
    for (int i = 0; i < (fileSize); i++) {
        buffer[i] = data[i] / key.at(keyc);
        keyc++;
        if (keyc >= keyl) {
            keyc = 0;
        }
    }
}

int saveTextFile(const std::string& filename, const std::string& text) {
    std::ofstream outFile;

    outFile.open(filename);

    if (!outFile.is_open()) {
        std::cerr << "Error opening the file!" << std::endl;
        return -1;
    }

    outFile << text;

    if (!outFile) {
        std::cerr << "Error writing to the file!" << std::endl;
        outFile.close();
        return -1;
    }

    outFile.close();
    std::cout << "Text has been saved to the file '" << filename << "' successfully." << std::endl;

    return 1;
}

string makestring(char* buffer, std::streampos sz, string seperate, bool num) {
    string ret = "";
    for (int i = 0; i < sz; i++) {
        if (num) {
            ret += std::to_string(short(buffer[i]));
        }
        else {
            ret += buffer[i];
        }
        ret += seperate;
    }
    return ret;
}

string makestring(int* buffer, std::streampos sz, string seperate) {
    string ret = "";
    for (int i = 0; i < sz; i++) {
        ret += std::to_string(buffer[i]);
        ret += seperate;
    }
    return ret;
}

void fix(string& sep) {
    for (int i = 0; i < sep.length(); i++) {
        if (sep[i] == '_') {
            sep[i] = ' ';
        }
    }
}

string getFlagData(int argc, char** argv, string flag) {
    string ret = "";
    for (int i = 4; i < argc; i++) {
        if (strcmp(argv[i], flag.c_str()) == 0) {
            ret = (argv[i + 1] != nullptr) ? argv[i + 1] : "";
            fix(ret);
        }
    }
    return ret;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        return 0;
    }

    string operation = (argv[1] != nullptr) ? argv[1] : "";
    string file = (argv[2] != nullptr) ? argv[2] : "";
    string key = (argv[3] != nullptr) ? argv[3] : "";

    if (operation == "" || file == "" || key == "") {
        cout << "Error: empty arguments!\n";
        return 0;
    }

    int* data = nullptr;
    char* buffer = nullptr;
    std::streampos size = 0;

    if (key == "-null") {
        key = char(1);
    }

    if (operation == "-e") {
        openFile(file, buffer, size);

        cout << "Encoding file: " << file << " with key : " << key << '\n';

        encode(data, buffer, size, key);

        saveFile(file, data, size);
    }
    else if (operation == "-d") {
        openFile(file, data, size);

        cout << "Decoding file: " << file << " with key : " << key << '\n';

        decode(buffer, data, size, key);

        saveFile(file, buffer, size);
    }
    else if (operation == "-t") {
        string seperate = getFlagData(argc, argv, "-s");

        if (key == "-int") {
            openFile(file, data, size);

            string text = makestring(data, size, seperate);

            saveTextFile((file + ".txt"), text);
        }
        else if (key == "-byte" || key == "-char") {
            openFile(file, buffer, size);
            string text = "";

            if (key == "-byte") {
                text = makestring(buffer, size, seperate, true);
            }
            else {
                text = makestring(buffer, size, seperate, false);
            }

            saveTextFile((file + ".txt"), text);
        }
    }
    else {
        delete[] data;
        delete[] buffer;

        cout << "\"" << operation << "\" is not a valid operation!\n";

        return 0;
    }

    delete[] data;
    delete[] buffer;

    return 0;
}