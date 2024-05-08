#include<bits/stdc++.h>
#include<iostream>
#include <arpa/inet.h>
#include <string>
#include <sys/socket.h>
#include<sys/select.h>
#include <unistd.h>
using namespace std;

int main(int argc, char *argv[]){

	int PORT = stoi(argv[2]);

	int status, valread, clientSocket;
	struct sockaddr_in serv_addr;

	serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(PORT);

	char hello[20] = "Hello from client";
	char buffer[1024] = { 0 };
	if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return 1;
	}
	else{
		cout<<"Socket created"<<endl;
	}

	if ((status
		= connect(clientSocket, (struct sockaddr*)&serv_addr,
				sizeof(serv_addr)))
		< 0) {
		cout<<"\nConnection Failed \n";
		return -1;
	}
	else{
		cout<<"Connected Successfully"<<endl;
	}
	
	while(true){
		string message;
		cout << "Client: ";
		getline(cin, message);
		
		if (message == "/exit"){
			break;
		}

		send(clientSocket, message.c_str(), message.length(), 0);

		char ackBuffer[1024];
		int bytesReceived = recv(clientSocket, ackBuffer, sizeof(ackBuffer), 0);
		if (bytesReceived > 0) {
			ackBuffer[bytesReceived] = '\0';
			cout << "From server: " << ackBuffer << endl;
		}
	}
	close(clientSocket);

	return 0;
}


