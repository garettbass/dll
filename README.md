``` C++
#include <dll/dll.hpp>

int main(int argc, char* argv[]) {
    dll::library shared {"shared"};
    assert(shared);

    dll::function<int()> fourty_two {shared,"fourty_two"};
    assert(fourty_two);
    assert(fourty_two() == 42);

    return 0;
}
```