#include <iostream>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "AI.hpp"

using namespace std;

const char* SERVER = "138.2.77.198";
const unsigned short PORT = 9001;
const char* KEY_MATCH = "asdads";
const int UID = 1;

enum {
    PKT_HI = 0,
    PKT_ID = 1,
    PKT_BOARD = 2,
    PKT_SEND = 3,
    PKT_RECEIVE = 4,
    PKT_ERROR = 5,
    PKT_END = 6
};

void prepare();
void close();

unsigned int convertFromBufferToUint(const char* buffer, const unsigned int bytes, const unsigned int startPos) {
    unsigned int result = 0;
    for (unsigned int i = 0; i < bytes; i++) {
        result += (unsigned char)buffer[startPos + i] << (i * 8);
    }
    return result;
}

int convertFromBufferToInt(const char* buffer, const unsigned int bytes, const unsigned int startPos, bool littleEndian = true) {
    int result = 0;
    if (littleEndian) {
        for (unsigned int i = 0; i < bytes; i++) {
            result += (unsigned char)buffer[startPos + i] << (i * 8);
        }
    }
    else {
        for (unsigned int i = 0; i < bytes; i++) {
            result += (unsigned char)buffer[startPos + i] << ((bytes - i - 1) * 8);
        }
    }
    return result;
}

unsigned int convertFromBufferToUint(vector<char> &buffer, const unsigned int bytes, const unsigned int startPos) {
    unsigned int result = 0;
    for (unsigned int i = 0; i < bytes; ++i) {
        result += (unsigned char) buffer[i + startPos] << (i * 8);
    }
    return result;
}

int convertFromBufferToInt(vector<char> &buffer, const unsigned int bytes, const unsigned int startPos, bool littleEndian = true) {
    int result = 0;
    if (littleEndian) {
        for (unsigned int i = 0; i < bytes; i++) {
            result += (unsigned char)buffer[startPos + i] << (i * 8);
        }
    }
    else {
        for (unsigned int i = 0; i < bytes; i++) {
            result += (unsigned char)buffer[startPos + i] << ((bytes - i - 1) * 8);
        }
    }
    return result;
}

char* convertUintToCharArray(unsigned int num) {
    char* result = new char[4];
    for (unsigned int i = 0; i < 4; i++) {
        result[i] = (char)(num >> (i * 8));
    }
    return result;
}

char* convertIntToCharArray(int num) {
    char* result = new char[4];
    for (unsigned int i = 0; i < 4; i++) {
        result[i] = (char)(num >> (i * 8));
    }
    return result;
}

struct pkt_header {
    unsigned int type;
    unsigned int len;
};

struct pkt {
    pkt_header head;
    char* payload;
};

std::ostream& operator << (std::ostream& out, const pkt &pkt) {
    out << pkt.head.type << "\n";
    out << pkt.head.len << "\n";
    for (int i = 0; i < pkt.head.len; i++) {
        out << (int)pkt.payload[i] << " ";
    }
    return out;
}

pkt initHiPacket() {
    pkt sendPkg{};
    sendPkg.head.type = PKT_HI;
    sendPkg.head.len = 4 + strlen(KEY_MATCH);
    sendPkg.payload = new char[4 + strlen(KEY_MATCH)];

    memcpy(sendPkg.payload, convertUintToCharArray(UID), 4);
    memcpy(&(sendPkg.payload[4]), KEY_MATCH, strlen(KEY_MATCH));
    return sendPkg;
}

pkt initSendPacket(int id, int x) {
    pkt sendPkg{};
    sendPkg.head.type = PKT_SEND;
    sendPkg.head.len = 8;
    sendPkg.payload = new char[8];
    memcpy(sendPkg.payload, convertIntToCharArray(id), 4);
    memcpy(&sendPkg.payload[4], convertIntToCharArray(x), 4);
    return sendPkg;
}

void resetServer(int &sockid) {
    pkt resetPkt{};
    resetPkt.head.type = -1;
    resetPkt.head.len = 0;

    char send_buf[sizeof(resetPkt)];

    memcpy(send_buf, &resetPkt, sizeof(resetPkt));
    send(sockid, send_buf, sizeof(resetPkt), 0);
}

int main()
{
    srand(time(nullptr));
    sockaddr_in serverAddr{};
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_addr.s_addr = inet_addr(SERVER);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    int sockid = socket(AF_INET, SOCK_STREAM, 0);
    if (sockid < 0) {
        exit(-1);
    }

    int clientId = connect(sockid, (sockaddr *) & serverAddr, sizeof(serverAddr));
    if (clientId < 0) {
        exit(-2);
    }

    //resetServer(sockid);
    pkt sendPkg = initHiPacket();

    char send_buf[sizeof(sendPkg)];

    memcpy(send_buf, &sendPkg, sizeof(sendPkg));
    send(sockid, send_buf, sizeof(sendPkg), 0);

    vector<char> buffer(1000);
    int recvBytes;

    //recv(sockid, buffer, 8, 0);
    bool running = true;

    unsigned int type = 0;
    unsigned int len = 0;
    unsigned int id = 0;
    bool first = true;

    unsigned int n = 0;
    unsigned int m = 0;
    unsigned int k = 0;
    unsigned int l = 0;

    AI* ai = nullptr;

    while (running) {
        //memset(buffer, 0, sizeof(buffer));
        recvBytes = recv(sockid, buffer.data(), buffer.size(), 0);

        cout << recvBytes << endl;

        if (recvBytes <= 0) {
            break;
        }

        cout << "Receive: " << recvBytes << " bytes from server\n";
        for (int i = 0; i < recvBytes; ++i) cout <<(int) buffer[i] << " ";
        cout << endl;

        type = convertFromBufferToUint(buffer, 4, 0);
        len = convertFromBufferToUint(buffer, 4, 4);

        cout << "type: " << type << " ";
        cout << "len: " << len << endl;

        if (type == PKT_ID) {
            id = convertFromBufferToInt(buffer, 4, 8);
            cout << "ID: " << id << "\n";
            first = convertFromBufferToInt(buffer, 4, 12);
            cout << "You are the first player: " << (first ? "Yes" : "No") << "\n";
        }
        else if (type == PKT_BOARD) {
            n = convertFromBufferToUint(buffer, 4, 8);
            m = convertFromBufferToUint(buffer, 4, 12);
            l = convertFromBufferToUint(buffer, 4, 16);
            k = convertFromBufferToUint(buffer, 4, 20);

            ai = new AI(m, n, k);

            for (int i = 0; i < l; ++i) {
                int x = convertFromBufferToInt(buffer, 4, 24 + i * 4);
                ai->board[x / n][x % n] = -1;
            }
        }
        else if ((type == PKT_RECEIVE || first) && ai != nullptr) {
            if (first) {
                while (true) {
                    int x = rand() % (n * m);
                    if (ai->board[x / n][x % n] == 0) {
                        ai->board[x / n][x % n] = 1;
                        sendPkg = initSendPacket(id, x);
                        memcpy(send_buf, &sendPkg, sizeof(sendPkg));
                        send(sockid, send_buf, sizeof(sendPkg), 0);
                        break;
                    }
                }
            }
            else {
                int x = convertFromBufferToInt(buffer, 4, 8);
                ai->board[x / n][x % n] = 2;

                /*while (true) {
                    int x = rand() % (n * m);
                    if (ai->board[x / n][x % n] == 0) {
                        ai->board[x / n][x % n] = 1;
                        sendPkg = initSendPacket(id, x);
                        memcpy(send_buf, &sendPkg, sizeof(sendPkg));
                        send(sockid, send_buf, sizeof(sendPkg), 0);
                        break;
                    }
                }*/

                x = ai->getBestPosition();
                sendPkg = initSendPacket(id, x);
                memcpy(send_buf, &sendPkg, sizeof(sendPkg));
                send(sockid, send_buf, sizeof(sendPkg), 0);
            }
        }
        else if (type == PKT_ERROR) {

        }
        else if (type == PKT_END) {
            running = false;
        }
    }

    close(sockid);
}