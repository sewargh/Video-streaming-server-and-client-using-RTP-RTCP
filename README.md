# multimedia-project
## To compile client.cpp:
g++ client.cpp client

## To execute client code:
./client x.x.x.x y --> where x.x.x.x: One of your ip addresses, y: Server port number

## To compile server.cpp:
g++ -o server server.cpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11

## To execute server code:
./server 5000 --> where y: server port number
