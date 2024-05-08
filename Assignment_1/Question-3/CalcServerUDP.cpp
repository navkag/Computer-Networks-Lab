#include <bits/stdc++.h>
#include <iostream>
#include <string.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <vector>
#include <thread>
using namespace std;

void handleClient(string message);

int main(int argc, char *argv[]){

    int PORT = stoi(argv[1]);

    int serverSocket, clientSocket, valread;
    struct sockaddr_in server_address;
    // int opt = 1;
    char buffer[1024];
    // char hello[10] = "Server";
  
    // Creating socket file descriptor
    if ((serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
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

    cout << "Server listening on port " << PORT << "\n\n";

    vector<thread> clientThreads;

    while(true){
        struct sockaddr_in clientAddress;
        socklen_t clientAddrLen = sizeof(clientAddress);

        int bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0,
                                     (struct sockaddr *)&clientAddress, &clientAddrLen);

        if (bytesReceived == -1) {
            std::cerr << "Error receiving data." << std::endl;
            continue;
        }

        buffer[bytesReceived] = '\0';
        

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddress.sin_addr, clientIP, sizeof(clientIP));
        int clientPort = ntohs(clientAddress.sin_port);

        if (buffer[0] == '-' && buffer[1] == '1' && buffer[2] == '\0'){
            cout << "Client " << clientIP << " on port " << clientPort << " closed connection." << "\n\n";
            continue;
        }

        cout << "Client " << clientIP << " on port " << clientPort << " connected."<< "\n\n";


        cout << "Received from client: " << buffer << endl;
        // Calculate the number.
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
    
        sendto(serverSocket, message.c_str(), message.size(), 0,
                (struct sockaddr *)&clientAddress, sizeof(clientAddress));
    }


    return 0;
}


