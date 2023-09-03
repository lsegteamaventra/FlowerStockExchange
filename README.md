# FlowerStockExchange

This project was developed as part of the "C++" workshop conducted by the London Stock Exchange Group (LSEG). The goal of this project is to create an application that simulates a stock exchange for different types of flowers, including Orchid, Rose, Lavender, and Tulip. Users can participate in this virtual stock exchange by buying or selling flower stocks.
Overview

Our Flower Stock Exchange application is built using C++ and employs socket programming to create a server-client mechanism. The core functionality of the application revolves around managing and processing buy and sell orders for various flower categories.

# Features

1. Order Processing: The application utilizes heap data structures and priority queues to efficiently process buy and sell orders. Two virtual tables, namely the BUY table and SELL table, are employed to manage these orders.
2. CSV Order Upload: Users can upload their order information in CSV format via the client interface. These orders are then transmitted to the server through virtual sockets for processing.
3. Real-time Execution: For each order entry, a corresponding execution entry is generated in real-time. This provides users with up-to-the-minute information on their orders.
4. exception Handling: The application is equipped to handle various exceptions, such as empty fields in the order CSV, invalid order sizes, and incorrect prices, ensuring a robust user experience.

 # Usage

To use the Flower Stock Exchange application, follow these steps:

1. Compile and Run: Compile the C++ code and run the server application on one terminal and the client application on another terminal.
2. Upload Orders: Users should upload their order details in CSV format via the client interface.
3. Processing: The server processes the incoming orders, matching buy and sell requests, and generating execution entries in real-time.
4. Execution CSV: After processing, the server returns an Execution CSV file to the users, providing them with the results of their orders.

# Getting Started

To get started with the Flower Stock Exchange application, you can follow these steps:

1. Clone this GitHub repository to your local machine.
2. Compile the C++ code for both the server and client components.
3. Run the server application on one terminal.
4. Run the client application on another terminal.
5. Upload your order CSV file using the client interface.
6. Monitor the order processing and execution entries in real-time.
7. Retrieve the Execution CSV file for your order results.

# Demo

https://github.com/lsegteamaventra/FlowerStockExchange/assets/110090880/a6036136-00cc-4dc2-bf43-f5bc9a494deb

# Contributors

Viruthshaan V. 
Vishagar A.    
