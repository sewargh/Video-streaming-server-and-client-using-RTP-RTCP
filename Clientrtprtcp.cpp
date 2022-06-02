#include "initialize.h"
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// rtcp client
int numpktsent = 0;
int sts[500];
int rts[500];
int index_s = 0;
int index_r = 0;
void *rtp_client_func(void *PORT)
{
    int PortNumServer = (int64_t)PORT;
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
    // struct timeval tv;
    // fd_set rfds;
    /*FD_ZERO(&rfds);
    FD_SET(Socket_Client, &rfds);
    tv.tv_usec = 0.0;
    int recVal = 0;
    bool timePassed = false;
    time_t startListenTime = time(NULL);

    tv.tv_sec = 30000;*/

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
    // BytesRead = recvfrom(Socket_Client, &Receivedpkt, sizeof(Receivedpkt), 0, (struct sockaddr *)&ServerAddr, &AddSenderLen);

    if (mkdir("vid2", 0777) == -1)
        cout << strerror(errno) << endl;

    CImgDisplay disp;
    for (int i = 1; i <= 500; i++)
    {
        cout << i << " ";
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
        /*else
        {
             cout << "Cannot open output file.\n";
        }*/
        CImg<unsigned char> img(name);
        disp = img;

        // usleep(30000);
        fclose(writeFile);
        pthread_mutex_lock(&mutex);
        numpktsent++;
        sts[index_s] = Receivedpkt.Timestamp;
        rts[index_r] = time(NULL);
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
    int PortNumServer = (int64_t)PORTNUM;
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
    cout << " when " << endl;
    int stlsr = 0;
    while (!flag)
    {
        sleep(5);
        if (numpktsent == 500)
            flag = true;
            cout << "rtcpclient before rcvfrom " << endl;
        recvfrom(Socket_Client, &rtcpclient_sr, 1000, 0, (struct sockaddr *)&ServerAddr, &AddSenderLen);
            cout << "rtcp after rcvfrom " << endl;
        stlsr = time(NULL);
        cout << "SR packet \n"
             << "Version :" << rtcpclient_sr.version << "   Padding :" << rtcpclient_sr.p << "        Reciever report count :" << rtcpclient_sr.rcount << "     Packet type :" << rtcpclient_sr.pt << endl;
        cout << "Lenght(in words) :" << rtcpclient_sr.wordlen << "  SSRC :" << rtcpclient_sr.ssrc << "        NTP timestamp:" << rtcpclient_sr.ntp_secfrac << " RTP timestamp :" << rtcpclient_sr.rtp_ts << endl;
        cout << "NO. of packets sent :" << rtcpclient_sr.pksent << "        NO. of octets sent :" << rtcpclient_sr.octsent << endl;
        if (flag == true)
        {
            recvfrom(Socket_Client, &rtcpclient_bye, 1000, 0, (struct sockaddr *)&ServerAddr, &AddSenderLen);
            cout << "BYE packet \n"
                 << "Version :" << rtcpclient_bye.version << "   Padding :" << rtcpclient_bye.p << "        Source count :" << rtcpclient_bye.scount << "     Packet type :" << rtcpclient_bye.pt << endl;
            cout << "Lenght(in words) :" << rtcpclient_bye.wordlen << "  SSRC :" << rtcpclient_bye.ssrc << endl;
        }
        for (int i = numpktsent - 1; i <= numpktsent; i++)
        {
            if (sts[i] != 0 && sts[i + 1] != 0 && rts[i] != 0 && rts[i + 1] != 0)
                d = rts[i + 1] - sts[i + 1] - rts[i] + sts[i];
            j = ((abs(d) - j) / 16) + j;
        }
        rtcpclient_rr.jitter = j;
        rtcpclient_rr.bfraction = ((166 - numpktsent) / 166) * 256;
        rtcpclient_rr.blost = rtcpclient_rr.blost + (166 - numpktsent);

        rtcpclient_rr.lsr = rtcpclient_sr.ntp_secfrac - 2208988800;
        rtcpclient_rr.dlsr = (time(NULL) - stlsr) / 65536;

        if (sendto(Socket_Client, &rtcpclient_rr, sizeof(rtcpclient_rr), 0, (struct sockaddr *)&ServerAddr, (socklen_t)AddrLen) == -1)
        {
            cout << "Error in sending RTCP BYE packet"
                 << " SORRY!\n"
                 << strerror(errno) << "\n**** EXIT ****\n"
                 << endl;
            return 0;
        }
    }
    shutdown(Socket_Client, SHUT_RDWR);
    return 0;
}
// server ip, server port, XXXXXX no XXXXXX video name
int main(int argc, char const *argv[])
{
    int PortNum = atoi(argv[2]); // SHOULD BE FOR RTSP SOCKET. BUT I WILL USE IT UNTIL WE IMPLEMENT RTSP.
    pthread_t rtp_client, rtcp_client;
    int rc = pthread_create(&rtp_client, NULL, rtp_client_func, (void *)PortNum);
    if (rc)
    {
        cout << "ERROR; couldn't creat thread 1 \n";
        return (0);
    }
    rc = pthread_create(&rtcp_client, NULL, rtcp_client_func, (void *)PortNum + 1);
    if (rc)
    {
        cout << "ERROR; couldn't creat thread 2 \n";
        return (0);
    }
    pthread_join(rtp_client, NULL);
    pthread_join(rtcp_client, NULL);
    return 0;
}