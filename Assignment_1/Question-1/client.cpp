#include <bits/stdc++.h>
#include <iostream>
#include <arpa/inet.h>
#include <string>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
using namespace std;

#define MSG_LEN 1024
string base64Decoder(char encoded[], int len_str);
string base64Encoder(char encoded[], int len_str);

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
		cout << "Enter a message to send: ";
		getline(cin, message);
		int n = message.length();
		

		if (message[0] == '3')
			break;
		else if (message[0] == '1'){    // Regular Message or close communication server.
			// Encode and send this message to the server.
			char message_char[MSG_LEN];
			strcpy(message_char, message.c_str());
			string send_message = base64Encoder(message_char, n + 1);

			send(clientSocket, send_message.c_str(), send_message.length(), 0);
		}
		else{
			cout << "Invalid message type! Try again\n";
			continue;
		}

		// send(clientSocket, message.c_str(), message.length(), 0);

		char ackBuffer[MSG_LEN];
		int bytesReceived = recv(clientSocket, ackBuffer, sizeof(ackBuffer), 0);
		if (bytesReceived > 0) {
			ackBuffer[bytesReceived] = '\0';
			string received_msg = base64Decoder(ackBuffer, bytesReceived);
			cout << "Recieved: " << received_msg << endl;
		}
	}
	return 0;
}

string base64Decoder(char encoded[], int len_str){
	int SIZE = 100;
	char* decoded_string;
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
	int SIZE = 100;
	// Character set of base64 encoding scheme
	char char_set[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
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
