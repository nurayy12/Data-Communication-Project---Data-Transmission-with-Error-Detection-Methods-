#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <ctime>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

string corrupt(string data)
{
    srand(time(0));
    data[rand() % data.length()] = 'X';
    return data;
}

int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(s, (sockaddr*)&addr, sizeof(addr));
    listen(s, 1);

    cout << "Server calisiyor...\n";

    SOCKET c1 = accept(s, NULL, NULL);

    char buffer[1024]{};
    recv(c1, buffer, 1024, 0);
    closesocket(c1);

    string packet(buffer);
    int p1 = packet.find("|");
    int p2 = packet.rfind("|");

    string data = packet.substr(0, p1);
    string method = packet.substr(p1 + 1, p2 - p1 - 1);
    string control = packet.substr(p2 + 1);

    string corrupted = corrupt(data);
    string newPacket = corrupted + "|" + method + "|" + control;

    SOCKET c2 = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr2{};
    addr2.sin_family = AF_INET;
    addr2.sin_port = htons(12346);
    inet_pton(AF_INET, "127.0.0.1", &addr2.sin_addr);

    connect(c2, (sockaddr*)&addr2, sizeof(addr2));
    send(c2, newPacket.c_str(), newPacket.length(), 0);
    closesocket(c2);

    WSACleanup();
    return 0;
}
