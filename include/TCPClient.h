#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H
// Ref : https://github.com/kashimAstro/SimpleNetwork

#include "../include/CommonUtil.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <vector>

#ifdef WINDOWS
#include <winsock2.h>
#include <winsock.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

using namespace std;

class TCPClient
{

public:
  static int receiveTimeout;

private:
  int sock;
  std::string address;
  int port;
  struct sockaddr_in server;

public:
  TCPClient();
  bool setup(string address, int port);
  bool Send(string data);
  string receive(int size = 4096);
  string receiveAll();
  string read();
  void exit();
};

#endif