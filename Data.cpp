#include"Data.hpp"
#include<iostream>

Data::Data(): content(NULL), last(NULL), size(0) {}

Data::Data(const char source[], unsigned _size) : size(_size) {
    if(_size == 0 || source == NULL) {
        content = NULL;
        return;
    }
    unsigned numBlocks = _size >> 10; // numBlocks = _size / 1024
    unsigned rem = _size & 1023; // rem = _size % 1023
    unsigned i, j, k;
    if(rem != 0)     // -size is not a multiple of 1024.
        numBlocks++; //  Adding an extra block.
    // Allocating first block.
    content = new Block;
    last = content;
    for(i = 1; i < numBlocks; i++) { // Copying data.
        k = --i << 10; // k = (i-1) * 1024
        for(j = 0; j < 1024; j++) {
            last->bytes[j] = source[j + k];
        }
        last->next = new Block;
        last = last->next;
    }
    rem != 0 ? rem : rem = 1024; // Filling the last block.
    for(j = 0, k += 1024; j < rem; j++) last->bytes[j] = source[j + k];
    last->next = NULL;
}

Data::~Data() {
    if(content != NULL) while(content != NULL) {
        Block* prev = content;
        content = content->next;
        delete prev;
    }
}

int Data::append(const char source[], unsigned sz) {
    if(sz == 0) return 0; // Nothing to append.

    size += sz; // Updating size.
    unsigned numBlocks = sz >> 10; // numBlocks = sz / 1024
    unsigned rem = sz & 1023;  // rem = sz % 1024
    unsigned i, j, k = 0;
    if(rem != 0)     // -sz is not a multiple of 1024.
        numBlocks++; //  Adding an extra block.

    // Allocating first extra block.
    if(content == NULL) { // In case of empty container.
        content = new Block;
        last = content;
    }
    else {
        last->next = new Block;
        last = last->next;
    }
    // Copying the new content.
    numBlocks--; // Last block is treated separately
    for(i = 0; i < numBlocks; i++) {
        k = i << 10; // k = i * 1024
        for(j = 0; j < 1024; j++) {
            last->bytes[j] = source[j + k];
        }
        last->next = new Block;
        last = last->next;
    }
    rem != 0 ? rem : rem = 1024;  // Filling the last block.
    for(j = 0, k = i << 10; j < rem; j++) last->bytes[j] = source[j + k];
    last->next = NULL;
    return 0;
}

Data::Block* Data::getContent() const {
    return content;
}

unsigned Data::getSize() const {
    return size;
}

