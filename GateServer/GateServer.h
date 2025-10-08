#ifndef GATESERVER_GATESERVRE_H_
#define GATESERVRE_GATESERVER_H_


class GateServer
{
public:
    GateServer();
    ~GateServer();


    bool init();
    void run();
    void shutdown();
private:

private:
    GateServer(const GateServer&) = delete;
    GateServer(GateServer&&) noexcept = delete;

    GateServer& operator=(const GateServer&) = delete;
    GateServer& operator=(GateServer&&) = delete;
};

#endif