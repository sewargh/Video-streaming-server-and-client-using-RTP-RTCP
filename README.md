**Don't forget to add _vid_ file to your working code directory.**

****Compile client.cpp:****

g++ -o Client Client.cpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11

****Execute client code:****

./Client IP PortNum
<sub> --> where IP: One of your ip addresses, PortNum: Server Port Number for RTP udp socket </sub>

****Compile server.cpp:****

g++ Server.cpp -o Server -pthread

****Execute server code:****

./server PortNum
<sub> --> where PortNum: Server Port Number for RTP udp socket </sub>

/* RTSP client+server code is commented in the .cpp files because it is not complete. */
