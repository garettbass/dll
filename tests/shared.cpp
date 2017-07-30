#include "../cxx/cxx.h"
#include "../dll.hpp"

extern "C" {

    dllexport int fourty_two() { return 42; }

    #if !CXX_OS_MICROSOFT
    #define __stdcall
    #endif // CXX_OS_MICROSOFT

    dllexport int __stdcall fourty_three() { return 43; }

}