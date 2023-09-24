#include"Data.hpp"
#include<iostream>

Data::Data(): content(NULL), size(0) {}

Data::Data(const char source[], unsigned _size) : size(_size) {
    content = new char[_size];
    for(unsigned i = 0; i < _size; i++) content[i] = source[i];
}

Data::~Data() {
    if(content != NULL) delete [] content;
}

int Data::append(const char source[], unsigned sz) {
    if(sz == 0) return 0; // Nothing to append.

    unsigned _size = size + sz, i = 0xFFFFFFFF;
    char* _content = new char[_size];

    // Copying the old content.
    while(++i < size) _content[i] = content[i];
    // Copying the new content.
    for(i == 0 ? i : i--; i < _size; i++ ) _content[i] = source[i];

    if(content != NULL) delete [] content;
    content = _content;
    size = _size;
    return 0;
}

char* Data::getContent() const {
    return content;
}

unsigned Data::getSize() const {
    return size;
}

