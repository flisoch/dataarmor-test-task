#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <string>
#include <iostream>
#include "proxy_connection.cpp"

namespace asio = boost::asio;

class proxy {
    public:
        proxy(
            asio::io_context& io_context,
            const std::string& proxy_host, unsigned short proxy_port,
            const std::string& remote_service_host, unsigned short remote_service_port
        ): 
            io_context{io_context},
            proxy_address{asio::ip::address_v4::from_string(proxy_host)}, // doesn't work, can't assign address
            acceptor{io_context, asio::ip::tcp::endpoint{asio::ip::tcp::v4(), proxy_port}},
            remote_service_host{remote_service_host},
            remote_service_port{remote_service_port}
        {}

        void accept_connections() {
            proxy_connection::pointer new_connection = proxy_connection::create(io_context);
            acceptor.async_accept(
                new_connection->get_client_socket(), 
                boost::bind(
                    &proxy::handle_accept,
                    this,
                    new_connection,
                    asio::placeholders::error
                )
            );
        }
    private:
        void handle_accept(proxy_connection::pointer new_connection,
            const boost::system::error_code& error) {
            if (!error)
            {   
                new_connection->start(remote_service_host, remote_service_port);
            }
            else
            {
               std::cerr << "Error: " << error.message() << std::endl;
            }

            accept_connections();
         }

        asio::io_context& io_context;
        asio::ip::tcp::acceptor acceptor;
        std::string remote_service_host;
        unsigned short remote_service_port;
        asio::ip::address_v4 proxy_address;

};