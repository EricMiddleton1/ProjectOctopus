#include <functional>
#include <queue>
#include <mutex>
#include <utility>
#include <thread>
#include <atomic>

#include <mrpt/comms/CServerTCPSocket.h>
#include <jsoncpp/json/value.h>

typedef std::function<void(const Json::Value&)> SCb;

class RobotConnection {
public:
    RobotConnection(std::shared_ptr<mrpt::comms::CClientTCPSocket> sock);
    // Reads any available data, and calls do_callback if data is complete
    void processReads(SCb callback);
    void write(const Json::Value& root);
private:
    std::shared_ptr<mrpt::comms::CClientTCPSocket> socket;
    std::vector<char> unprocessed_data;
    uint32_t size_read = 0;
    // Size remaining in this message
    uint32_t size_remaining = 0;
};

class Server {
public:
    Server(size_t n_robots, const SCb callback, unsigned short listenPort, std::string ipAddr="0.0.0.0");

    int write(const Json::Value data, size_t robot_id);
    void processReads();

private:
    SCb cb_func;
    std::vector<RobotConnection> robots;
    mrpt::comms::CServerTCPSocket serverSock;

    std::queue<std::pair<Json::Value, RobotConnection*>> send_q;

    std::mutex send_q_lock;

    std::thread write_thread;
    std::atomic<int> pending_outgoing;
    void write_loop();
};
