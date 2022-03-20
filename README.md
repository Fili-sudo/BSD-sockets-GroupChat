# BSD-sockets-GroupChattest
In order for the chat to work you must use these commands
# Compile server.c:
 gcc server.c -o server.out -pthread
 # Compile client.c:
gcc client.c -o client.out -pthread
 # Run server:
 ./server.out port_number
 # Run client:
 ./client.out username port_number
 # Useful tips:
 To log out a client, or to shut down the server you can use ctrl+C
 
 The port number for the client should be the same as the server port number.
 
 The ip address must be verified and changed in the server.c and client.c.
 It can be discovered by using the ipconfig command.
