#include <iostream>
#include <WS2tcpip.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main()
{
    WSAData wsData;
    WORD ver = MAKEWORD(2, 2);

    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0)
    {
        cerr << "Can't Initialize winsock! Quitting" << endl;
        return;
    }

    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET)
    {
        cerr << "Can't creating a socket! Quitting" << endl;
        return;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;

    bind(listening, (sockaddr*)&hint, sizeof(hint));

    listen(listening, SOMAXCONN);

    sockaddr_in client;
    int clientSize = sizeof(client);

    SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    ZeroMemory(host, NI_MAXHOST);
    ZeroMemory(service, NI_MAXSERV);

    if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
    {
        cout << host << " connect on port " << service << endl;
    }
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " connected on port "
            << ntohs(client.sin_port) << endl;
    }

    closesocket(listening);

    char buf[4096];
    while (true)
    {
        ZeroMemory(buf, 4096);
        int byteReceived = recv(clientSocket, buf, 4096, 0); // bug
        if (byteReceived == SOCKET_ERROR)
        {
            cerr << "Error int recv(). Quitting" << endl;
            break;
        }

        if (byteReceived == 0)
        {
            cout << "Client disconected " << endl;
            break;
        }

        cout << string(buf, 0, byteReceived) << endl;

        send(clientSocket, buf, byteReceived + 1, 0);
    }

    closesocket(clientSocket);

    WSACleanup();
    //system("pause");
}
