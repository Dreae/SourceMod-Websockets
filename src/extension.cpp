 /**
  * SourceMod Encrypted Socket Extension
  * Copyright (C) 2020  Dreae
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details. 
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "extension.hpp"
#include "smn_json.hpp"
#include <atomic>

WebSocketExtension extension;
SMEXT_LINK(&extension);

WebSocketBase *WebSocketBase::head = NULL;
std::atomic<bool> unloaded;

bool WebSocketExtension::SDK_OnLoad(char *error, size_t err_max, bool late) {
    WebSocketBase *head = WebSocketBase::head;
    while (head) {
        head->OnExtLoad();
        head = head->next;
    }
    smn_json.init_json();
    
    unloaded.store(false);
    return true;
}

void WebSocketExtension::SDK_OnUnload() {
    WebSocketBase *head = WebSocketBase::head;
    while (head) {
        head->OnExtUnload();
        head = head->next;
    }
    smn_json.unload_json();
    unloaded.store(true);
}

void log_msg(void *msg) {
    if (!unloaded.load()) {
        smutils->LogMessage(myself, reinterpret_cast<char *>(msg));
    }
    free(msg);
}


void log_err(void *msg) {
    if (!unloaded.load()) {
        smutils->LogError(myself, reinterpret_cast<char *>(msg));
    }
    free(msg);
}

void WebSocketExtension::LogMessage(const char *msg, ...) {
    char *buffer = reinterpret_cast<char *>(malloc(3072));
    va_list vp;
    va_start(vp, msg);
    vsnprintf(buffer, 3072, msg, vp);
    va_end(vp);

    smutils->AddFrameAction(&log_msg, reinterpret_cast<void *>(buffer));
}

void WebSocketExtension::LogError(const char *msg, ...) {
    char *buffer = reinterpret_cast<char *>(malloc(3072));
    va_list vp;
    va_start(vp, msg);
    vsnprintf(buffer, 3072, msg, vp);
    va_end(vp);
    
    smutils->AddFrameAction(&log_err, reinterpret_cast<void *>(buffer));
}

void execute_cb(void *cb) {
    std::unique_ptr<std::function<void()>> callback(reinterpret_cast<std::function<void()> *>(cb));
    callback->operator()();
}

void WebSocketExtension::Defer(std::function<void()> callback) {
    std::unique_ptr<std::function<void()>> cb = std::make_unique<std::function<void()>>(callback);
    smutils->AddFrameAction(&execute_cb, cb.release());
}
