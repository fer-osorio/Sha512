#include"UnsignedInt128.hpp"

UnsignedInt128::UnsignedInt128() : number{0,0} {}

UnsignedInt128::UnsignedInt128(ui64 a, ui64 b)
: number{a,b} {/*Nothing more to do.*/}

UnsignedInt128::UnsignedInt128(const UnsignedInt128& a) {
    copyConstructor(*this, a);
}

UnsignedInt128& UnsignedInt128::operator = (const UnsignedInt128& a) {
    copyAssigment(*this, a);
    return *this;
}

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
    if(n > 128) n &= 127; // n %= 128

    // -Case n < 64
    if(n < 64) {
        number[1] >>= n;
        number[1] |= (number[0] << (64 - n));
        number[0] >>= n;
    } else { // -Case n >= 64
        number[1] = number[0] >> (n - 64);
        number[0] = 0;
    }
    return *this;
}

UnsignedInt128& UnsignedInt128::operator <<= (int n) {
    if(n < 0) n = -n;
    if(n > 128) n %= 128;

    // -Case n < 64
    if(n < 64) {
        number[0] <<= n;
        number[0] |= (number[1] >> (64 - n));
        number[1] <<= n;
    } else { // -Case n >= 64
        number[0] = number[1] << (n - 64);
        number[1] = 0;
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

UnsignedInt128& copyAssigment(UnsignedInt128& a,const UnsignedInt128& b) {
    a.number[0] = b.number[0];
    a.number[1] = b.number[1];
    return a;
}

void copyConstructor(UnsignedInt128& a,const UnsignedInt128& b){
    a.number[0] = b.number[0];
    a.number[1] = b.number[1];
}

// Multiplication of integers of 64 bits.------------------------------------

// 0 <= a < 2^64 & 0 <= b < 2^64;
// ua are the most significant 32 bits (upper a) and ua are the least
// significant 32 bits (lower a); same with b.
// a = 2^32·ua + la with 0 <= ua, la < 2^32
// b = 2^32·ub + lb with 0 <= ub, lb < 2^32
// 0 <= a·b < 2^128

// a·b = (2^32·ua + la)·(2^32·ub + lb) =
//     = 2^64·(ua·ub) + 2^32·(ua·lb) + 2^32·(la·ub) + la·lb
//     = 2^64·(ua·ub) + 2^32·(ua·lb + la·ub) + la·lb := r
//       |----1-----|   |---------2--------|   |-3-|

// Assuming ua, ub, la, lb > 0 ==> 1 <= ua·ub, ua·lb, la·ub, la·lb < 2^64 &
// i)   2^64 <= 2^64·(ua·ub) < 2^128
// ii)  ua·lb + la·ub <= 2·(2^32-1)·(2^32-1) = 2·(2^64 - 2·2^32 + 1), then
//      2^32 < 2^32·(ua·lb + la·ub) < 2^97 &
// iii) 0 < la·lb < 2^64

// How many bits takes the expression ua·lb + la·ub + u(la·lb)?
// la·lb = 2^32·u(la·lb) + l(la·lb) with 0 <= l(la·lb), u(la·lb) < 2^32 ==>
// a·b = 2^64·(ua·ub) + 2^32·(ua·lb + la·ub + u(la·lb)) + l(la·lb)
// From (ii)  0 <= ua·lb + la·ub + u(la·lb) <= 2·(2^64 - 2·2^32 + 1) + 2^32
//                                        = 2·(2^64 - 2·2^32 + 2^31 + 1)
//                                        < 2·(2^64 - 2^32 + 1) < 2·2^64
// In brief 0 <= ua·lb + la·ub + u(la·lb) < 2^65 then we can write
// ua·lb + la·ub + u(la·lb) = d·2^64 + x with d in {0,1} and 0 <= x < 2^64
// Finally we can write:
// a·b = 2^64·(ua·ub + d) + 2^32·x + l(la·lb)

// But, how do we calculate x and d?

// Lets proof (a + b)%n = (a%n + b)%n.
// s = q(s)·n + r(s) 0 <= r(s) < |n|
// a = q(a)·n + r(a); b = q(b)·n + r(b); a + b = q(a + b)·n + r(a + b)
// define R := r(a) +b, then R = q(R)·n + r(R)
// then a + b = q(a)·n + r(a) + b
//            = (q(a) + q(R)·n + r(R)
//            = (q(a) + q(R))·n + r(R)
//            = (q(a) + q(r(a) + b))·n + r(r(a) + b)
// From Euclid's division lemma, we just proved that (a + b)%n = (a%n + b)%n
// The proofs for (a + b)%n = (a + b%n)%n is similar.

// Lets proof (a + b)/n = a/n + b/n + (a%n + b%n)/n
// define R := r(a) + r(b), then R = q(R)·n + r(R)
// then a + b = q(a)·n + r(a) + q(b)·n + r(b)
//            = [q(a) + q(b)] + q(R)·n + r(R)
//            = [q(a) + q(b) + q(R)]·n + r(R)
//            = [q(a) + q(b) + q( r(a)+r(b) )]·n + r(r(a) + r(b))
// From Euclid's division lemma, we just proved that (a + b)/n =
// a/n + b/n + (a%n + b%n)/n.
// Also, we proved (a + b)%n = (a%n + b%n)%n

// (a+b+c)/n = (a+b)/n + c/n + [(a+b)%n + c%n]/n
//           = a/n + b/n + c/n + (a%n + b%n)/n + [(a+b)%n + c%n]/n
//           = a/n + b/n + c/n + (a%n + b%n)/n + [(a%n+b)%n + c%n]/n

// Facts for integers and integral operations:
// First fact::  (a+b)%n == (a%n + b)%n
// Second fact:: x = (x/n)*n + x % n
// Third fact::  (a+b)/2 = a/2 + b/2 + (a%2 + b%2)/2
// Fourth fact:: x % 2^n == x & (n-1)
// Fifth fact::  x·2^n == x << n and x / 2^n == x >> n

// From right to left, d represents the 64th bit (counting from zero), but we
// have just 63 bits positions. Somehow we have to break down the expression
// ua·lb + la·ub + u(la·lb) in other expressions that fit in 64 bits and allow
// us to know the value of d.
// Using the facts:
// ua·lb + la·ub + u(la·lb) = {[ua·lb + la·ub + u(la·lb)] / 2} * 2 +
//                             [ua·lb + la·ub + u(la·lb)] % 2
//                          = {[ua·lb / 2 + la·ub / 2 + u(la·lb) / 2 +
//                             (ua·lb % 2 + la·ub % 2)/2 +
//                             [(ua·lb % 2 + la·ub) % 2 +
//                             u(la·lb) % 2] / 2} * 2 +
//                             [(ua·lb % 2 + la·ub) % 2 + u(la·lb)] % 2
//                          = {[ua·lb >> 1 + la·ub >> 1 + u(la·lb) >> 1 +
//                             (ua·lb & 1 + la·ub & 1) >> 1 +
//                             [(ua·lb & 1 + la·ub) & 1 +
//                             u(la·lb) & 1] >> 1} << 1 +
//                             [(ua·lb & 1 + la·ub) & 1 + u(la·lb)] & 1
//                          = {[ua·lb >> 1 + la·ub >> 1 + u(la·lb) >> 1 +
//                             (ua·lb & 1) & (la·ub & 1)  +
//                             [(ua·lb & 1 + la·ub)&1] & [u(la·lb)&1]} << 1 +
//                             [(ua·lb & 1 + la·ub) & 1 + u(la·lb)] & 1

// Notice that the operations between braces can be performed inside 64 bits;
// moreover, the last bit of this result is exactly the desired value d.
// Finally, to get x just notice that the shift to the left after the closing
// brace is going to eliminate the d bit (at this point that bit must have
// been saved somewhere), leaving the rest of the value, witch is exactly x.
// After all this, we are finally prepared for the scripting of the
// multiplication algorithm for integers of 64 bits.

UnsignedInt128 ui64product(ui64 a, ui64 b) {
    ui64 ones = 0xFFFFFFFF; // first 32 bits are ones.
    // first 32 bits of a,   last 32 bits of a
    ui64 lower_a = a & ones, upper_a = (a & (ones << 32)) >> 32;
    // first 32 bits of b,   last 32 bits of b
    ui64 lower_b = b & ones, upper_b = (b & (ones << 32)) >> 32;

    ui64 lalb = lower_a * lower_b;
    ui64 laub = lower_a * upper_b;
    ui64 ualb = upper_a * lower_b;
    ui64 uaub = upper_a * upper_b;
//  ua·lb + la·ub + u(la·lb) = {[ua·lb >> 1 + la·ub >> 1 + u(la·lb) >> 1 +
//                             (ua·lb & 1) & (la·ub & 1)  +
//                             [(ua·lb & 1 + la·ub)&1] & [u(la·lb)&1]} << 1 +
//                             [(ua·lb & 1 + la·ub) & 1 + u(la·lb)] & 1
    ui64 ulalb = (lalb & (ones << 32)) >> 32, x, d;
    x = (ualb >> 1) + (laub >> 1) + (ulalb >> 1) +
        (ualb & 1) + (laub & 1) + ((((ualb & 1) + laub) & 1) & ulalb & 1);
    d = (x & ((ui64)1 << 63)) >> 63;
    x = (x << 1) + ( ( ( ( (ualb & 1) + laub) & 1) + ulalb) & 1);

    UnsignedInt128 result(uaub + (x >> 32) + d, (x << 32) + lalb);
    return result;
}

/*(2^{64}-1)^2 2^{128}-2^{67}+1

int main(void) {
    ui64 a = 0xFFFFFFFFFFFFFFFF, b = 0xFFFFFFFFFFFFFFFF;
    UnsignedInt128 r = ui64product(a,b);
    std::cout << r << '\n';
    return 0;
}*/

