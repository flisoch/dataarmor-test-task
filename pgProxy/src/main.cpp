#include "proxy.cpp"

int main(int argc, char* argv[]) {

    if (argc != 5) {
        std::cerr << "usage: tcpproxy_server <local host ip> <local port> <forward host ip> <forward port>" << std::endl;
        return 1;
    }
    const std::string proxy_host      = argv[1];
    const unsigned short proxy_port   = static_cast<unsigned short>(::atoi(argv[2]));
    const std::string remote_service_host    = argv[3];
    const unsigned short remote_service_port = static_cast<unsigned short>(::atoi(argv[4]));

    boost::asio::io_context io_context;

    try {
        proxy proxy(io_context, proxy_host, proxy_port, remote_service_host, remote_service_port);
        proxy.accept_connections();
        io_context.run();
    } 
    catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}