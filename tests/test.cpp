#include <cassert>
#include <cstdio>
#include "../cxx/cxx.h"
#include "../dll.hpp"

int main(int argc, char* argv[]) {
    library shared {"shared.dll"};
    assert(shared);

    int(* const fourty_two)() {shared("fourty_two")};
    assert(fourty_two);
    assert(fourty_two() == 42);

    #if !CXX_OS_MICROSOFT
    #define __stdcall
    #endif // CXX_OS_MICROSOFT

    int(__stdcall* const fourty_three)() {
        shared({
            "fourty_three", // undecorated dllexport
            "fourty_three@0", // mingw stdcall decorated dllexport
            "_fourty_three@0", // msvc stdcall decorated dllexport
        })
    };
    assert(fourty_three);
    assert(fourty_three() == 43);

    puts("OK");
    return 0;
}