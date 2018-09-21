#include "../include/TCPClient.h"

TCPClient::TCPClient()
{
    sock = -1;
    port = 0;
    address = "";
#ifdef WINDOWS
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        CommonUtil::log_message("WSAStartup failed.");
        return;
    }
#endif
}

int TCPClient::receiveTimeout = 3;

bool TCPClient::setup(string address, int port)
{
    if (sock == -1)
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1)
        {
            CommonUtil::log_message(("Could not create socket for address : " + address).c_str());
        }
        else
        {
            //Setting recieve timeout for function recv()
#ifdef WINDOWS
            int nTimeout = receiveTimeout * 1000; // Timeout in ms
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&nTimeout, sizeof(int));
#else
            struct timeval timeout;
            timeout.tv_sec = receiveTimeout;  /* Timeout in seconds*/
            timeout.tv_usec = receiveTimeout; /* Timeout in seconds*/
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&timeout, sizeof(struct timeval));
            //setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const void *)&timeout, sizeof(struct timeval));
#endif
        }
    }
    if (inet_addr(address.c_str()) == -1)
    {
        struct hostent *he;
        struct in_addr **addr_list;
        if ((he = gethostbyname(address.c_str())) == NULL)
        {
            //herror("gethostbyname");
            CommonUtil::log_message("Failed to resolve hostname");
            return false;
        }
        addr_list = (struct in_addr **)he->h_addr_list;
        for (int i = 0; addr_list[i] != NULL; i++)
        {
            server.sin_addr = *addr_list[i];
            break;
        }
    }
    else
    {
        server.sin_addr.s_addr = inet_addr(address.c_str());
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return false;
    }
    return true;
}

bool TCPClient::Send(string data)
{
    if (sock != -1)
    {
        if (send(sock, data.c_str(), strlen(data.c_str()), 0) < 0)
        {
            CommonUtil::log_message(("Send failed : " + data).c_str());
            return false;
        }
    }
    else
        return false;
    return true;
}

string TCPClient::receive(int size)
{
    char buffer[size];
    memset(&buffer[0], 0, sizeof(buffer));

    string reply;
    if (recv(sock, buffer, size, 0) < 0)
    {
        CommonUtil::log_message("receive failed!");
        return nullptr;
    }
    buffer[size - 1] = '\0';
    reply = buffer;
    return reply;
}

string TCPClient::receiveAll()
{
    string reply = "";
    int buffer_size = 1024;
    char buffer[buffer_size];
    int n;
    while ((n = recv(sock, buffer, sizeof(buffer), 0)) > 0)
    {
        //CommonUtil::log_message((n + " ************ " + errno).c_str());
        if (n > 0)
        {
            &reply.append(buffer, n);
        }
        if (n < buffer_size)
        {
            //break;
        }
    }
    return reply;
}

string TCPClient::read()
{
    char buffer[1] = {};
    string reply;
    while (buffer[0] != '\n')
    {
        if (recv(sock, buffer, sizeof(buffer), 0) < 0)
        {
            CommonUtil::log_message("receive failed!");
            return nullptr;
        }
        reply += buffer[0];
    }
    return reply;
}

void TCPClient::exit()
{
    close(sock);
    sock = -1;
    port = 0;
    address = "";
}