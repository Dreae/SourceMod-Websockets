#include "event_loop.hpp"
#include <thread>

websocket_eventloop event_loop;

void ev_run() {
    event_loop.run();
}

void websocket_eventloop::OnExtLoad() {
    std::thread(ev_run).detach();
}

void websocket_eventloop::OnExtUnload() {
    this->context.stop();
    this->context.reset();
}

void websocket_eventloop::run() {
    this->context.run();
}

boost::asio::io_context& websocket_eventloop::get_context() {
    return this->context;
}

boost::asio::ssl::context& websocket_eventloop::get_ssl_context() {
    return this->ssl_ctx;
}

