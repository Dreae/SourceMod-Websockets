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

#pragma once
#include "smsdk_ext.h"
#include <memory>
#include <functional>

class WebSocketExtension : public SDKExtension {
public:
    virtual bool SDK_OnLoad(char *error, size_t err_max, bool late);
    virtual void SDK_OnUnload();
    virtual void LogMessage(const char *msg, ...);
    virtual void LogError(const char *msg, ...);
    virtual void Defer(std::function<void()> callback);
};

class WebSocketBase {
    friend class WebSocketExtension;

public:
    WebSocketBase() {
        next = WebSocketBase::head;
        WebSocketBase::head = this;
    }

    virtual void OnExtLoad() { };
    virtual void OnExtUnload() { };
private:
    WebSocketBase *next;
    static WebSocketBase *head;
};

extern WebSocketExtension extension;
