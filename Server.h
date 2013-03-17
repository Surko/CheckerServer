#pragma comment(lib, "Ws2_32.lib")

#include <sdkddkver.h>
#include <conio.h>
#include <stdio.h>

#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <string>

// Host na koreho sa napajame, mozne zmenit na nacitavanie z txt suboru
// alebo nacitanie priamo z hry.
static const char * hostname = "127.0.0.1";
// Port na ktorom hra bezi
static const int port = 5000;