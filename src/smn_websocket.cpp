#include "smn_websocket.hpp"
#include "websocket_connection.hpp"
#include "smn_json.hpp"
#include <nlohmann/json.hpp>

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
    delete reinterpret_cast<websocket_connection *>(object);
}

bool WebSocket::GetHandleApproxSize(HandleType_t type, void *object, unsigned int *size) {
    *size = sizeof(websocket_connection);

    return true;
}

HandleError websocket_read_handle(Handle_t hndl, IPluginContext *p_context, websocket_connection **obj) {
    HandleSecurity sec;
    
    sec.pOwner = p_context->GetIdentity();
    sec.pIdentity = myself->GetIdentity();
    HandleError herr;
    if ((herr = handlesys->ReadHandle(hndl, websocket_handle_type, &sec, reinterpret_cast<void **>(obj))) != HandleError_None) {
        p_context->ReportError("Invalid JSON handle (error %d)", herr);
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

static cell_t native_Connect(IPluginContext *p_context, const cell_t *params) {
    websocket_connection *connection;
    if (websocket_read_handle(params[1], p_context, &connection) != HandleError_None) {
        return 0;
    }

    connection->connect();
    return 0;
}

static cell_t native_SetReadCallback(IPluginContext *p_context, const cell_t *params) {
    websocket_connection *connection;
    if (websocket_read_handle(params[1], p_context, &connection) != HandleError_None) {
        return 0;
    }

    uint32_t callback_type = params[2];
    auto callback = p_context->GetFunctionById((funcid_t)params[3]);
    if (!callback) {
        p_context->ReportError("Invalid handler callback provided");
        return 0;
    }
    connection->set_read_callback([callback, p_context](auto buffer, auto size) {
        extension.Defer([callback, buffer, size, p_context]() {
            try {
                string message(reinterpret_cast<const char*>(buffer), size);
                auto j = json::parse(message);
                auto j_ptr = new json(j);

                Handle_t handle = handlesys->CreateHandle(json_handle_type, j_ptr, p_context->GetIdentity(), myself->GetIdentity(), NULL);
                callback->PushCell(handle);
                callback->Execute(nullptr);
            } catch(std::invalid_argument e) {
                smutils->LogError(myself, "Error parsing WebSocket JSON: %s", e.what());
            }
        });
    });

    return 0;
}

static cell_t native_Write(IPluginContext *p_context, const cell_t *params) {
    websocket_connection *connection;
    if (websocket_read_handle(params[1], p_context, &connection) != HandleError_None) {
        return 0;
    }

    connection->write(boost::asio::buffer("{\"message\": \"foobar\"}"));
    return 0;
}

const sp_nativeinfo_t sm_websocket_natives[] = {
    {"WebSocket.WebSocket", native_WebSocket},
    {"WebSocket.Connect", native_Connect},
    {"WebSocket.SetReadCallback", native_SetReadCallback},
    {"WebSocket.Write", native_Write},
    {NULL, NULL}
};