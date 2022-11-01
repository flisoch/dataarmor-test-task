#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <iostream>

#include "query_logger.cpp"

namespace asio = boost::asio;
typedef asio::ip::tcp::socket socket_type;
constexpr int payload_capacity = 8192;

class proxy_connection : 
            public boost::enable_shared_from_this<proxy_connection> {
    public:
        proxy_connection(
            asio::io_context& io_context
        ): 
            remote_service_socket(io_context),
            client_socket(io_context)
        {}

        typedef boost::shared_ptr<proxy_connection> pointer;
        static pointer create(boost::asio::io_context& io_context) {
            return pointer(new proxy_connection(io_context));
        }

        socket_type& get_client_socket() {
            return client_socket;
        }

        void start(const std::string& remote_service_host, 
                            unsigned short remote_service_port) {
            remote_service_socket.async_connect(
                asio::ip::tcp::endpoint(
                    asio::ip::address::from_string(remote_service_host),
                    remote_service_port
                ),
                boost::bind(&proxy_connection::handle_connection_established,
                    shared_from_this(),
                    asio::placeholders::error
                )
            );
        }

        void handle_connection_established(const boost::system::error_code& error) {
            if (!error) {

                // Setup async read from remote server
                remote_service_socket.async_read_some(
                    asio::buffer(remote_service_data, payload_capacity),
                    boost::bind(
                        &proxy_connection::handle_read_from_remote,
                        shared_from_this(),
                        asio::placeholders::error,
                        asio::placeholders::bytes_transferred
                    )
                );

                // Setup async read from client
                client_socket.async_read_some(
                    boost::asio::buffer(client_data, payload_capacity),
                    boost::bind(
                        &proxy_connection::handle_read_from_client,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred
                    )
                );
            }
            else {
                close();
            }
        }

    private:
        // Read message from remote server, now write it to client
        void handle_read_from_remote(const boost::system::error_code& error,
                                const size_t& bytes_transferred) {
            if (!error) {
                asio::async_write(client_socket,
                    boost::asio::buffer(remote_service_data, bytes_transferred),
                    boost::bind(&proxy_connection::handle_write_to_client,
                        shared_from_this(),
                        boost::asio::placeholders::error
                    )
                );
            }
            else {
                std::cerr << error.message() << std::endl;
                close();
            }
        }

        // Wrote to client, now read from remote server again
        void handle_write_to_client(const boost::system::error_code& error) {
            if (!error) {  
                remote_service_socket.async_read_some(
                    boost::asio::buffer(remote_service_data, payload_capacity),
                    boost::bind(
                        &proxy_connection::handle_read_from_remote,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred
                    )
                );
            }
            else {
                close();
            }
        }

        // Read message from client, now write it to remote server
        void handle_read_from_client(const boost::system::error_code& error,
                                  const size_t& bytes_transferred) {
            if (!error) {  
                
                logger.log_query(client_data, bytes_transferred);

                asio::async_write(remote_service_socket,
                    boost::asio::buffer(client_data, bytes_transferred),
                    boost::bind(
                        &proxy_connection::handle_write_to_remote,
                        shared_from_this(),
                        boost::asio::placeholders::error
                    )
                );
            }
            else {
                close();
            }
        }

        // Write to remote server complete, Async read from client
        void handle_write_to_remote(const boost::system::error_code& error) {
            if (!error) {  
                client_socket.async_read_some(
                    boost::asio::buffer(client_data, payload_capacity),
                    boost::bind(
                        &proxy_connection::handle_read_from_client,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred
                    )
                );
            }
            else {
                close();
            }
        }

        void close() {
            boost::mutex::scoped_lock lock(mutex);
            if (client_socket.is_open()) {
                client_socket.close();
            }
            if (remote_service_socket.is_open()) {
                remote_service_socket.close();
            }
        }

        asio::io_context io_context;
        socket_type remote_service_socket;
        socket_type client_socket; 
        unsigned char remote_service_data[payload_capacity];
        unsigned char client_data[payload_capacity];
        boost::mutex mutex;
        query_logger logger;
};