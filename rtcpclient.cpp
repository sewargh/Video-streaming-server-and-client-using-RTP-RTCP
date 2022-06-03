void *rtcp_client_func(void *PORTNUM)
{
    bool flag = false;
    int d = 0;
    int j = 0;
    int n= 0;
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
        cout << "|Lenght(in words) :" << rtcpclient_sr.wordlen << "\t|SSRC :" << rtcpclient_sr.ssrc << "\t|NTP timestamp:" << rtcpclient_sr.ntp_secfrac<<endl;
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

            if (sts[numpktsent - 2] != 0 && sts[numpktsent - 1] != 0 && rts[numpktsent - 2] != 0 && rts[numpktsent - 1] != 0){
                //cout<<rts[numpktsent - 2]<<"\t"<<sts[numpktsent - 2]<<"\t"<<rts[numpktsent - 1]<<"\t"<<sts[numpktsent - 1]<<endl;
                d = rts[numpktsent - 2] - sts[numpktsent - 2] - rts[numpktsent - 1] + sts[numpktsent - 1];
                j = float(((abs(d) - j) / 16) + j); }
        rtcpclient_rr.jitter = j;
        rtcpclient_rr.bfraction = float(((166 - numpktsend5) / 166) * 256);
        rtcpclient_rr.blost += (166 - numpktsend5);
        if (n==3) rtcpclient_rr.blost += (2 - numpktsend5);
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
