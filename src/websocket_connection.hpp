#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include <memory>
#include "websocket_connection_base.hpp"

namespace websocket = boost::beast::websocket;
namespace beast = boost::beast;
using tcp = boost::asio::ip::tcp;
using namespace std;

class websocket_connection : public websocket_connection_base {
public:
    websocket_connection(string address, string endpoint, uint16_t port);
    void connect();
    void write(boost::asio::const_buffer buffer);
    void close();
private:
    void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep);
    void on_handshake(beast::error_code ec);
    void on_write(beast::error_code ec, size_t bytes_transferred);
    void on_read(beast::error_code ec, size_t bytes_transferred);
    void on_close(beast::error_code ec);

    unique_ptr<websocket::stream<beast::tcp_stream>> ws;
    unique_ptr<boost::asio::io_context::work> work;
    shared_ptr<tcp::resolver> resolver;
};