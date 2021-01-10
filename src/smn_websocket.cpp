#include "smn_websocket.hpp"
#include "websocket_connection_base.hpp"
#include "websocket_connection_ssl.hpp"
#include "websocket_connection.hpp"
#include "smn_json.hpp"
#include <nlohmann/json.hpp>
#include <url.hpp>

using json = nlohmann::json;

HandleType_t websocket_handle_type;
extern const sp_nativeinfo_t sm_websocket_natives[];
WebSocket smn_websocket;

void WebSocket::OnExtLoad() {
    HandleAccess hacc;
    TypeAccess tacc;

    handlesys->InitAccessDefaults(&tacc, &hacc);
    tacc.ident = myself->GetIdentity();
    hacc.access[HandleAccess_Read] = HANDLE_RESTRICT_OWNER;
    tacc.access[HTypeAccess_Create] = true;
    tacc.access[HTypeAccess_Inherit] = true;

    websocket_handle_type = handlesys->CreateType("WebSocket", this, 0, &tacc, &hacc, myself->GetIdentity(), NULL);
    sharesys->AddNatives(myself, sm_websocket_natives);
}

void WebSocket::OnExtUnload() {
    handlesys->RemoveType(websocket_handle_type, myself->GetIdentity());
}

void WebSocket::OnHandleDestroy(HandleType_t type, void *object) {
    reinterpret_cast<websocket_connection_base *>(object)->destroy();
}

bool WebSocket::GetHandleApproxSize(HandleType_t type, void *object, unsigned int *size) {
    *size = sizeof(websocket_connection_base);

    return true;
}

HandleError websocket_read_handle(Handle_t hndl, IPluginContext *p_context, websocket_connection_base **obj) {
    HandleSecurity sec;
    
    sec.pOwner = p_context->GetIdentity();
    sec.pIdentity = myself->GetIdentity();
    HandleError herr;
    if ((herr = handlesys->ReadHandle(hndl, websocket_handle_type, &sec, reinterpret_cast<void **>(obj))) != HandleError_None) {
        p_context->ReportError("Invalid WebSocket handle (error %d)", herr);
        return herr;
    }

    return HandleError_None;
}

static cell_t native_WebSocket(IPluginContext *p_context, const cell_t *params) {
    char *address, *path;
    p_context->LocalToString(params[1], &address);
    p_context->LocalToString(params[2], &path);
    uint16_t port = params[3];
    auto connection = new websocket_connection(std::string(address), std::string(path), port);

    return handlesys->CreateHandle(websocket_handle_type, connection, p_context->GetIdentity(), myself->GetIdentity(), NULL);
}

static cell_t native_WebSocketSSL(IPluginContext *p_context, const cell_t *params) {
    char *address, *path;
    p_context->LocalToString(params[1], &address);
    p_context->LocalToString(params[2], &path);
    uint16_t port = params[3];
    auto connection = new websocket_connection_ssl(std::string(address), std::string(path), port);

    return handlesys->CreateHandle(websocket_handle_type, connection, p_context->GetIdentity(), myself->GetIdentity(), NULL);
}

static cell_t native_Connect(IPluginContext *p_context, const cell_t *params) {
    websocket_connection_base *connection;
    if (websocket_read_handle(params[1], p_context, &connection) != HandleError_None) {
        return 0;
    }

    connection->connect();
    return 0;
}

static cell_t native_Close(IPluginContext *p_context, const cell_t *params) {
    websocket_connection_base *connection;
    if (websocket_read_handle(params[1], p_context, &connection) != HandleError_None) {
        return 0;
    }

    connection->close();
    return 0;
}

static cell_t native_SetReadCallback(IPluginContext *p_context, const cell_t *params) {
    websocket_connection_base *connection;
    Handle_t hndl_websocket = params[1];
    if (websocket_read_handle(hndl_websocket, p_context, &connection) != HandleError_None) {
        return 0;
    }

    uint32_t callback_type = params[2];
    auto callback = p_context->GetFunctionById((funcid_t)params[3]);
    if (!callback) {
        p_context->ReportError("Invalid handler callback provided");
        return 0;
    }

    cell_t data = params[4];

    connection->set_read_callback([callback, hndl_websocket, p_context, data](auto buffer, auto size) {
        string message(reinterpret_cast<const char*>(buffer), size);
        free(buffer);

        extension.Defer([callback, hndl_websocket, message, p_context, data]() {
            try {
                auto j = make_unique<json>(json::parse(message));
                Handle_t handle = handlesys->CreateHandle(json_handle_type, j.release(), p_context->GetIdentity(), myself->GetIdentity(), NULL);
                callback->PushCell(hndl_websocket);
                callback->PushCell(handle);
                callback->PushCell(data);
                callback->Execute(nullptr);
            } catch(nlohmann::detail::parse_error e) {
                smutils->LogError(myself, "Error parsing WebSocket JSON: %s", e.what());
            }
        });
    });

    return 0;
}

static cell_t native_SetDisconnectCallback(IPluginContext *p_context, const cell_t *params) {
    websocket_connection_base *connection;
    Handle_t hndl_websocket = params[1];
    if (websocket_read_handle(hndl_websocket, p_context, &connection) != HandleError_None) {
        return 0;
    }

    auto callback = p_context->GetFunctionById((funcid_t)params[2]);
    if (!callback) {
        p_context->ReportError("Invalid handler callback provided");
        return 0;
    }

    cell_t data = params[3];

    connection->set_disconnect_callback([callback, hndl_websocket, p_context, data]() {
        extension.Defer([callback, hndl_websocket, p_context, data]() {
            callback->PushCell(hndl_websocket);
            callback->PushCell(data);
            callback->Execute(nullptr);
        });
    });

    return 0;
}

static cell_t native_SetConnectCallback(IPluginContext *p_context, const cell_t *params) {
    websocket_connection_base *connection;
    Handle_t hndl_websocket = params[1];
    if (websocket_read_handle(hndl_websocket, p_context, &connection) != HandleError_None) {
        return 0;
    }

    auto callback = p_context->GetFunctionById((funcid_t)params[2]);
    if (!callback) {
        p_context->ReportError("Invalid handler callback provided");
        return 0;
    }

    cell_t data = params[3];

    connection->set_connect_callback([callback, hndl_websocket, p_context, data]() {
        extension.Defer([callback, hndl_websocket, p_context, data]() {
            callback->PushCell(hndl_websocket);
            callback->PushCell(data);
            callback->Execute(nullptr);
        });
    });

    return 0;
}

static cell_t native_Write(IPluginContext *p_context, const cell_t *params) {
    websocket_connection_base *connection;
    json *j;
    if (websocket_read_handle(params[1], p_context, &connection) != HandleError_None) {
        return 0;
    } else if(json_read_handle(params[2], p_context, &j) != HandleError_None) {
        return 0;
    }

    connection->write(boost::asio::buffer(j->dump()));
    return 0;
}

static cell_t native_SetHeader(IPluginContext *p_context, const cell_t *params) {
    websocket_connection_base *connection;
    if (websocket_read_handle(params[1], p_context, &connection) != HandleError_None) {
        return 0;
    }

    char *header, *value;
    p_context->LocalToString(params[2], &header);
    p_context->LocalToString(params[3], &value);
    connection->set_header(string(header), string(value));
    return 0;
}

static cell_t native_FromURL(IPluginContext *p_context, const cell_t *params) {
    char *s_url;
    p_context->LocalToString(params[1], &s_url);
    try {
        Url url(s_url);
        websocket_connection_base *connection;
        if (url.path().empty()) {
            url.path("/");
        }

        string path(url.path());
        string host(url.host());
        if (url.scheme() == "wss") {
            if (url.port().empty()) {
                url.port("443");
            }
            connection = new websocket_connection_ssl(host, path, stoi(url.port()));
        } else if (url.scheme() == "ws") {
            if (url.port().empty()) {
                url.port("80");
            }
            connection = new websocket_connection(host, path, stoi(url.port()));
        }

        return handlesys->CreateHandle(websocket_handle_type, connection, p_context->GetIdentity(), myself->GetIdentity(), NULL);
    } catch (...) {
        p_context->ReportError("Invalid websocket URL: %s", s_url);
        return 0;
    }
}


const sp_nativeinfo_t sm_websocket_natives[] = {
    {"WebSocket.Connect", native_Connect},
    {"WebSocket.SetHeader", native_SetHeader},
    {"WebSocket.Close", native_Close},
    {"WebSocket.SetReadCallback", native_SetReadCallback},
    {"WebSocket.SetDisconnectCallback", native_SetDisconnectCallback},
    {"WebSocket.SetConnectCallback", native_SetConnectCallback},
    {"WebSocket.Write", native_Write},
    {"WebSocket_FromURL", native_FromURL},
    {"WebSocket_Create", native_WebSocket},
    {"WebSocket_CreateSSL", native_WebSocketSSL},
    {NULL, NULL}
};