#include "../dll.hpp"

extern "C" {

    dllexport int fourty_two() { return 42; }

    dllexport int stdcall fourty_three() { return 43; }

}