#include <functional>
#include <queue>
#include <mutex>
#include <utility>
#include <thread>
#include <atomic>

#include <mrpt/utils/CServerTCPSocket.h>
#include <jsoncpp/json/value.h>

typedef std::function<void(const Json::Value&)> SCb;

class RobotConnection {
public:
    RobotConnection(mrpt::utils::CClientTCPSocket* sock);
    ~RobotConnection();
    // Reads any available data, and calls do_callback if data is complete
    void processReads(SCb callback);
    void write(const Json::Value& root);
private:
    mrpt::utils::CClientTCPSocket* socket;
    std::vector<char> unprocessed_data;
    uint32_t size_read = 0;
    // Size remaining in this message
    uint32_t size_remaining = 0;
};

class Server {
public:
    Server(size_t n_robots, const SCb callback, unsigned short listenPort, std::string ipAddr="127.0.0.1");

    int write(const Json::Value data, size_t robot_id);
    void processReads();

private:
    SCb cb_func;
    std::vector<RobotConnection> robots;
    mrpt::utils::CServerTCPSocket serverSock;

    std::queue<std::pair<Json::Value, RobotConnection*>> send_q;

    std::mutex send_q_lock;

    std::thread write_thread;
    std::atomic<int> pending_outgoing;
    void write_loop();
};
