#include<iostream>
#include<string>

#include "TCPListener.h"
using namespace std;

void Listener_MessageReceived(CTCPListener* listener, int client, string msg);

int main()
{
	CTCPListener server("127.0.0.1", 54010, Listener_MessageReceived);

	if (server.Init())
	{
		server.Run();
	}



}

void Listener_MessageReceived(CTCPListener* listener, int client, string msg)
{
	cout << msg << '\n';
	listener->Send(client, msg);

}