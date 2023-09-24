#include"UnsignedInt128.hpp"

UnsignedInt128::UnsignedInt128(ui64 a, ui64 b)
: number{a,b} {/*Nothing more to do.*/}

UnsignedInt128 UnsignedInt128::operator ++ (int) {
    UnsignedInt128 original(number[0], number[0]);
    if(number[0] == 0) { // Special case < 2^64
        if(number[1] == ui64(-1)) {
            // -Digit 1 has the maximum value for an
            //  unsigned integer of 64 bits, then we
            //  have to use the "other digit".
            number[0] = 1; number[1] = 0;
        }
        else
            // -Number is smaller than 2^64 - 1, so
            //  we can handle this case as follows
            number[1]++;

        return original;
    }

     // -Case >= 2^64
    if(number[0] == ui64(-1)) {
        // -Digit 0 has the maximum value for an
        //  unsigned integer of 64 bits.
        if(number[1] == ui64(-1))
            // -At this point we have the maximum value
            //  for an unsigned integer of 128 bits
            //  (2^128-1), then adding 1 will take us
            //  back to zero.
            number[1] = number[0] = 0;
        else
            number[1]++;
    }
    else number[1]++;

    return original;
}

UnsignedInt128 UnsignedInt128::operator -- (int) {
    UnsignedInt128 original(number[0], number[0]);
    if(number[0] == 0) { // Special case < 2^64
        if(number[1] == 0)
            // -If our number is zero, we set the our
            //  value as 2^128 - 1 because 2^128 - 1
            //  is congruent with -1 modulus 128.
            //  Maybe we can change this.
            number[0] = number[1] = ui64(-1);
        else
            // -Number not zero, but is smaller than
            //  2^64, so we can handle this case as
            //  follows
            number[1]--;

        return original;
    }
     // -Case >= 2^64
    if(number[1] == 0) {
        // -Case when we need to borrow.
        number[0]--;
        number[1] = ui64(-1);
    }
    else number[1]--;

    return original;
}

UnsignedInt128& UnsignedInt128::operator >>= (int n) {
    if(n < 0) n = -n;
    if(n > 128) n %= 128;

    // -Case n < 64
    if(n < 64) {
        number[1] >>= n;
        number[1] |= (number[0] << (64 - n));
        number[0] >>= n;
    } else { // -Case n >= 64
        number[0] = 0;
        number[1] >>= (n - 64);
    }
    return *this;
}

bool UnsignedInt128::operator != (int n) {
    if(n < 0) return true;  //  -Because our number is unsigned.

    // number[0] > 0 then number[0]·2^64 + number[1] > 2^31-1.
    if(number[0] > 0) return true;

    // Remember: 0xFFFFFFF == 2^31 - 1 maximum value for an int.
    if(number[1] > ui64(0xFFFFFFF)) return true;

    return n != int(number[1]);
}

bool UnsignedInt128::operator > (int n) {
    if(n < 0) return true;  //  -Because our number is unsigned.

    // number[0] > 0 then number[0]·2^64 + number[1] > 2^31-1.
    if(number[0] > 0) return true;

    // Remember: 0xFFFFFFF == 2^31 - 1 maximum value for an int.
    if(number[1] > ui64(0xFFFFFFF)) return true;

    return n > int(number[1]);
}

std::ostream& operator << (std::ostream& s, UnsignedInt128 n) {
    int i, j;
    char c;
    bool nonZero_found = false;
    for(i = 0; i < 2; i++)
        for(j = 0; j < 16; j++) {
            c = (char) (n.number[i] >> ((15 - j) << 2)) & (char)15;
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

