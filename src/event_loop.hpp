#pragma once
#include "extension.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>

class websocket_eventloop : public WebSocketBase {
public:
    void OnExtLoad();
    void OnExtUnload();
    void run();

    boost::asio::io_context& get_context();
    boost::asio::ssl::context& get_ssl_context();

    websocket_eventloop() : work(context), ssl_ctx(boost::asio::ssl::context::tlsv12_client) {
        this->ssl_ctx.set_verify_mode(boost::asio::ssl::verify_peer);
        this->ssl_ctx.set_default_verify_paths();
    }
private:
    boost::asio::io_context context;
    boost::asio::io_context::work work;
    boost::asio::ssl::context ssl_ctx;
};

extern websocket_eventloop event_loop;