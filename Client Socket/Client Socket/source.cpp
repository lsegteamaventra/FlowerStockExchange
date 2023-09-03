#include <winsock.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

#define PORT 9909

int nClientSocket;
struct sockaddr_in srv;
int i = 1;

int main()
{





    int nRet = 0;
    // Initialize the WSA variables
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0) {
        cout << endl << "WSA Failed to initialize";
        WSACleanup();
        exit(EXIT_FAILURE);

    }

    // initialize the socket 
    nClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (nClientSocket < 0) {
        cout << endl << "The socket call failed";
        WSACleanup();

        exit(EXIT_FAILURE);

    }


    // initialize the environment for sockaddr structure
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    srv.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(&(srv.sin_zero), 0, 8);
    //about the blocking
    // optval = 0 means blocking and != 0 means non blocking
    nRet = connect(nClientSocket, (struct sockaddr*)&srv, sizeof(srv));
    if (nRet < 0)
    {
        cout << endl << "connect failed";
        WSACleanup();

        exit(EXIT_FAILURE);

    }
    else {
        cout << endl << "connected to the server";
        char buff[255] = { 0, };
        // Talk to the server
        recv(nClientSocket, buff, 255, 0);
        cout << endl << "Just press any key to send";
        getchar();

        FILE* csvFile;
        if (fopen_s(&csvFile, "../order.csv", "r") != 0) {
            perror("Error opening file");
            return 1;
        }
        // to remove the heading part of csv file
        fgets(buff, sizeof(buff), csvFile);


        while (fgets(buff, sizeof(buff), csvFile) != nullptr) {
            // Remove the newline character if it exists
            size_t len = strlen(buff);
            if (len > 0 && buff[len - 1] == '\n') {
                buff[len - 1] = '\0';
            }

            // Print the read line
            std::cout << "Line read: " << buff << std::endl;

            send(nClientSocket, buff, sizeof(buff), 0);
            cout << buff << endl;



            recv(nClientSocket, buff, 255, 0);
            cout << buff << endl;
        }



        // Sent all the data
        std::stringstream stream;
        stream << "end";
        std::string data = stream.str();
        send(nClientSocket, data.c_str(), data.size(), 0);
        cout << "end" << endl;
        //  Acknowledgement from server
        recv(nClientSocket, buff, 255, 0);
        cout << buff << endl;

        // execution done, asking for execution csv
        stream.str(""); // empty the stream
        stream << "create_csv,";
        data = stream.str();
        send(nClientSocket, data.c_str(), data.size(), 0);
        cout << "create_csv" << endl;

        //receiving from server
        ///* while (buff != "sent") {

        //     recv(nClientSocket, buff, 255, 0);
        //     cout << buff << endl;
        //     send(nClientSocket, data.c_str(), data.size(), 0);
        //     cout << "send_execution" << endl;

        // }*/
        // recv(nClientSocket, buff, 255, 0);
        // cout << buff << endl;
        // stream.str(""); // empty the stream
        // stream << "ok";
        // while (buff != "sent") {
        //     recv(nClientSocket, buff, 255, 0);
        //     cout << buff << endl;
        //     send(nClientSocket, data.c_str(), data.size(), 0);
        //     cout << "ok" << endl;
        // }
        // 

        fclose(csvFile);  // Close the file


    }


}

