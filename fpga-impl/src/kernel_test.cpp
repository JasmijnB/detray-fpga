#include <iostream>
#include "detray/core/detector.hpp"

int main() {
    std::cout << "C++ Standard Version Information:\n";
    std::cout << "=================================\n";
    
    // Print the __cplusplus macro value
    std::cout << "__cplusplus macro value: " << __cplusplus << "\n";
    
    // Determine and print the C++ standard version
    std::cout << "C++ Standard: ";
    #if __cplusplus >= 202302L
        std::cout << "C++23 or later";
    #elif __cplusplus >= 202002L
        std::cout << "C++20";
    #elif __cplusplus >= 201703L
        std::cout << "C++17";
    #elif __cplusplus >= 201402L
        std::cout << "C++14";
    #elif __cplusplus >= 201103L
        std::cout << "C++11";
    #elif __cplusplus >= 199711L
        std::cout << "C++98/C++03";
    #else
        std::cout << "Pre-standard C++";
    #endif
    std::cout << "\n";
    
    // Print compiler information if available
    std::cout << "\nCompiler Information:\n";
    std::cout << "=====================\n";
    
    #ifdef __GNUC__
        std::cout << "GCC version: " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << "\n";
    #endif
    
    #ifdef __clang__
        std::cout << "Clang version: " << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__ << "\n";
    #endif
    
    return 0;
}
