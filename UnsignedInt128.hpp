// -Handling operations with unsigned integers of 128 bits.
// -Designed specifically for the implementation of Sha512.

#include<cstdint>
#include<iostream>

#ifndef _INCLUDED_UNSIGNEDINT128_
#define _INCLUDED_UNSIGNEDINT128_

typedef std::int64_t   i64;
typedef std::uint64_t ui64;
typedef std::uint32_t ui32;

struct UnsignedInt128 {
	private:ui64 digits[2];														// -Interpretation:: digits[0]·2^64 + digits[1]. This is small endian; the reason of
	public:																		//  this selection if pure convenience at the moment of printing
	static const ui64 ui64MAX = 0xFFFFFFFFFFFFFFFF;								// -Maximum value for a 64 bits unsigned int
	UnsignedInt128(): digits{0,0} {}											// -Default constructor. Builds zero.
	UnsignedInt128(ui64 a): digits{a,0} {}										// -Cast from ui64 to UnsignedInt128.
	UnsignedInt128(ui64 a, ui64 b): digits{a,b} {}								// -Constructor. The resulting object will represent the integer a·2^64 + b.
	UnsignedInt128(const UnsignedInt128&);

	UnsignedInt128& operator = (const UnsignedInt128&);

	UnsignedInt128 operator ++ (int);
	UnsignedInt128 operator -- (int);

	UnsignedInt128& operator >>= (int n);										// -Bitwise shift to the right
	UnsignedInt128& operator <<= (int n);										// -Bitwise shift to the left
	bool operator !=(int n);
	bool operator > (int n);
	friend std::ostream& operator << (std::ostream& s, UnsignedInt128 n);
};

UnsignedInt128 ui64product(ui64, ui64);											// Product of two 64 bits integers

#endif