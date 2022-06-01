#include <iostream>
#include <unistd.h>
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
#include <array>
#include <cstdlib>
#include <arpa/inet.h>

using namespace std;

int numpktsent = 0;
int numoctsent = 0;
bool flag = false;
int sts[500];
int rts[500];
int d;
int j = 0;
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
    int PortNumServer = atoi(argv[2]);
    int opt = 1;
    struct sockaddr_in ClientAddr, ServerAddr; //socket file descriptor for the server to connect to.
    int AddrLen = sizeof(ClientAddr);
    socklen_t AddSenderLen;
    // socket creation -- 1_ communication domain. 2_ comm type (UDP). 3_ protocol.
    // returns zero if creation succes else -1
    int Socket_Client = socket(AF_INET, SOCK_DGRAM, 0);
    if (Socket_Client == -1){cout << "Error in creation socket.\n**** EXIT ****\n";return 0;}

    memset(&ServerAddr, 0, sizeof(ServerAddr));
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, argv[1], &ServerAddr.sin_addr);
    ServerAddr.sin_port = htons(PortNumServer);
    int BytesRead, FrameRead;
    AddSenderLen = sizeof (ServerAddr);
    char request [] = "Hello";
    sendto(Socket_Client, request, sizeof(request), 0, (struct sockaddr *)&ServerAddr, (socklen_t)AddSenderLen );
    // BytesRead = recvfrom(Socket_Client,&reveivedpkt,10000,0,NULL,NULL);
    //Buffer[BytesRead]=0;
    //cout << "client received : " << reveivedpkt.data << endl;

    memset(&rtcp_rr, 0, sizeof(rtcp_rr));
    memset(sts, 0, 500*sizeof(*sts));
    memset(rts, 0, 500*sizeof(*rts));


rtcp_rr.version &= ~(1UL << 0);
rtcp_rr.version |= 1UL << 1;
//cout<<rtcp_rr.version<<endl;
rtcp_rr.p &= ~(1UL << 0);
//cout<<rtcp_rr.p<<endl;
rtcp_rr.rcount |= 1UL << 0;
rtcp_rr.rcount &= ~(1UL << 1);
rtcp_rr.rcount &= ~(1UL << 2);
rtcp_rr.rcount &= ~(1UL << 3);
rtcp_rr.rcount &= ~(1UL << 4);
//cout<<rtcp_sr.rcount<<endl;
rtcp_rr.pt |= 1UL << 0;
rtcp_rr.pt &= ~(1UL << 1);
rtcp_rr.pt &= ~(1UL << 2);
rtcp_rr.pt |= 1UL << 3;
rtcp_rr.pt &= ~(1UL << 4);
rtcp_rr.pt &= ~(1UL << 5);
rtcp_rr.pt |= 1UL << 6;
rtcp_rr.pt |= 1UL << 7;
//cout<<rtcp_rr.pt<<endl;
rtcp_rr.wordlen = 16;
//cout<<rtcp_rr.wordlen<<endl;
rtcp_rr.rssrc = 0;
//rtcp_sr.sssrc = RTPpkt.SSRC;
//rtcp_rr.lastseq = RTPpkt.SeqNum;

/*tcp_rr.bfraction = numpktsent;
rtcp_rr.blost = numoctsent;
rtcp_rr.jitter = time(NULL);
rtcp_rr.lsr =
rtcp_rr.dlsr =*/
int stlsr=0;
while (!flag) {
sleep(5);
if (numpktsent == 500) flag = true;
recvfrom(Socket_Client,&rtcp_sr,1000,0, (struct sockaddr *)&ServerAddr, &AddSenderLen);
stlsr = time(NULL);
cout<<"SR packet \n"<<"Version :"<<rtcp_sr.version<<"   Padding :"<<rtcp_sr.p<<"        Reciever report count :"<<rtcp_sr.rcount<<"     Packet type :"<<rtcp_sr.pt<<endl;
cout<<"Lenght(in words) :"<<rtcp_sr.wordlen<<"  SSRC :"<<rtcp_sr.ssrc<<"        NTP timestamp:"<<rtcp_sr.ntp_secfrac<<" RTP timestamp :"<<rtcp_sr.rtp_ts<<endl;
cout<<"NO. of packets sent :"<<rtcp_sr.pksent<<"        NO. of octets sent :"<<rtcp_sr.octsent<<endl;
if (flag == true) {
recvfrom(Socket_Client,&rtcp_bye,1000,0, (struct sockaddr *)&ServerAddr, &AddSenderLen );
cout<<"BYE packet \n"<<"Version :"<<rtcp_bye.version<<"   Padding :"<<rtcp_bye.p<<"        Source count :"<<rtcp_bye.scount<<"     Packet type :"<<rtcp_bye.pt<<endl;
cout<<"Lenght(in words) :"<<rtcp_bye.wordlen<<"  SSRC :"<<rtcp_bye.ssrc<<endl;
}
 for (int i = numpktsent-1 ; i <= numpktsent ;i++){
if (sts[i] != 0 && sts[i+1] != 0 && rts[i] != 0 && rts[i+1] != 0)
 d = rts[i+1] - sts[i+1] - rts[i] + sts[i];
 j = ((abs(d) - j)/16) + j ; }

rtcp_rr.bfraction = ((166 - numpktsent)/166) * 256;
rtcp_rr.blost = rtcp_rr.blost + (166 - numpktsent);

rtcp_rr.lsr = rtcp_sr.ntp_secfrac - 2208988800;
rtcp_rr.dlsr = (time(NULL) - stlsr )/65536;

if (sendto(Socket_Client, &rtcp_rr, sizeof(rtcp_rr), 0, (struct sockaddr *)&ServerAddr, (socklen_t)AddrLen) == -1) {
            cout << "Error in sending RTCP BYE packet" << " SORRY!\n"<< strerror(errno) << "\n**** EXIT ****\n"<<endl;
            return 0; }
}
    shutdown(Socket_Client, SHUT_RDWR);

}
