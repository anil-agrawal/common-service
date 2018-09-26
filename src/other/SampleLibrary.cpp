#include <stdio.h>

// Ref : https://renenyffenegger.ch/notes/development/languages/C-C-plus-plus/GCC/create-libraries/index

// Create Object files
// g++ -c -fPIC SampleLibrary.cpp -o SampleLibrary.o

// Create static library with object files
// ar rcs ../../app/libTestLib.a SampleLibrary.o

// Create shared library with object files
// g++ -shared SampleLibrary.o -o ../../app/libTestLib.so

// Create shared library Without object files
// g++ -shared -fPIC SampleLibrary.cpp -o ../../app/libTestLib.so

/*

Link statically
With the static library, we can statically link main.o with the library.
The -L flag indicates (non standard) directory where the libraries can be found.
The -l flag indicates the name of the library. 
Note, that it assumes the library to start with lib and end with .o (so lib and .o must not be specified)

Ex: g++ TestLibrary.cpp -L../../app -lTestLib

*/

/*

Link dynamically with the shared library
Note the similarity to linking with the static library. Since we specify the shared directory bin/shared with the -L flag, the linker links with libtq84.so`.

Ex: g++ TestLibrary.cpp -L../../app -l:libTestLib.so

*/

void __attribute__((constructor)) initLibrary(void)
{
    //
    // Function that is called when the library is loaded
    //
    printf("Library is initialized\n");
}
void __attribute__((destructor)) cleanUpLibrary(void)
{
    //
    // Function that is called when the library is »closed«.
    //
    printf("Library is exited\n");
}

int getCPUSpeedMHz(){
    printf("Library function getCPUSpeedMHz() called.\n");
    return 100;
}

bool test1(){
    printf("Library function test1() called.\n");
    return true;
}

void test2(){
    printf("Library function test2() called.\n");
}