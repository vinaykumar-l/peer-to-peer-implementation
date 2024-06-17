// Include necessary libraries for input/output, file handling, and network operations
#include <iostream>     // For input and output operations (std::cout, std::cerr)
#include <fstream>      // For file handling operations (std::ofstream)
#include <cstring>      // For C-style string functions (std::strncpy)
#include <sys/socket.h> // For socket functions
#include <arpa/inet.h>  // For inet_pton function (to convert IP addresses)
#include <unistd.h>     // For close function (to close sockets)

// Constants for client settings
const int PORT = 8080;        // The port number to connect to the server
const int BUFFER_SIZE = 1024; // The size of the buffer for receiving data

// Function to receive a file from the server
void receiveFile(int socket, const std::string &fileName)
{
    // Open the specified file in binary mode for writing
    std::ofstream file(fileName, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file to write: " << fileName << std::endl;
        close(socket);
        return;
    }

    char buffer[BUFFER_SIZE];
    int bytesReceived;

    // Receive data from the server and write it to the file
    while ((bytesReceived = recv(socket, buffer, BUFFER_SIZE, 0)) > 0)
    {
        file.write(buffer, bytesReceived);
    }

    // Close the file and the socket after receiving the file
    file.close();
    close(socket);
    std::cout << "File received successfully: " << fileName << std::endl;
}

int main(int argc, char *argv[])
{
    // Check if the user provided the correct number of arguments
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <Server IP> <File Name>" << std::endl;
        return -1;
    }

    // Get the server IP address and file name from the command line arguments
    const char *serverIp = argv[1];
    std::string fileName = "/home/vinaykumar/Documents/vinay/" + std::string(argv[2]);

    int clientSocket;
    struct sockaddr_in serverAddr;

    // Create a TCP socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        std::cerr << "Error: Failed to create socket" << std::endl;
        return -1;
    }

    // Set up the server address structure
    serverAddr.sin_family = AF_INET;                    // Use IPv4
    serverAddr.sin_port = htons(PORT);                  // Set the port number
    inet_pton(AF_INET, serverIp, &serverAddr.sin_addr); // Convert IP address from text to binary form

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cerr << "Error: Connection failed" << std::endl;
        close(clientSocket);
        return -1;
    }

    // Send the file name to the server
    send(clientSocket, fileName.c_str(), fileName.size(), 0);

    // Receive the requested file from the server
    receiveFile(clientSocket, fileName);

    return 0;
}
