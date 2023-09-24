// -Functions to print data in a tabular form.
// -Hexadecimal base will be use in all the cases.
#include"DataPrintFunctions.hpp"

int len(const char str[]) {
    if(str == NULL) return 0;

    int l = -1;
    while(str[++l] != 0) {}
    return l;
}

int maxLen(const char* strArray[], int numStrs) {
    if(strArray == NULL) return 0;

    int ml = len(strArray[0]), i = 0;
    while(++i < numStrs){
        if(ml < len(strArray[i])) ml = len(strArray[i]);
    }
    return ml;
}

void printSpace(int length) {
    for(int i = 0; i < length; i++) std::cout << ' ';
}

void printCentered(const char str[], int spaceAvaible) {
    int strlen = len(str), offset = 0;
    // -In any case, the space used for the printing is
    //  going to be at least the length of the string.
    if(spaceAvaible < strlen) spaceAvaible = strlen;

    // -Length of the empty space for the centering.
    offset = (spaceAvaible - strlen) / 2;

    printSpace(offset);
    std::cout << str;
    // -Offset of the ending side.
    offset = spaceAvaible - (strlen + offset);
    printSpace(offset);
}

void printHexUnsg(ui64 n) {
    unsigned int b;
    for(int i = 0; i < 8; i++) {
        b = (n >> (56 - (i << 3))) & 255;
        if(b < 16) std::cout << '0';
        printf("%X", b);
    }
}

void printCentered(ui64 number, int spaceAvaible) {
    int offset;
    // -In any case, the space used for the printing is
    //  going to be at least the length of the string.
    if(spaceAvaible < 16) spaceAvaible = 16;

    // -Length of the empty space for the centering.
    offset = (spaceAvaible - 16) / 2;

    printSpace(offset);
    printHexUnsg(number);
    // -Offset of the ending side.
    offset = spaceAvaible - (16 + offset);
    printSpace(offset);
}

void toString(int n, char buffer[]) {
    int i = 0, j = 0, rem;
    char aux;
    if(n == 0) {
        buffer[0] = '0'; buffer[1] =  0 ;
        return ;
    }
    if(n < 0) {
        buffer[i++] = '-'; n = -n;
    }
    while(n > 0) {
        rem = n % 10; buffer[i++] = rem + 48;
        n -= rem; n /= 10;
    }
    buffer[i--] = 0;
    if(buffer[0] == '-') j = 1;
    while(i > j) {
        aux = buffer[i];
        buffer[i--] = buffer[j];
        buffer[j++] = aux;
    }
}

void printArrayTableForm(ui64 data[], // -What we want to print.
                         int dataSize, // -Number of elements in data array.
                         int numColumns,// -Will determine the number of rows.
                         const char* headers[],
                         const char* leftTags[]) {
    int aux, i, j;       // -Auxiliary variable and counters.
    int columnLen = 18;  // -Length of the columns in spaces.
    int leftTagsLen = 3; // -Length of left tags (if any).
    char indexStr[6];    // -In case of not having left tags.
    // -Determining the number of rows.
    int numRows = dataSize / numColumns;
    if(dataSize % numColumns != 0) numRows++;

    if(leftTags != NULL) { // -Left tags column length.
        if( leftTagsLen < (aux = maxLen(leftTags, numRows)) )
            leftTagsLen = aux;
    }

    if(headers != NULL) { // -Data length < headers length?
        if( columnLen < (aux = maxLen(headers, numColumns)) ) {
            columnLen = aux;
        }
        // -Printing headers.
        std::cout << '|';
        printSpace(leftTagsLen);
        std::cout << '|';
        for(i = 0; i < numColumns; i++) {
            std::cout << '|';
            printCentered(headers[i], columnLen);
            std::cout << '|';
        }
        std::cout << '\n';
    }
    for(i = 0; i < numRows; i++) {
        std::cout << '|';
        if(leftTags != NULL) printCentered(leftTags[i], leftTagsLen);
        else {
            toString(i, indexStr);
            printCentered(indexStr, leftTagsLen);
        }
        std::cout << '|';
        aux = i*numColumns;
        for(j = 0; j < numColumns; j++) {
            std::cout << '|';
            if(aux + j < dataSize) printCentered(data[aux + j], columnLen);
            else printSpace(columnLen);
            std::cout << '|';
        }
        std::cout << '\n';
    }
}

