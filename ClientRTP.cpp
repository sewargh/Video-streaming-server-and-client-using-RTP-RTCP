#include <iostream>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <pthread.h>
#include <thread>

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
    char data[1024];

};

int main(int argc, char const *argv[])
{
    int PortNum = stoi(argv[1]);
    char Buffer[1024];
    int opt = 1;
    // use sockaddr_in ?
    struct sockaddr_in ClientAddr, ServerAddr; //socket file descriptor for the server to connect to.
    int AddrLen = sizeof(ClientAddr);
    socklen_t AddSenderLen;
    // socket creation -- 1_ communication domain. 2_ comm type (UDP). 3_ protocol.
    // returns zero if creation succes else -1
    int Socket_Client = socket(AF_INET, SOCK_DGRAM, 0);
    // should we use handle_error("socket");?
    if (Socket_Client == -1){cout << "error in creation socket.\n**** EXIT ****\n";return 0;}

    int setsocketerr = setsockopt(Socket_Client, SOL_SOCKET,SO_REUSEADDR, &opt,sizeof(opt));
    if (setsocketerr == -1){cout << "error in set socket reuseadd.\n" << strerror(errno) << "\n**** EXIT ****\n";return 0;}
    setsocketerr = setsockopt(Socket_Client, SOL_SOCKET,SO_REUSEPORT, &opt,sizeof(opt));
    if (setsocketerr == -1){cout << "error in set socket reuseport.\n" << strerror(errno) << "\n**** EXIT ****\n";return 0;}

    ClientAddr.sin_family = AF_INET;
    ClientAddr.sin_addr.s_addr = INADDR_ANY;
    ClientAddr.sin_port = htons(PortNum);
    // assgin name to a asocket. attaching socket to the port 5000
    //(struct sockaddr *) &address --> convert if initialize it as sockaddr_in
    // returns zero if creation succes else -1
    int binderr = bind(Socket_Client, (struct sockaddr*)&ClientAddr, AddrLen);
    if (binderr == -1){cout << "error in binding socket.\n" << strerror(errno) << "\n**** EXIT ****\n";return 0;}
    //puts the server socket in a passive mode, where it waits for the client to approach the server to make a connection.
    //how many request max ? 1? 
    //int listenerr = listen(Socket_Client,3);
    //if (listenerr == -1){cout << "error in server listening.\n" << strerror(errno) << "\n**** EXIT ****\n";return 0;}

    //UDP sockets do not have an 'accept' call for server applications.
    /*while(1)
        {
            // Block until receive message from a client
            AddSenderLen = sizeof (ServerAddr);
            int BytesRead;
            cout<<"Waiting on port " << PortNum << endl;
            BytesRead = recvfrom(Socket_Client,Buffer,1024,0,(struct sockaddr*)&ServerAddr,(socklen_t*)&AddSenderLen);

        } */
    //creates a new connected socket, and returns a new file descriptor referring to that socket
    //int acceptsocket = accept(Socket_Client,(struct sockaddr*)&ClientAddr, (socklen_t*)& AddrLen);
    //if (acceptsocket == -1){cout << "error in acceptsocket.\n" << strerror(errno) << "\n**** EXIT ****\n";return 0;}
    //close(acceptsocket);


//is the thread used for sending the images ?
    //thread t(&thread_function);
    
    char request [] = "Hi  Sewar, I'm Rufaida";
    send (Socket_Client, request, sizeof(request), 0);

    //static int HeaderSize = 12;
    //pthread_t Threads[MaxThreads];
    //pthread_mutex_t MutexThreads[MaxThreads];
    //pthread_cond_t threads_cond[MaxThreads];		// cond wait array.
    //int threads_state[MaxThreads];	




    shutdown(Socket_Client, SHUT_RDWR);


    return 0;
}