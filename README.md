# A simple DLL library in C++

[![Build Status](https://travis-ci.org/garettbass/dll.svg?branch=master)](https://travis-ci.org/garettbass/dll)

``` C++
#include <dll/dll.hpp>

int main(int argc, char* argv[]) {
    library shared {"shared"};
    assert(shared);

    int(* const fourty_two)() {shared("fourty_two")};
    assert(fourty_two);
    assert(fourty_two() == 42);

    return 0;
}
```