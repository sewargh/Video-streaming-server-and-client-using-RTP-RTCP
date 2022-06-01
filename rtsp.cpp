#include <iostream>
#include <string.h>
#include <time>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <thread>
#include "CImg.h"


// define portNum 554

using namespace cimg_library;
using namespace std;

//client side 

struck  {
       unsigned int v:2 ;           //version        
       unsigned int p:1 ;           //padding
       short int    CSeq ;          //sequence number
       int          SSRC; 
} rtsp_clipkt;

//server side

struck rtsppck {
       unsigned int  v:2 ;               
       unsigned int  p:1 ;         
       int           SSRC;
       short int     CSeq ;           
       string        Pt;            //payload type
       unsigned int  len;           //length 
} rtsp_srvpkt;

int main(int argc, char const *argv[])
{
    int PortNum = atoi(argv[1]);
    char Buffer[10000];
    int opt = 1;

    struct sockaddr_in ServerAddr; 
    struct sockaddr_in ClientAddr;
    int AddrLen = sizeof(ServerAddr);
    socklen_t ClientAddLen;

    /******** Create A Socket To Communicate With Server **********/
    int Socket_Server = socket(AF_INET, SOCK_DGRAM, 0);   // returns zero if creation succes else -1
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
        cout << "Error in binding socket. SORRY!\n"<< strerror(errno) << "\n**** EXIT ****\n";
        return 0;
    }
    // puts the server socket in a passive mode, where it waits for the client to approach the server to make a connection.
    
shutdown(Socket_Server, SHUT_RDWR);

    return 0;
}