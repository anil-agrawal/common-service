#!/bin/bash
# g++ -g ../src/nixservice/LinuxService.cpp ../src/ServiceProcess.cpp ../src/Config.cpp ../src/CommonUtil.cpp ../src/HttpUtil.cpp ../src/TCPClient.cpp ../src/JSONUtil.cpp ../src/JSON.cpp ../src/JSONValue.cpp -std=c++11 -lpthread

cd ../Build
cmake . 
make