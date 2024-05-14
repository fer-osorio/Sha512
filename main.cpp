/* MIT License
 *
 * Copyright (c) 2023 Fernando Osorio
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * SPDX-License-Identifier: MIT
 */

#include "Sha512.hpp"
#include<fstream>

#define BLOCK_SIZE 1024
#define NAME_MAX_LEN 30

void copyStr(const char source[], char dest[]) {                                // Supposing source is a formatted string and dest has enough space
    for(int i = 0; source[i] != 0; i++) dest[i] = source[i];
}

struct TXT {
	char name[NAME_MAX_LEN] = {0};
    unsigned size = 0;
    char* content = NULL; // Text file content.

    public:
    TXT(): name() {}                                                            // -Just for type declaration.

    TXT(const char* fname) {                                                    // -Building from file.
        std::ifstream file;
        int i = 0, nameMaxLen = NAME_MAX_LEN - 1;
        for( ;fname[i] != 0; i++) {                                             // -Assigning file name to name object
            name[i] = fname[i];
            if(i == nameMaxLen) {                                               // -If maximum length reached, truncate the file name.
                name[i] = 0;                                                    // -Indicate the end of the string
                break;                                                          // -End for loop
            }
        }
        file.open(fname);
        if(file.is_open()) {
            file.seekg(0, std::ios::end);                                       // -Look for the end of the file
            std::streampos fileSize = file.tellg();
            this->size = fileSize;
            file.seekg(0, std::ios::beg);                                       // -Go to the beginning
            this->content = new char[fileSize];                                 // -Allocate memory and copy file content
            file.read(this->content, fileSize);                                 // ...
            file.close();
        } else {
            char errmsg[] = "\nIn TXT.cpp file, TXT::TXT(const char* fname): "
                            "Could not open file ";
            std::cout << errmsg << fname << '\n';
            throw errmsg;
        }
    }
    TXT(const TXT& t): size(t.size){                                            // -Copy constructor
        copyStr(t.name, this->name);
        this->content = new char[t.size];
        for(unsigned i = 0; i < t.size; i++) this->content[i] = t.content[i];
    }

    ~TXT() {
        if(this->content != NULL) delete[] this->content;
        this->content = NULL;
    }

	TXT& operator = (const TXT& t) {
	     if(this != &t) {                                                       // -Guarding against self assignment
            if(this->content != NULL) delete[] this->content;                   // -Deleting old content
            copyStr(t.name, this->name);
            this->size = t.size;
            this->content = new char[t.size];
            for(unsigned i = 0; i < t.size; i++) this->content[i] = t.content[i];
        }
        return *this;
	}

    void save(const char* fname = NULL) const{                                  // -Saving the content in a .txt file
        std::ofstream file;
        char _fname[NAME_MAX_LEN];
        if(fname == NULL) {                                                     // -If a file name is not provided, then we assign the name of the object
            copyStr(this->name, _fname);
            file.open(_fname);
        } else {
            file.open(fname);
        }
        if(file.is_open()) {
            file.write(this->content, this->size);
            file.close();
        } else {
            throw "File could not be written.";
        }
    }
};

int main (int argc, char* argv[]) {
    if(argc > 1) {
        const TXT txtFile = argv[1];                                            // Calling TXT(const char*) constructor
        Sha512 s(txtFile.content, txtFile.size);
        s.save();
        return 1;
    }

    char buffer[BLOCK_SIZE];
    unsigned sz = 0;

    std::cout << "\nWrite the string you want to hash. To process the string sent the value 'EOF', which you can do by:\n\n"
                 "- Pressing twice the keys CTRL+Z for Windows.\n"
                 "- Pressing twice the keys CTRL+D for Unix and Linux.\n\n";

    while((buffer[sz++] = getchar()) != EOF)
        if(sz == BLOCK_SIZE) std::cout << "\nBuffer exhausted...\n";
    buffer[--sz] = 0;
    Sha512 sha(buffer, sz);
    std::cout << "\nHash :: "; sha.println();

    return 0;
}
