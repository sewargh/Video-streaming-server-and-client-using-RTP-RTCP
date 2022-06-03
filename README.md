Don't forget to add vid file to your working code directory.

****to compile client.cpp:

g++ -o Client Client.cpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11

****to execute client code:

./Client IP PortNum
<sub> --> where IP: One of your ip addresses, PortNum: Server Port Number for RTP udp socket </sub>

****to compile server.cpp:

g++ Server.cpp -o Server -pthread

****to execute server code:

./server PortNum
--> where PortNum: Server Port Number for RTP udp socket

/* RTSP client+server code is commented in the .cpp files because it is not complete. */
