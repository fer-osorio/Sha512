// -Handling input data.
// -It is mostly aimed to act as a buffer for large amounts of data that can
//  be set as input for the Sha512 algorithm.
#define BLOCK_SIZE 1024

struct Data{
	private: struct Block {
		char bytes[BLOCK_SIZE];
		Block* next;
	};
	Block *content, *last;
	unsigned size; // -Size of data in bytes. Upper limit is 2^32 - 1.

	public:
	// Copying bytes from source.
	Data(const char source[], unsigned _size = 0);

	Data(); // content <- {NULL, NULL}, size <- 0.

	~Data(); // Releasing memory.

	// -Appends information at the end of the content.
	int append(const char source[], unsigned sz);

	Block* getContent() const;
	unsigned getSize() const;
};
