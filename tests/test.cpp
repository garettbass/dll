#include "../dll.hpp"
#include <cassert>
#include <cstdio>

int main(int argc, char* argv[]) {
    dll::library shared {"shared.dll"};
    assert(shared);

    {
        dll::function<int()> fourty_two {shared,"fourty_two"};
        assert(fourty_two);
        assert(fourty_two() == 42);
    }

    {
        dll::function<int(*)()> fourty_two {shared,"fourty_two"};
        assert(fourty_two);
        assert(fourty_two() == 42);
    }

    {
        dll::function<int(stdcall*)()> fourty_three {
            shared,{
                "fourty_three",
                "fourty_three@0", // mingw stdcall dllexport
                "_fourty_three@0", // msvc stdcall dllexport
            }
        };
        assert(fourty_three);
        assert(fourty_three() == 43);
    }

    puts("OK");
    return 0;
}