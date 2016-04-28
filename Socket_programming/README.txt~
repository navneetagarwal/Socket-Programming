			Socket Programming Project


Name :
Navneet Agarwal 140100090
Tanmay Parekh   140100011


List of Relevant files:

1) User Code: user.cpp
	- This contains the code for the client side. The client runs this file as described in the format.
	- To compile, use 'g++ -o user -std=c++11 user.cpp' 
	- To run, use './user <server ip> <server-port> <hash> <passwd-length> <binary-string>'
	- If there is some error, the error message is printed on the console
	- Else, the password is printed on the first line and the time is printed on the next line.

2) Server Code: server.cpp
	- This contains the code for the server.
	- To compile, use 'g++ -o server -std=c++11 server.cpp'
	- To run, use './server <server-port>'
	- If there is any error at some point, it will print the error on the console.
	- Also, it prints any message that it receives from the client/worker side. This helps to debug and understand the protocol.

3) Worker Code: worker.cpp
	- This contains the code for the worker.
	- To compile, use 'g++ -o worker -std=c++11 worker.cpp -lcrypt -lpthread'
	- To run, use './worker <server ip> <server-port>'
	- It prints on the console what the worker is doing right now.

4) Graph plot: results.jpg
	- The data is stored in the Avg_Time.ods
	- Due to scaling issues, the graph looks skewed.

Configuration Files:

1) random_length.py:
	- The file generates 10 'smart' random numbers of the given length (length must be provided as argument).
	- Run program by 'python random_length.py <length of number'.
	- First random number starts from 0, second from 1, ... tenth starts from 9. Hence, idea is to test on all different numbers.
	- Due to above reason, it generates 'smart' random numbers.
	- It stores all these numbers in a file called 'Output.txt'

2) crypt_ran.cpp:
	- This file crypts the numbers from the file 'Output.txt' and stores it in another one called 'Crypt.txt'.
	- Compile this file by 'g++ -o crypt_ran -std=c++11 crypt_ran.cpp -lcrypt'.
	- Run this file by './crypt_ran'.
	- This file uses "aa" as the salt for crypting.

3) Avg.py:
	- This file is used to compute the average and print it on the console.
	- It reads from the file called 'text.txt' and extracts the time from the text file. It computes the average simultaneously.
	- After reading, it prints the average on the console in seconds.

4) Run_client.sh:
	- This is a bash script which is used to automate the entire process.
	- It runs random_length.py and crypt_ran.cpp and we obtain the 'Crypt.txt'.
	- Now, for each hashed password in 'Crypt.txt', it runs the client object. It stores the output in a file 'text.txt'.
	- You need to manually put the IP, port number, length, flags in the command of the run client in this file.
	- Also, you need to manually assign the length of the password while running random_length.py as argument.
	- After this, it runs the Avg.py which averages everything and outputs the average.
	- At the end, it deletes 'Output.txt', 'Crypt.txt' and 'text.txt'.

5) Avg_Time.ods:
	- This file contains the data of the average times varied with number of workers and password length.

Running Instructions:

1) make clean 

2) make


make clean : Contains 
	     rm $(PROJECT_ROOT)worker; \
	     rm $(PROJECT_ROOT)server; \
	     rm $(PROJECT_ROOT)user; \

Description : It removes the executables if they are present in the folder by using the rm command.

make : Executes all:

all:userexec workerexec serverexec

This compiles the respective user, worker and server.cpp files and creates their executables. And then they can be executed using the commands stated above.


SOME IMPORTANT POINTS :

1) If there are no workers or no free workers then the server doesn't crack password for the user stating that it does not have free workers. In such a case a user is expected to try again later.

2) If a new user comes the server distributes the work amongst the available free workers. Thus it does not wait for all the workers to get free.

3) If a worker cracks the password the server sends an abort message to all the workers telling them to stop working for the concerned user. The workers cracking the password for the concerned user stop immediately and become free.

4) If the server is stopped, you may see some unwanted messages on the worker console.