#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <sstream>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

//PARITY
string parityBit(const string& data)
{
    int ones = 0;
    for (char c : data)
        for (int i = 0; i < 8; i++)
            if (c & (1 << i)) ones++;

    return (ones % 2 == 0) ? "0" : "1";
}

//2D PARITY
string parity2D(const string& data)
{
    int rows = data.size();
    int colParity[8] = { 0 };
    string result = "";

    for (char c : data)
    {
        int rowOnes = 0;
        for (int i = 0; i < 8; i++)
        {
            int bit = (c >> i) & 1;
            rowOnes += bit;
            colParity[i] += bit;
        }
        result += (rowOnes % 2 == 0 ? '0' : '1');
    }

    result += "|";
    for (int i = 0; i < 8; i++)
        result += (colParity[i] % 2 == 0 ? '0' : '1');

    return result;
}

//CRC16
string crc16(const string& data)
{
    unsigned short crc = 0xFFFF;
    for (char c : data)
    {
        crc ^= (unsigned char)c << 8;
        for (int i = 0; i < 8; i++)
            crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : crc << 1;
    }

    stringstream ss;
    ss << hex << crc;
    return ss.str();
}

// HAMMING (7,4) 
string hammingGen(const string& data)
{
    string result = "";
    for (char c : data)
    {
        int d[4] = {
            (c >> 3) & 1,
            (c >> 2) & 1,
            (c >> 1) & 1,
            c & 1
        };

        int p1 = d[0] ^ d[1] ^ d[3];
        int p2 = d[0] ^ d[2] ^ d[3];
        int p3 = d[1] ^ d[2] ^ d[3];

        result += char('0' + p1);
        result += char('0' + p2);
        result += char('0' + d[0]);
        result += char('0' + p3);
        result += char('0' + d[1]);
        result += char('0' + d[2]);
        result += char('0' + d[3]);
    }
    return result;
}

//IP CHECKSUM 
string ipChecksum(const string& data)
{
    unsigned int sum = 0;
    for (char c : data)
        sum += (unsigned char)c;

    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    sum = ~sum;

    stringstream ss;
    ss << hex << sum;
    return ss.str();
}

int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    string data, method;
    cout << "Veri gir: ";
    cin >> data;
    cout << "Method (CRC / PARITY / 2DPARITY / HAMMING / CHECKSUM): ";
    cin >> method;

    string control;

    if (method == "CRC") control = crc16(data);
    else if (method == "PARITY") control = parityBit(data);
    else if (method == "2DPARITY") control = parity2D(data);
    else if (method == "HAMMING") control = hammingGen(data);
    else if (method == "CHECKSUM") control = ipChecksum(data);
    else
    {
        cout << "Gecersiz method\n";
        return 0;
    }

    string packet = data + "|" + method + "|" + control;

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    connect(s, (sockaddr*)&addr, sizeof(addr));
    send(s, packet.c_str(), packet.length(), 0);
    closesocket(s);

    cout << "Gonderilen paket: " << packet << endl;
    WSACleanup();
}
