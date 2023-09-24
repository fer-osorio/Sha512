// -Handling operations with unsigned integers of 128 bits.
// -Designed specifically for the implementation of Sha512.

#include<cstdint>
#include<iostream>

typedef std::int64_t   i64;
typedef std::uint64_t ui64;

struct UnsignedInt128 {
	private:
	// -Interpretation:: number[0]·2^64 + number[1]
	ui64 number[2];

	public:
	// -Constructor. The resulting object will represent
	//  the integer a·2^64 + b.
	UnsignedInt128(ui64 a, ui64 b);

	// -Necessary operators.

	// -Addition of one unit.
	UnsignedInt128 operator ++ (int);
	// -Subtraction of one unit.
	UnsignedInt128 operator -- (int);

	// -Bitwise shift to the right.
	// -Intended for the application of the integer division
	//  operation when the denominator is a power of two.
	UnsignedInt128& operator >>= (int n);

	// -Comparison operator.
	bool operator != (int n);

	// -Bigger than operator.
	bool operator > (int n);

	// -Insertion operator.
	friend std::ostream& operator << (std::ostream& s, UnsignedInt128 n);
};
