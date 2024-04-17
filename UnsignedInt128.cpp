#include"UnsignedInt128.hpp"

union ui64toui32{
    ui64 _ui64_;                                                                // -Intended for cast from a unsigned integer of 64 bits to an array of two unsigned
    ui32 _ui32[2];                                                              //  integers of 32 bits
};

UnsignedInt128::UnsignedInt128(const UnsignedInt128& a) {
    this->digits[0] = a.digits[0];
    this->digits[1] = a.digits[1];
}

UnsignedInt128& UnsignedInt128::operator = (const UnsignedInt128& b) {
    if(&b != this) {
        this->digits[0] = b.digits[0];
        this->digits[1] = b.digits[1];
    }
    return *this;
}

UnsignedInt128 UnsignedInt128::operator ++ (int) {                              // -digits[0]^64 + digits[1]
    UnsignedInt128 r = *this;
    if(r.digits[1] == ui64MAX) {
        if(r.digits[0] == ui64MAX) return 0;                                    // -Maximum value for 128 bits integer plus 1. Returning 0
        else {
            r.digits[1] = 0;                                                    // -Carriage generated
            r.digits[0]++;                                                      // -Adding carriage
        }
    } else r.digits[1]++;
    return r;
}

UnsignedInt128 UnsignedInt128::operator -- (int) {                              // -digits[0]^64 + digits[1]
    UnsignedInt128 r = *this;
    if(r.digits[1] == 0) {
        if(r.digits[0] == 0) {                                                  // Zero minus one, returning maximum value for a 128 bit integer
            r.digits[0] = ui64MAX;
            r.digits[1] = ui64MAX;
            return r;
        }
        else {
            r.digits[0]--;                                                      // -Taking a loan
            r.digits[1] = ui64MAX;                                              // -Applying loan
        }
    } else r.digits[1]--;
    return r;
}

UnsignedInt128& UnsignedInt128::operator >>= (int n) {
    if(n < 0)   n =  -n;
    if(n > 128) n &= 127;                                                       // n %= 128
    if(n < 64) {                                                                // -Case n < 64
        digits[1] >>= n;
        digits[1] |= (digits[0] << (64 - n));
        digits[0] >>= n;
    } else {                                                                    // -Case n >= 64
        digits[1] = digits[0] >> (n - 64);
        digits[0] = 0;
    }
    return *this;
}

UnsignedInt128& UnsignedInt128::operator <<= (int n) {
    if(n < 0) n = -n;
    if(n > 128) n %= 128;
    if(n < 64) {                                                                // -Case n < 64
        digits[0] <<= n;
        digits[0] |= (digits[1] >> (64 - n));
        digits[1] <<= n;
    } else {                                                                    // -Case n >= 64
        digits[0] = digits[1] << (n - 64);
        digits[1] = 0;
    }
    return *this;
}

bool UnsignedInt128::operator != (int n) {
    if(n < 0) return true;                                                      // -Because our number is unsigned.
    if(digits[0] > 0) return true;                                              //  digits[0] > 0 then digits[0]·2^64 + digits[1] > 2^31-1.
    if(digits[1] > ui64(0x7FFFFFFF)) return true;                               //  Remember: 0x7FFFFFFF == 2^31 - 1 maximum value for an int.
    return n != int(digits[1]);
}

bool UnsignedInt128::operator > (int n) {
    if(n < 0) return true;                                                      // -Because our number is unsigned.
    if(digits[0] > 0) return true;                                              // digits[0] > 0 then digits[0]·2^64 + digits[1] > 2^31-1.
    if(digits[1] > ui64(0x7FFFFFFF)) return true;                               // Remember: 0x7FFFFFFF == 2^31 - 1 maximum value for an int.
    return int(digits[1]) > n;
}

std::ostream& operator << (std::ostream& s, UnsignedInt128 n) {
    int i, j;
    char c;
    bool nonZero_found = false;
    for(i = 0; i < 2; i++)
        for(j = 0; j < 16; j++) {
            c = (char) (n.digits[i] >> ((15 - j) << 2)) & (char)15;
            if(c != 0) nonZero_found = true;
            if(nonZero_found) {
                if(c < 10) c += 48;
                else c += 55;
                s << c;
            }
        }
    if(!nonZero_found) s << '0';
    return s;
}

UnsignedInt128 ui64product(ui64 a, ui64 b) {
    ui64toui32 _a_, _b_, c = {0}, d = {0}, buff;
    _a_._ui64_ = a; _b_._ui64_ = b;                                             // Cast from ui64 to ui32[2]

    c._ui64_    = _a_._ui32[0]*_b_._ui32[0];                                    // Using the pencil and paper multiplication algorithm
    buff._ui64_ = _a_._ui32[0]*_b_._ui32[1] + c._ui32[1];                       // ...
    c._ui32[1]  = buff._ui32[0];                                                // ...
    d._ui32[0]  = buff._ui32[1];                                                // ...
    buff._ui64_ = _a_._ui32[1]*_b_._ui32[0] + c._ui32[1];                       // ...
    c._ui32[1]  = buff._ui32[0];                                                // ...
    buff._ui64_ = _a_._ui32[1]*_b_._ui32[1] + d._ui32[0] + buff._ui32[1];       // ...
    d._ui32[0]  = buff._ui32[0];                                                // ...
    d._ui32[1] += buff._ui32[1];                                                // ...

    return UnsignedInt128(d._ui64_, c._ui64_);
}

/*(2^{64}-1)^2 2^{128}-2^{67}+1

int main(void) {
    ui64 a = 0xFFFFFFFFFFFFFFFF, b = 0xFFFFFFFFFFFFFFFF;
    UnsignedInt128 r = ui64product(a,b);
    std::cout << r << '\n';
    return 0;
}*/

