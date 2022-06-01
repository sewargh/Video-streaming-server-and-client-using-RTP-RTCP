#include <iostream>
#include <unistd.h>
#include "date.h"
#include <ctime>
#include <iomanip>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <thread>
#include <string.h>
#include <pthread.h>
#include <thread>
using namespace std;

int numpktsent = 0;
int numoctsent = 0;
bool flag = false;
typedef unsigned char  u_int8;
typedef unsigned short u_int16;
typedef unsigned long int   u_int64;
typedef unsigned int   u_int32;
typedef          short int16;

/* Reception report*/
struct {
       unsigned int version:2;   /* protocol version */
       unsigned int p:1;         /* padding flag */
       unsigned int rcount:5;     /* varies by packet type */
       unsigned int pt:8;        /* RTCP packet type */
       u_int16 wordlen:16;           /* pkt len in words, w/o this word */
       u_int32 rssrc;              /* receiver generating this report */
       u_int32 sssrc;             /* data source being reported */
       unsigned int bfraction:8;  /* fraction lost since last SR/RR */
       int blost:24;              /* cumul. no. pkts lost (signed!) */
       u_int32 lastseq;         /* extended last seq. no. received */
       u_int32 jitter;           /* interarrival jitter */
       u_int32 lsr;              /* last SR packet from this source */
       u_int32 dlsr;             /* delay since last SR packet */
   } rtcp_rr;

/* sender report */
struct {
       unsigned int version:2;   /* protocol version */
       unsigned int p:1;         /* padding flag */
       unsigned int rcount:5;     /* varies by packet type */
       unsigned int pt:8;        /* RTCP packet type */
       u_int16 wordlen;           /* pkt len in words, w/o this word */
       u_int32 ssrc;     /* sender generating this report */
       u_int64 ntp_secfrac;  /* NTP timestamp */
       u_int32 rtp_ts;   /* RTP timestamp */
       u_int32 pksent;    /* packets sent */
       u_int32 octsent;    /* octets sent */
   } rtcp_sr;

/* BYE */
struct {
       unsigned int version:2;   /* protocol version */
       unsigned int p:1;         /* padding flag */
       unsigned int scount:5;     /* varies by packet type */
       unsigned int pt:8;        /* RTCP packet type */
       u_int16 wordlen;           /* pkt len in words, w/o this word */
       u_int32 ssrc;   /* list of sources */
       /* can't express trailing text for reason */
   } rtcp_bye;

int main(int argc, char const *argv[]) {
    int PortNum = atoi(argv[1]);
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
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = INADDR_ANY;
    ServerAddr.sin_port = htons(PortNum);

    /******** Bind Address To Socket **********/
    int binderr = bind(Socket_Server, (struct sockaddr *)&ServerAddr, AddrLen); // returns zero if creation succes else -1
    if (binderr == -1)
    {
        cout << "Error in binding socket. SORRY!\n"<< strerror(errno) << "\n**** EXIT ****\n";
        return 0;
    }
    // puts the server socket in a passive mode, where it waits for the client to approach the server to make a connection.

    memset(&rtcp_sr, 0, sizeof(rtcp_sr));
    memset(&rtcp_bye, 0, sizeof(rtcp_bye));

     ClientAddLen = sizeof(ClientAddr);
    int BytesRead;
    cout << "Waiting on port " << PortNum << endl;
    BytesRead = recvfrom(Socket_Server, Buffer, 10000, 0, (struct sockaddr *)&ClientAddr, &ClientAddLen);
    Buffer[BytesRead] = 0;
    cout << " client ip after rcv " << ClientAddr.sin_addr.s_addr << endl;
    cout << "#of read bytes = " << BytesRead << "\nData is : -" << Buffer << endl;
    sendto(Socket_Server, &rtcp_sr, sizeof(rtcp_sr), 0, (struct sockaddr *)&ClientAddr, ClientAddLen);
    cout << " client ip " << ClientAddr.sin_addr.s_addr << endl;


rtcp_bye.version &= ~(1UL << 0);
rtcp_bye.version |= 1UL << 1;
//cout<<rtcp_bye.version<<endl;
rtcp_bye.p &= ~(1UL << 0);
//cout<<rtcp_bye.p<<endl;
rtcp_bye.scount |= 1UL << 0;
rtcp_bye.scount &= ~(1UL << 1);
rtcp_bye.scount &= ~(1UL << 2);
rtcp_bye.scount &= ~(1UL << 3);
rtcp_bye.scount &= ~(1UL << 4);
//cout<<rtcp_bye.scount<<endl;
rtcp_bye.pt |= 1UL << 0;
rtcp_bye.pt |= 1UL << 1;
rtcp_bye.pt &= ~(1UL << 2);
rtcp_bye.pt |= 1UL << 3;
rtcp_bye.pt &= ~(1UL << 4);
rtcp_bye.pt &= ~(1UL << 5);
rtcp_bye.pt |= 1UL << 6;
rtcp_bye.pt |= 1UL << 7;
//cout<<rtcp_bye.pt<<endl;
rtcp_sr.wordlen = 4;
//cout<<rtcp_sr.wordlen<<endl;
//rtcp_bye.ssrc = RTPpkt.SSRC;

rtcp_sr.version &= ~(1UL << 0);
rtcp_sr.version |= 1UL << 1;
//cout<<rtcp_sr.version<<endl;
rtcp_sr.p &= ~(1UL << 0);
//cout<<rtcp_sr.p<<endl;
rtcp_sr.rcount &= ~(1UL << 0);
rtcp_sr.rcount &= ~(1UL << 1);
rtcp_sr.rcount &= ~(1UL << 2);
rtcp_sr.rcount &= ~(1UL << 3);
rtcp_sr.rcount &= ~(1UL << 4);
//cout<<rtcp_sr.rcount<<endl;
rtcp_sr.pt &= ~(1UL << 0);
rtcp_sr.pt &= ~(1UL << 1);
rtcp_sr.pt &= ~(1UL << 2);
rtcp_sr.pt |= 1UL << 3;
rtcp_sr.pt &= ~(1UL << 4);
rtcp_sr.pt &= ~(1UL << 5);
rtcp_sr.pt |= 1UL << 6;
rtcp_sr.pt |= 1UL << 7;
//cout<<rtcp_sr.pt<<endl;
rtcp_sr.wordlen = 14;
//cout<<rtcp_sr.wordlen<<endl;
//rtcp_sr.ssrc = RTPpkt.SSRC;


while (!flag) {
sleep(5);
if (numpktsent == 500) flag = true;
rtcp_sr.pksent = numpktsent;
rtcp_sr.octsent = numoctsent;
rtcp_sr.ntp_secfrac = time(NULL) + 2208988800;
rtcp_sr.rtp_ts = time(NULL);
 
if (sendto(Socket_Server, &rtcp_sr, sizeof(rtcp_sr), 0, (struct sockaddr *)&ClientAddr, ClientAddLen) == -1) {
            cout << "Error in sending RTCP SR packet "<< " SORRY!\n"<< strerror(errno) << "\n**** EXIT ****\n"<<endl;
            return 0; }
if (flag == true) {
    if (sendto(Socket_Server, &rtcp_sr, sizeof(rtcp_sr), 0, (struct sockaddr *)&ClientAddr, ClientAddLen) == -1){
            cout << "Error in sending RTCP BYE packet" << " SORRY!\n"<< strerror(errno) << "\n**** EXIT ****\n"<<endl;
            return 0; }}

recvfrom(Socket_Server, &rtcp_rr, 10000, 0, (struct sockaddr *)&ClientAddr, &ClientAddLen);
cout<<"RR packet \n"<<"Version :"<<rtcp_rr.version<<"   Padding :"<<rtcp_rr.p<<"        Reciever report count :"<<rtcp_rr.rcount<<"     Packet type :"<<rtcp_rr.pt<<endl;
cout<<"Lenght(in words) :"<<rtcp_rr.wordlen<<"  Receiver generating this report :"<<rtcp_rr.rssrc<<"  SSRC :"<<rtcp_rr.sssrc<<endl;
cout<<"Fraction of lost packets :"<<rtcp_rr.bfraction<<" Cumulative NO. of packets lost :"<<rtcp_rr.blost<<endl;
cout<<"Last sequence number recieved :"<<rtcp_rr.lastseq<<"        Jitter :"<<rtcp_rr.jitter<<endl;
cout<<"Last SR packet :"<<rtcp_rr.lsr<<"        Delay from last SR packet :"<<rtcp_rr.dlsr<<endl;

// memset(&rtcp_sr, 0, sizeof(rtcp_sr));
// memcpy(rtcp_sr.wordlen, 16, sizeof(rtcp_sr.wordlen));
//std::time_t t = std::time(nullptr);
//std::cout << "UTC:   " << std::put_time(std::gmtime(&t), "%c %Z") << '\n';

}

shutdown(Socket_Server, SHUT_RDWR);
}
