#include <iostream>
#include <string.h>
#include <string>  
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <crypt.h>
#include <cstdlib>
#include <pthread.h>

using namespace std;

bool found;
string trial;
char salt[2]; 
string hashedPassword;
int user_id;
int my_sockfd;
int STOP;


struct thread_data {
	string message;
};



char *crypt(const char *key, const char *salt);
//Function to check if the given string is the password or not.
bool checkPassword(string attempt) {

	salt[0] = hashedPassword[0];
	salt[1] = hashedPassword[1];
	//Hash the password and match it with the actual hashed password. (Change condition inside for the same)
	if (crypt(attempt.c_str(),salt) == hashedPassword) return 1;
	else return 0;

}

//Recursive call for each vacant position in the string. 
//i is the position of the string. Flags are as described before. length is the length of the password.
void layering(int i, int Flags, int length) {

	/////////////////// VALIDITY CHECKS ///////////////////

	//Checking validity of i
	if (i >= length) {
		// cout<<"Invalid length or trying to access non-accessible elements"<<endl;
		return;
	}

	//Checking validity of flags
	if (Flags == 100 || Flags == 010 || Flags == 001 || Flags == 101 || Flags == 110 || Flags == 011 || Flags == 111 ) {}
	else {
		// cout<<"Invalid Flags"<<endl;
		return;
	}

	//////////////VALIDITY CHECK COMPLETE ////////////////

	if (STOP == 1) {
		return;
	}


	/////////////// MAIN ALGORITHM //////////////////////
	//Proceeding to brute-force algorithm. Implemented a backtracking algorithm and using recursive calls for the same.


	//If the first bit is 1, lower case is allowed. Checking for each lower case alphabet.
	if (Flags/100 == 1) {

		for (char letter = 'a';letter <= 'z';) {				//Keeping a check if we got a valid string

			if (STOP == 1) {
				return;
			}

			trial[i] = letter;

			if (i == length-1) {								//The string is complete. Check for validity
				found = checkPassword(trial);
			}
			else layering(i+1,Flags,length);

			if (found == 0) letter++;							//String not found. Check next character.
			else return;										//The string has been found. Return.

		}

	}
	//If the second bit is 1, upper case is allowed. Checking for each upper case alphabet.
	if ( ((Flags/10)%10) == 1) {

		for (char letter = 'A';letter <= 'Z';) {				//Keeping a check if we got a valid string
			
			if (STOP == 1) {
				return;
			}
			
			trial[i] = letter;
			
			if (i == length-1) {								//The string is complete. Check for validity
				found = checkPassword(trial);
			}
			else layering(i+1,Flags,length);

			if (found == 0) letter++;							//String not found. Check next character.
			else return;										//The string has been found. Return.

		}

	}
	//If the third bit is 1, numbers is allowed. Checking for each number.
	if ( Flags%10 == 1) {

		for (char letter = '0';letter <= '9';) {				//Keeping a check if we got a valid string
			
			if (STOP == 1) {
				return;
			}
			
			trial[i] = letter;
			
			if (i == length-1) {								//The string is complete. Check for validity
				found = checkPassword(trial);
			}
			else layering(i+1,Flags,length);

			if (found == 0) letter++;							//String not found. Check next character.
			else return;										//The string has been found. Return.

		}
	}

	/////////////// ALGORITHM COMPLETE /////////////////

	return;

}

//It finds if the password starts with the given initial letters.
//Flags are as usual. Length is the length of the password.
string findPassword(string FirstLetter, int Flags, int length) {

	found = 0;							//Global variable for checking if the password has been cracked.
	trial.resize(length);				//Restrict size of trial string to the length of password.

	//For every intial letter, check all possible words and update trial accordingly. If password is found, trial is the password.
	for (int i=0;i<FirstLetter.size() && found == 0;i++) {

		trial[0] = FirstLetter[i];
		layering(1,Flags,length);

	}
	if (found == 0) {					//If not found, output *****. Indication that password not found.
		trial.resize(0);
		trial.resize(length,'*');
	}

	return trial;

}


void *CrackPassword(void *thr) {

	struct thread_data *my_data;
	my_data = (struct thread_data *) thr;
	string inString = my_data->message;

	cout<<"Work Assigned. Cracking password..."<<endl;

	user_id = inString[0] - '0';

	int lenOfPassword;				// Length of the password to crack
	int Flags;						// Flags to indicate whether it is lower/upper/numeric case.
	string rcvd_password;			// Stores the received hashed password.
	string initChars;				// The initial characters of which the worker has to check the password for.

	// Extracting data out of string and storing it in the relevant fields.

		lenOfPassword = atoi( (inString.substr(1,2)).c_str() );
		Flags = atoi( (inString.substr(3,3)).c_str() );
		rcvd_password = inString.substr(6,13);			

		int i;
		for (i=0;i<100 && inString[i] != '*';i++) {	}			// Find the actual length of the string received.
		int lenOfString = i;

		initChars = inString.substr(19,lenOfString - 19);

		// IMPORTANT - Restore string to original format for further use.
			for (int i=0; i<100; i++) {
				inString[i] = '*';
			}
		// Restoration complete.


	////////// EXTRACTION COMPLETE //////////////



	///////////// Run Brute-Force algorithm. //////////////////

		hashedPassword = rcvd_password;
		string fakePassword(lenOfPassword,'*');

		string password_found = findPassword(initChars, Flags, lenOfPassword);

	//////////// Algorithm Complete. /////////////////////



	// SEND MESSAGE BACK TO SERVER BASED ON THE MESSAGE RECEIVED.

		string toSend;

		//Checking if the password has been found.
		if (password_found == fakePassword) {				// Password not found. Sad luck.

			toSend.append("W00");							// W -> worker, 0 -> old connection, 0 -> Password not cracked

			if( send(my_sockfd,(void*)toSend.c_str(),(toSend.length()),0) == -1 )
			{
				cout<<"Error in sending message to server\n";
				exit(3);
			}

			user_id = -1;

			if( STOP == 1) {
				cout<<"Password cracked by different worker. Stopping process..."<<endl;
			}

			cout<<"Worker is free."<<endl;

		}
		else {												// Password has been cracked. Congrats!

			toSend.append("WO1");
			toSend.append(to_string(user_id));							// W -> worker, 0 -> old connection, 1 -> Password cracked
			toSend.append(password_found);					// Next few bits is the password.

			if( send(my_sockfd,(void*)toSend.c_str(),(toSend.length()),0) == -1 )
			{
				cout<<"Error in sending message to server\n";
				exit(3);
			}

			user_id = -1;
			cout<<"Worker cracked password. Kudos!"<<endl;
			cout<<"Worker is free"<<endl;

		}

	// MESSAGE SENDING OVER.

	pthread_exit(NULL);
	
}

// arguments format ./user <server ip/host-name> <server-port> <hash> <passwd-length> <binary-string>
// later scale to accomodate server host name, right now works only with IP address
int main(int argc, char* argv[])
{
	
	user_id = -1;	// Stores the user the worker is cracking the password of.
	STOP = 0;


	// VALIDITY CHECK //
	if(argc!=3)
	{
		cout<<"Wrong number of arguments\n";
		exit(1);
	}


	int connection_fd; // listen on my_sock_fd, new connection on connection_fd
	struct sockaddr_in my_addr; // my address information
	struct sockaddr_in server_addr; // server's address information
	socklen_t sin_size;

	my_sockfd = socket(PF_INET, SOCK_STREAM, 0);

	// ACCOUNTING FOR SOCKET CREATION FAILURE //
	if(my_sockfd==-1)
	{
		cout<<"failure in creating socket\n";
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]); // FILL THIS WITH ACTUAL SERVER IP LATER
	memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct

	connection_fd=connect(my_sockfd,(struct sockaddr*)&server_addr,sizeof(struct sockaddr)); // make connection to server

	// ACCOUNTING FOR CONNECTION ERROR //
	if(connection_fd==-1)
	{
		cout<<"Error in connection\n";
		exit(2);
	}

	///////// Idea is to send a message to the server saying that I am a worker. //////////

	string request = "W1"; // W -> worker, 1 -> New

	if( send(my_sockfd,(void*)request.c_str(),(request.length()),0) == -1 )
	{
		cout<<"Error in sending message to server\n";
		exit(3);
	}
	//////// Message sent. New worker registered. ////////


	pthread_t thread;
	struct thread_data td;
	int rc;


	while (true) {

		//////// If a message is received, it is either of the following -
		//////// 	1. Crack the given password.
		////////	2. Stop the work as the password has been cracked by someone else.

		string inString(100,'*');	// String to store the received message. Initially stuffing it with 

		// Receive the message //
		if(recv(my_sockfd, (void*)inString.c_str(),strlen(inString.c_str()),0)==-1)
		{
			cout<<"Error in receiving message\n";
			exit(4);
		}
		// If the message is to stop the work, it's 2nd to 5th bits would be "STOP".
		if (inString.substr(1,4) == "STOP") {


			// Checking if the password has been cracked for the user who the worker is working for 
			if (user_id == inString[0] - '0') {

				STOP = 1;
				user_id = -1;


			// CODE TO STOP WORK AND BECOME IDLE AND LISTEN AGAIN

			}

		}
		// New Assignment - Crack the given password.
		else {

			STOP = 0;
			td.message = inString;
			rc = pthread_create(&thread,NULL,CrackPassword, (void *)&td);
			if (rc) {
				cout << "Error:unable to create thread," << rc <<endl;
				exit(-1);
			}
			
		}

	}

	pthread_exit(NULL);
	return 0;

}