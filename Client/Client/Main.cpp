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
#include <string>

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
    std::cout << ":.CLIENT.:" << std::endl;

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

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons((unsigned short)30000); // Host to network short

    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    while (true)
    {
        std::string str;
        std::getline(std::cin, str);

        int sendOk = sendto(handle, str.c_str(), str.size() + 1, 0 , (sockaddr*)&server, sizeof(server));
        if (sendOk == SOCKET_ERROR)
        {
            std::cout << "Message not send" << std::endl;
        }
    }
}

