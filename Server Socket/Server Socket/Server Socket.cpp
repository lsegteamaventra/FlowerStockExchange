// Server Socket.cpp : This file contains the 'main' function. Program execution begins and ends there.


#include <winsock.h>
#include <iostream>
#include <queue>
#include <string> 
#include <vector> 
#include <fstream> 
#include <sstream> 
#include <ctime>
#include <chrono>
#include <iomanip>

using namespace  std;
#define PORT 9909
struct sockaddr_in srv;
fd_set fr, fw, fe;
int nSocket;
int nArrClient[5];
int i = 1;


std::string getCurrentFormattedTime() {

    // Get the current system time
    auto currentTime = std::chrono::system_clock::now();

    // Convert the current time to time_t
    std::time_t currentTimeT = std::chrono::system_clock::to_time_t(currentTime);

    // Convert time_t to tm struct (local time)
    std::tm localTimeStruct;
    localtime_s(&localTimeStruct, &currentTimeT);

    // Get milliseconds
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime.time_since_epoch() % std::chrono::seconds(1));

    // Format the time as YYYYMMDDHHMMSS.sss
    std::ostringstream formattedTime;
    formattedTime << std::put_time(&localTimeStruct, "%Y%m%d-%H%M%S") << "." << std::setfill('0') << std::setw(3) << ms.count();

    return formattedTime.str();
}


// Define a custom comparison function for the priority queue
auto compare = [](const std::vector<string>& v1, const std::vector<string>& v2) {
    // for buying
    if (v1[3] == "1") {
        if (v1[5] == v2[5]) {
            return stoi(v1[0]) > stoi(v2[0]); // Compare based on item number
        }
        else {
            return stoi(v1[5]) < stoi(v2[5]);
        }
    }
    // for selling
    else if ((v1[3] == "2")) {
        if (v1[5] == v2[5]) {
            return stoi(v1[0]) > stoi(v2[0]); // Compare based on item number

        }
        else {
            return stoi(v1[5]) > stoi(v2[5]);
        }
    }
    else {
        // display error
    }
};

// Variable Initialization

std::vector<std::vector<std::string>> execution_rep = { {"Order ID", "Client Order ID" , "Instrument", "Side", "Exec Status" ,"Quantity", "Price", "Reason", "Transaction Time"} }; // Vector of vectors to hold execution data
std::vector<std::string> updated_record_buy, updated_record_sell, execution_buy, execution_sell, execution_error;
int verify, error = 0;
// Initialize a priority queue of vectors using the custom comparison function
std::priority_queue<std::vector<std::string>,
    std::vector<std::vector<std::string>>,
    decltype(compare)
>   rpqb(compare), rpqs(compare), lapqb(compare), lapqs(compare), lopqb(compare), lopqs(compare), tupqb(compare), tupqs(compare), orpqb(compare), orpqs(compare);




void writeCSV(const std::vector<std::vector<std::string>>& data, const std::string& filename) {
    std::ofstream file(filename);

    if (file.is_open()) {
        for (const auto& row : data) {
            for (const auto& cell : row) {
                file << cell;
                if (&cell != &row.back()) {
                    file << ",";
                }
            }
            file << "\n";
        }
        file.close();
        std::cout << "CSV file '" << filename << "' created successfully." << std::endl;
    }
    else {
        std::cerr << "Unable to create CSV file." << std::endl;
    }
}

void FindError(vector<std::string> row)
{
    // find error
    if (row.size()<6) {  // when there is no price, it nt return as "", it will stop without adding ""
        execution_error = { "ord" + row[0],row[1], row[2], row[3], "Reject", row[4], "", "Invalid price"};
        execution_rep.push_back(execution_error);
        error = 1;

    }
    else {
    
        int value;
        try {
             value = std::stoi(row[5]);
        }
        catch (const std::invalid_argument& e) {
            execution_error = { "ord" + row[0],row[1], row[2], row[3], "Reject", row[4], "", "Invalid price" };
            execution_rep.push_back(execution_error);
            error = 1;

        }
        try {
             value = std::stoi(row[4]);
        }
        catch (const std::invalid_argument& e) {
            execution_error = { "ord" + row[0],row[1], row[2], row[3], "Reject", row[4], row[5], "Invalid Size" };
            execution_rep.push_back(execution_error);

            error = 1;

        }
        try {
            value = std::stoi(row[3]);
        }
        catch (const std::invalid_argument& e) {
            execution_error = { "ord" + row[0],row[1], row[2], row[3], "Reject", row[4], row[5], "Invalid side" };
            execution_rep.push_back(execution_error);

            error = 1;

        }
        

    if (row[1] == "") {
    // display 
    execution_error = { "ord" + row[0],row[1], row[2], row[3], "Reject", row[4], row[5], "Invalid client order id" };
    execution_rep.push_back(execution_error);
    error = 1;

    }
    else if (row[2] == "") {
    execution_error = { "ord" + row[0],row[1], row[2], row[3], "Reject", row[4], row[5], "Invalid instrument" };
    execution_rep.push_back(execution_error);
    error = 1;

    }
   
    else if (row[4] == "") {
    execution_error = { "ord" + row[0],row[1], row[2], row[3], "Reject", row[4], row[5], "Invalid Size" };
    execution_rep.push_back(execution_error);
    error = 1;

    }


    else if (stoi(row[4]) % 10 != 0) {
    execution_error = { "ord" + row[0],row[1], row[2], row[3], "Reject", row[4], row[5], "Invalide Size" };
    execution_rep.push_back(execution_error);
    error = 1;

    }
    else if (stoi(row[4]) < 9 ) {
        execution_error = { "ord" + row[0],row[1], row[2], row[3], "Reject", row[4], row[5], "Invalide Size" };
        execution_rep.push_back(execution_error);
        error = 1;
    }
    else if ( stoi(row[4]) > 1001) {
        execution_error = { "ord" + row[0],row[1], row[2], row[3], "Reject", row[4], row[5], "Invalide Size" };
        execution_rep.push_back(execution_error);
        error = 1;
    }
    else if (stoi(row[5]) <= 0) {
    execution_error = { "ord" + row[0],row[1], row[2], row[3], "Reject", row[4], row[5], "Invalid Price" };
    execution_rep.push_back(execution_error);
    error = 1;



    }
    
    }
    
    

}

void BuyOrder(std::priority_queue<std::vector<std::string>,
    std::vector<std::vector<std::string>>,
    decltype(compare) >& buy_table, std::priority_queue<std::vector<std::string>,
    std::vector<std::vector<std::string>>,
    decltype(compare) >& sell_table, vector<std::string> row)
{
    if (!sell_table.empty()) {
        if (stoi(row[5]) >= stoi(sell_table.top()[5])) {
            while ((stoi(row[5]) >= stoi(sell_table.top()[5]))) {
                if (stoi(row[4]) >= stoi(sell_table.top()[4])) 
                {
                    verify = 1;
                    execution_sell = { "ord" + sell_table.top()[0],sell_table.top()[1],sell_table.top()[2],  sell_table.top()[3], "Fill" ,sell_table.top()[4], sell_table.top()[5], "-" };
                    updated_record_buy = { row[0], row[1], row[2], row[3], to_string(stoi(row[4]) - stoi(sell_table.top()[4])), row[5],"-" };
                    if (stoi(row[4]) - stoi(sell_table.top()[4]) == 0) {
                        execution_buy = { "ord" + row[0],row[1], row[2],row[3], "Fill" , sell_table.top()[4], sell_table.top()[5],"-" };
                    }
                    else {
                        execution_buy = { "ord" + row[0],row[1], row[2], row[3], "Pfill" , sell_table.top()[4], sell_table.top()[5], "-" };
                    }
                    execution_rep.push_back(execution_buy);
                    execution_rep.push_back(execution_sell);
                    row[4] = to_string(stoi(row[4]) - stoi(sell_table.top()[4]));
                    if (row[4] == "0") { break; }

                    sell_table.pop();
                }
                else {
                    updated_record_sell = { sell_table.top()[0], sell_table.top()[1], sell_table.top()[2], sell_table.top()[3], to_string(stoi(sell_table.top()[4]) - stoi(row[4])), sell_table.top()[5] };
                    
                    sell_table.pop();
                    sell_table.push(updated_record_sell);
                    execution_buy = { "ord" + row[0],row[1], row[2], row[3], "Fill" , row[4], sell_table.top()[5], "-" };
                    execution_sell = { "ord" + sell_table.top()[0],sell_table.top()[1],sell_table.top()[2], sell_table.top()[3], "Pfill" , row[4], sell_table.top()[5], "-"};
                    execution_rep.push_back(execution_buy);
                    execution_rep.push_back(execution_sell);
                    break;
                }
                // if the row is empty	
                if (sell_table.empty()) {
                    break;
                }
            } // while loop ends
            if (verify == 1) {
                if (row[4] != "0") {
                    buy_table.push(updated_record_buy);
                }
                verify = 0;
            }
        }
        else {
            buy_table.push(row);
            execution_buy = { "ord" + row[0],row[1],row[2], row[3], "New" , row[4], row[5], "-" };
            execution_rep.push_back(execution_buy);
       
        }
    }
    else {
        buy_table.push(row);
        execution_buy = { "ord" + row[0],row[1],row[2], row[3], "New" , row[4], row[5],"-" };
        execution_rep.push_back(execution_buy);
      
    }


}

void SellOrder(std::priority_queue<std::vector<std::string>,
    std::vector<std::vector<std::string>>,
    decltype(compare) >& buy_table, std::priority_queue<std::vector<std::string>,
    std::vector<std::vector<std::string>>,
    decltype(compare) >& sell_table, vector<std::string> row)
{

    if (!buy_table.empty()) {
        if (stoi(row[5]) <= 0) {
            // Display
            execution_error = { "ord" + row[0],row[1], row[2], row[3], "Reject", row[4], row[5], "Invalid Price" };
            execution_rep.push_back(execution_error);
        }
        else if (stoi(row[5]) <= stoi(buy_table.top()[5])) {
            while ((stoi(row[5]) <= stoi(buy_table.top()[5]))) {
                if (stoi(row[4]) >= stoi(buy_table.top()[4])) {
                    verify = 1;
                    execution_buy = { "ord" + buy_table.top()[0],buy_table.top()[1],buy_table.top()[2], buy_table.top()[3], "Fill" , buy_table.top()[4], buy_table.top()[5],"-" };

                    updated_record_sell = { row[0], row[1], row[2], row[3], to_string(stoi(row[4]) - stoi(buy_table.top()[4])), row[5] };

                    if (stoi(row[4]) - stoi(buy_table.top()[4]) == 0) {
                        execution_sell = { "ord" + row[0],row[1], row[2], row[3], "Fill" , buy_table.top()[4], buy_table.top()[5] ,"-" };
                    }
                    else {
                        execution_sell = { "ord" + row[0],row[1], row[2], row[3], "Pfill" , buy_table.top()[4], buy_table.top()[5], "-" };
                    }
                    execution_rep.push_back(execution_sell);
                    execution_rep.push_back(execution_buy);
                    row[4] = to_string(stoi(row[4]) - stoi(buy_table.top()[4]));
                    if (row[4] == "0") { break; }
                    buy_table.pop();
                }
                else {
                    updated_record_buy = { buy_table.top()[0], buy_table.top()[1], buy_table.top()[2], buy_table.top()[3], to_string(stoi(buy_table.top()[4]) - stoi(row[4])), buy_table.top()[5] };
                    buy_table.pop();
                    buy_table.push(updated_record_buy);
                    execution_sell = { "ord" + row[0],row[1], row[2], row[3], "Fill" , row[4], buy_table.top()[5] ,"-" };
                    execution_buy = { "ord" + buy_table.top()[0],buy_table.top()[1],buy_table.top()[2], buy_table.top()[3], "Pfill" , row[4], buy_table.top()[5], "-" };
                    execution_rep.push_back(execution_sell);
                    execution_rep.push_back(execution_buy);
                    break;

                }
                // if the row is empty	
                if (buy_table.empty()) {
                    break;
                }
            } // while loop ends
            if (verify == 1) {
                if (row[4] != "0") {
                    sell_table.push(updated_record_sell);
                }
                verify = 0;
            }
        }
        else {
            sell_table.push(row);
            execution_sell = { "ord" + row[0],row[1],row[2], row[3], "New" , row[4], row[5] ,"-" };
            execution_rep.push_back(execution_sell);
        }
    }
    else {
        sell_table.push(row);
        execution_sell = { "ord" + row[0],row[1],row[2], row[3], "New" , row[4], row[5] ,"-" };
        execution_rep.push_back(execution_sell);
    }


}


void ServerExecution(vector<std::string> row, int nClientSocket)
{
   

    if ( row[1] != "create_csv" )
    {
        // checking for any error
        FindError(row);

        // deleting the decimal points in price
        if (error == 0) {
            if (row[5].size() >= 3) {
                std::string result = row[5].substr(0, row[5].size() - 3);
                row[5] = result;
                std::cout << "Modified string: " << result << std::endl;
            }
            else {
                std::cout << "Input string is too short to remove 3 characters." << std::endl;
                execution_error = { "ord" + row[0],row[1], row[2], row[3], "Reject", row[4], "", "Invalid price" };
                execution_rep.push_back(execution_error);
                error = 1;
            }
        }
        // work

        if (error == 0) {
            if (row[3] == "1") {
                if (row[2] == "Rose") {
                    BuyOrder(rpqb, rpqs,row);
                }
                else if (row[2] == "Lavender") {
                    BuyOrder(lapqb, lapqs, row);
                }
                else if (row[2] == "Lotus") {
                    BuyOrder(lopqb, lopqs, row);

                }
                else if (row[2] == "Tulip") {
                    BuyOrder(tupqb, tupqs, row);
                }
                else if (row[2] == "Orchid") {
                    BuyOrder(orpqb, orpqs, row);
                }
                else {
                    execution_error = { "ord" + row[0],row[1], row[2], row[3], "Reject", row[4], row[5], "Invalid Instrument" };
                    execution_rep.push_back(execution_error);
                }
            }
            else if (row[3] == "2") {
                if (row[2] == "Rose") {
                    SellOrder(rpqb, rpqs, row);


                }
                else if (row[2] == "Lavender") {
                    SellOrder(lapqb, lapqs, row);

                }
                else if (row[2] == "Lotus") {
                    SellOrder(lopqb, lopqs, row);

                }
                else if (row[2] == "Tulip") {
                    SellOrder(tupqb, tupqs, row);

                }
                else if (row[2] == "Orchid") {
                    SellOrder(orpqb, orpqs, row);

                }
                else {
                    execution_error = { "ord" + row[0],row[1], row[2], row[3], "Reject", row[4], row[5], "Invalid Instrument" };
                    execution_rep.push_back(execution_error);
                }
            }

            else {

                // display 
                execution_error = { "ord" + row[0],row[1], row[2], row[3], "Reject", row[4], row[5], "Invalid Side" };
                execution_rep.push_back(execution_error);

            }
        }
      
        error = 0;


        std::string end_time = getCurrentFormattedTime();
        execution_rep.back().push_back(end_time);
        send(nClientSocket, "Transaction Success", 20, 0);



    }
   
    else if (row[1] == "create_csv")
    {
        // send whole the execution table
        writeCSV(execution_rep, "Output.csv");
    }
}



void ProcessNewMessage(int nClientSocket)
{
    cout << endl << "Processing the new message for client socket: " << nClientSocket;
    char buff[256 + 1] = { 0, };
    int nRet = recv(nClientSocket, buff, 256, 0);
    if (nRet < 0)
    {
        cout << endl << "Something wrong happened.. closing the connection for client";
        closesocket(nClientSocket);
        for (int nIndex = 0; nIndex < 5; nIndex++)
        {
            if (nArrClient[nIndex] == nClientSocket)
            {
                nArrClient[nIndex] = 0;
                break;

            }
        }
    }
    else
    {
        cout << endl << "The message received from client is: " << buff;
        // Send the response to client

        char delimiter = ',';
        std::vector<std::string> row;

        std::istringstream ss(buff);
        std::string item;


        while (std::getline(ss, item, ',')) {
            row.push_back(item);
        }
        // adding order number
        row.insert(row.begin(), to_string(i));
        // increasing order number
        i++;


        ServerExecution(row, nClientSocket);

        // update the buff to execution report status

         //
        /*send(nClientSocket, buff, 50, 0);
        cout << endl << "***************************";*/
    }
}



void ProcessTheNewRequest() {
    // New connection Request
    if (FD_ISSET(nSocket, &fr)) {
        int nLen = sizeof(struct sockaddr);
        int nClientSocket = accept(nSocket, NULL, &nLen);
        if (nClientSocket > 0)
        {
            // Put it into the client fd_set.
            int nIndex;
            for (nIndex = 0; nIndex < 5; nIndex++)
            {
                if (nArrClient[nIndex] == 0)
                {
                    nArrClient[nIndex] = nClientSocket;
                    send(nClientSocket, "Got the connection done successfully", 37, 0);
                    break;
                }

            }
            if (nIndex == 5)
            {
                cout << endl << "No space for a new connection";
            }
        }
    }
    else
    {
        for (int nIndex = 0; nIndex < 5; nIndex++)
        {
            if (FD_ISSET(nArrClient[nIndex], &fr))
            {

                // Got the new message from the client
                //just recv new message
                // just queue that for new works of your server to fullfil the req
                ProcessNewMessage(nArrClient[nIndex]);
            }

        }
    }

}



int main()
{
    int nMaxFd;
    int nRet = 0;
    // Initialize the WSA variables
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0) {
        cout << endl << "WSA Failed to initialize";
        WSACleanup();
        exit(EXIT_FAILURE);

    }
    else {
        cout << endl << "WSA initialized";

    }
    // initialize the socket 
    nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (nSocket < 0) {
        cout << endl << "The socket is not opened";
        WSACleanup();

        exit(EXIT_FAILURE);

    }

    else {
        cout << endl << "The socket is opened successfully" << nSocket;

    }
    // initialize the environment for sockaddr structure
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    srv.sin_addr.s_addr = INADDR_ANY;
    memset(&(srv.sin_zero), 0, 8);
    //about the blocking
    // optval = 0 means blocking and != 0 means non blocking
    u_long optval = 1;
    nRet = ioctlsocket(nSocket, FIONBIO, &optval);
    if (nRet != 0)
    {
        cout << endl << "ioctlsocket call failed";
    }
    else {
        cout << endl << "ioctlsocket call passed";

    }

    //setsockopt

    int nOptVal = 0;
    int nOptLen = sizeof(nOptVal);
    nRet = setsockopt(nSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&nOptVal, nOptLen);
    if (!nRet) {
        cout << endl << "The setsockopt call successful.";
    }
    else {
        cout << endl << "The setsockopt call failed.";
        WSACleanup();
        exit(EXIT_FAILURE);



    }

    //bind the socket to the local port
    nRet = bind(nSocket, (sockaddr*)&srv, sizeof(sockaddr));
    if (nRet < 0) {
        cout << endl << "Fail to bind to local port";
        WSACleanup();

        exit(EXIT_FAILURE);
    }
    else {
        cout << endl << "Successfully bind to local port";
    }

    // lis ten the request from client (queues the requests)

    nRet = listen(nSocket, 5);
    if (nRet < 0) {
        cout << endl << "Fail to start listen to local port";
        exit(EXIT_FAILURE);
    }
    else {
        cout << endl << "Started listening to local port";
    }


    nMaxFd = nSocket;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;




    while (1) {

        FD_ZERO(&fr);
        FD_ZERO(&fw);
        FD_ZERO(&fe);

        FD_SET(nSocket, &fr);
        FD_SET(nSocket, &fe);

        for (int nIndex = 0; nIndex < 5; nIndex++)
        {
            if (nArrClient[nIndex] != 0)
            {
                FD_SET(nArrClient[nIndex], &fr);
                FD_SET(nArrClient[nIndex], &fe);


            }
        }
        // keep waiting for new requests and proceed as per the request
        nRet = select(nMaxFd + 1, &fr, &fw, &fe, &tv);

        if (nRet > 0) {
            // when someone connect or communicats with a message over 
                // a dedicated connection
            cout << endl << "Data on port... Processing now...";
            // Process the request

            ProcessTheNewRequest();

        }
        else if (nRet == 0) {
            // no connection or any communication request made or you can 
                // say that none of the socket descriptors are ready
           // cout << endl << "Nothing on port : " << PORT;
        }
        else {
            // it failed and your application should show some useful message
            //cout << endl << "I Failed..";
            WSACleanup();

            exit(EXIT_FAILURE);
        }


    }
}