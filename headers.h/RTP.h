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
#include <sys/stat.h>
#include <filesystem>
#include <fstream>
#include "CImg.h"

using namespace cimg_library;
using namespace std;

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
    int buf[10000];
} RTP_Server_pkt;

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
} Receivedpkt;
