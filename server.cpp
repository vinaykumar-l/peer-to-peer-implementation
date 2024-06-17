// Include necessary libraries for input/output, file handling, and network operations
#include <iostream>     // For input and output operations (std::cout, std::cerr)
#include <fstream>      // For file handling operations (std::ifstream)
#include <cstring>      // For C-style string functions (std::strncpy)
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in structure (for defining socket addresses)
#include <unistd.h>     // For close function (to close sockets)

// Constants for server settings
const int PORT = 8080;        // The port number the server will listen on
const int BUFFER_SIZE = 1024; // The size of the buffer for sending and receiving data

void sendFile(int clientSocket, const std::string &fileName)
{
    // Open the specified file in binary mode for reading
    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open())
    {
        // Print an error message if the file cannot be opened
        std::cerr << "Error: Failed to open file for reading: " << fileName << std::endl;
        // Close the connection with the client
        close(clientSocket);
        return;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesSent;

    // Read data from the file and send it to the client in chunks
    while (file.read(buffer, BUFFER_SIZE))
    {
        bytesSent = send(clientSocket, buffer, file.gcount(), 0);
        if (bytesSent == -1)
        {
            std::cerr << "Error: Failed to send data to client" << std::endl;
            file.close();
            close(clientSocket);
            return;
        }
    }

    // Check for errors or end-of-file condition after the loop
    if (!file.eof())
    {
        std::cerr << "Error: Failed to read file completely" << std::endl;
        file.close();
        close(clientSocket);
        return;
    }

    // Send any remaining data after reaching the end of the file
    if (file.gcount() > 0)
    {
        bytesSent = send(clientSocket, buffer, file.gcount(), 0);
        if (bytesSent == -1)
        {
            std::cerr << "Error: Failed to send final data to client" << std::endl;
            file.close();
            close(clientSocket);
            return;
        }
    }

    // Close the file and the connection with the client
    file.close();
    close(clientSocket);
    std::cout << "File sent successfully: " << fileName << std::endl;
}
int main()
{
    int serverSocket, clientSocket;            // Variables for server and client sockets
    struct sockaddr_in serverAddr, clientAddr; // Structures for storing address information
    socklen_t addrLen = sizeof(clientAddr);    // Variable for storing the length of the client's address
    char buffer[BUFFER_SIZE];                  // Buffer for storing data

    // Create a TCP socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        // Print an error message if the socket cannot be created
        std::cerr << "Error: Failed to create socket" << std::endl;
        return -1;
    }

    // Set up the server address structure
    serverAddr.sin_family = AF_INET;         // Use IPv4
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP address
    serverAddr.sin_port = htons(PORT);       // Set the port number

    // Bind the socket to the server address
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        // Print an error message if the binding fails
        std::cerr << "Error: Bind failed" << std::endl;
        close(serverSocket);
        return -1;
    }

    // Set the socket to listen for incoming connections
    if (listen(serverSocket, 3) < 0)
    {
        // Print an error message if the listening fails
        std::cerr << "Error: Listen failed" << std::endl;
        close(serverSocket);
        return -1;
    }

    // Print a message indicating that the server is ready to accept connections
    std::cout << "Server is listening on port " << PORT << std::endl;

    // Infinite loop to accept and handle incoming connections
    while (true)
    {
        // Accept a new connection from a client
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen);
        if (clientSocket < 0)
        {
            // Print an error message if the connection cannot be accepted
            std::cerr << "Error: Accept failed" << std::endl;
            continue; // Continue to the next iteration of the loop to accept another connection
        }

        // Print a message indicating that a connection has been accepted
        std::cout << "Connection accepted" << std::endl;

        // Clear the buffer
        std::memset(buffer, 0, BUFFER_SIZE);
        // Receive the file name from the client
        ssize_t bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived <= 0)
        {
            std::cerr << "Error: Failed to receive file name" << std::endl;
            close(clientSocket);
            continue;
        }
        // Convert the buffer to a string and store it in fileName
        std::string fileName(buffer, bytesReceived);

        // Send the requested file to the client
        sendFile(clientSocket, fileName);
    }

    // Close the server socket (not reached in this example)
    close(serverSocket);
    return 0;
}
