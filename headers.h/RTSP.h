#include "RTCP.h"

// client side
struct
{
    unsigned int v : 2; // version
    unsigned int p : 1; // padding
    short int CSeq;     // sequence number
    int SSRC;
    char moviename[1024];
    int PortNum;
} rtsp_clipkt;

// server side
struct
{
    unsigned int v : 2;
    unsigned int p : 1;
    int SSRC;
    short int CSeq;
    string Pt;        // payload type
    unsigned int len; // length
    char moviename[1024];
    int PortNum;
} rtsp_srvpkt;