// Headers.
#include<iostream>
#include<cstdint>

typedef std::uint64_t  ui64;

int len(const char str[]);
int maxLen(const char* strArray[], int numStrs);
void printSpace(int length);
void printCentered(const char str[], int spaceAvaible);
void printHexUnsg(ui64 n);
void printCentered(ui64 number, int spaceAvaible);
void toString(int n, char buffer[]);

void printArrayTableForm(ui64* data, // -What we want to print.
                         int dataSize, // -Number of elements in data array.
                         int numColumns, // -Will determine the number of rows.
                         const char* headers[] = NULL,
                         const char* leftTags[] = NULL);
