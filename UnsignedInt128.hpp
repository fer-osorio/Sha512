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
	private:
	// -Interpretation:: number[0]·2^64 + number[1]
	ui64 number[2];

	public:
	// -Default constructor. Builds zero.
	UnsignedInt128();
	// -Constructor. The resulting object will represent
	//  the integer a·2^64 + b.
	UnsignedInt128(ui64 a, ui64 b);
	UnsignedInt128(const UnsignedInt128&);

	UnsignedInt128& operator = (const UnsignedInt128&);

	UnsignedInt128 operator ++ (int);
	UnsignedInt128 operator -- (int);

	// -Bitwise shift to the right.
	// -Intended for the application of the integer division
	//  operation when the denominator is a power of two.
	UnsignedInt128& operator >>= (int n);
	// -Bitwise shift to the left.
	// -Intended for the multiplication by power of two.
	UnsignedInt128& operator <<= (int n);
	bool operator != (int n);
	bool operator > (int n);
	friend std::ostream& operator << (std::ostream& s, UnsignedInt128 n);
};

UnsignedInt128 ui64product(ui64, ui64);

#endif