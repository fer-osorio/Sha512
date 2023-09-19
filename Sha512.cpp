// -Definitions for the Sha512 Functions.

#include"Sha512.hpp"

Sha512::Sha512(const char data[], const ui64 size[2]) {
    calculateHash(data, size);
}

// -Rotation to the right.
i64 Sha512::RotR(i64 x, int n) {
    if(n <  0) n = -n;
    if(n > 64) n %= 64;
    return x >> n | x << (64 - n);
}

// -Rotation to the left.
i64 Sha512::RotL(i64 x, int n) {
    if(n <  0) n = -n;
    if(n > 64) n %= 64;
    return x << n | x >> (64 - n);
}

i64 Sha512::Ch(i64 x, i64 y, i64 z) {
    return x&y ^ ~x&z;
}

i64 Sha512::Maj(i64 x, i64 y, i64 z) {
    return x&y ^ x&z ^ y&z;
}

i64 Sha512::SIGMA0(i64 x) {
    return RotR(x, 28) ^ RotR(x, 34) ^ RotR(x, 39);
}

i64 Sha512::SIGMA1(i64 x) {
    return RotR(x, 14) ^ RotR(x, 18) ^ RotR(x, 41);
}

i64 Sha512::sigma0(i64 x) {
    return RotR(x, 1) ^ RotR(x, 8) ^ (x >> 7);
}

i64 Sha512::sigma1(i64 x) {
    return RotR(x, 19) ^ RotR(x, 61) ^ (x >> 6);
}

i64 Sha512::_8bytes_to_int64(const char bytes[8]) {
    i64 r = 0; // -64 zero bits
    for(int i = 0; i < 8; i++) {
        // Making room for the bits of the next byte
        r <<= 8;
        // Allocating bits at the end.
        r |= i64(bytes[i]);
    }
    return r;
}

void Sha512::int64_to_8bytes(const i64 n, char result[8]) {
    int i;
    for(i = 0; i < 8; i++)  // (7-i) * 8
        result[i] = char(n >> ((7-i) << 3));
}

void Sha512::processBlock(const char M[128], i64 W[80], i64 H[8]) {
    i64 a, b, c, d, e, f, g, h, T1, T2; // Working variables
    const char *dptr = M;
    int i;
    // -Creating message schedule.
        for(i = 0; i < 80; i++) {
            if(i < 16) {
                W[i] = _8bytes_to_int64(dptr);
                dptr += 8;
            } else
                W[i] = sigma1(W[i-2]) + W[i-7] + sigma0(W[i-15]) + W[i-16];
        }
        // -Initializing working variables.
        a = H[0]; b = H[1]; c = H[2]; d = H[3];
        e = H[4]; f = H[5]; g = H[6]; h = H[7];

        for(i = 0; i < 80; i++) {
            T1 = h + SIGMA1(e) + Ch(e, f, g) + i64(K[i]) + W[i];
            T2 = SIGMA0(a) + Maj(a, b, c);
            h = g; g = f; f = e; e = d + T1;
            d = c; c = b; b = a; a = T1 + T2;
        }
        // -Computing intermediate value.
        H[0] += a; H[4] += e;
        H[1] += b; H[5] += f;
        H[2] += c; H[6] += g;
        H[3] += d; H[7] += h;
}

void Sha512::calculateHash(const char data[], const ui64 size[2]) {
    int r = size[1] & 1023, k = 895 - r, i; // k = 895 - size % 1024
    int blocksAdded = 1;

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
    i64 W[80], H[8];

    // -Initializing hash.
    for(i = 0; i < 8; i++) H[i] = (i64)H0[i];

    while(N > blocksAdded) {
        processBlock(dptr, W, H);
        dptr += 128;
        N--;
    }
    r >>= 3; // r /= 8
    for(i = 0; i < r; i++) auxBlock[i] = dptr[i];
    auxBlock[r] = 0x80; k -= 7; k >>= 8;
    for(i = r + 1; i < 128 && k > 0; i++, k--) auxBlock[i] = 0;
    if(i == 128) processBlock(auxBlock, W, H);

    // -Converting the integers of 64 bits in 8 bytes. i * 8.
    for(i = 0; i < 8; i++) int64_to_8bytes(H[i], &Hash[i << 3]);
}

void Sha512::print(void) {
    for(int i = 0; i < 64; i++) printf("%X", (unsigned char) Hash[i]);
}

void Sha512::println(void) {
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
