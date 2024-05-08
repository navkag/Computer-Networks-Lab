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

void handleClient(int clientSocket, int PORT, char *IP, int serverPORT);

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
        cout<<"Bind failed!\n"<<endl;
        return 1;
    }
    else{
        // cout<<"Bound successfully.\n"<<endl;
    }
  

    if (listen(serverSocket, 3) < 0) {
        cout<<"Listening error!\n"<<endl;
        return 1;
    }
    else{
        cout<<"Server listening on port " << PORT << "\n\n";
    }

    vector<thread> clientThreads;

    while(true){
        struct sockaddr_in clientAddress;
        int addrlen = sizeof(clientAddress);
        char clientIp[INET_ADDRSTRLEN];
        int client_port;
        if ((clientSocket
            = accept(serverSocket, (struct sockaddr*)&clientAddress,
                    (socklen_t*)&addrlen))
            < 0) {
            cout<<"Error in accepting"<<endl;
            return 1;
        }
        else{ 
            inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIp, INET_ADDRSTRLEN);
            int client_port = ntohs(clientAddress.sin_port);
            cout<<"Client " << clientIp << " on port " << client_port << " connected."<< "\n\n";
        }
        client_port = ntohs(clientAddress.sin_port);
        clientThreads.emplace_back(handleClient, clientSocket, client_port, clientIp, PORT);
    }

    for (auto &thread : clientThreads) {
        thread.join(); // Wait for all threads to finish
    }

    close(serverSocket);

    return 0;
}


void handleClient(int clientSocket, int PORT, char *IP, int serverPORT){
    char buffer[1024];
    while (true){
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0){
            cout <<"Client " << IP << " on port " << PORT << " disconnected."<< "\n\n";
            cout << "Server listening on port " << serverPORT << "\n\n";
            break;
        }
        // cout << "Client " << clientSocket  << " connected!" << endl;
        buffer[bytesReceived] = '\0';

        cout << "Received from client: " << buffer << endl;

        int n = bytesReceived;
        vector<string> splits;

        int i = 0;

        string word = "";
        while (i < n){
            if (buffer[i] != ' ')
                word += buffer[i];
            else{
                splits.push_back(word);
                word = "";
            }
            i++;
        }
        splits.push_back(word);
        string message = "AAAAA";
        if (splits[1] == "+")
            message = to_string(stof(splits[0]) + stof(splits[2]));
        else if (splits[1] == "-")
            message = to_string(stof(splits[0]) - stof(splits[2]));
        else if (splits[1] == "/")
            message = to_string(stof(splits[0]) / stof(splits[2]));
        else if (splits[1] == "*")
            message = to_string(stof(splits[0]) * stof(splits[2]));
        else if (splits[1] == "^")
            message = to_string(pow(stof(splits[0]), stof(splits[2])));
        else
            message = "Invalid";

        // Process the received data and send a response
        cout << "Sending to client: " << message << "\n\n";
        send(clientSocket, message.c_str(), message.length(),0);
    }
    close(clientSocket);
}