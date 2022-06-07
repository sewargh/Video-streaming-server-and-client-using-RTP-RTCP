#include "initialize.h"
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// rtcp server
int numpktsent = 0;
int numoctsent = 0;
void print_report()
{
    cout << "RR packet \n"
         << "|Version: " << rtcp_rr.version << "\t\t\t|Padding: " << rtcp_rr.p << "\t|Reciever_report_count: " << rtcp_rr.rcount << "\t|Packet_type: " << rtcp_rr.pt << endl;
    cout << "|Lenght(in words): " << rtcp_rr.wordlen << "\t\t|Receiver_generating_this_report: " << rtcp_rr.rssrc << "\t\t|SSRC: " << rtcp_rr.sssrc << endl;
    cout << "|Fraction_of_lost_packets: " << rtcp_rr.bfraction << "\t|Cumulative_NO._of_packets_lost:" << rtcp_rr.blost << endl;
    cout << "|Last_seq._ number_recieved: " << rtcp_rr.lastseq << "\t|Jitter: " << rtcp_rr.jitter << endl;
    cout << "|Last_SR_packet: " << rtcp_rr.lsr << "\t|Delay_from_last_SR_packet: " << rtcp_rr.dlsr << endl;
    cout << endl
         << endl;
}
/*void *rtsp_server_func(void *PORT){
int RTSP_PortNum =  (int)(intptr_t)PORT;
    struct sockaddr_in RTSP_ServerAddr; // socket file descriptor for the server to connect to.
    struct sockaddr_in RTSP_ClientAddr;
    int RTSP_AddrLen = sizeof(RTSP_ServerAddr);
    socklen_t RTSP_ClientAddLen;
    int RTSP_Socket_Server = socket(AF_INET, SOCK_DGRAM, 0); // returns zero if creation succes else -1
    if (RTSP_Socket_Server == -1)
    {
        cout << "Error in set socket reuseadd. SORRY!\n"
             << strerror(errno) << "\n**** EXIT ****\n";
        return 0;
    }

    memset(&RTSP_ServerAddr, 0, sizeof(RTSP_ServerAddr));
    memset(&RTSP_ClientAddr, 0, sizeof(RTSP_ClientAddr));
    bzero(&RTSP_ServerAddr, sizeof(RTSP_ServerAddr));
    RTSP_ServerAddr.sin_family = AF_INET;
    RTSP_ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    RTSP_ServerAddr.sin_port = htons(RTSP_PortNum);

    int RTSP_binderr = bind(RTSP_Socket_Server, (struct sockaddr *)&RTSP_ServerAddr, RTSP_AddrLen); // returns zero if creation succes else -1
    if (RTSP_binderr == -1)
    {
        cout << "Error in binding socket. SORRY!\n"
             << strerror(errno) << "\n**** EXIT ****\n";
        return 0;
    }
    memset(&rtsp_srvpkt, 0, sizeof(rtsp_srvpkt));
    RTSP_ClientAddLen = sizeof(RTSP_ClientAddr);
    int RTSP_BytesRead;
    cout << "Waiting on port " << RTSP_PortNum << endl;
    RTSP_BytesRead = recvfrom(RTSP_Socket_Server, &rtsp_srvpkt, sizeof(rtsp_srvpkt), 0, (struct sockaddr *)&RTSP_ClientAddr, &RTSP_ClientAddLen);
    rtsp_srvpkt.moviename[RTSP_BytesRead] = 0;
    cout << "Requested movie name : " << rtsp_srvpkt.moviename << endl;
    cout << "rtp port : " << rtsp_srvpkt.PortNum << endl;
    shutdown(RTSP_Socket_Server, SHUT_RDWR);
        return 0;
}*/
void *rtpfunc(void *port)
{
    int PortNum = (int)(intptr_t)port;
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
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    ServerAddr.sin_port = htons(PortNum);

    /******** Bind Address To Socket **********/
    int binderr = bind(Socket_Server, (struct sockaddr *)&ServerAddr, AddrLen); // returns zero if creation succes else -1
    if (binderr == -1)
    {
        cout << "Error in binding socket. SORRY!\n"
             << strerror(errno) << "\n**** EXIT ****\n";
        return 0;
    }
    memset(&RTP_Server_pkt, 0, sizeof(RTP_Server_pkt));
    // memcpy(RTP_Server_pkt.data, "Hello from Server !\n", sizeof(RTP_Server_pkt.data));
    {
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
    }
    /******** Listen To The Port to Any Connection **********/
    ClientAddLen = sizeof(ClientAddr);
    int BytesRead;
    pthread_mutex_lock(&mutex);
    cout << "RTP Waiting on port " << PortNum << endl;
    pthread_mutex_unlock(&mutex);
    // puts the server socket in a passive mode, where it waits for the client to approach the server to make a connection.
    BytesRead = recvfrom(Socket_Server, Buffer, 10000, 0, (struct sockaddr *)&ClientAddr, &ClientAddLen);
    Buffer[BytesRead] = 0;
    // cout << "Requested movie name : " << Buffer << endl;
    for (int i = 1; i <= 500; i++)
    {
        if (i != 1)
            RTP_Server_pkt.SeqNum += 1;
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
            cout << "Could not read image file.\n";
            return 0;
        }
        RTP_Server_pkt.Timestamp = time(NULL);
        int BytesSent = 0;
        BytesSent = sendto(Socket_Server, &RTP_Server_pkt, sizeof(RTP_Server_pkt), 0, (struct sockaddr *)&ClientAddr, ClientAddLen);
        if (BytesSent < 0)
            cout
                << "FILE WAS NOT SENT \n"
                << strerror(errno) << "\n**** EXIT ****\n";
        ifstream in_file(name, ios::binary);
        in_file.seekg(0, ios::end);
        int file_size = in_file.tellg();

        pthread_mutex_lock(&mutex);
        numpktsent++;
        numoctsent += file_size;
        pthread_mutex_unlock(&mutex);

        usleep(30000);
        fclose(readFile);
    }
    shutdown(Socket_Server, SHUT_RDWR);
    pthread_exit(NULL);
    return 0;
}
void *rtcpfunc(void *portnum)
{
    bool flag = false;
    int PortNum = (int)(intptr_t)portnum;
    char Buffer[10000];
    int opt = 1;

    struct sockaddr_in RTCP_Server_Addr; // socket file descriptor for the server to connect to.
    struct sockaddr_in RTCP_Client_Addr;
    int AddrLen = sizeof(RTCP_Server_Addr);
    socklen_t ClientAddLen = sizeof(RTCP_Client_Addr);
    /******** Create Socket To Communicate With Server **********/
    int Socket_Server = socket(AF_INET, SOCK_DGRAM, 0); // returns zero if creation succes else -1
    if (Socket_Server == -1)
    {
        cout << "Error in set socket reuseadd. SORRY!\n"
             << strerror(errno) << "\n**** EXIT ****\n";
        return 0;
    }

    /******** Create An Address For Server To Communicate **********/
    memset(&RTCP_Server_Addr, 0, sizeof(RTCP_Server_Addr));
    RTCP_Server_Addr.sin_family = AF_INET;
    RTCP_Server_Addr.sin_addr.s_addr = INADDR_ANY;
    RTCP_Server_Addr.sin_port = htons(PortNum);

    /******** Bind Address To Socket **********/
    int binderr = bind(Socket_Server, (struct sockaddr *)&RTCP_Server_Addr, AddrLen);
    if (binderr == -1)
    {
        cout << "Error in binding socket. SORRY!\n"
             << strerror(errno) << "\n**** EXIT ****\n";
        return 0;
    }
    // puts the server socket in a passive mode, where it waits for the client to approach the server to make a connection.

    memset(&rtcp_sr, 0, sizeof(rtcp_sr));
    memset(&rtcp_bye, 0, sizeof(rtcp_bye));

    int BytesRead;
    pthread_mutex_lock(&mutex);
    cout << "RTCP Waiting on port " << PortNum << endl;
    pthread_mutex_unlock(&mutex);
    BytesRead = recvfrom(Socket_Server, Buffer, 10000, 0, (struct sockaddr *)&RTCP_Client_Addr, &ClientAddLen);
    Buffer[BytesRead] = 0;
    {
        rtcp_bye.version &= ~(1UL << 0);
        rtcp_bye.version |= 1UL << 1;

        rtcp_bye.p &= ~(1UL << 0);

        rtcp_bye.scount |= 1UL << 0;
        rtcp_bye.scount &= ~(1UL << 1);
        rtcp_bye.scount &= ~(1UL << 2);
        rtcp_bye.scount &= ~(1UL << 3);
        rtcp_bye.scount &= ~(1UL << 4);

        rtcp_bye.pt |= 1UL << 0;
        rtcp_bye.pt |= 1UL << 1;
        rtcp_bye.pt &= ~(1UL << 2);
        rtcp_bye.pt |= 1UL << 3;
        rtcp_bye.pt &= ~(1UL << 4);
        rtcp_bye.pt &= ~(1UL << 5);
        rtcp_bye.pt |= 1UL << 6;
        rtcp_bye.pt |= 1UL << 7;

        rtcp_sr.wordlen = 4;

        rtcp_bye.ssrc = RTP_Server_pkt.SSRC;

        rtcp_sr.version &= ~(1UL << 0);
        rtcp_sr.version |= 1UL << 1;

        rtcp_sr.p &= ~(1UL << 0);

        rtcp_sr.rcount &= ~(1UL << 0);
        rtcp_sr.rcount &= ~(1UL << 1);
        rtcp_sr.rcount &= ~(1UL << 2);
        rtcp_sr.rcount &= ~(1UL << 3);
        rtcp_sr.rcount &= ~(1UL << 4);

        rtcp_sr.pt &= ~(1UL << 0);
        rtcp_sr.pt &= ~(1UL << 1);
        rtcp_sr.pt &= ~(1UL << 2);
        rtcp_sr.pt |= 1UL << 3;
        rtcp_sr.pt &= ~(1UL << 4);
        rtcp_sr.pt &= ~(1UL << 5);
        rtcp_sr.pt |= 1UL << 6;
        rtcp_sr.pt |= 1UL << 7;

        rtcp_sr.wordlen = 14;

        rtcp_sr.ssrc = RTP_Server_pkt.SSRC;
    }
    while (!flag)
    {
        sleep(5);
        if (numpktsent == 500)
            flag = true;
        rtcp_sr.pksent = numpktsent;
        rtcp_sr.octsent = numoctsent;
        rtcp_sr.ntp_secfrac = time(NULL) + 2208988800;
        rtcp_sr.rtp_ts = time(NULL);
        if (sendto(Socket_Server, &rtcp_sr, sizeof(rtcp_sr), 0, (struct sockaddr *)&RTCP_Client_Addr, ClientAddLen) == -1)
        {
            cout << "Error in sending RTCP SR packet "
                 << " SORRY!\n"
                 << strerror(errno) << "\n**** EXIT ****\n"
                 << endl;
            return 0;
        }
        if (flag == true)
        {
            if (sendto(Socket_Server, &rtcp_sr, sizeof(rtcp_sr), 0, (struct sockaddr *)&RTCP_Client_Addr, ClientAddLen) == -1)
            {
                cout << "Error in sending RTCP BYE packet"
                     << " SORRY!\n"
                     << strerror(errno) << "\n**** EXIT ****\n"
                     << endl;
                return 0;
            }
        }

        recvfrom(Socket_Server, &rtcp_rr, sizeof(rtcp_rr), 0, (struct sockaddr *)&RTCP_Client_Addr, &ClientAddLen);
        print_report();
    }
    shutdown(Socket_Server, SHUT_RDWR);
    pthread_exit(NULL);
    return 0;
}
int main(int argc, char const *argv[])
{
    /*int rtspPort = atoi(argv[2]);
    int PortNum = rtsp_clipkt.PortNum;
    pthread_t rtsp;
    int rc = pthread_create(&rtsp, NULL, rtsp_server_func, (void *)rtspPort);
    if (rc)
    {
        cout << "ERROR; couldn't creat thread 1 \n";
        return (0);
    }*/

    int PortNum = atoi(argv[1]); // SHOULD BE FOR RTSP SOCKET. BUT RTP WILL USE IT BECAUSE WE COULDN'T IMPLEMENT RTSP.
    pthread_t rtp, rtcp;
    int rc = pthread_create(&rtp, NULL, rtpfunc, (void *)PortNum);
    int rtcpport = PortNum + 1;
    if (rc)
    {
        cout << "ERROR; couldn't creat thread 1 \n";
        return (0);
    }
    rc = pthread_create(&rtcp, NULL, rtcpfunc, (void *)rtcpport);
    if (rc)
    {
        cout << "ERROR; couldn't creat thread 2 \n";
        return (0);
    }
    pthread_join(rtp, NULL);
    pthread_join(rtcp, NULL);
    return 0;
}