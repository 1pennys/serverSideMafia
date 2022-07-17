#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#define TIMER_TIME -600000000 // Для 60 секунд заменить значение на -600000000
#define TIMER_TIME2 -300000000 // Для 60 секунд заменить значение на -600000000
#pragma comment (lib, "ws2_32.lib")

using namespace std;
void timer60secs(SOCKET outSock) {

	LARGE_INTEGER _time;
	HANDLE _timer = 0;

	_time.QuadPart = TIMER_TIME;

	_timer = CreateWaitableTimer(0, 1, 0);
	if (!_timer)
	{
		cout << "CreateWaitableTimer() failed" << endl;
		return;
	}

	cout << "Waiting for 3 seconds..." << endl; // Waiting for 60 seconds...

	if (!SetWaitableTimer(_timer, &_time, 0, NULL, NULL, 0))
	{
		cout << "SetWaitableTimer() failed " << endl;
		return;
	}

	if (WaitForSingleObject(_timer, INFINITE) != WAIT_OBJECT_0)
		cout << "WaitForSingleObject() failed" << endl;
	else
		send(outSock, "next player", 12, 0);//отправить хуйню через 60 секунд

	return;

}
void timer30secs(SOCKET outSock) {

	LARGE_INTEGER _time;
	HANDLE _timer = 0;

	_time.QuadPart = TIMER_TIME2;

	_timer = CreateWaitableTimer(0, 1, 0);
	if (!_timer)
	{
		cout << "CreateWaitableTimer() failed" << endl;
		return;
	}

	cout << "Waiting for 3 seconds..." << endl; // Waiting for 60 seconds...

	if (!SetWaitableTimer(_timer, &_time, 0, NULL, NULL, 0))
	{
		cout << "SetWaitableTimer() failed " << endl;
		return;
	}

	if (WaitForSingleObject(_timer, INFINITE) != WAIT_OBJECT_0)
		cout << "WaitForSingleObject() failed" << endl;
	else
		send(outSock, "next player", 12, 0);//отправить хуйню через 30 секунд

	return;

}
int main()
{
	vector<string> arrayOfNickWebs;
	list<int> listOfClients;
	int count = 1;
	bool gotAllWebcams = false;
	int numOfPlayerSpeaking=0;
	bool nightGoing=false;
	int indexOfSheriff=1;
	int indexOfDon=2;
	int indexOfMafia1=3;
	int indexOfMafia2=4;
	// Initialze winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		cerr << "Can't Initialize winsock! Quitting" << endl;
		return 99;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		return 99;
	}

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton .... 

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socket is for listening 
	listen(listening, SOMAXCONN);

	// Create the master file descriptor set and zero it
	fd_set master;
	FD_ZERO(&master);

	// Add our first socket that we're interested in interacting with; the listening socket!
	// It's important that this socket is added for our server or else we won't 'hear' incoming
	// connections 
	FD_SET(listening, &master);

	// this will be changed by the \quit command (see below, bonus not in video!)
	bool running = true;

	while (running)
	{
		// Make a copy of the master file descriptor set, this is SUPER important because
		// the call to select() is _DESTRUCTIVE_. The copy only contains the sockets that
		// are accepting inbound connection requests OR messages. 

		// E.g. You have a server and it's master file descriptor set contains 5 items;
		// the listening socket and four clients. When you pass this set into select(), 
		// only the sockets that are interacting with the server are returned. Let's say
		// only one client is sending a message at that time. The contents of 'copy' will
		// be one socket. You will have LOST all the other sockets.

		// SO MAKE A COPY OF THE MASTER LIST TO PASS INTO select() !!!

		fd_set copy = master;

		// See who's talking to us
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		// Loop through all the current connections / potential connect
		for (int i = 0; i < socketCount; i++)
		{
			// Makes things easy for us doing this assignment
			SOCKET sock = copy.fd_array[i];

			// Is it an inbound communication?
			if (sock == listening)
			{
				// Accept a new connection
				SOCKET client = accept(listening, nullptr, nullptr);
				// Add the new connection to the list of connected clients
				FD_SET(client, &master);

				// Send a welcome message to the connected client
				//string welcomeMsg = "SERVER:Welcome to the Awesome Chat Server!";
				auto itr = find(master.fd_array, master.fd_array + master.fd_count, client);
				int	indexOfNewClient;
				if (itr != end(master.fd_array))
				{
					indexOfNewClient = distance(master.fd_array, itr);
				}
				cout << "User # " << indexOfNewClient << " with socket # " << client << " connected" << endl;
				//send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
			}
			else // It's an inbound message
			{
				char buf[4096];
				ZeroMemory(buf, 4096);
				//индекс отправителя по сокету
				auto itr = find(master.fd_array, master.fd_array + master.fd_count, sock);
				int	indexOfClient;
				if (itr != end(master.fd_array))
				{
					indexOfClient = distance(master.fd_array, itr);
				}
				//конец поиска индекса
				// Receive message
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					// Drop the client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{
					// Send message to other clients, and definiately NOT the listening socket
					if (arrayOfNickWebs.size() < 11)//загоняем в массив вебки и ники, если 10 записей в  массиве - отсылаем юзерам
					{
						for (u_int i = 0; i < master.fd_count; i++)
						{
							SOCKET outSock = master.fd_array[i];
							if (outSock == listening)
							{
								continue;
							}

							ostringstream ss;

							if (outSock != sock)
							{
								ss << indexOfClient << "|" << buf << "\r\n";
							}
							else
							{
								ss << indexOfClient << "|" << buf << "\r\n";
							}

							string strOut = ss.str();
							arrayOfNickWebs.push_back(strOut);
							
						}
					}
					else 
					{
						for (u_int i = 0; i < arrayOfNickWebs.size()+1; i++)
						{
							SOCKET outSock = master.fd_array[i];
							send(outSock, arrayOfNickWebs[i].c_str(), arrayOfNickWebs[i].size() + 1, 0);
						}
						gotAllWebcams = true;
						for (u_int i = 0; i < arrayOfNickWebs.size() + 1; i++)
						{
							SOCKET outSock = master.fd_array[i];
							send(outSock, "game starts", 12, 0);
						}
					}
					if (gotAllWebcams)//стадия первого дня
					{
						for (u_int i = 0; i < master.fd_count; i++)
						{
							SOCKET SockSpeaking = master.fd_array[numOfPlayerSpeaking];
							timer60secs(SockSpeaking);
						}
						for (u_int i = 0; i < arrayOfNickWebs.size() + 1; i++)
						{
							SOCKET outSock = master.fd_array[i];
							send(outSock, "night starts", 13, 0);
						}
						nightGoing = true;
						if (nightGoing)
						{
							for (u_int i = 0; i < master.fd_count; i++)
							{
								SOCKET outSock = master.fd_array[i];
								
								ostringstream ss;

								if (outSock != sock)
								{
									ss << indexOfClient << "|" << buf << "\r\n";
								}
								else
								{
									ss << indexOfClient << "|" << buf << "\r\n";
								}
								string strOut = ss.str();
								send(outSock, strOut.c_str(), strOut.size(), 0);//разослать всем сделанные выстрелы, проверки на шерифство, проверки на мафов
							}
							nightGoing = false;
						}
					}
				}
			}
		}
	}

	// Remove the listening socket from the master file descriptor set and close it
	// to prevent anyone else trying to connect.
	FD_CLR(listening, &master);
	closesocket(listening);

	// Message to let users know what's happening.
	string msg = "SERVER:Server is shutting down. Goodbye\r\n";

	while (master.fd_count > 0)
	{
		// Get the socket number
		SOCKET sock = master.fd_array[0];

		// Send the goodbye message
		send(sock, msg.c_str(), msg.size() + 1, 0);

		// Remove it from the master file list and close the socket
		FD_CLR(sock, &master);
		closesocket(sock);
	}

	// Cleanup winsock
	WSACleanup();

	system("pause");
	return 0;
}
