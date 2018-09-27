#include <stdio.h>
#include <cstdlib>
#include "TestLibrary.h"

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

int main()
{
    char msg[100];
    std::wstring osName1;
    std::string osName2;
    int i;
    printf("going to check getCPUSpeedMHz()\n");
    printf("getCPUSpeedMHz : %d\n", getCPUSpeedMHz());
    printf("checked getCPUSpeedMHz()\n");
    // getOSName(osName1, 100);
    // printf("getOSName : %s\n", osName1.c_str());

    // getOSName(osName2, 512);
    // printf("getOSName : %s\n", osName2.c_str());

    char osName[512];
    getOSName(osName, 512);
    printf("getOSName : %s\n", osName);
    printf("checked getOSName()\n");
}