#include "TCPListener.h"

CTCPListener::CTCPListener(std::string ipAddress, int port, MessageReceivedHandler handler)
	: m_ipAddress(ipAddress), m_port(port), MessageReceived(handler)
{

}

CTCPListener::~CTCPListener()
{
	Cleanup();						// In case not calling CTCPListener::Cleanup();
}

// Send a message to the specified client
void CTCPListener::Send(int ClientSocket, std::string msg)
{
	send(ClientSocket, msg.c_str(), msg.size() + 1, 0);
}

// Initialize winsock
bool CTCPListener::Init()
{
	WSADATA data;
	WORD ver = MAKEWORD(2, 2);

	int wsInit = WSAStartup(ver, &data);
	// TODO: Inform caller the error that occured

	return wsInit == 0;
}

// The main processing loop
void CTCPListener::Run()
{
	char buf[MAX_BUFFER_SIZE];

	while (true)
	{
		// Create a listening socket
		SOCKET listening = CreateSocket();
		if (listening == INVALID_SOCKET)
		{
			break;
		}

		SOCKET client = WaitForConnection(listening);
		if (client != INVALID_SOCKET)
		{
			std::cout << "Successfully connected" << '\n';
			closesocket(listening);

			int bytesReceived = 0;
			do {
				ZeroMemory(buf, MAX_BUFFER_SIZE);

				bytesReceived = recv(client, buf, MAX_BUFFER_SIZE, 0);
				if (bytesReceived > 0)
				{
					if (MessageReceived != NULL)
					{
						MessageReceived(this, client, std::string(buf, 0, bytesReceived));
					}
				}

			} while (bytesReceived > 0);

			closesocket(client);
			std::cout << "Disconnected" << '\n';
		}
	}
}

void CTCPListener::Cleanup()
{
	WSACleanup();
}


// Create a socket
SOCKET CTCPListener::CreateSocket()
{
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening != INVALID_SOCKET)
	{
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(m_port);
		inet_pton(AF_INET, m_ipAddress.c_str(), &hint.sin_addr);

		int bindOK = bind(listening, (sockaddr*)&hint, sizeof(hint));
		if (bindOK != SOCKET_ERROR)
		{
			int listenOK = listen(listening, SOMAXCONN);
			if (listenOK == SOCKET_ERROR)
			{
				return -1;				// Listen fail
			}
		}
		else
		{
			return -1;					// Bind fail
		}
	}

	return listening;
}

// Wait for a connection
SOCKET CTCPListener::WaitForConnection(SOCKET listening)
{
	SOCKET client = accept(listening, NULL, NULL);
	return client;
}
