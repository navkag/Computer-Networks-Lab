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

#define MSG_LEN 1024

string base64Encoder(char input_str[], int len_str);
string base64Decoder(char encoded[], int len_str);

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
            cout<<"Accepted connection from client " << clientSocket <<endl;
        }


        // char buffer[1024];
        // int bytesReceived = recv(new_socket, buffer, sizeof(buffer), 0);
        // if (bytesReceived <= 0) {
        //     cout<<"Nothing recieved"<<endl;
        // }
        // buffer[bytesReceived] = '\0';
        // cout << "Received: " << buffer << endl;

        // string ackmessage = "Server acknowledgement";
        // send(new_socket,ackmessage.c_str(),ackmessage.length(),0);

        clientThreads.emplace_back(handleClient, clientSocket);
    }

    for (auto &thread : clientThreads) {
        thread.join(); // Wait for all threads to finish
    }

    close(serverSocket);

    return 0;
}


void handleClient(int clientSocket){
    char buffer[MSG_LEN];
    while (true){
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0){
            cerr << "Client " << clientSocket  << " disconnected!" << endl;
            break;
        }
        cout << "Client " << clientSocket << " connected!" << endl;
        buffer[bytesReceived] = '\0';
        string buffer_msg = base64Decoder(buffer, bytesReceived + 1);
        cout << "Received: " << buffer_msg.substr(2) << endl;

        // Process the received data and send a response
        string message = "Acknowledgement from server";
        // Encode the message and send the acknowledgement message to the client.
        char message_char[MSG_LEN];
		strcpy(message_char, message.c_str());
        string ack_msg = base64Encoder(message_char, message.length() + 1);
        send(clientSocket, ack_msg.c_str(), ack_msg.length(), 0);
    }
    close(clientSocket);
}


string base64Decoder(char encoded[], int len_str){
	char* decoded_string;
    int SIZE = MSG_LEN;
	decoded_string = (char*)malloc(sizeof(char) * SIZE);

	int i, j, k = 0;

	// stores the bitstream.
	int num = 0;

	// count_bits stores current
	// number of bits in num.
	int count_bits = 0;

	// selects 4 characters from
	// encoded string at a time.
	// find the position of each encoded
	// character in char_set and stores in num.
	for (i = 0; i < len_str; i += 4)
	{
		num = 0, count_bits = 0;
		for (j = 0; j < 4; j++)
		{
			
			// make space for 6 bits.
			if (encoded[i + j] != '=')
			{
				num = num << 6;
				count_bits += 6;
			}

			/* Finding the position of each encoded
			character in char_set
			and storing in "num", use OR
			'|' operator to store bits.*/

			// encoded[i + j] = 'E', 'E' - 'A' = 5
			// 'E' has 5th position in char_set.
			if (encoded[i + j] >= 'A' && encoded[i + j] <= 'Z')
				num = num | (encoded[i + j] - 'A');

			// encoded[i + j] = 'e', 'e' - 'a' = 5,
			// 5 + 26 = 31, 'e' has 31st position in char_set.
			else if (encoded[i + j] >= 'a' && encoded[i + j] <= 'z')
				num = num | (encoded[i + j] - 'a' + 26);

			// encoded[i + j] = '8', '8' - '0' = 8
			// 8 + 52 = 60, '8' has 60th position in char_set.
			else if (encoded[i + j] >= '0' && encoded[i + j] <= '9')
				num = num | (encoded[i + j] - '0' + 52);

			// '+' occurs in 62nd position in char_set.
			else if (encoded[i + j] == '+')
				num = num | 62;

			// '/' occurs in 63rd position in char_set.
			else if (encoded[i + j] == '/')
				num = num | 63;

			// ( str[i + j] == '=' ) remove 2 bits
			// to delete appended bits during encoding.
			else {
				num = num >> 2;
				count_bits -= 2;
			}
		}

		while (count_bits != 0)
		{
			count_bits -= 8;

			// 255 in binary is 11111111
			decoded_string[k++] = (num >> count_bits) & 255;
		}
	}

	// place NULL character to mark end of string.
	decoded_string[k] = '\0';
    string word = "";
    for (int i = 0; i < k; i++){
        word += decoded_string[i];
    }

	return word;
}

string base64Encoder(char input_str[], int len_str){

	// Character set of base64 encoding scheme
	char char_set[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int SIZE = MSG_LEN;
	// Resultant string
	char *res_str = (char *) malloc(SIZE * sizeof(char));
	
	int index, no_of_bits = 0, padding = 0, val = 0, count = 0, temp;
	int i, j, k = 0;
	
	// Loop takes 3 characters at a time from
	// input_str and stores it in val
	for (i = 0; i < len_str; i += 3)
		{
			val = 0, count = 0, no_of_bits = 0;

			for (j = i; j < len_str && j <= i + 2; j++)
			{
				// binary data of input_str is stored in val
				val = val << 8;
				
				// (A + 0 = A) stores character in val
				val = val | input_str[j];
				
				// calculates how many time loop
				// ran if "MEN" -> 3 otherwise "ON" -> 2
				count++;
			
			}

			no_of_bits = count * 8;

			// calculates how many "=" to append after res_str.
			padding = no_of_bits % 3;

			// extracts all bits from val (6 at a time)
			// and find the value of each block
			while (no_of_bits != 0)
			{
				// retrieve the value of each block
				if (no_of_bits >= 6)
				{
					temp = no_of_bits - 6;
					
					// binary of 63 is (111111) f
					index = (val >> temp) & 63;
					no_of_bits -= 6;		
				}
				else
				{
					temp = 6 - no_of_bits;
					
					// append zeros to right if bits are less than 6
					index = (val << temp) & 63;
					no_of_bits = 0;
				}
				res_str[k++] = char_set[index];
			}
	}

	// padding is done here
	for (i = 1; i <= padding; i++)
	{
		res_str[k++] = '=';
	}

	res_str[k] = '\0';
    string word = "";

    for (int i = 0; i < k; i++){
        word += res_str[i];
    }

	return word;

}