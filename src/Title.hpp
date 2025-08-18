#ifndef TITLE_HPP
#define TITLE_HPP

    #include <iostream>
    #include <string>

    using namespace std;

    #ifndef VERSION
        #define VERSION "Unknown"
    #endif

    #if defined(__clang__)
            const string COMPILER_PLATFORM = "CLANG";
            const string COMPILER_VERSION = to_string(__clang_major__) + "." + to_string(__clang_minor__) + "." + to_string(__clang_patchlevel__);
    #elif defined(__GNUC__)
        const string COMPILER_PLATFORM = "GCC";
        const string COMPILER_VERSION = to_string(__GNUC__) + "." + to_string(__GNUC_MINOR__) + "." + to_string(__GNUC_PATCHLEVEL__);

    #elif defined(_MSC_VER)
        const string COMPILER_PLATFORM = "MSC";
        const string COMPILER_VERSION = to_string(_MSC_FULL_VER);
    #endif

    const string Title = string("Welcome to MiLang!\n") +
                         "Compiled on "                 +
                         __DATE__                       +
                         " "                            +
                         __TIME__                       +
                         " ["                           +
                         COMPILER_PLATFORM              +
                         " "                            +
                         COMPILER_VERSION               +
                         "]\n"                          +
                         "Version: " + VERSION          +
                         "\n"                           +
                         "Variable echo mode has been enabled";

    void title() {
        cout << Title << endl;
    };

#endif