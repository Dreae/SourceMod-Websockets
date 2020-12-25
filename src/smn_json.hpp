#pragma once
#include "extension.hpp"

extern HandleType_t json_handle_type;
void init_json(IHandleTypeDispatch *parent);

class SMJson : public IHandleTypeDispatch {
public:
    void init_json();
    void unload_json();
    void OnHandleDestroy(HandleType_t type, void *object);
    bool GetHandleApproxSize(HandleType_t type, void *object, unsigned int *size);
};

extern SMJson smn_json;