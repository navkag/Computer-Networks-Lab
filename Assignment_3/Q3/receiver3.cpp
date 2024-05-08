#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
using namespace std;


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <serverSocket_Port_Number>\n", argv[0]);
        return 1;
    }
    int PORT = atoi(argv[1]);

    int serverSocket, newSock, num = 0;

    struct sockaddr_in server, client;
    struct sockaddr_storage store; // accomodate all supported protocol specific address structure
    socklen_t addrSize;

    serverSocket = socket(AF_INET, SOCK_DGRAM, 0); // socket creation
    if (serverSocket == -1)
    {
        printf("\nSOCKET CREATION FAILURE\n");
        exit(0);
    }

    server.sin_family = AF_INET;                            // address family
    server.sin_port = htons(PORT);                          // port in network byte order
    server.sin_addr.s_addr = INADDR_ANY;                    // internet address
    memset(server.sin_zero, '\0', sizeof(server.sin_zero)); // clear the contiguous memory blocks

    // bind(serverSocket, (struct sockaddr *)&server, sizeof(server)); // binding
    int nbind = ::bind(serverSocket, (sockaddr *)&server, sizeof(server));
    if (nbind == -1)
    {
        cout << "Bind failed" << endl;
        return 1;
    }
    else
    {
        cout << "Bounded successfully" << endl;
    }

    int frame = 1;
    while (1)
    {
        sleep(1);
        char buffer[1024];
        socklen_t nlen = sizeof(struct sockaddr);
        ssize_t bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (sockaddr *)&client, &nlen);
        buffer[bytesReceived] = '\0';
        if (bytesReceived == -1)
        {
            cout << "Error in receiving." << endl;
            exit(1);
        }

        if (strncmp(buffer, "frame", 5) == 0)
        {
            cout << "Received packet " << frame << " successfully."; // comparing received frame
            if (frame < 10)
                cout << " \t";
            else
                cout << "\t";
            strcpy(buffer, "ACK");
            cout << " No Error in packet. Hence sending ACK" << endl;
            num++;
        }
        else
        {
            cout << "Received packet " << frame << " successfully."; // comparing received frame
            if (frame < 10)
                cout << " \t";
            else
                cout << "\t";
            strcpy(buffer, "NAK");
            cout << " Error in packet. Hence sending NAK" << endl;
        }

        int z = sendto(serverSocket, buffer, strlen(buffer), 0, (struct sockaddr *)&client, nlen); // send function
        if (z == -1)
        {
            printf("Error in sending.\n");
            exit(1);
        }

        if (num > 0)
        {
            frame++;
            num = 0;
        }
    }
    close(serverSocket); // socket close
}