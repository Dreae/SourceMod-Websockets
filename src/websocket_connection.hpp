#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include <memory>

namespace websocket = boost::beast::websocket;
namespace beast = boost::beast;
using tcp = boost::asio::ip::tcp;
using namespace std;

class websocket_connection {
public:
    websocket_connection(string address, string endpoint, uint16_t port);
    void connect();
    void write(boost::asio::const_buffer buffer);
    void destroy();
    void close();
    void set_write_callback(std::function<void(std::size_t)> callback);
    void set_read_callback(std::function<void(uint8_t *, std::size_t)> callback);
    void set_connect_callback(std::function<void()> callback);
    void set_disconnect_callback(std::function<void()> callback);
private:
    void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep);
    void on_ssl_handshake(beast::error_code ec);
    void on_handshake(beast::error_code ec);
    void on_write(beast::error_code ec, size_t bytes_transferred);
    void on_read(beast::error_code ec, size_t bytes_transferred);
    void on_close(beast::error_code ec);

    unique_ptr<std::function<void(uint8_t *, std::size_t)>> read_callback;
    unique_ptr<std::function<void(std::size_t)>> write_callback;
    unique_ptr<std::function<void()>> connect_callback;
    unique_ptr<std::function<void()>> disconnect_callback;
    unique_ptr<websocket::stream<beast::ssl_stream<beast::tcp_stream>>> ws;
    unique_ptr<boost::asio::io_context::work> work;
    shared_ptr<tcp::resolver> resolver;
    beast::flat_buffer buffer;
    string address;
    string endpoint;
    uint16_t port;
    bool pending_delete = false;
};