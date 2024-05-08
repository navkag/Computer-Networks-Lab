#include <bits/stdc++.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <vector>
#include <thread>
using namespace std;

void handleClient(int clientSocket);

int main(int argc, char *argv[]){

    int PORT = stoi(argv[1]);

    int serverSocket, clientSocket, valread;
    struct sockaddr_in server_address;
    // int opt = 1;
    char buffer[1024] = { 0 };
    // char hello[10] = "Server";
  
    // Creating socket file descriptor
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        cout<<"Error making server socket"<<endl;
        return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);
  
    //Binding to the server
    int nbind = ::bind(serverSocket, (sockaddr*)&server_address, sizeof(server_address));
    if (nbind == -1) {
        cout<<"Bind failed"<<endl;
        return 1;
    }
    else{
        cout<<"Bounded successfully"<<endl;
    }
  

    if (listen(serverSocket, 3) < 0) {
        cout<<"Listening error"<<endl;
        return 1;
    }
    else{
        cout<<"Listening"<<endl;
    }

    vector<thread> clientThreads;

    while(true){
        struct sockaddr_in clientAddress;
        int addrlen = sizeof(clientAddress);
        if ((clientSocket
            = accept(serverSocket, (struct sockaddr*)&clientAddress,
                    (socklen_t*)&addrlen))
            < 0) {
            cout<<"Error in accepting"<<endl;
            return 1;
        }
        else{
            cout<<"Accepted connection from client: " << clientSocket <<endl;
        }

        clientThreads.emplace_back(handleClient, clientSocket);
    }

    for (auto &thread : clientThreads) {
        thread.join(); // Wait for all threads to finish
    }

    close(serverSocket);

    return 0;
}


void handleClient(int clientSocket){
    char buffer[1024];
    while (true){
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0){
            cerr << "Client " << clientSocket << " disconnected!" << endl;
            break;
        }
        // cout << "Client " << clientSocket << " connected!" << endl;
        buffer[bytesReceived] = '\0';

        cout << "From Client " << clientSocket << ": " << buffer << endl;

        // Process the received data and send a response
        cout << "Server: ";
        string message;
        getline(cin, message);
        send(clientSocket, message.c_str(), message.length(),0);
    }
    close(clientSocket);
}