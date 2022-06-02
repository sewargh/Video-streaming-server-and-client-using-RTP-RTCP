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
#include <array>
#include <cstdlib>
#include <arpa/inet.h>
#include "CImg.h"

using namespace cimg_library;
using namespace std;

//rtcp client + server;
typedef unsigned char  u_int8;
typedef unsigned short u_int16;
typedef unsigned long int   u_int64;
typedef unsigned int   u_int32;
typedef          short int16;

/* Reception report*/
struct
{
    unsigned int version : 2;   /* protocol version */
    unsigned int p : 1;         /* padding flag */
    unsigned int rcount : 5;    /* varies by packet type */
    unsigned int pt : 8;        /* RTCP packet type */
    u_int16 wordlen : 16;       /* pkt len in words, w/o this word */
    u_int32 rssrc;              /* receiver generating this report */
    u_int32 sssrc;              /* data source being reported */
    unsigned int bfraction : 8; /* fraction lost since last SR/RR */
    int blost : 24;             /* cumul. no. pkts lost (signed!) */
    u_int32 lastseq;            /* extended last seq. no. received */
    u_int32 jitter;             /* interarrival jitter */
    u_int32 lsr;                /* last SR packet from this source */
    u_int32 dlsr;               /* delay since last SR packet */
} rtcp_rr;
/* sender report */
struct
{
    unsigned int version : 2; /* protocol version */
    unsigned int p : 1;       /* padding flag */
    unsigned int rcount : 5;  /* varies by packet type */
    unsigned int pt : 8;      /* RTCP packet type */
    u_int16 wordlen;          /* pkt len in words, w/o this word */
    u_int32 ssrc;             /* sender generating this report */
    u_int64 ntp_secfrac;      /* NTP timestamp */
    u_int32 rtp_ts;           /* RTP timestamp */
    u_int32 pksent;           /* packets sent */
    u_int32 octsent;          /* octets sent */
} rtcp_sr;

/* BYE */
struct
{
    unsigned int version : 2; /* protocol version */
    unsigned int p : 1;       /* padding flag */
    unsigned int scount : 5;  /* varies by packet type */
    unsigned int pt : 8;      /* RTCP packet type */
    u_int16 wordlen;          /* pkt len in words, w/o this word */
    u_int32 ssrc;             /* list of sources */
    /* can't express trailing text for reason */
} rtcp_bye;

struct RTPServer_pkt
{
    unsigned int V : 2;
    unsigned int P : 1;
    unsigned int X : 1;
    unsigned int CC : 4;
    unsigned int M : 1;
    unsigned int PT : 7;
    unsigned short int SeqNum;
    uint32_t Timestamp;
    int SSRC;
    char buf[10000];
    // CImg<unsigned char> img;
    //  char data[10000];
} RTP_Server_pkt;

/* Reception report*/
struct
{
    unsigned int version : 2;   /* protocol version */
    unsigned int p : 1;         /* padding flag */
    unsigned int rcount : 5;    /* varies by packet type */
    unsigned int pt : 8;        /* RTCP packet type */
    u_int16 wordlen : 16;       /* pkt len in words, w/o this word */
    u_int32 rssrc;              /* receiver generating this report */
    u_int32 sssrc;              /* data source being reported */
    unsigned int bfraction : 8; /* fraction lost since last SR/RR */
    int blost : 24;             /* cumul. no. pkts lost (signed!) */
    u_int32 lastseq;            /* extended last seq. no. received */
    u_int32 jitter;             /* interarrival jitter */
    u_int32 lsr;                /* last SR packet from this source */
    u_int32 dlsr;               /* delay since last SR packet */
} rtcpclient_rr;

/* sender report */
struct
{
    unsigned int version : 2; /* protocol version */
    unsigned int p : 1;       /* padding flag */
    unsigned int rcount : 5;  /* varies by packet type */
    unsigned int pt : 8;      /* RTCP packet type */
    u_int16 wordlen;          /* pkt len in words, w/o this word */
    u_int32 ssrc;             /* sender generating this report */
    u_int64 ntp_secfrac;      /* NTP timestamp */
    u_int32 rtp_ts;           /* RTP timestamp */
    u_int32 pksent;           /* packets sent */
    u_int32 octsent;          /* octets sent */
} rtcpclient_sr;

/* BYE */
struct
{
    unsigned int version : 2; /* protocol version */
    unsigned int p : 1;       /* padding flag */
    unsigned int scount : 5;  /* varies by packet type */
    unsigned int pt : 8;      /* RTCP packet type */
    u_int16 wordlen;          /* pkt len in words, w/o this word */
    u_int32 ssrc;             /* list of sources */
    /* can't express trailing text for reason */
} rtcpclient_bye;

struct RTPClient_pkt
{
    unsigned int V : 2;
    unsigned int P : 1;
    unsigned int X : 1;
    unsigned int CC : 4;
    unsigned int M : 1;
    unsigned int PT : 7;
    unsigned short int SeqNum;
    uint32_t Timestamp;
    int SSRC;
    int buf[10000];
}Receivedpkt;

//client side 
struct  {
       unsigned int v:2 ;           //version        
       unsigned int p:1 ;           //padding
       short int    CSeq ;          //sequence number
       int          SSRC; 
} rtsp_clipkt;

//server side
struct rtsppck {
       unsigned int  v:2 ;               
       unsigned int  p:1 ;         
       int           SSRC;
       short int     CSeq ;           
       string        Pt;            //payload type
       unsigned int  len;           //length 
} rtsp_srvpkt;