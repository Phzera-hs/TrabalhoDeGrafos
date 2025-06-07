#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <windows.h>


// ------------------------- UTIL -----------------------------
void limparTela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
