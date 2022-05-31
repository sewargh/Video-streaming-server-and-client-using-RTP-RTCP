#include <iostream>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <pthread.h>
#include <thread>
#include "CImg.h"

using namespace cimg_library;
using namespace std;
//#define PortNum 5000
#define MaxThreads 10
struct RTPpkt
{
    unsigned int V : 2;
    unsigned int P : 1;
    unsigned int X : 1;
    unsigned int CC : 4;
    unsigned int M : 1;
    unsigned int PT : 7;
    short int SeqNum;
    int Timestamp;
    int SSRC;
    char data[10000];
}ACKrtp;
// takes one argument which is the port number
int main(int argc, char const *argv[])
{
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
        cout << "Error in binding socket. SORRY!\n"
             << strerror(errno) << "\n**** EXIT ****\n";
        return 0;
    }
    // puts the server socket in a passive mode, where it waits for the client to approach the server to make a connection.
    memset(&ACKrtp, 0, sizeof(ACKrtp));
    memcpy(ACKrtp.data, "Hello from Server !\n", sizeof(ACKrtp.data));

    ACKrtp.V &= ~(1UL << 0);
    ACKrtp.V |= 1UL << 1;
    cout << "V2 = " << ACKrtp.V << endl;
    ACKrtp.P &= ~(1UL << 0);
    cout << "P0 = " << ACKrtp.P << endl;
    ACKrtp.X &= ~(1UL << 0);
    cout << "X0 = " << ACKrtp.X << endl;
    ACKrtp.CC &= ~(1UL << 0);
    ACKrtp.CC &= ~(1UL << 1);
    ACKrtp.CC &= ~(1UL << 2);
    ACKrtp.CC &= ~(1UL << 3);
    cout << "CC0 = " << ACKrtp.CC << endl;
    ACKrtp.M &= ~(1UL << 0);
    cout << "M0 = " << ACKrtp.M << endl;
    ACKrtp.PT &= ~(1UL << 0);
    ACKrtp.PT |= 1UL << 1;
    ACKrtp.PT &= ~(1UL << 2);
    ACKrtp.PT |= 1UL << 3;
    ACKrtp.PT |= 1UL << 4;
    ACKrtp.PT &= ~(1UL << 5);
    ACKrtp.PT &= ~(1UL << 6);
    cout << "PT26 = " << ACKrtp.PT << endl;
    ACKrtp.SeqNum = 258;
    cout << "SeqNum = " << ACKrtp.SeqNum << endl;

    /******** Listen To The Port to Any Connection **********/
    // while (1)

    // Block until receive message from a client
    ClientAddLen = sizeof(ClientAddr);
    int BytesRead;
    cout << "Waiting on port " << PortNum << endl;
    BytesRead = recvfrom(Socket_Server, Buffer, 10000, 0, (struct sockaddr *)&ClientAddr, &ClientAddLen);
    Buffer[BytesRead] = 0;
    cout << "#of read bytes = " << BytesRead << "\nData is : -" << Buffer << endl;
    sendto(Socket_Server, &ACKrtp, sizeof(ACKrtp), 0, (struct sockaddr *)&ClientAddr, ClientAddLen);

    for (int i = 1; i <= 500; i++)
    {
        char name[1000];

        sprintf(name, "vid/image%03d.jpg", i);
        CImg<unsigned char> img(name);
        if (sendto(Socket_Server, img, sizeof(img), 0, (struct sockaddr *)&ServerAddr, (socklen_t)AddrLen) == -1)
        {
            cout << "Error in sending frame #" << i << " SORRY!\n"
                 << strerror(errno) << "\n**** EXIT ****\n";
            return 0;
        }
        usleep(30000);
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