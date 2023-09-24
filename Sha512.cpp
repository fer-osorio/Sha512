// -Definitions for the Sha512 Functions.

#include"Sha512.hpp"
#include"DataPrintFunctions.hpp"

Sha512::Sha512(const char data[], const ui64 size[2]) {
    calculateHash(data, size);
}

Sha512::Sha512(const char data[], const ui64 size) {
    ui64 _size[2] = {0, size};
    calculateHash(data, _size);
}

// -Rotation to the right.
ui64 Sha512::RotR(ui64 x, int n) {
    if(n <  0) n = -n;
    if(n > 64) n &= 63; // n %= 64
    return x >> n | x << (64 - n);
}

// -Rotation to the left.
ui64 Sha512::RotL(ui64 x, int n) {
    if(n <  0) n = -n;
    if(n > 64) n &= 63; // n %= 64
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
    ui64 r = 0; // -64 zero bits
    for(int i = 0; i < 8; i++) {
        // Making room for the bits of the next byte
        r <<= 8;
        // Allocating bits at the end.
        r |= ui64((unsigned char)bytes[i]);
    }
    return r;
}

void Sha512::int64_to_8bytes(const ui64 n, char result[8]) {
    int i;
    for(i = 0; i < 8; i++)  // (7-i) * 8
        result[i] = char(n >> ((7-i) << 3));
}

void Sha512::processBlock(const char M[128], ui64 W[80], ui64 H[8]) {
    ui64 WV[8]; // Working variables
    ui64 T1, T2; // Temporal.
    const char *dptr = M;
    int i;

    bool debug = true; // True value shows the process.

    // -Creating message schedule.
    for(i = 0; i < 80; i++) {
        if(i < 16) {
            W[i] = _8bytes_to_int64(dptr);
            dptr += 8;
        } else
            W[i] = sigma1(W[i-2]) + W[i-7] + sigma0(W[i-15]) + W[i-16];
    }
    if(debug) {
        const char* Headers0[] =  {"Initial Hash Value"};
        const char* leftTags0[] = {"H[0] = ", "H[1] = ", "H[2] = ", "H[3] = ",
                                   "H[4] = ", "H[5] = ", "H[6] = ", "H[7] = "};
        printArrayTableForm(H, 8, 1, Headers0, leftTags0);
        std::cout << "\n\n";
        const char* Headers1[] = {"Block","Contents"};
        const char* leftTags1[] = {"","",""," W[0,15] = ","","","", ""};
        printArrayTableForm(W, 16, 2, Headers1, leftTags1);
        std::cout << "\n\n";

    }
    // -Initializing working variables.
    WV[0] = H[0]; WV[1] = H[1]; WV[2] = H[2]; WV[3] = H[3];
    WV[4] = H[4]; WV[5] = H[5]; WV[6] = H[6]; WV[7] = H[7];

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
    // -Computing intermediate value.
    H[0] += WV[0]; H[4] += WV[4];
    H[1] += WV[1]; H[5] += WV[5];
    H[2] += WV[2]; H[6] += WV[6];
    H[3] += WV[3]; H[7] += WV[7];
}

void Sha512::calculateHash(const char data[], const ui64 size[2]) {
    int r = size[1] & 1023, k = 895 - r, i; // k = 895 - size % 1024
    int blocksAdded = 1, counter = 0;
    bool debug = true; // -Shows the process.

    // -Getting the size in blocks of 1024.
    UnsignedInt128 N(size[0], size[1]);
    N >>= 10; // -N /= 1024 (integer division)
    // -We need enough space to add the '1' extra bit, the k zeros
    //  and the binary representation of the data size (in bits).
    N++;      // -In any case well add one more 1024 block.
    if(k < 0) {
        N++;  // -One block isn't enough, adding another one.
        blocksAdded = 2;
        k += 1024; // -Making k positive.
    }

    const char* dptr = data; // -Pointer that moves trough the data.
    char auxBlock[128]; // -Whit this will handle the case of the last blocks.
    ui64 W[80], H[8];

    // -Initializing hash.
    for(i = 0; i < 8; i++) H[i] = (ui64)H0[i];

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
    r >>= 3; // r /= 8 // -Amount of bytes hasn't been processed.
    // !!!!!!!!!!!!!!!!! I'm supposing that r < 128!!!!!!!!!!!!!!
    for(i = 0; i < r; i++) auxBlock[i] = dptr[i];

    // -Putting the '1' at the end of the data. Since the added byte is
    //  1000,0000, we have to decrease k by 7.
    //  !!!!!!!!! Need to check if k-7 is divisible by 8 !!!!!!!!!!!!!!
    auxBlock[r] = (char)0x80; k -= 7;
    k >>= 3; // k /= 8 // -Amount of zero bytes we have to add.
    for(i = r + 1; i < 128 && k > 0; i++, k--) auxBlock[i] = 0;

    if(i == 128) {
        if(debug) {
            std::cout << "\n\n******************************************"
            "*********************************| Block "  << counter++ <<
            " |*********************************************************"
            "****************\n\n";
        }
        processBlock(auxBlock, W, H);
        // -Adding the rest of the zero's
        // !!!!! Need to check if k > 0 !!!!!!
        for(i = 0; i < k; i++) auxBlock[i] = 0;
    }
    // -Putting the size at the end of the block. Changing the purpose
    //  of k, now is a counting variable.
    for(k = 0; k < 16; i++, k++) {
        if(k < 8) auxBlock[i] = size[0] >> (56 - (k << 3));
        else auxBlock[i] = size[1] >> (56 - ((k-8) << 3));
    }
    /*if(debug) {
        std::cout << "\n";
        for(k = 0; k < 128; k++) {
            if(k != 0 && (k & 7) == 0) {
                k == 64 ? std::cout << '\n' : std::cout << ',';
            }
            if(auxBlock[k] < 16 && auxBlock[k] >= 0) std::cout << '0';
            printf("%X", (unsigned char)auxBlock[k]);
        }
        std::cout << "\n\n";
    }*/
    if(debug) {
        std::cout << "\n\n******************************************"
        "*********************************| Block "  << counter++ <<
        " |*********************************************************"
        "****************\n\n";
    }
    processBlock(auxBlock, W, H);
    if(i != 128) std::cout << "\n\n¡Something went wrong!\n\n";

    // -Converting the integers of 64 bits in 8 bytes. i * 8.
    for(i = 0; i < 8; i++) int64_to_8bytes(H[i], &Hash[i << 3]);

}

void Sha512::print(void) const {
    int i;
    for(i = 0; i < 64; i++) {
        if(i != 0 && (i & 7) == 0) printf(",");
        if(Hash[i] < 16 && Hash[i] >= 0) std::cout << '0';
        printf("%X", (unsigned char) Hash[i]);
    }
}

void Sha512::println(void) const {
    print();
    std::cout << '\n';
}

std::ostream& operator << (std::ostream& s, Sha512 sha) {
    int i;
    char a, b; // -Each byte can be represented by two hexadecimal characters
    for(i = 0; i < 64; i++) {
        // If i is not zero and i is divisible by 8 (i & 7 == i % 8) then...
        if(i != 0 && (i & 7) == 0) s << ',';
        a = sha.Hash[i] >> 4;  // -Taking the first four bits.
        b = sha.Hash[i] & 15; // -Taking the last four bits.
        // -Implementing ascii code.
        a < 10 ? a += 48 : a += 55;
        b < 10 ? b += 48 : b += 55;
        // -Sending to output stream.
        s << a; s << b;
    }
    return s;
}

