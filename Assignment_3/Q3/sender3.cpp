#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
using namespace std;

#define NUM_FRAMES 10

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <IP_ADDR> <Server_Port_Number>\n", argv[0]);
        return 1;
    }

    int client, y;
    char buffer[1024];

    struct sockaddr_in server;
    socklen_t addrSize; // type definition for length and size values used by socket programming
    
    client = socket(AF_INET, SOCK_DGRAM, 0); //UDP
    if (client == -1)
    {
        cout << "SOCKET CREATION FAILURE." << endl;
        exit(1);
    }

    int PORT = atoi(argv[2]);
    server.sin_family = AF_INET;                              // address family
    server.sin_port = htons(PORT);                            // port in network byte order
    server.sin_addr.s_addr = inet_addr(argv[1]);          //
    memset(server.sin_zero, '\0', sizeof(server.sin_zero)); // clear the contiguous memory blocks

    int frame = 1;
    while (frame <= NUM_FRAMES)
    {
        // 30% probab of corrupt
        int num = rand() % 10;
        int count = 0;
        if (num == 0 || num == 1 || num == 2)
            strcpy(buffer, "error");
        else
        {
            strcpy(buffer, "frame");
            count++;
        }

        int x = sendto(client, buffer, strlen(buffer),0,(struct sockaddr *) &server,sizeof(server));
        if (x == -1)
        {
            cout << "Error in sending." << endl;
            exit(1);
        }
        else
        {
            cout << "Sending Frame " << frame;
            if (frame < 10)
                cout << " \t\t";
            else
                cout << "\t\t";

            while (1)
            {
                socklen_t len=sizeof(server);
                int z = recvfrom(client, buffer, 50,MSG_WAITALL,(struct sockaddr *) &server,&len); // recv function
                if (z == -1)
                {
                    cout << "Error in receiving data" << endl;
                    exit(1);
                }
                else
                {
                    if (strncmp(buffer, "ACK", 3) == 0)
                    {
                        cout << "Acknowledgment received for Frame " << frame << endl;
                        break;
                    }
                    else
                    {
                        cout << "Acknowledgment lost or corrupted. Retransmitting Frame " << frame <<"." << endl;
                        cout << "Sending Frame " << frame;
                        if (frame < 10)
                            cout << " \t\t";
                        else
                            cout << "\t\t";

                        // 30% probab of corrupt
                        int num = rand() % 10;
                        if (num == 0 || num == 1 || num == 2)
                            strcpy(buffer, "error");
                        else
                        {
                            strcpy(buffer, "frame");
                            count++;
                        }

                        int x = sendto(client, buffer, strlen(buffer),0,(struct sockaddr *) &server,sizeof(server));
                        if (x == -1)
                        {
                            cout << "Error in sending." << endl;
                            exit(1);
                        }
                    }
                }
            }
        }

        if (count > 0)
        {
            frame++;
            count = 0;
        }
    }
    close(client);
}