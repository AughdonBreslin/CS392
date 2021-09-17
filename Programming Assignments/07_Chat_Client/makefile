#*******************************************************************************
 # Name        : chatclient.c
 # Author      : Matthew Oyales & Audie Breslin
 # Date        : 7 July 2021
 # Description : Chat client using sockets API in C.
 # Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 #*****************************************************************************/
CC     = gcc
CFLAGS = -g -Wall -Werror -pedantic-errors
all:
	$(CC) $(CFLAGS) -o chatclient chatclient.c 
	$(CC) $(CFLAGS) -o chatserver chatserver.c 
clean:
	rm chatclient chatserver