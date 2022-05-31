#include <iostream>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <pthread.h>
#include <thread>
#include "CImg.h"
#include <arpa/inet.h>

using namespace cimg_library;
using namespace std;
//#define PortNum 5000
#define MaxThreads 10
struct RTP_pkt
{
    unsigned int V : 2;
    unsigned int P : 1;
    unsigned int X : 1;
    unsigned int CC : 4;
    unsigned int M : 1;
    unsigned int PT : 7;
    short int SeqNum;
    uint32_t Timestamp;
    int SSRC;
    //CImg<unsigned char> img;
    // char data[10000];
} RTPpkt;
// takes one argument which is the port number // argv[2] is ip address for server
int main(int argc, char const *argv[])
{
    int PortNum = atoi(argv[1]);
    char Buffer[10000];
    int opt = 1;
    cout << "size of uint32_t " << sizeof(uint32_t) << endl;
    cout << "size of unsigned int " << sizeof(unsigned int) << endl;
    cout << "size of RTPpkt " << sizeof(RTPpkt) << endl;
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
    ServerAddr.sin_addr.s_addr = INADDR_ANY ;
    cout << "server ip " << ServerAddr.sin_addr.s_addr << endl;
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
    memset(&RTPpkt, 0, sizeof(RTPpkt));
    // memcpy(RTPpkt.data, "Hello from Server !\n", sizeof(RTPpkt.data));

    RTPpkt.V &= ~(1UL << 0);
    RTPpkt.V |= 1UL << 1;
    cout << "V2 = " << RTPpkt.V;
    RTPpkt.P &= ~(1UL << 0);
    cout << "   |P0 = " << RTPpkt.P;
    RTPpkt.X &= ~(1UL << 0);
    cout << "   |X0 = " << RTPpkt.X;
    RTPpkt.CC &= ~(1UL << 0);
    RTPpkt.CC &= ~(1UL << 1);
    RTPpkt.CC &= ~(1UL << 2);
    RTPpkt.CC &= ~(1UL << 3);
    cout << "   |CC0 = " << RTPpkt.CC;
    RTPpkt.M &= ~(1UL << 0);
    cout << "   |M0 = " << RTPpkt.M;
    RTPpkt.PT &= ~(1UL << 0);
    RTPpkt.PT |= 1UL << 1;
    RTPpkt.PT &= ~(1UL << 2);
    RTPpkt.PT |= 1UL << 3;
    RTPpkt.PT |= 1UL << 4;
    RTPpkt.PT &= ~(1UL << 5);
    RTPpkt.PT &= ~(1UL << 6);
    cout << "   |PT26 = " << RTPpkt.PT;
    srand(time(0));
    RTPpkt.SeqNum = rand() % 64535; // Make sure SeqNum is enough to hold the sent rtp packets for the 500 frames + other rtp pkts.((2^16-1) - 1000)
    cout << "   |SeqNum = " << RTPpkt.SeqNum << endl;
    RTPpkt.Timestamp = time(NULL);
    srand(time(0));
    RTPpkt.SSRC = rand() % 4294967296;
    cout << "SSRC " << RTPpkt.SSRC << endl;
    /******** Listen To The Port to Any Connection **********/
    // while (1)

    // Block until receive message from a client
    ClientAddLen = sizeof(ClientAddr);
    int BytesRead;
    cout << "Waiting on port " << PortNum << endl;
    BytesRead = recvfrom(Socket_Server, Buffer, 10000, 0, (struct sockaddr *)&ClientAddr, &ClientAddLen);
    Buffer[BytesRead] = 0;
    cout << " client ip after rcv " << ClientAddr.sin_addr.s_addr << endl;
    cout << "#of read bytes = " << BytesRead << "\nData is : -" << Buffer << endl;
    sendto(Socket_Server, &RTPpkt, sizeof(RTPpkt), 0, (struct sockaddr *)&ClientAddr, ClientAddLen);
    cout << " client ip " << ClientAddr.sin_addr.s_addr << endl;
    for (int i = 1; i <= 500; i++)
    {

        char name[1000];

        sprintf(name, "vid/image%03d.jpg", i);
        CImg<unsigned char> img(name);
        //RTPpkt.img(name);
        if (sendto(Socket_Server, img, sizeof(img), 0, (struct sockaddr *)&ServerAddr, (socklen_t)AddrLen) == -1)
        {
            cout << "Error in sending frame #" << i << " SORRY!\n"
                 << strerror(errno) << "\n**** EXIT ****\n";
            return 0;
        }
        usleep(30000);
        RTPpkt.SeqNum += 1;
        RTPpkt.Timestamp = time(NULL);
    }
    
    // static int HeaderSize = 12;
    // cout << "Create thread \n";
    // pthread_t Threads[MaxThreads];
    // pthread_mutex_t MutexThreads[MaxThreads];
    // pthread_cond_t threads_cond[MaxThreads];		// cond wait array.
    // int threads_stat[MaxThreads];

    shutdown(Socket_Server, SHUT_RDWR);

    return 0;
}