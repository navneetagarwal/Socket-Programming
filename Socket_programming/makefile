PROJECT_ROOT=./

all:userexec workerexec serverexec

userexec:
	@g++ -o user -std=c++11 user.cpp; \

workerexec:
	@g++ -o worker -std=c++11 worker.cpp -lcrypt -lpthread; \

serverexec:
	@g++ -o server -std=c++11 server.cpp; \

clean:
	@cd $(PROJECT_ROOT); \
	rm $(PROJECT_ROOT)worker; \
	rm $(PROJECT_ROOT)server; \
	rm $(PROJECT_ROOT)user; \

