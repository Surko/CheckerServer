#include "Server.h"
#include <vector>

using namespace std;

// Socket s pripojenim.
SOCKET * Connections;
// Pocitadlo pripojeni (moznych pripojeni = 2)
int counter;

// Obsadenie bielej strany. Rozhodovanie ako priraduje strany hracom.
int whiteSide = -1;

/*
Buffer - Stavba spravy ktora prichadza.
ID - id hraca/PC ktoremu spravu posielame.
Message - samotna sprava
*/
struct Buffer {
	int ID;
	char message[256];
};

/*
Serverove vlakno spracovavajuce spravy od hraca zadaneho parametrom ID.
Vlakno prijima spravy a nasledne ich hned odposle druhemu hracovi.
Pri strateni spojenia (parameter ret) vypise do konzoli akeho hraca stratil, uvolni stranu a
druhemu hracovi (ked existuje) odposle spravu o uteku jeho oponenta. 
Prijimanie a odposielanie sprav prebieha v cykle so Sleepom 10 milisekund.
Cyklus zanika pri strate hraca.
*/
int ServerThread(int ID) {
	Buffer sBuffer;

	char* rcvmsg = new char[256];
	ZeroMemory(rcvmsg, 256);

	char * sendmsg = new char[sizeof(Buffer)];
	ZeroMemory(sendmsg, sizeof(Buffer));

	int nRet;

	for (;; Sleep(10)) {
		if ((nRet = recv(Connections[ID], rcvmsg, 256, NULL)) == SOCKET_ERROR) {
			cout << "Player Disconnected " << ID << endl;			
			if (whiteSide == ID) {
				whiteSide = -1;							
			}
			// poslanie spravy o ukonceni spojenia druhemu hracovi
			sBuffer.ID = -1;
			string msg = "RUNAWAY";
			
			ZeroMemory(sBuffer.message, 256);

			memcpy(sBuffer.message, msg.c_str(), msg.size());
			memcpy(sendmsg, &sBuffer, sizeof(Buffer));
			for (int i = 0; i != counter; i++)
				if (Connections[i] == Connections[ID]) {
				} else {
					send(Connections[i], sendmsg, sizeof(Buffer), NULL);
				}
			break;
		} else {
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
	closesocket(Connections[ID]);
	counter--;
	return 0;
}

/*
Hlavna metoda ktora nastavi Serverovu stranu hry. Nastavuje socket, adresu a cyklus pre
prijimanie spojeni (accept). Pri najdeni spojenia vypise informacie o najdeni spojenia do konzoli.
Nasledne odposle spojeniu uvitaciu spravu a podla toho ktoru stranu mu priradi tak pripoji ku sprave cislo 0/1.
Pri najdeni druheho spojenia odposle spravy obidvom spojeniam o najdeni oponenta (sprava OPONENT).
Po odposlani sprav nastartuje vlakno ktore prijima spravy od pripojeni.
*/
void main() {
	char message[256];
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
					
			Buffer sBuffer;			
			string toSend = "WELCOME ";
			if (whiteSide == -1) {
				Connections[0] = sConnect;
				sBuffer.ID = 0;
				toSend.append("1");
				whiteSide = counter;

				ZeroMemory(sBuffer.message, 256);
				memcpy(sBuffer.message, toSend.c_str(), toSend.size());

				char * sendMsg = new char[sizeof(Buffer)];
				memcpy(sendMsg, &sBuffer, sizeof(Buffer));

				send(Connections[counter], sendMsg, sizeof(Buffer), NULL);

				if (counter == 1) {
					toSend = "OPONENT";
					sBuffer.ID = 0;

					ZeroMemory(sBuffer.message, 256);
					memcpy(sBuffer.message, toSend.c_str(), toSend.size());

					char * sendMsg = new char[sizeof(Buffer)];
					memcpy(sendMsg, &sBuffer, sizeof(Buffer));

					send(Connections[0], sendMsg, sizeof(Buffer), NULL);
					send(Connections[1], sendMsg, sizeof(Buffer), NULL);
				}
				
			} else {
				Connections[1] = sConnect;
				sBuffer.ID = 1;
				toSend.append("0");

				ZeroMemory(sBuffer.message, 256);
				memcpy(sBuffer.message, toSend.c_str(), toSend.size());

				char * sendMsg = new char[sizeof(Buffer)];
				memcpy(sendMsg, &sBuffer, sizeof(Buffer));

				send(Connections[counter], sendMsg, sizeof(Buffer), NULL);
				
				if (counter == 1) {
					toSend = "OPONENT";
					sBuffer.ID = 1;

					ZeroMemory(sBuffer.message, 256);
					memcpy(sBuffer.message, toSend.c_str(), toSend.size());

					char * sendMsg = new char[sizeof(Buffer)];
					memcpy(sendMsg, &sBuffer, sizeof(Buffer));

					send(Connections[0], sendMsg, sizeof(Buffer), NULL);
					send(Connections[1], sendMsg, sizeof(Buffer), NULL);
				}
			}				

			counter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) ServerThread,(LPVOID)(counter - 1), NULL, NULL);
		}
	}
}