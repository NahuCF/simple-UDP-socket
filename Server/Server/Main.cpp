#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#if defined(_WIN32)
    #define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #define PLATFORM PLATFORM_MAC
#else
    #define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_WINDOWS
    #include <winsock2.h>
    #pragma comment( lib, "wsock32.lib" )
    #pragma comment(lib, "Ws2_32.lib")
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
#endif

#include <stdio.h>
#include <iostream>
#include <ws2tcpip.h>

bool InitializeSockets()
{
    #if PLATFORM == PLATFORM_WINDOWS
        WSADATA WsaData;
        return WSAStartup(MAKEWORD(2, 2),
            &WsaData)
            == NO_ERROR;
    #else
        return true;
    #endif
}

void ShutdownSockets()
{
    #if PLATFORM == PLATFORM_WINDOWS
        WSACleanup();
    #endif
}

int main()
{
    std::cout << ":.SERVER.:" << std::endl;
    if (!InitializeSockets())
    {
        printf("Failed to initialize socket\n");
        return false;
    }

    // Create socket
    int handle = socket(AF_INET, // Address family
        SOCK_DGRAM, 
        IPPROTO_UDP); // UDP

    if (handle <= 0)
    {
        printf("Failed to create socket\n");
        return false;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons((unsigned short)30000); // Host to network short

    // Bind address to socket
    if (bind(handle,
        (const sockaddr*)&address,
        sizeof(sockaddr_in)) < 0)
    {
        printf("Failed to bind socket\n");
        return false;
    }

    // Client data
    sockaddr_in client;
    int clientLength = sizeof(client);

    // Incoming message
    char buf[1024];

    while (true)
    {
        ZeroMemory(buf, sizeof(buf) / sizeof(char));

        int bytesIn = recvfrom(handle, buf, sizeof(buf) / sizeof(char), 0, (sockaddr*)&client, &clientLength);
        if (bytesIn == SOCKET_ERROR)
        {
            printf("Error recieving from client\n");
            continue;
        }

        char clientIp[256];
        inet_ntop(AF_INET, &client.sin_addr, clientIp, sizeof(clientIp) / sizeof(char));

        std::cout << "Message recived from " << clientIp << std::endl;
        std::cout << "Message: " << buf << "\n" << std::endl;

        ZeroMemory(clientIp, sizeof(clientIp) / sizeof(char));
    }
}

