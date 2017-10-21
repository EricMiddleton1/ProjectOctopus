#include <functional>
#include <mrpt/utils/CServerTCPSocket.h>
#include <jsoncpp/json/value.h>

typedef std::function<void(const Json::Value&)> SCb;

class Server {
public:
    Server(const SCb callback, unsigned short listenPort, std::string ipAddr="127.0.0.1");

    void start();

private:
    SCb cb_func;
    mrpt::utils::CServerTCPSocket serverSock;
};
