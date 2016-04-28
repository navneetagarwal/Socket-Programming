#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <queue>
#include <time.h>
#include <string>
#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
using namespace std;
#define BACKLOG 8 // pending connections-maximum 5 workers and 3 users

//Function to distribute load depending on flags and the no of available workers.
//Returns a vector of strings where the ith element stores the initial characters to be checked by the ith worker.
vector <string> Distribute(int Flags, int NoOfWorkers) {

	vector <string> output;

	/////////////// VALIDITY CHECK ////////////////

	//Checking validity of flags
	if (Flags == 100 || Flags == 010 || Flags == 001 || Flags == 101 || Flags == 110 || Flags == 011 || Flags == 111 ) {}
	else {
		cout<<"Invalid Flags"<<endl;
		return output;
	}

	//Checking validity of NoOfWorkers
	if (NoOfWorkers <=0) {
		cout<<"No of Workers should be non-negative."<<endl;
		return output;
	}
	else if (NoOfWorkers > 5) {
		cout<<"No of workers exceeds the required number."<<endl;
		return output;	
	}

	//////// VALIDITY CHECK COMPLETE //////////

	//Make output the size of the No of workers. Thus each element in the output is the string being assigned to the corresponding worker.
	output.resize(NoOfWorkers);

	string universe = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";		//Universe of all characters possible.
	int CharPerWorker;																		//Variable to store number of characters per worker

	//If Flag 1 (lower case) is not present, remove it from universe.
	if (Flags/100 != 1) universe = universe.erase(universe.find('a'),26);
	//If Flag 2 (upper case) is not present, remove it from universe.
	if ( ((Flags/10)%10) != 1) universe = universe.erase(universe.find('A'),26);
	//If Flag 3 (numeric) is not present, remove it from universe.
	if ( Flags%10 != 1) universe = universe.erase(universe.find('0'),10);

	CharPerWorker = universe.size()/NoOfWorkers;											//Update the CharPerWorker.

	for (int i=0; i < NoOfWorkers; i++) {

		//For each worker (except the last one), assign the corresponding letters (no of letters = CharPerWorker)
		if (i != NoOfWorkers - 1) {
			output[i] = universe.substr(i*CharPerWorker,CharPerWorker);
		}
		else {																//The last worker takes away all the remaining Characters left.
			output[i] = universe.substr(i*CharPerWorker);
		}

	}

	return output;
}

void printVecOfStr(vector <string> input) {

	for (int i=0;i<input.size();i++) {
		cout<<input[i]<<' ';
	}
	cout<<endl;
	return;
}

int main(int argc, char* argv[])
{
	clock_t t;
	if(argc!=2)
	{
		cout<<"Wrong number of arguments\n";
		exit(1);
	}
	int my_sockfd, new_fd; // listen on sock_fd, new connection on client_socket[i]
	struct sockaddr_in my_addr,their_addr; // my address information in my_addr, new connected machine's information in their_addr

	int worker_fd[5]={0,0,0,0,0}; // Array to store the socket numbers for connections with workers, size 5 since maximum number of workers is 5
	int free_worker[5]={0,0,0,0,0};
	int user_fd[3]={0,0,0}; // to store the socket number corresponding to user connection
	int user_ind=0; // index of new user to be connected to server
	int worker_ind=0; // index of new worker to be connected to server, this mainly helps to know how many workers are already registered with server
	socklen_t sin_size;
	int client_socket[8]={0,0,0,0,0,0,0,0};
	int sd,new_socket,their_addrlen;
	fd_set readfds; // to maintain the set of sockets for select statement
	int max_sd=0; // maximum socket number value to be passed to select statement


	my_sockfd = socket(PF_INET, SOCK_STREAM, 0); // socket of server
	if(my_sockfd==-1)
	{
		cout<<"Error in creating socket\n";
		exit(1);
	}

// Initialising values of my_addr
my_addr.sin_family = AF_INET;
my_addr.sin_port = htons(atoi(argv[1]));
my_addr.sin_addr.s_addr = INADDR_ANY; // auto-fill server IP
memset(&(my_addr.sin_zero), '\0', 8); 

if(bind(my_sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))==-1) // bind the socket information to my_sockfd
{
	cout<<"Error while binding server socket\n";
	exit(2);
}

// Initialise the set of file-descripters
FD_ZERO(&readfds); // initialise to zero contents
FD_SET(my_sockfd, &readfds); // insert the socket of server into set readfds (?)

if(listen(my_sockfd, BACKLOG)==-1) // should listen be outside while true loop?
	{
		cout<<"Error in listening\n";
		exit(2);
	}
int addrlen = sizeof(my_addr);
cout<<"listening....\n";
char buffer[1025];
while(true){
	FD_ZERO(&readfds);
	FD_SET(my_sockfd, &readfds);
	max_sd = my_sockfd;
	for (int i = 0 ; i < 8 ; i++) 
        {
            //socket descriptor
            sd = client_socket[i];
             
            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET( sd , &readfds);
             
            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }
    int activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
    if ((activity < 0)) 
    {
            cout<<"FD Select Error";
    }
    //If something happened on the master socket , then its an incoming connection
    if (FD_ISSET(my_sockfd, &readfds)) 
        {
            if ((new_socket = accept(my_sockfd, (struct sockaddr *)&my_addr, (socklen_t*)&addrlen))<0)
            {
                // perror("accept");
                exit(EXIT_FAILURE);
            }

        	for (int i = 0; i < 8; ++i)
        		{
        			if(client_socket[i]==0){
        				client_socket[i]=new_socket;
        				break;
        			}

        		}
        }
    for (int i = 0; i < 8; i++) 
        {
            sd = client_socket[i];
              
            if (FD_ISSET( sd , &readfds)) 
            {
            	int valread;
                //Check if it was for closing , and also read the incoming message
                if ((valread = read( sd , buffer, 1024)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&my_addr , (socklen_t*)&addrlen);
                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client_socket[i] = 0;

                    //Search for the worker with the concerned fd and free the slot.
                    for(int j=0;j<5;j++){
                    	if(worker_fd[j]==sd){
                    		worker_fd[j]=0;
                    		free_worker[j]=0;
                    		break;
                    	}
                    }
                    //Search for the user with the concerned fd and free the slot.
                    for(int j=0;j<3;j++){
                    	if(user_fd[j]==sd){
                    		user_fd[j]=0;
                    		break;
                    	}
                    }
                }
                  
                //Echo back the message that came in
                else
                {
                    //set the string terminating NULL byte on the end of the data read
                    buffer[valread] = '\0';
                    cout<<buffer<<endl;

                    string request(buffer);

				    //If first bit is 'C' -> It is a client
					if(request[0]=='C')
					{
						//Counting the number of free workers at this point of time.
						int worker_free_ind=0;
						for (int j = 0; j < 5; ++j)
						{
							if(free_worker[j]==1)
								worker_free_ind++;
						}

						//If no free workers/zero workers, then reject password cracking request.
						if(worker_free_ind==0)
						{
							cout<<"No workers, try later\n";
							string toSend="*Sorry, No free workers";
							if(send(client_socket[i],(void*)toSend.c_str(),toSend.length(),0)==-1)
							{
								cout<<"Error sending response to user\n";
							}
							continue;
						}
						//Else, we proceed to assigning workers to crack password.
						else
						{
							//Check which user position is free.
							if(user_fd[0]==0)       
								user_fd[0]=client_socket[i];
							else if(user_fd[1]==0)
								user_fd[1]=client_socket[i];
							else if(user_fd[2]==0)
								user_fd[2]=client_socket[i];
							else {										//All user positions are occupied. Can't accept request.
								cout<<"Already maximum Users";
								string toSend="*Max user limit reached";
								if(send(client_socket[i],(void*)toSend.c_str(),toSend.length(),0)==-1)
								{
									cout<<"Error sending response to user\n";
								}
								continue;
							}
							//At this point, user position given. Worker assignment to user has to be done.

							//////// LOAD DISTRIBUTION //////////
							int Flag = atoi( ( request.substr(3,3) ).c_str() );							//Extracting the 3-bit flag from the request message.

							vector <string> initialChars = Distribute(Flag,worker_free_ind);		//Based on the flag message and no of workers, distribute the load.
							
							/////// LOAD DISTRIBUTION COMPLETE //////////

								//Find id corresponding to the new connection
									int id;
									for (id = 0; id < 3; ++id)
									{
										if(user_fd[id]==client_socket[i])
											break;
									}
								//id now stores the id of the concerned client.

							int workerNo = 0;
							int k =0;
							while(k<5)
							{
								//If the worker 'i' is free, allot him the work.
								if (free_worker[k] == 1) {

									//Making the string that has to be sent to worker.
										string SendtoWorker;
										SendtoWorker.append(to_string(id));						//1st bit is id.
										SendtoWorker.append(request.substr(1,2));				//Next 2 bits are the length of the Hashed Password
										SendtoWorker.append(request.substr(3,3));				//Next 3 bits are the Flags sent by the client
										SendtoWorker.append(request.substr(6,13));				//Next 13 bits is the hashed password sent by client
										SendtoWorker.append(initialChars[workerNo++]);			//Next few bits are the intial characters. (incrementing the worker no as well)
									//The string is complete.

									//Sending the message to the concerned worker.
										if(send(worker_fd[k],(void*)SendtoWorker.c_str(),SendtoWorker.length(),0)==-1)
										{
											cout<<"Error in sending request to workers\n";
											exit(1);
										}
									//Send Complete.

									free_worker[k] = 0;

								}

								k++;

							}
	
						}

					}
					else if(request[0]=='W')
					{
						//If it is a new user
						if(request[1]=='1')
						{
							//If max no of workers reached, can't accomodate more.
								if (worker_ind == 5) {
									cout<<"Max No of workers reached!!!"<<endl;
									if(send(client_socket[i],"Max No of workers reached!!!",1,0)==-1)
									{
										cout<<"Error in sending can't accept message to workers\n";
										exit(1);
									}
								}
							//End of validity of no of workers.

							//Makinf profile of new worker.
								worker_fd[worker_ind]=client_socket[i];
								free_worker[worker_ind]++;
								worker_ind++;
							//Profile completed.

						}
						//Hence it is an old user.
						//Next bit tells us if the password has been cracked.
						else if(request[2]=='1') {

							int id = int(request[3]-'0');
							string CrackedPassword = request.substr(4);

							if(send(user_fd[id],(void*)CrackedPassword.c_str(),CrackedPassword.length(),0)==-1)
							{
								cout<<"Error in sending final password to user\n";
								exit(1);
							}

							//Free this worker
								int j;
								for(j=0;j<5;j++) {
									if (worker_fd[j] == client_socket[i]) break;
								}

								free_worker[j] = 1;
							//Worker free.

							////// INFORM ALL THE WORKERS WORKING FOR THE CONCERNED USER TO STOP /////////
								string abortMessage = to_string(id) + "STOP";
								for (int l=0;l<worker_ind;l++) {
									
									if (worker_fd[l] != 0) {
										if(send(worker_fd[l],(void*)abortMessage.c_str(),abortMessage.length(),0)==-1) {
											cout<<"Error in sending abort message to other workers\n";
											exit(1);
										}
									}

								}
							// All workers informed.

						}
						//The worker tried all combinations but didn't get the password. Time to free the worker.
						else if(request[2]=='0') {

							int j;
							for(j=0;j<5;j++) {
								if (worker_fd[j] == client_socket[i]) break;
							}

							free_worker[j] = 1;

						}

					}
                }
            }
        }


}
}