#include <sourcemod>
#include <websocket>

public Plugin myinfo = {
  name = "WebSocket Test",
  author = "Dreae <dreae@dreae.onl>",
  description = "Used for testing websockets",
  version = "1.0.0",
  url = "https://github.com/Dreae/SourceMod-Websockets"
}

public void OnPluginStart() {
    RegServerCmd("websocket_connect", websocket_connect);
    RegServerCmd("websocket_write", websocket_write);
    RegServerCmd("websocket_close", websocket_close);
}
WebSocket ws;

public Action websocket_connect(int args) {
    ws = new WebSocket("echo.websocket.org", "/", 443);
    ws.SetReadCallback(WebSocket_JSON, ReadCallback);
    ws.SetConnectCallback(OnConnected);
    ws.SetDisconnectCallback(OnDisconnected);
    ws.SetHeader("foo", "bar");
    ws.Connect();
}

public void OnConnected(WebSocket _ws) {
    PrintToServer("WS connected");
}


public void OnDisconnected(WebSocket _ws) {
    PrintToServer("WS disconnected");
    delete _ws;
}

public Action websocket_write(int args) {
    JSON json = new JSON();
    json.SetString("message", "foobar");
    ws.Write(json);

    delete json;
}

public Action websocket_close(int args) {
    ws.Close();
}

public void ReadCallback(WebSocket _ws, JSON json) {
    char reply[256];
    json.GetString("message", reply, sizeof(reply));
    PrintToServer("Got reply from test command: %s", reply);

    delete json;
}