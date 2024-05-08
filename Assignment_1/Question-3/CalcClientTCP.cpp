#include <bits/stdc++.h>
#include <iostream>
#include <arpa/inet.h>
#include <string>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <netdb.h>
using namespace std;

int main(int argc, char *argv[]){
	if (argc == 1){
		cout << "Insufficient arguements!\n";
		return 1;
	}

    // Convert hostname to IP.

    const char *hostname = argv[1];
    struct addrinfo hints, *result;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // Use IPv4
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname, NULL, &hints, &result);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return 1;
    }

    struct sockaddr_in *addr = (struct sockaddr_in *)result->ai_addr;
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr->sin_addr), ip, INET_ADDRSTRLEN);

    // std::cout << "IP address of " << hostname << " is: " << ip << std::endl;

    // freeaddrinfo(result);

    // Server code starts here.
    int PORT = stoi(argv[2]);
	int valread, clientSocket;
	struct sockaddr_in serv_addr;

	serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(PORT);

	// char hello[20] = "Hello from client";
	// char buffer[1024] = { 0 };
	if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return 1;
	}
	else{
		// cout<<"Socket created"<<endl;
	}

	if ((status
		= connect(clientSocket, (struct sockaddr*)&serv_addr,
				sizeof(serv_addr)))
		< 0) {
		cout<<"\nConnection Failed \n";
		return -1;
	}
	else{
		cout<<"TCP client connected to " << ip << " on port " << PORT << "\n\n";
	}
	
	while(true){
        string message;
		cout << "Enter an expression in the following format:" << endl <<
                "operand1 operator operand2" << endl << 
                "Valid operators are + - * / ^.:" << endl << 
                "To quit, enter -1." << endl;
		getline(cin, message);
		
		if (message == "-1"){
            cout << "Bye!\n";
            return 1;
        }

		send(clientSocket, message.c_str(), message.length(), 0);

		char ackBuffer[1024];
		int bytesReceived = recv(clientSocket, ackBuffer, sizeof(ackBuffer), 0);
		if (bytesReceived > 0) {
			ackBuffer[bytesReceived] = '\0';
            cout << "ANS:" << message << " = " << ackBuffer << "\n\n";
		}
	}
	return 0;
}


