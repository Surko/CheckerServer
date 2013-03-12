#include "Server.h"

using namespace std;

SOCKET * Connections;
int counter;

struct Buffer {
	int ID;
	char message[256];
};

int ServerThread(int ID) {
	Buffer sBuffer;

	char* rcvmsg = new char[256];
	ZeroMemory(rcvmsg, 256);

	char * sendmsg = new char[sizeof(Buffer)];
	ZeroMemory(sendmsg, sizeof(Buffer));

	for (;; Sleep(10)) {

		if (recv(Connections[ID], rcvmsg, 256, NULL)) {
			sBuffer.ID = ID;
			memcpy(sBuffer.message, rcvmsg, 256);
			memcpy(sendmsg, &sBuffer, sizeof(Buffer));
		}
		for (int i = 0; i != counter; i++)
			if (Connections[i] == Connections[ID]) {
			} else {
				send(Connections[i], sendmsg, sizeof(Buffer), NULL);
			}

	}
	return 0;
}

void main() {
	char message[200];
	string strmessage;

	long answer;
	WSAData wsaData;
	// Verzia DLL
	WORD dll_version;
	dll_version = MAKEWORD(2,1);

	answer = WSAStartup(dll_version, &wsaData);

	// Definovanie struktury soketu
	SOCKADDR_IN addr;
	int adrlen = sizeof(addr);

	Connections = (SOCKET*)calloc(2, sizeof(SOCKET));

	// SOKETY (listen - nacuva na prichadzajuce pripojenia, connect - operuje s pripojeniami)
	SOCKET sListen;
	SOCKET sConnect;

	sConnect = socket(AF_INET, SOCK_STREAM, NULL);

	// Nastavenie struktury
	addr.sin_addr.s_addr = inet_addr(hostname);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	// Nastavenie listen soketu
	sListen = socket(AF_INET, SOCK_STREAM, NULL);

	// Bindnutie soketu so strukturou
	bind(sListen, (SOCKADDR *)&addr, sizeof(addr));

	// Nastavenie soketu aby pocuval
	listen(sListen, 2);

	for (;;Sleep(50)) {

		cout << "Waiting for connection" << endl;

		if (sConnect = accept(sListen, (SOCKADDR *)&addr, &adrlen)) {
			cout << "Connection found " << endl;	

			Connections[counter] = sConnect;

			char * ID = new char[64];
			ZeroMemory(ID, sizeof(ID));

			itoa(counter, ID, 10);
			send(Connections[counter], ID, sizeof(ID), NULL);

			counter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) ServerThread,(LPVOID)(counter - 1), NULL, NULL);
		}
	}
}