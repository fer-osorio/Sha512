/* MIT License
 *
 * Copyright (c) 2023 Fernando Osorio
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * SPDX-License-Identifier: MIT
 */

#include "Sha512.hpp"
#define BLOCK_SIZE 1024

int main (int argc, char* argv[])
{
    if(argc > 1) {
        std::cout << '\n' << argv[0] << " does not support command line arguments.\n";
        return 1;
    }

    char buffer[BLOCK_SIZE];
    unsigned sz = 0;

    std::cout << "\nWrite the string you want to hash. To process the string sent the value 'EOF', which you can do by:\n\n"
                 "- Pressing twice the keys CTRL+Z for Windows.\n"
                 "- Pressing twice the keys CTRL+D for Unix and Linux.\n\n";

    while((buffer[sz++] = getchar()) != EOF)
        if(sz == BLOCK_SIZE) std::cout << "\nBuffer exhausted...\n";
    buffer[--sz] = 0;
    Sha512 sha(buffer, sz);
    std::cout << "\nHash :: "; sha.println();

    return 0;
}
