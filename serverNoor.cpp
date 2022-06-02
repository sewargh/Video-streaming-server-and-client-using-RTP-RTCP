#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <thread>
#include "CImg.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "struct.h"
#include <ctime>

// takes one argument which is the port number.
int main(int argc, char const *argv[])
{
    int valread;
    memset(&rtsp_srvpkt, 0, sizeof(rtsp_srvpkt));
    int RTSP_Server_PortNum = atoi(argv[1]);
    struct sockaddr_in ServerRTSP_Addr;
    memset(&ServerRTSP_Addr, 0, sizeof(ServerRTSP_Addr));
    int RTSP_AddrLen = sizeof(ServerRTSP_Addr);
    int RTSP_Socket_Server = socket(AF_INET, SOCK_STREAM, 0);
    ServerRTSP_Addr.sin_family = AF_INET;
    ServerRTSP_Addr.sin_addr.s_addr = INADDR_ANY;
    ServerRTSP_Addr.sin_port = htons(RTSP_Server_PortNum);
    bind(RTSP_Socket_Server, (struct sockaddr *)&ServerRTSP_Addr, RTSP_AddrLen);
    listen(RTSP_Socket_Server, 1);

    int new_socket = accept(RTSP_Socket_Server, (struct sockaddr *)&ServerRTSP_Addr, (socklen_t *)&RTSP_AddrLen);
    valread = read(new_socket, &rtsp_srvpkt, sizeof(rtsp_srvpkt));

    /*valread = read(new_socket, buffer, 1024);
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");*/

    int PortNum = rtsp_srvpkt.PortNum; // should be used for RTSP.
    cout << "movie name " << rtsp_srvpkt.moviename<< endl;
    cout << "port " << PortNum << endl;
    char Buffer[10000];
    int opt = 1;
    struct sockaddr_in ServerAddr; // socket file descriptor for the server to connect to.
    struct sockaddr_in ClientAddr;
    int AddrLen = sizeof(ServerAddr);
    socklen_t ClientAddLen;
    /******** Create A Socket To Communicate With Server **********/
    int Socket_Server = socket(AF_INET, SOCK_DGRAM, 0); // returns zero if creation succes else -1
    if (Socket_Server == -1)
    {
        cout << "Error in set socket reuseadd. SORRY!\n"
             << strerror(errno) << "\n**** EXIT ****\n";
        return 0;
    }

    /******** Create An Address For Server To Communicate **********/
    memset(&ServerAddr, 0, sizeof(ServerAddr));
    memset(&ClientAddr, 0, sizeof(ClientAddr));
    bzero(&ServerAddr, sizeof(ServerAddr));
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //cout << "server ip " << ServerAddr.sin_addr.s_addr << endl;
    ServerAddr.sin_port = htons(PortNum);

    /******** Bind Address To Socket **********/
    int binderr = bind(Socket_Server, (struct sockaddr *)&ServerAddr, AddrLen); // returns zero if creation succes else -1
    if (binderr == -1)
    {
        cout << "Error in binding socket. SORRY!\n"
             << strerror(errno) << "\n**** EXIT ****\n";
        return 0;
    }
    // puts the server socket in a passive mode, where it waits for the client to approach the server to make a connection.
    memset(&RTP_Server_pkt, 0, sizeof(RTP_Server_pkt));
    // memcpy(RTP_Server_pkt.data, "Hello from Server !\n", sizeof(RTP_Server_pkt.data));

    RTP_Server_pkt.V &= ~(1UL << 0);
    RTP_Server_pkt.V |= 1UL << 1;
    RTP_Server_pkt.P &= ~(1UL << 0);
    RTP_Server_pkt.X &= ~(1UL << 0);
    RTP_Server_pkt.CC &= ~(1UL << 0);
    RTP_Server_pkt.CC &= ~(1UL << 1);
    RTP_Server_pkt.CC &= ~(1UL << 2);
    RTP_Server_pkt.CC &= ~(1UL << 3);
    RTP_Server_pkt.M &= ~(1UL << 0);
    RTP_Server_pkt.PT &= ~(1UL << 0);
    RTP_Server_pkt.PT |= 1UL << 1;
    RTP_Server_pkt.PT &= ~(1UL << 2);
    RTP_Server_pkt.PT |= 1UL << 3;
    RTP_Server_pkt.PT |= 1UL << 4;
    RTP_Server_pkt.PT &= ~(1UL << 5);
    RTP_Server_pkt.PT &= ~(1UL << 6);
    srand(time(0));
    RTP_Server_pkt.SeqNum = rand() % 64535; // Make sure SeqNum is enough to hold the sent rtp packets for the 500 frames + other rtp pkts.((2^16-1) - 1000)
    RTP_Server_pkt.Timestamp;
    srand(time(0));
    RTP_Server_pkt.SSRC = rand() % 4294967296;
    /******** Listen To The Port to Any Connection **********/
    ClientAddLen = sizeof(ClientAddr);
    int BytesRead;
    cout << "Waiting on port " << PortNum << endl;
    BytesRead = recvfrom(Socket_Server, Buffer, 10000, 0, (struct sockaddr *)&ClientAddr, &ClientAddLen);
    Buffer[BytesRead] = 0;
    cout << " Requested movie name : " << Buffer << endl;
    // char ClientIP[256];
    // memset(ClientIP, 0, 256);
    // inet_ntop(AF_INET, &ClientAddr.sin_addr, ClientIP, 256); // number to pointer to string
    sendto(Socket_Server, &RTP_Server_pkt, sizeof(RTP_Server_pkt), 0, (struct sockaddr *)&ClientAddr, ClientAddLen);
    // cout << " client ip " << ClientAddr.sin_addr.s_addr << endl;
    /*cout << "SENDING FIRST FRAME\n";
    for (int i = 1; i <= 500; i++){
    char name[1000];
    sprintf(name,"vid/image%03d.jpg",i);
    CImg<unsigned char>img(name);//img.load(name);
    sendto(Socket_Server,img,sizeof(img),0,(struct sockaddr *)&ClientAddr,ClientAddLen);
    usleep(30000);
    }*/

    for (int i = 1; i <= 500; i++)
    {
        if (i != 1)
            RTP_Server_pkt.SeqNum += 1;
        RTP_Server_pkt.buf;
        char name[1000];
        sprintf(name, "vid/image%03d.jpg", i);
        FILE *readFile = fopen(name, "rb");
        fseek(readFile, 0, SEEK_END);
        size_t ifsize = ftell(readFile);
        fseek(readFile, 0, SEEK_SET);

        if (readFile)
        {
            if (fread(RTP_Server_pkt.buf, 1, ifsize, readFile) <= 0)
            {
                cout << "No contents or error reading file \n";
            }
        }
        else
        {
            cout << "Could not read audio file.\n";
            return 0;
        }
        if (sendto(Socket_Server, &RTP_Server_pkt, sizeof(RTP_Server_pkt), 0, (struct sockaddr *)&ClientAddr, ClientAddLen) < 0)
            cout << "FILE WAS NOT SENT \n"
                 << strerror(errno) << "\n**** EXIT ****\n";
        else
            cout << "FILE SENT \n";
        RTP_Server_pkt.Timestamp = time(NULL);
        usleep(30000);
        fclose(readFile);
    }
    close(new_socket);
    shutdown(Socket_Server, SHUT_RDWR);

    return 0;
}