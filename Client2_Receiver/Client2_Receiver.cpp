#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

//PARITY
int parityBit(const string& data)
{
    int ones = 0;
    for (char c : data)
        for (int i = 0; i < 8; i++)
            ones += (c >> i) & 1;
    return ones % 2;
}

//CRC16
unsigned short crc16(const string& data)
{
    unsigned short crc = 0xFFFF;
    for (char c : data)
    {
        crc ^= (unsigned char)c << 8;
        for (int i = 0; i < 8; i++)
            crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : crc << 1;
    }
    return crc;
}
//IP CHECKSUM
unsigned short ipChecksum(const string& data)
{
    unsigned int sum = 0;
    for (char c : data)
        sum += (unsigned char)c;

    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    return ~sum;
}

int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12346);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server, (sockaddr*)&addr, sizeof(addr));
    listen(server, 1);

    cout << "Client2 hazir, veri bekleniyor...\n";

    SOCKET s = accept(server, NULL, NULL);
    char buffer[2048]{};
    recv(s, buffer, 2048, 0);
    closesocket(s);

    string packet(buffer);
    int p1 = packet.find("|");
    int p2 = packet.rfind("|");

    string data = packet.substr(0, p1);
    string method = packet.substr(p1 + 1, p2 - p1 - 1);
    string sentControl = packet.substr(p2 + 1);

    cout << "Received Data : " << data << endl;
    cout << "Method : " << method << endl;
    cout << "Sent Control : " << sentControl << endl;

    if (method == "PARITY")
    {
        int computed = parityBit(data);
        cout << "Computed Parity : " << computed << endl;
        cout << (sentControl[0] - '0' == computed ?
            "Status : DATA CORRECT\n" : "Status : DATA CORRUPTED\n");
    }
    else if (method == "CRC")
    {
        unsigned short computed = crc16(data);
        unsigned short sent = (unsigned short)atoi(sentControl.c_str());
        cout << (sent == computed ?
            "Status : DATA CORRECT\n" : "Status : DATA CORRUPTED\n");
    }
    else if (method == "CHECKSUM")
    {
        unsigned short computed = ipChecksum(data);
        unsigned short sent = (unsigned short)atoi(sentControl.c_str());
        cout << (sent == computed ?
            "Status : DATA CORRECT\n" : "Status : DATA CORRUPTED\n");
    }
    else if (method == "HAMMING")
    {
        cout << "Status : HAMMING CODE RECEIVED\n";
    }

    WSACleanup();
}
