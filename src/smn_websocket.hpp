#pragma once
#include "extension.hpp"

class WebSocket : public WebSocketBase, public IHandleTypeDispatch {
public:
    void OnExtLoad();
    void OnExtUnload();
    void OnHandleDestroy(HandleType_t type, void *object);
    bool GetHandleApproxSize(HandleType_t type, void *object, unsigned int *size);    
};