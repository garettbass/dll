#pragma once
#include "cxx/push.h"
#include "cxx/cxx.h"
#include <initializer_list>
#include <tuple>

#ifndef DLL_NAMESPACE
#define DLL_NAMESPACE dll
#endif // DLL_NAMESPACE

namespace DLL_NAMESPACE {

    extern "C" {

    #ifdef CXX_MICROSOFT

        inline void* open(const char* path) {
            extern void* __stdcall LoadLibraryA(const char*);
            return LoadLibraryA(path);
        }

        inline int close(void* module) {
            extern int __stdcall FreeLibrary(void*);
            return not FreeLibrary(module);
        }

        inline void* find(void* module, const char* name) {
            extern void* __stdcall GetProcAddress(void*,const char*);
            return GetProcAddress(module,name);
        }

    #else

        inline void* open(const char* path) {
            extern void* dlopen(const char*,int);
            enum { RTLD_LAZY = 0x1, RTLD_LOCAL = 0x4 };
            return dlopen(path,RTLD_LAZY|RTLD_LOCAL);
        }

        inline int close(void* module) {
            extern int dlclose(void*);
            return dlclose(module);
        }

        inline void* find(void* module, const char* name) {
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
        : address(open(path)) {
            if (not address) printf("library '%s' not found\n",path);
        }

        library(library&& rhs) : address(rhs.address) { new(&rhs) library(); }

        library& operator =(library&& rhs) {
            if (this != &rhs) { new(this) library(std::move(rhs)); }
            return *this;
        }

       ~library() { if (address) { close(address); } }

        explicit operator bool() const { return address != nullptr; }

        operator void*() const { return address; }

        symbol operator ()(const char* name) const {
            return find(address,name);
        }

        symbol operator ()(name_list names) const {
            for (const char* name : names) {
                if (void* const symbol {find(address,name)}) {
                    return symbol;
                }
            }
            return nullptr;
        }

    };

    //--------------------------------------------------------------------------

    template<typename Pointer, typename Result, typename... Params>
    struct function_base {

        using name_list = std::initializer_list<const char*>;

        using result = Result;

        using pointer = Pointer;

        using parameters = std::tuple<Params...>;

        template<unsigned N>
        using parameter = typename std::tuple_element<N,parameters>::type;

        enum { parameter_count = sizeof...(Params) };

        pointer const address = nullptr;

        function_base() = default;

        function_base(decltype(nullptr)) : function_base() {}

        function_base(pointer address) : address(address) {}

        explicit function_base(void* address) : address(pointer(address)) {}

        function_base(const library& library, const char* name)
        : function_base(pointer(library(name))) {
            if (not address) printf("function '%s' not found\n",name);
        }

        function_base(const library& library, name_list names)
        : function_base(pointer(library(names))) {
            if (not address and names.size()) {
                printf("function '%s' not found\n",names.begin()[0]);
            }
        }

        function_base(const function_base& rhs) = default;

        function_base& operator =(const function_base& rhs) {
            if (this != &rhs) { new(this) function_base(rhs); }
            return *this;
        }

        operator pointer() const { return pointer(address); }

        explicit operator bool() const { return address != nullptr; }

        template<typename F>
        explicit operator F() const { return F(address); }

        Result operator ()(Params... params) const {
            return pointer(address)(params...);
        }

    };

    //--------------------------------------------------------------------------

    template<typename Pointer>
    struct function;

    template<typename R, typename... Params>
    struct function<R(Params...)>
    : function_base<R(*)(Params...),R,Params...> {
        using base = function_base<R(*)(Params...),R,Params...>;
        using base::base;
        using base::operator=;
    };

    template<typename R, typename... Params>
    struct function<R(*)(Params...)>
    : function_base<R(*)(Params...),R,Params...> {
        using base = function_base<R(*)(Params...),R,Params...>;
        using base::base;
        using base::operator=;
    };

    #if CXX_MICROSOFT && CXX_X86

    template<typename R, typename... Params>
    struct function<R(__fastcall*)(Params...)>
    : function_base<R(__fastcall*)(Params...),R,Params...> {
        using base = function_base<R(__stdcall*)(Params...),R,Params...>;
        using base::base;
        using base::operator=;
    };

    template<typename R, typename... Params>
    struct function<R(__stdcall*)(Params...)>
    : function_base<R(__stdcall*)(Params...),R,Params...> {
        using base = function_base<R(__stdcall*)(Params...),R,Params...>;
        using base::base;
        using base::operator=;
    };

    template<typename R, typename... Params>
    struct function<R(__thiscall*)(Params...)>
    : function_base<R(__thiscall*)(Params...),R,Params...> {
        using base = function_base<R(__stdcall*)(Params...),R,Params...>;
        using base::base;
        using base::operator=;
    };

    template<typename R, typename... Params>
    struct function<R(__vectorcall*)(Params...)>
    : function_base<R(__vectorcall*)(Params...),R,Params...> {
        using base = function_base<R(__stdcall*)(Params...),R,Params...>;
        using base::base;
        using base::operator=;
    };

    #endif // CXX_MICROSOFT && CXX_X86

} // namespace DLL_NAMESPACE

//------------------------------------------------------------------------------

#if CXX_MICROSOFT

    #ifndef fastcall
    #define fastcall __fastcall
    #endif

    #ifndef stdcall
    #define stdcall __stdcall
    #endif

    #ifndef thiscall
    #define thiscall __thiscall
    #endif

    #ifndef vectorcall
    #define vectorcall __vectorcall
    #endif

#else

    #ifndef fastcall
    #define fastcall
    #endif

    #ifndef stdcall
    #define stdcall
    #endif

    #ifndef thiscall
    #define thiscall
    #endif

    #ifndef vectorcall
    #define vectorcall
    #endif

#endif // !(CXX_MICROSOFT && CXX_X86)

//------------------------------------------------------------------------------

#if CXX_CLANG || CXX_GCC

    #define dllexport __attribute__((visibility("default")))

#elif CXX_MICROSOFT

    #define dllexport __declspec(dllexport)

#endif

//------------------------------------------------------------------------------

#include "cxx/pop.h"