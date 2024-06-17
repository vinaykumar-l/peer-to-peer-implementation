#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <ifaddrs.h>

std::string getLocalIPAddress() {
    struct ifaddrs *ifAddrStruct = nullptr;
    struct ifaddrs *ifa = nullptr;
    void *tmpAddrPtr = nullptr;
    std::string ipAddress = "";

    if (getifaddrs(&ifAddrStruct) == -1) {
        std::cerr << "Error in getifaddrs" << std::endl;
        return "";
    }

    for (ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) continue;

        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            if (strcmp(ifa->ifa_name, "lo") != 0) { // ignore loopback
                ipAddress = addressBuffer;
                break;
            }
        }
    }

    if (ifAddrStruct != nullptr) freeifaddrs(ifAddrStruct);
    return ipAddress;
}

int main() {
    std::string localIP = getLocalIPAddress();
    if (!localIP.empty()) {
        std::cout << "Local IP Address: " << localIP << std::endl;
    } else {
        std::cerr << "Could not retrieve the local IP address." << std::endl;
    }
    return 0;
}

