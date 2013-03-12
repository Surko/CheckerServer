#pragma comment(lib, "Ws2_32.lib")

#include <sdkddkver.h>
#include <conio.h>
#include <stdio.h>

#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <string>

static const char * hostname = "127.0.0.1";
static const int port = 5000;