@Rem g++ -g -D WINDOWS=\"1\" ../src/ServiceProcess.cpp ../src/Config.cpp ../src/CommonUtil.cpp ../src/HttpUtil.cpp ../src/TCPClient.cpp ../src/JSONUtil.cpp ../src/JSON.cpp ../src/JSONValue.cpp -lws2_32 -std=c++11 -lpthread
@Rem g++ ../src/winservice/WinService.cpp ../src/winservice/ServiceBase.cpp ../src/winservice/ServiceBaseImpl.cpp ../src/winservice/ServiceInstaller.cpp -DUNICODE -D_UNICODE -static-libgcc -static-libstdc++

@Rem --static -shared-libgcc

@Rem g++ ../src/winservice/WinService.cpp ../src/winservice/ServiceBase.cpp ../src/winservice/ServiceBaseImpl.cpp ../src/winservice/ServiceInstaller.cpp ../src/ServiceProcess.cpp ../src/Config.cpp ../src/CommonUtil.cpp ../src/HttpUtil.cpp ../src/TCPClient.cpp ../src/JSONUtil.cpp ../src/JSON.cpp ../src/JSONValue.cpp -DWINDOWS=1 --static -static-libgcc -static-libstdc++ -lws2_32 -std=c++11 -lpthread -shared-libgcc

@Rem g++ ../src/winservice/WinService.cpp ../src/winservice/ServiceBase.cpp ../src/winservice/ServiceBaseImpl.cpp ../src/winservice/ServiceInstaller.cpp ../src/ServiceProcess.cpp ../src/Config.cpp ../src/CommonUtil.cpp ../src/HttpUtil.cpp ../src/TCPClient.cpp ../src/JSONUtil.cpp ../src/JSON.cpp ../src/JSONValue.cpp -DWINDOWS=1 -lws2_32 -std=c++11 -lpthread

@Rem g++ ../src/winservice/WinService.cpp ../src/winservice/ServiceBase.cpp ../src/winservice/ServiceBaseImpl.cpp ../src/winservice/ServiceInstaller.cpp ../src/ServiceProcess.cpp ../src/Config.cpp ../src/CommonUtil.cpp ../src/HttpUtil.cpp ../src/TCPClient.cpp ../src/JSONUtil.cpp ../src/JSON.cpp ../src/JSONValue.cpp -DWINDOWS=1 --static -lws2_32 -std=c++11 -lpthread


@Rem g++ ../src/winservice/WinService.cpp ../src/winservice/ServiceBase.cpp ../src/winservice/ServiceBaseImpl.cpp ../src/winservice/ServiceInstaller.cpp ../src/ServiceProcess.cpp ../src/Config.cpp ../src/CommonUtil.cpp ../src/HttpUtil.cpp ../src/TCPClient.cpp ../src/JSONUtil.cpp ../src/JSON.cpp ../src/JSONValue.cpp -DUNICODE -D_UNICODE -DWINDOWS=1 -static-libgcc -static-libstdc++ -lws2_32 -std=c++11 -lpthread

@Rem g++ ../src/winservice/WinService.cpp ../src/winservice/ServiceBase.cpp ../src/winservice/ServiceBaseImpl.cpp ../src/winservice/ServiceInstaller.cpp ../src/ServiceProcess.cpp ../src/Config.cpp ../src/CommonUtil.cpp ../src/HttpUtil.cpp ../src/TCPClient.cpp ../src/JSONUtil.cpp ../src/JSON.cpp ../src/JSONValue.cpp -DWINDOWS=1 -lws2_32 -std=c++11 -lpthread

cd ../Build
cmake . -G "MinGW Makefiles"
make
