#include <bits/stdc++.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <vector>
#include <map>
#include <thread>
using namespace std;

map<int, int> m;
void handleClient(int clientSocket);
void close_server(int serverSocket);

int main(int argc, char *argv[]){

    int PORT = stoi(argv[1]);

    int serverSocket, clientSocket, valread;
    struct sockaddr_in server_address;
    char buffer[1024] = { 0 };
  
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
  

    if (listen(serverSocket, 10) < 0) {    // Max clients here is 10
        cout<<"Listening error"<<endl;
        return 1;
    }
    else{
        cout<<"Listening"<<endl;
    }

    thread(close_server, serverSocket).detach();
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
            m[clientSocket]++;
            cout<<"Accepted connection from client: " << clientSocket <<endl;
        }

        thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }
    close(serverSocket);

    return 0;
}


void handleClient(int clientSocket){
    char buffer[1024];
    while (true){
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0){
            m[clientSocket]--;
            cerr << "Client " << clientSocket << " disconnected!" << endl;
            break;
        }
        buffer[bytesReceived] = '\0';

        string temp = buffer;
        string response = "User: ";
        char c = clientSocket + '0';
        response += c;
        response += ": ";

        response += temp.substr(2);

        // DM code.
        int flag = buffer[0] - '0';
        if (flag <= 9 && flag >= 0){
            if (flag == clientSocket){
                string res1 = "You: ";
                res1 += temp.substr(2);
                send(flag, res1.c_str(), res1.size(), 0);
            }
            else{
                send(flag, response.c_str(), response.size(), 0);
            }
        }
        else if (buffer[0] == 'A'){
            for (auto it: m){
                if (it.first == clientSocket){
                    string res1 = "You: ";
                    res1 += temp.substr(2);
                    send(it.first, res1.c_str(), response.size(), 0);
                }
                else if (it.second > 0){
                    send(it.first, response.c_str(), response.size(), 0);
                }
            }
        }
        else{
            string s1 = "Invalid message type/client don't exist";
            send(clientSocket, s1.c_str(), s1.size(), 0);
        }
    }
    close(clientSocket);
}


void close_server(int serverSocket){
    string inpMessage;
    cin >> inpMessage;
    if (inpMessage == "/exit")
    {
        cout << "Exiting..." << endl;
        exit(0);
    }
}