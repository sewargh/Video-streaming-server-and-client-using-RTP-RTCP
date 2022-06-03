Don't forget to add vid file to your working code directory.

##TO COMPILE CLIENT.CPP:

g++ -o Client Client.cpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11

##TO EXECUTE CLIENT CODE:

./Client IP PortNum

##TO COMPILE SERVER.CPP:

g++ Server.cpp -o Server -pthread

##TO EXECUTE SERVER CODE:

./server PortNum

/* RTSP client+server code is commented in the .cpp files because it is not complete. */
