all: Sha512


WARNINGS = -Wall -Weffc++ -Wextra -Wsign-conversion -pedantic-errors
DEBUG = -ggdb -fno-omit-frame-pointer
OPTIMIZE = -O2
STANDARD = -std=c++2a
SOURCE = main.cpp Sha512.cpp UnsignedInt128.cpp
HEADERS = Sha512.hpp UnsignedInt128.hpp

Sha512: Makefile $(SOURCE) $(HEADERS)
	$(CXX) -o $@ $(WARNINGS) $(DEBUG) $(OPTIMIZE) $(STANDARD) $(SOURCE)

clean:
	rm -f Sha512

# Builder will call this to install the application before running.
install:
	echo "Installing is not supported"

# Builder uses this target to run your application.
run:
	./Sha512

