#include "websocket_connection_base.hpp"

websocket_connection_base::websocket_connection_base(string address, string endpoint, uint16_t port) {
    this->address = address;
    this->endpoint = endpoint;
    this->port = port;
}

void websocket_connection_base::set_write_callback(function<void(size_t)> callback) {
    this->write_callback = make_unique<function<void(size_t)>>(callback);
}

void websocket_connection_base::set_read_callback(function<void(uint8_t *, size_t)> callback) {
    this->read_callback = make_unique<function<void(uint8_t *, size_t)>>(callback);
}

void websocket_connection_base::set_connect_callback(function<void()> callback) {
    this->connect_callback = make_unique<function<void()>>(callback);
}

void websocket_connection_base::set_disconnect_callback(function<void()> callback) {
    this->disconnect_callback = make_unique<function<void()>>(callback);
}

void websocket_connection_base::set_header(string header, string value) {
    lock_guard<mutex> guard(this->header_mutex);
    this->headers.insert_or_assign(header, value);
}

void websocket_connection_base::add_headers(websocket::request_type& req) {
    req.set(beast::http::field::user_agent, string(BOOST_BEAST_VERSION_STRING) + " SourceMod-WebSockets v" + SMEXT_CONF_VERSION);
    lock_guard<mutex> guard(this->header_mutex);
    for (pair<string, string> elem : this->headers) {
        req.set(elem.first, elem.second);
    }
}

void websocket_connection_base::destroy() {
    this->pending_delete = true;
    this->close();
}