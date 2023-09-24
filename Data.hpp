// -Handling input data.
// -It is mostly aimed to act as a buffer for large amounts of data that can
//  be set as input for the Sha512 algorithm.

struct Data{
	private:
	char* content;
	// -Size of the data in bytes. Upper limit is 2^128 - 1 bytes.
	unsigned size;

	public:
	// Copying bytes from source.
	Data(const char source[], unsigned _size = 0);
	// content <- {NULL, NULL}, size <- 0.
	Data();
	// Releasing memory.
	~Data();

	// -Appends information at the end of the content.
	int append(const char source[], unsigned sz);

	char* getContent() const;
	unsigned getSize() const;
};
