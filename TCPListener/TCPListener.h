#pragma once

#include<iostream>
#include <string>
#include <WS2tcpip.h>							// Header file for Winsock funcions

#pragma comment(lib, "ws2_32.lib")				// Winsock library file

#define MAX_BUFFER_SIZE (49152)

// Forward declaration of class
class CTCPListener;

// TODO: Callback to data received
typedef void(*MessageReceivedHandler)(CTCPListener* listener, int socketID, std::string msg);

class CTCPListener
{
public:
	CTCPListener(std::string ipAddress, int port, MessageReceivedHandler handler);

	~CTCPListener();

	// Send a message to the specified client
	void Send(int ClientSocket, std::string msg);

	// Initialize winsock
	bool Init();

	// The main processing loop
	void Run();

	// Clean up after using the service
	void Cleanup();


private:

	// Create a socket
	SOCKET CreateSocket();

	// Wait for a connection
	SOCKET WaitForConnection(SOCKET listening);

	std::string				m_ipAddress;
	int						m_port;
	MessageReceivedHandler	MessageReceived;


};