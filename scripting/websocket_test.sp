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
}
WebSocket ws;

public Action websocket_connect(int args) {
    ws = new WebSocket("echo.websocket.org", "/", 443);
    ws.SetReadCallback(WebSocket_JSON, ReadCallback);
    ws.Connect();
}

public Action websocket_write(int args) {
    ws.Write();
}

public void ReadCallback(JSON json) {
    char reply[256];
    json.GetString("message", reply, sizeof(reply));
    PrintToServer("Got reply from test command: %s", reply);
}