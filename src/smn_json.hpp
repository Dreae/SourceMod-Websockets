#pragma once
#include "extension.hpp"
#include <nlohmann/json.hpp>

extern HandleType_t json_handle_type;
void init_json(IHandleTypeDispatch *parent);

using json = nlohmann::json;

class SMJson : public IHandleTypeDispatch {
public:
    void init_json();
    void unload_json();
    void OnHandleDestroy(HandleType_t type, void *object);
    bool GetHandleApproxSize(HandleType_t type, void *object, unsigned int *size);
};

HandleError json_read_handle(Handle_t hndl, IPluginContext *p_context, json **obj);

extern SMJson smn_json;