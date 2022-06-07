#include "initialize.h"
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// rtcp client
int numpktsent = 0;
int sts[500];
int rts[500];
int index_s = 0;
int index_r = 0;
int numpktsend5 = 0;
/*void *rtsp_client_func(void *PORT){

    int RTSP_PortNumServer = (int)(intptr_t)PORT;
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
    memcpy(rtsp_clipkt.moviename, "Movie.mjpeg", sizeof(rtsp_clipkt.moviename));
    cout << "movie from client send name " << rtsp_clipkt.moviename <<endl;
    sendto(RTSP_Socket_Client, &rtsp_clipkt, sizeof(rtsp_clipkt), 0, (struct sockaddr *)&RTSP_ServerAddr, (socklen_t)RTSP_AddSenderLen); // rtsp is supposed to send it.
    shutdown(RTSP_Socket_Client, SHUT_RDWR);
    return 0;
}*/
void *rtp_client_func(void *PORT)
{
    int PortNumServer = (int)(intptr_t)PORT;
    int opt = 1;
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
    ServerAddr.sin_port = htons(PortNumServer);
    int BytesRead, FrameRead;
    AddSenderLen = sizeof(ServerAddr);
    {
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
    }
    char request[] = "Movie.MJPEG";
    sendto(Socket_Client, request, sizeof(request), 0, (struct sockaddr *)&ServerAddr, (socklen_t)AddSenderLen); // rtsp is supposed to send it.
    mkdir("vid2", 0777) == -1;

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
        if (writeFile)
        {
            fwrite(Receivedpkt.buf, ifsize, 1, writeFile);
        }
        else
        {
            cout << "Cannot open output file.\n";
        }
        CImg<unsigned char> img(name);
        disp = img;

        fclose(writeFile);
        pthread_mutex_lock(&mutex);
        numpktsent++;
        sts[index_s++] = Receivedpkt.Timestamp;
        rts[index_r++] = time(NULL);
        numpktsend5++;
        pthread_mutex_unlock(&mutex);
    }

    shutdown(Socket_Client, SHUT_RDWR);
    return 0;
}
void *rtcp_client_func(void *PORTNUM)
{
    bool flag = false;
    int d = 0;
    int j = 0;
    int n = 0;
    int PortNumServer = (int)(intptr_t)PORTNUM;
    int opt = 1;
    struct sockaddr_in ClientAddr, ServerAddr; // socket file descriptor for the server to connect to.
    int AddrLen = sizeof(ClientAddr);
    socklen_t AddSenderLen;
    int Socket_Client = socket(AF_INET, SOCK_DGRAM, 0);
    if (Socket_Client == -1)
    {
        cout << "Error in creation socket.\n**** EXIT ****\n";
        return 0;
    }

    memset(&ServerAddr, 0, sizeof(ServerAddr));
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = INADDR_ANY;
    // inet_pton(AF_INET, argv[1], &ServerAddr.sin_addr);
    ServerAddr.sin_port = htons(PortNumServer);
    int BytesRead, FrameRead;
    AddSenderLen = sizeof(ServerAddr);
    char request[] = "Hello";
    sendto(Socket_Client, request, sizeof(request), 0, (struct sockaddr *)&ServerAddr, (socklen_t)AddSenderLen);

    memset(&rtcpclient_rr, 0, sizeof(rtcpclient_rr));
    memset(sts, 0, 500 * sizeof(*sts));
    memset(rts, 0, 500 * sizeof(*rts));
    {
        rtcpclient_rr.version &= ~(1UL << 0);
        rtcpclient_rr.version |= 1UL << 1;
        rtcpclient_rr.p &= ~(1UL << 0);
        rtcpclient_rr.rcount |= 1UL << 0;
        rtcpclient_rr.rcount &= ~(1UL << 1);
        rtcpclient_rr.rcount &= ~(1UL << 2);
        rtcpclient_rr.rcount &= ~(1UL << 3);
        rtcpclient_rr.rcount &= ~(1UL << 4);
        rtcpclient_rr.pt |= 1UL << 0;
        rtcpclient_rr.pt &= ~(1UL << 1);
        rtcpclient_rr.pt &= ~(1UL << 2);
        rtcpclient_rr.pt |= 1UL << 3;
        rtcpclient_rr.pt &= ~(1UL << 4);
        rtcpclient_rr.pt &= ~(1UL << 5);
        rtcpclient_rr.pt |= 1UL << 6;
        rtcpclient_rr.pt |= 1UL << 7;
        rtcpclient_rr.wordlen = 16;
        rtcpclient_rr.rssrc = 0;
    }

    rtcpclient_rr.sssrc = Receivedpkt.SSRC;
    rtcpclient_rr.lastseq = Receivedpkt.SeqNum;
    int stlsr = 0;
    while (!flag)
    {
        sleep(5);
        if (numpktsent == 500)
            flag = true;
        recvfrom(Socket_Client, &rtcpclient_sr, sizeof(rtcpclient_sr), 0, (struct sockaddr *)&ServerAddr, &AddSenderLen);
        stlsr = time(NULL);
        cout << "SR packet \n"
             << "|Version :" << rtcpclient_sr.version << "\t|Padding :" << rtcpclient_sr.p << "\t|Reciever report count :" << rtcpclient_sr.rcount << "\t|Packet type :" << rtcpclient_sr.pt << endl;
        cout << "|Lenght(in words) :" << rtcpclient_sr.wordlen << "\t|SSRC :" << rtcpclient_sr.ssrc << "\t|NTP timestamp:" << rtcpclient_sr.ntp_secfrac << endl;
        cout << "|RTP timestamp :" << rtcpclient_sr.rtp_ts << "\t|NO. of packets sent :" << rtcpclient_sr.pksent << "\t|NO. of octets sent :" << rtcpclient_sr.octsent << endl;
        cout << endl
             << endl;
        if (flag == true)
        {
            recvfrom(Socket_Client, &rtcpclient_bye, sizeof(rtcpclient_bye), 0, (struct sockaddr *)&ServerAddr, &AddSenderLen);
            cout << "BYE packet \n"
                 << "|Version :" << rtcpclient_bye.version << "\t|Padding :" << rtcpclient_bye.p << "\t|Source count :" << rtcpclient_bye.scount << "\t|Packet type :" << rtcpclient_bye.pt << endl;
            cout << "|Lenght(in words) :" << rtcpclient_bye.wordlen << "\t|SSRC :" << rtcpclient_bye.ssrc << endl;
            cout << endl
                 << endl;
        }
        pthread_mutex_lock(&mutex);

        if (sts[numpktsent - 2] != 0 && sts[numpktsent - 1] != 0 && rts[numpktsent - 2] != 0 && rts[numpktsent - 1] != 0)
        {
            // cout<<rts[numpktsent - 2]<<"\t"<<sts[numpktsent - 2]<<"\t"<<rts[numpktsent - 1]<<"\t"<<sts[numpktsent - 1]<<endl;
            d = rts[numpktsent - 2] - sts[numpktsent - 2] - rts[numpktsent - 1] + sts[numpktsent - 1];
            j = float(((abs(d) - j) / 16) + j);
        }
        rtcpclient_rr.jitter = j;
        rtcpclient_rr.bfraction = float(((166 - numpktsend5) / 166) * 256);
        rtcpclient_rr.blost += (166 - numpktsend5);
        if (n == 3)
            rtcpclient_rr.blost += (2 - numpktsend5);
        rtcpclient_rr.lsr = rtcpclient_sr.ntp_secfrac - 2208988800;
        rtcpclient_rr.dlsr = float((time(NULL) - stlsr) / 65536);
        n++;
        pthread_mutex_unlock(&mutex);

        if (sendto(Socket_Client, &rtcpclient_rr, sizeof(rtcpclient_rr), 0, (struct sockaddr *)&ServerAddr, (socklen_t)AddrLen) == -1)
        {
            cout << "Error in sending RTCP BYE packet"
                 << " SORRY!\n"
                 << strerror(errno) << "\n**** EXIT ****\n"
                 << endl;
            return 0;
        }
        pthread_mutex_lock(&mutex);
        numpktsend5 = 0;
        pthread_mutex_unlock(&mutex);
    }
    shutdown(Socket_Client, SHUT_RDWR);
    return 0;
}
// server ip, server port
int main(int argc, char const *argv[])
{
    /*int rtspPort = atoi(argv[2]);
    int RTPportNum = rtsp_clipkt.PortNum;
    pthread_t rtsp_client;
    int rc = pthread_create(&rtsp_client, NULL, rtsp_client_func, (void *)rtspPort);
    if (rc)
    {
        cout << "ERROR; couldn't creat thread 1 \n";
        return (0);
    }*/

    int PortNum = atoi(argv[2]); // SHOULD BE FOR RTSP SOCKET. BUT RTP WILL USE IT BECAUSE WE COULDN'T IMPLEMENT RTSP.
    pthread_t rtp_client, rtcp_client;
    int rc = pthread_create(&rtp_client, NULL, rtp_client_func, (void *)PortNum);
    if (rc)
    {
        cout << "ERROR; couldn't creat thread 1 \n";
        return (0);
    }
    int rtcpport = PortNum + 1;
    rc = pthread_create(&rtcp_client, NULL, rtcp_client_func, (void *)rtcpport);
    if (rc)
    {
        cout << "ERROR; couldn't creat thread 2 \n";
        return (0);
    }
    pthread_join(rtp_client, NULL);
    pthread_join(rtcp_client, NULL);
    // pthread_join(rtsp_client, NULL);
    return 0;
}