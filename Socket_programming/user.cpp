#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <sys/time.h>
using namespace std;

//This function called when a system call fails . Displays a message and then aborts the program
void error(const char *msg)
{
    perror(msg);
    exit(0);
}
//End of error

int main(int argc, char **argv) {


	if(argc!=6)
	{
		cout<<"Wrong number of arguments\n";
		exit(1);
	}
	struct sockaddr_in servaddr;  //A sockaddr_in is a structure containing an internet address
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//The socket() system call creates a new socket. It takes three arguments. The first is the address domain of the socket.
	struct hostent *server;   //It defines a host computer on the Internet. 
	timeval start, end;

	if (sock == -1) perror("Socket");

	server = gethostbyname(argv[1]);//The variable argv[1] contains the IP of a host on the Internet
	//If error in the above code
	if (server == NULL) {
	    fprintf(stderr,"ERROR, no such host\n");
	    exit(0);
	}


	bzero((void *) &servaddr, sizeof(servaddr));            //The function bzero() sets all values in a buffer to zero. It takes two arguments, the first is a pointer to the buffer and the second is the size of the buffer. Thus, this line initializes serv_addr to zeros. ----
	servaddr.sin_family = AF_INET;            //The variable serv_addr is a structure of type struct sockaddr_in. This structure has four fields. The first field is short sin_family, which contains a code for the address family. It should always be set to the symbolic constant AF_INET.
	/*The second field of serv_addr is unsigned short sin_port, 
	which contain the port number. However, instead of simply copying the port number to this field, it is necessary to convert this 
	to network byte order using the function htons() which converts a port number in host byte order to a port number in network byte order.*/
	int portno = atoi(argv[2]);
	servaddr.sin_port = htons(portno);
	// printf("YO\n");
	//Copies h_length number of bytes from h_addr to sin_addr.s_addr
	bcopy((char *)server->h_addr, 
	     (char *)&servaddr.sin_addr.s_addr,
	     server->h_length);
	
	/*
	The connect function is called by the client to establish a connection to the server.
	It takes three arguments, the socket file descriptor, the address of the host to which it wants to connect
	(including the port number), and the size of this address. This function returns 0 on success and -1 if it fails.*/
	if (-1 == connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)))
	perror("Connect");

	// Message to send //

		string HashedPassword(argv[3]);
		int lenInitChars = atoi(argv[4]);
		string Flags(argv[5]);

		string toSend;
		toSend.append("C");

		// Making length two bits ( for 2 digit lengths as well )
		string len;
		if (lenInitChars/10 == 0) len = '0' + to_string(lenInitChars);
		else len = to_string(lenInitChars);

		toSend.append(len);
		toSend.append(Flags);
		toSend.append(HashedPassword);

	// MESSAGE COMPLETE
		gettimeofday(&start, NULL);
		
		if(send(sock,(void*)toSend.c_str(),toSend.length(),0)==-1)
		{
			cout<<"Error in sending request to workers\n";
			exit(1);
		}


		char buffer[255];

		int n = recv(sock,buffer,255,0);

		if (n < 0) error("ERROR reading from socket");
		else {
			//Printing out the input String
			if(buffer[0]=='*'){
				for (int i = 0; i < 23; ++i)
				{
					cout<<buffer[i];
				}
				cout<<endl;
			}
			else{
				//Printing out the input String
				for (int i = 0; i < lenInitChars; ++i)
				{
					cout<<buffer[i];
				}
				cout<<endl;
			
				gettimeofday(&end, NULL);
				double time = (end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec)/1000000;
			
				cout<<time<<"seconds"<<endl;
				// break;	
			}
		}
	
	close(sock);
}