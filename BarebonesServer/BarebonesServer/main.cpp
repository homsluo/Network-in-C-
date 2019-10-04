#include<iostream>
#include<WS2tcpip.h>
#include<string>
#include<sstream>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main()
{

	// Initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOK = WSAStartup(ver, &wsData);
	if (wsOK != 0)
	{
		cerr << "Can't Initialize winsock! Quitting..." << '\n';
		return;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting..." << '\n';
		return;
	}

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton ...

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell winsock the socket is for listening
	listen(listening, SOMAXCONN);

	fd_set master;
	FD_ZERO(&master);

	FD_SET(listening, &master);

	while (true)
	{
		fd_set copy = master;		// Make a copy because select() will destruct the set

		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];
			if (sock == listening)
			{
				// Accept a new connection
				sockaddr_in clientSocket;
				int clientSize = sizeof(clientSocket);
				SOCKET client = accept(listening, (sockaddr*)&clientSocket, &clientSize);

				// Add the new connection to the list of connected client
				FD_SET(client, &master);
				cout << "Here comes a new client!" << '\n' << "We now have " << master.fd_count - 1
					<< " client online" << '\n';

				// Send a welcome message to the connected client
				string welcomeMsg = "Welcome to homs's multiple client server!\r\n";
				send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
				// TODO: Boradcast we have a new connection
			}
			else
			{
				// Accept a new message
				char buf[4096];
				ZeroMemory(buf, 4096);

				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					// Drop the client;
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{
					// Send message to other clients

					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock)
						{
							ostringstream ss;
							ss << "SOCKET #" << sock << ": " << buf << "\r";
							string strOut = ss.str();

							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}
				}
			}

		}
	}
	FD_CLR(listening, &master);
	closesocket(listening);
	
	string msg = "Server is shutting down, goodbye!";

	while (master.fd_count > 0)
	{
		SOCKET sock = master.fd_array[0];
		send(sock, msg.c_str(), msg.size() + 1, 0);

		FD_CLR(sock, &master);
		closesocket(sock);
	}

	WSACleanup();

}
