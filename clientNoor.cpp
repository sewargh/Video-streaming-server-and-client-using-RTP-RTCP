#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <thread>
#include "CImg.h"
#include "initialize.h"

#include <fstream>
#include <filesystem>

using namespace cimg_library;
using namespace std;
#define MaxThreads 10
#define size 100000

// Takes 3 arguments 1: server ip, 2: server port for rtsp, 3: moviename
int main(int argc, char const *argv[])
{
    int RTSP_PortNumServer = atoi(argv[2]);
    struct sockaddr_in RTSP_ClientAddr, RTSP_ServerAddr; // socket file descriptor for the server to connect to.
    int RTSP_AddrLen = sizeof(RTSP_ClientAddr);
    socklen_t RTSP_AddSenderLen;
    int RTSP_Socket_Client = socket(AF_INET, SOCK_DGRAM, 0);
    if (RTSP_Socket_Client == -1)
    {
        cout << "Error in socket creation.\n**** EXIT ****\n";
        return 0;
    }

    memset(&RTSP_ServerAddr, 0, sizeof(RTSP_ServerAddr));
    RTSP_ServerAddr.sin_family = AF_INET;
    RTSP_ServerAddr.sin_addr.s_addr = INADDR_ANY;
    RTSP_ServerAddr.sin_port = htons(RTSP_PortNumServer);

    int RTSP_BytesRead, RTSP_FrameRead;
    RTSP_AddSenderLen = sizeof(RTSP_ServerAddr);

    rtsp_clipkt.PortNum = 2000;
    memcpy(rtsp_clipkt.moviename, argv[3], sizeof(rtsp_clipkt.moviename));
    cout << "movie from client send name " << rtsp_clipkt.moviename <<endl;
    sendto(RTSP_Socket_Client, &rtsp_clipkt, sizeof(rtsp_clipkt), 0, (struct sockaddr *)&RTSP_ServerAddr, (socklen_t)RTSP_AddSenderLen); // rtsp is supposed to send it.
    //BytesRead = recvfrom(Socket_Client, &Receivedpkt, sizeof(Receivedpkt), 0, (struct sockaddr *)&ServerAddr, &AddSenderLen);
    shutdown(RTSP_Socket_Client, SHUT_RDWR);
    /*******************/

    int PortNumServer = rtsp_clipkt.PortNum;
    cout << "rtpSERVER listening on port : " << PortNumServer << endl;
    struct sockaddr_in ClientAddr, ServerAddr; // socket file descriptor for the server to connect to.
    int AddrLen = sizeof(ClientAddr);
    socklen_t AddSenderLen;
    int Socket_Client = socket(AF_INET, SOCK_DGRAM, 0);
    if (Socket_Client == -1)
    {
        cout << "Error in socket creation.\n**** EXIT ****\n";
        return 0;
    }

    memset(&ServerAddr, 0, sizeof(ServerAddr));
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, argv[1], &ServerAddr.sin_addr);
    ServerAddr.sin_port = htons(PortNumServer);

    int BytesRead, FrameRead;
    AddSenderLen = sizeof(ServerAddr);

    Receivedpkt.V &= ~(1UL << 0);
    Receivedpkt.V |= 1UL << 1;
    Receivedpkt.P &= ~(1UL << 0);
    Receivedpkt.X &= ~(1UL << 0);
    Receivedpkt.CC &= ~(1UL << 0);
    Receivedpkt.CC &= ~(1UL << 1);
    Receivedpkt.CC &= ~(1UL << 2);
    Receivedpkt.CC &= ~(1UL << 3);
    Receivedpkt.M &= ~(1UL << 0);
    Receivedpkt.PT &= ~(1UL << 0);
    Receivedpkt.PT |= 1UL << 1;
    Receivedpkt.PT &= ~(1UL << 2);
    Receivedpkt.PT |= 1UL << 3;
    Receivedpkt.PT |= 1UL << 4;
    Receivedpkt.PT &= ~(1UL << 5);
    Receivedpkt.PT &= ~(1UL << 6);
    srand(time(0));
    Receivedpkt.SeqNum; // Make sure SeqNum is enough to hold the sent rtp packets for the 500 frames + other rtp pkts.((2^16-1) - 1000)
    Receivedpkt.Timestamp;
    srand(time(0));
    Receivedpkt.SSRC = rand() % 4294967296;

    char request[] = "Movie.MJPEG";
    sendto(Socket_Client, request, sizeof(request), 0, (struct sockaddr *)&ServerAddr, (socklen_t)AddSenderLen); // rtsp is supposed to send it.
    BytesRead = recvfrom(Socket_Client, &Receivedpkt, sizeof(Receivedpkt), 0, (struct sockaddr *)&ServerAddr, &AddSenderLen);
    // Buffer[BytesRead]=0;
    cout << "client received : " << Receivedpkt.V << endl;
    cout << Receivedpkt.M << endl;
    cout << Receivedpkt.P << endl;
    cout << Receivedpkt.PT << endl;
    cout << Receivedpkt.SeqNum << endl;
    cout << Receivedpkt.SSRC << endl;
    cout << Receivedpkt.SeqNum << endl;
    cout << Receivedpkt.X << endl;

    if (mkdir("vid2", 0777) == -1)
        cout << "Error :  " << strerror(errno) << endl;
    else
        cout << "Directory created";

    CImgDisplay disp;
    for (int i = 1; i <= 500; i++)
    {
        Receivedpkt.buf;
        char name[1000] = {};
        sprintf(name, "vid2/image%03d.jpg", i);
        FILE *writeFile = fopen(name, "wb");
        size_t ifsize;

        ifsize = recvfrom(Socket_Client, &Receivedpkt, sizeof(Receivedpkt), 0, (struct sockaddr *)&ServerAddr, &AddSenderLen);
        if (i == 1)
            Receivedpkt.Timestamp = RTP_Server_pkt.SeqNum;
        else
            Receivedpkt.SeqNum += 1;
        Receivedpkt.Timestamp = time(NULL);
        // cout << "ifsize  " << i << "   " << ifsize << endl;
        if (writeFile)
        {
            fwrite(Receivedpkt.buf, ifsize, 1, writeFile);
            // cout << "File received successfully.\n";
        }
        else
        {
            // cout << "Cannot open output file.\n";
        }
        CImg<unsigned char> img(name);
        disp = img;

        // usleep(30000);
        fclose(writeFile);
    }

    shutdown(Socket_Client, SHUT_RDWR);

    return 0;
}
