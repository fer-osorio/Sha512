// -Definitions for the Sha512 Functions.

#include<fstream>
#include"Sha512.hpp"
#include"DataPrintFunctions.hpp"

Sha512::Sha512(const char data[], const ui64 size[2]) {
    ui64 sz[2];
    sz[0] = size[0] << 3;                                                       // -From size in bytes to
    sz[0] |= (size[1] >> 61);                                                   //  size in bits, witch is
    sz[1] = size[1] << 3;                                                       //  just to multiply by 8
    calculateHash(data, sz);
}

Sha512::Sha512(const char data[], const ui64 size) {
    calculateHash(data, size << 3);
}

Sha512::Sha512(const char* data[], ui64 numRows, ui64 rowLength) {
    // Still in progress...
    UnsignedInt128 sz = ui64product(numRows, rowLength);
    ui64 i, H[8];
    calculateHash(data[0], rowLength);                                          // -Hash of the first row
    auto updateH = [&H, this](void) {
        for(int j = 0; j < 8; j++) {
            H[j] = _8bytes_to_int64(&Hash[j<<3]);
        }
    };
    for(i = 1; i < numRows; i++) {
        updateH();
        calculateHash(data[i], rowLength);
    }
}

Sha512::Sha512(const Sha512& sha) {
    for(int i = 0; i < 64; i++) this->Hash[i] = sha.Hash[i];
}

ui64 Sha512::RotR(ui64 x, int n) {                                              // -Rotation to the right.
    if(n <  0) n = -n;
    if(n > 64) n &= 63;                                                         // n %= 64
    return x >> n | x << (64 - n);
}

ui64 Sha512::RotL(ui64 x, int n) {                                              // -Rotation to the left.
    if(n <  0) n = -n;
    if(n > 64) n &= 63;                                                         // n %= 64
    return x << n | x >> (64 - n);
}

ui64 Sha512::Ch(ui64 x, ui64 y, ui64 z) {
    return (x&y) ^ (~x&z);
}

ui64 Sha512::Maj(ui64 x, ui64 y, ui64 z) {
    return (x&y) ^ (x&z) ^ (y&z);
}

ui64 Sha512::SIGMA0(ui64 x) {
    return RotR(x, 28) ^ RotR(x, 34) ^ RotR(x, 39);
}

ui64 Sha512::SIGMA1(ui64 x) {
    return RotR(x, 14) ^ RotR(x, 18) ^ RotR(x, 41);
}

ui64 Sha512::sigma0(ui64 x) {
    return RotR(x, 1) ^ RotR(x, 8) ^ (x >> 7);
}

ui64 Sha512::sigma1(ui64 x) {
    return RotR(x, 19) ^ RotR(x, 61) ^ (x >> 6);
}

ui64 Sha512::_8bytes_to_int64(const char bytes[8]) {
    ui64 r = 0;                                                                 // -64 zero bits
    for(int i = 0; i < 8; i++) {
        r <<= 8;                                                                // Making room for the bits of the next byte
        r |= ui64((unsigned char)bytes[i]);                                     // Allocating bits at the end.
    }
    return r;
}

void Sha512::int64_to_8bytes(const ui64 n, char result[8]) {
    int i;
    for(i = 0; i < 8; i++)
        result[i] = char(n >> ((7-i) << 3));                                    // (7-i) * 8
}

void Sha512::processBlock(const char M[128], ui64 W[80], ui64 H[8]) {
    ui64 WV[8];                                                                 // Working variables
    ui64 T1, T2;                                                                // Temporal.
    const char *dptr = M;
    int i;

    bool debug = true;                                                          // True value shows the process.

    for(i = 0; i < 80; i++) {                                                   // -Creating message schedule.
        if(i < 16) {
            W[i] = _8bytes_to_int64(dptr);
            dptr += 8;
        } else
            W[i] = sigma1(W[i-2]) + W[i-7] + sigma0(W[i-15]) + W[i-16];
    }
    if(debug) {
        const char* Headers0[] =  {"Initial Hash Value"};
        const char* leftTags0[] = {"H[0] = ", "H[1] = ", "H[2] = ", "H[3] = ", "H[4] = ", "H[5] = ", "H[6] = ", "H[7] = "};
        printArrayTableForm(H, 8, 1, Headers0, leftTags0);
        std::cout << "\n\n";
        const char* Headers1[] = {"Block","Contents"};
        const char* leftTags1[] = {"","",""," W[0,15] = ","","","", ""};
        printArrayTableForm(W, 16, 2, Headers1, leftTags1);
        std::cout << "\n\n";

    }
    WV[0] = H[0]; WV[1] = H[1]; WV[2] = H[2]; WV[3] = H[3];                     // -Initializing working variables.
    WV[4] = H[4]; WV[5] = H[5]; WV[6] = H[6]; WV[7] = H[7];                     // ...

    if(debug) {
        std::cout << '\n';
        const char headers[] = "| i |"
                               "|         A        |""|         B        |"
                               "|         C        |""|         D        |"
                               "|         E        |""|         F        |"
                               "|         G        |""|         H        |";
        std::cout << headers << '\n';
    }
    for(i = 0; i < 80; i++) {
        T1 = WV[7] + SIGMA1(WV[4]) + Ch(WV[4], WV[5], WV[6]) + K[i] + W[i];
        T2 = SIGMA0(WV[0]) + Maj(WV[0], WV[1], WV[2]);
        WV[7] = WV[6];
        WV[6] = WV[5];
        WV[5] = WV[4];
        WV[4] = WV[3] + T1;
        WV[3] = WV[2];
        WV[2] = WV[1];
        WV[1] = WV[0];
        WV[0] = T1 + T2;
        if(debug) {
            char _si[1][5];
            toString(i, _si[0]);
            const char* si[1] = {_si[0]};
            printArrayTableForm(WV, 8, 8, NULL, si);
        }
    }
    if(debug) std::cout << '\n';
    H[0] += WV[0]; H[4] += WV[4];                                               // -Computing intermediate value
    H[1] += WV[1]; H[5] += WV[5];                                               // ...
    H[2] += WV[2]; H[6] += WV[6];                                               // ...
    H[3] += WV[3]; H[7] += WV[7];                                               // ...
}


void Sha512::calculateHash(const char data[], const ui64 size[2]) {
    int r = size[1] & 1023;                                                     // -r = size % 1024
    int k = 888 - r, i;                                                         // -The NIST standard specify the relation (l+1+k)%1024 == 896, but since we'll
                                                                                //  need a hole byte to put the '1' at the end of the data, the relation we'll use
                                                                                //  is (l+8+k)%1024 == 896, that is k := 896 - r - 8 = 888 - r
    int blocksAdded = 1, counter = 0;
    bool debug = true;                                                          // -If true shows the process.

    UnsignedInt128 N(size[0], size[1]);                                         // -Getting the size in blocks of 1024.
    N >>= 10;                                                                   // -N = N/1024 (integer division)
    N++;                                                                        // -We need enough space to add the '1' extra bit, the k zeros and the binary
                                                                                //  representation of the data size (in bits). In any case well add one more 1024
                                                                                //  block.
    if(k < 0) {
        N++;                                                                    // -One block isn't enough, adding another one.
        blocksAdded = 2;
        k += 1024;                                                              // -Making k positive while preserving the established congruence
    }

    const char* dptr = data;                                                    // -Pointer that moves trough the data.
    char auxBlock[128];                                                         // -Whit this will handle the case of the last blocks.
    ui64 W[80], H[8];

    for(i = 0; i < 8; i++) H[i] = (ui64)H0[i];                                  // -Initializing hash.

    while(N > blocksAdded) {
        if(debug) {
            std::cout << "\n\n******************************************"
            "*********************************| Block "  << counter++ <<
            " |*********************************************************"
            "****************\n\n";
        }
        processBlock(dptr, W, H);
        dptr += 128;
        N--;
    }
    r >>= 3;                                                                    // -Amount of bytes hasn't been processed. Since r = size % 1024, then
    for(i = 0; i < r; i++) auxBlock[i] = dptr[i];                               //  0 <= r < 1024 therefore  0 <= r/8 < 128. Notice that r>>3 is equivalent to r/8

    auxBlock[r] = (char)0x80;                                                   // -Putting the '1' at the end of the data. As said before, we must add a hole byte
                                                                                //  in this step, specifically the byte 1000,0000. This is the reason why we use the
                                                                                //  relation (l+8+k)%1024 == 896.
    k >>= 3;                                                                    // -Amount of zero bytes we have to add.
    for(i = r + 1; i < 128 && k > 0; i++, k--) auxBlock[i] = 0;

    if(i == 128) {
        if(debug) {
            std::cout << "\n\n****************************************************"
            "*********************************| Block "  << counter++ <<
            " |***************************************************************\n\n";
        }
        processBlock(auxBlock, W, H);
        for(i = 0; i < k; i++) auxBlock[i] = 0;                                 // -Adding the rest of the zero's
    }
    for(k = 0; k < 16; i++, k++) {                                              // -Putting the size at the end of the block. Changing the purpose
        if(k < 8) auxBlock[i] = size[0] >> (56 - (k << 3));                     //  of k, now is a counting variable.
        else auxBlock[i] = size[1] >> (56 - ((k-8) << 3));
    }
    if(debug) {
        std::cout << "\n\n****************************************************"
        "*********************************| Block "  << counter++ <<
        " |***************************************************************\n\n";
    }
    processBlock(auxBlock, W, H);
    if(i != 128) std::cout << "\n\n¡Something went wrong!\n\n";

    for(i = 0; i < 8; i++) int64_to_8bytes(H[i], &Hash[i << 3]);                // -Converting the integers of 64 bits in 8 bytes. i * 8.

}

void Sha512::calculateHash(const char data[], const ui64 size) {
    ui64 sz[2] = {0, size};
    calculateHash(data, sz);
}

void Sha512::print(void) const {
    int i;
    for(i = 0; i < 64; i++) {
        if(i != 0 && (i & 7) == 0) printf(",");                                 // Grouping in sets of eight separated by a comma ','
        if(Hash[i] < 16 && Hash[i] >= 0) std::cout << '0';                      // Single digit number; putting a zero in front of it
        printf("%X", (unsigned char) Hash[i]);
    }
}

void Sha512::println(void) const {
    print(); std::cout << '\n';
}

char Sha512::operator [] (int i) const {
    if(i < 0)   i = -i;                                                         // -Negative index
    if(i >= 64) i &= 63;                                                        // -i %= 64
    return Hash[i];
}

Sha512& Sha512::operator=(const Sha512 &sha) {
    if(this != &sha) for(int i = 0; i < 64; i++) this->Hash[i] = sha.Hash[i];
    return *this;
}

void Sha512::save(const char* fname) {                                // -Saving the content in a .txt file
    std::ofstream file;
    char _fname[30] = {'h', 'a', 's', 'h', '.', 't', 'x', 't'};
    if(fname == NULL) {                                                     // -If a file name is not provided, then we assign the name of the object
        file.open(_fname);
    } else {
        file.open(fname);
    }
    if(file.is_open()) {
        file.write(this->Hash, 64);
        file.close();
    } else {
        throw "File could not be written.";
    }
}

std::ostream& operator << (std::ostream& s, Sha512 sha) {
    int i;
    char a, b;                                                                  // -Each byte can be represented by two hexadecimal characters
    for(i = 0; i < 64; i++) {
        if(i != 0 && (i & 7) == 0) s << ',';                                    // If i is not zero and i is divisible by 8 (i & 7 == i % 8) then...
        a = sha.Hash[i] >> 4;                                                   // -Taking the first four bits.
        b = sha.Hash[i] & 15;                                                   // -Taking the last four bits.
        a < 10 ? a += 48 : a += 55;                                             // -Implementing ASCII code.
        b < 10 ? b += 48 : b += 55;                                             // ...
        s << a; s << b;                                                         // -Sending to output stream.
    }
    return s;
}
