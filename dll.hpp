#pragma once
#include "cxx/push.h"
#include "cxx/cxx.h"
#include <initializer_list>
#include <new>

//------------------------------------------------------------------------------

#if CXX_CLANG || CXX_GCC

    #define dllexport __attribute__((visibility("default")))

#elif CXX_MSVC

    #define dllexport __declspec(dllexport)

#endif

//------------------------------------------------------------------------------

#ifdef DLL_NAMESPACE
namespace DLL_NAMESPACE {
#endif // DLL_NAMESPACE

    extern "C" {

    #ifdef CXX_OS_MICROSOFT

        inline void* dll_open(const char* path) {
            extern void* __stdcall LoadLibraryA(const char*);
            return LoadLibraryA(path);
        }

        inline int dll_close(void* module) {
            extern int __stdcall FreeLibrary(void*);
            return not FreeLibrary(module);
        }

        inline void* dll_find(void* module, const char* name) {
            extern void* __stdcall GetProcAddress(void*,const char*);
            return GetProcAddress(module,name);
        }

    #else

        inline void* dll_open(const char* path) {
            extern void* dlopen(const char*,int);
            enum { RTLD_LAZY = 0x1, RTLD_LOCAL = 0x4 };
            return dlopen(path,RTLD_LAZY|RTLD_LOCAL);
        }

        inline int dll_close(void* module) {
            extern int dlclose(void*);
            return dlclose(module);
        }

        inline void* dll_find(void* module, const char* name) {
            extern void* dlsym(void*,const char*);
            return dlsym(module,name);
        }

    #endif

        extern int printf(const char*,...);

    } // extern "C"

    //--------------------------------------------------------------------------

    struct library final {

        using name_list = std::initializer_list<const char*>;

        struct symbol {
            void* address = nullptr;
            symbol() = default;
            symbol(decltype(nullptr)) : symbol() {}
            symbol(void* address) : address(address) {}
            explicit operator bool() const { return address != nullptr; }
            template<typename T> operator T*() const { return (T*)address; }
            symbol operator *() const { return *((void**)address); }
        };

        void* const address = nullptr;

        library() = default;

        library(decltype(nullptr)) : library() {}

        library(const char* path)
        : address(dll_open(path)) {
            if (not address) printf("library '%s' not found\n",path);
        }

        library(library&& rhs) : address(rhs.address) { new(&rhs) library(); }

        library& operator =(library&& rhs) {
            if (this != &rhs) { new(this) library(std::move(rhs)); }
            return *this;
        }

       ~library() { if (address) { dll_close(address); } }

        explicit operator bool() const { return address != nullptr; }

        operator void*() const { return address; }

        symbol operator ()(const char* name) const {
            return dll_find(address,name);
        }

        symbol operator ()(name_list names) const {
            for (const char* name : names) {
                if (void* const symbol {dll_find(address,name)}) {
                    return symbol;
                }
            }
            return nullptr;
        }

    };

#ifdef DLL_NAMESPACE
} // namespace DLL_NAMESPACE
#endif // DLL_NAMESPACE

//------------------------------------------------------------------------------

#include "cxx/pop.h"