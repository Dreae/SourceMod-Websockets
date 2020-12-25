#if defined __websocket_include
    #endinput
#endif
#define __websocket_include

typedef WebSocket_ReadCallback = function void (WebSocket ws, JSON message);
typedef WebSocket_ConnectCallback = function void (WebSocket ws);

enum WebSocket_Protocol {
    WebSocket_JSON
}

methodmap JSON < Handle {
  // Creates a new JSON object. This object represents either a JSON array
  // or a JSON object.
  // 
  // @return              A handle to the new JSON object.
  public native JSON();

  // Sets the int value of the JSON object represented by this 
  // handle at the specified key.
  // 
  // @param key           A string representing the key to set.
  // @param value         The value to store in the JSON object.
  public native void SetInt(const char[] key, int value);

  // Sets the float value of the JSON object represented by this 
  // handle at the specified key.
  // 
  // @param key           A string representing the key to set.
  // @param value         The value to store in the JSON object.
  public native void SetFloat(const char[] key, float value);

  // Sets the bool value of the JSON object represented by this 
  // handle at the specified key.
  // 
  // @param key           A string representing the key to set.
  // @param value         The value to store in the JSON object.
  public native void SetBool(const char[] key, bool value);

  // Sets the string value of the JSON object represented by this 
  // handle at the specified key.
  // 
  // @param key           A string representing the key to set.
  // @param value         The value to store in the JSON object.
  //                      The string value of this method is copied.
  public native void SetString(const char[] key, const char[] value);

  // Sets the JSON value of the JSON object represented by this 
  // handle at the specified key.
  // 
  // @param key           A string representing the key to set.
  // @param value         The value to store in the JSON object.
  //                      The object pointed to by this handle is
  //                      copied
  public native void SetJSON(const char[] key, JSON value);

  // Pushes a string value onto the JSON array represented by this 
  // handle
  // 
  // @param value         The string value to be pushed onto the
  //                      array
  public native void PushString(const char[] value);

  // Pushes an int value onto the JSON array represented by this 
  // handle
  // 
  // @param value         The int value to be pushed onto the
  //                      array
  public native void PushInt(int value);

  // Pushes a float value onto the JSON array represented by this 
  // handle
  // 
  // @param value         The float value to be pushed onto the
  //                      array
  public native void PushFloat(float value);

  // Pushes a bool value onto the JSON array represented by this 
  // handle
  // 
  // @param value         The bool value to be pushed onto the
  //                      array
  public native void PushBool(bool value);

  // Pushes a JSON value onto the JSON array represented by this 
  // handle
  // 
  // @param value         The JSON value to be pushed onto the
  //                      array. The object pointed to by this
  //                      handle is copied.
  public native void PushJSON(JSON value);

  // Gets an int value from the JSON object represented by this
  // handle
  // 
  // @param key           The key to look up
  // @return              The value at object[key]
  public native int GetInt(const char[] key);

  // Gets a float value from the JSON object represented by this
  // handle
  // 
  // @param key           The key to look up
  // @return              The value at object[key]
  public native float GetFloat(const char[] key);

  // Gets a bool value from the JSON object represented by this
  // handle
  // 
  // @param key           The key to look up
  // @return              The value at object[key]
  public native bool GetBool(const char[] key);

  // Gets a JSON value from the JSON object represented by this
  // handle
  // 
  // @param key           The key to look up
  // @return              The value at object[key]
  public native JSON GetJSON(const char[] key);

  // Gets a string value from the JSON object represented by this
  // handle
  // 
  // @param key           The key to look up
  // @param buffer        The buffer to store the string value 
  //                      at object[key]
  // @param maxlen        The maximum size of the buffer
  public native void GetString(const char[] key, char[] buffer, int maxlen);

  // Gets a string value at pos from the JSON array represented by
  // this handle
  // 
  // @param pos           The position to fetch from the array
  // @param buffer        The buffer to store the fetched string
  // @param maxlen        The maximum size of the buffer
  public native void GetArrayString(int pos, char[] buffer, int maxlen);

  // Gets an int value at pos from the JSON array represented by
  // this handle
  // 
  // @param pos           The position to fetch from the array
  // @return              The value at array[pos]
  public native int GetArrayInt(int pos);

  // Gets a bool value at pos from the JSON array represented by
  // this handle
  // 
  // @param pos           The position to fetch from the array
  // @return              The value at array[pos]
  public native bool GetArrayBool(int pos);

  // Gets a float value at pos from the JSON array represented by
  // this handle
  // 
  // @param pos           The position to fetch from the array
  // @return              The value at array[pos]
  public native float GetArrayFloat(int pos);

  // Gets a JSON value at pos from the JSON array represented by
  // this handle
  // 
  // @param pos           The position to fetch from the array
  // @return              The value at array[pos]
  public native JSON GetArrayJSON(int pos);

  // Gets the size of the JSON array represented by this handle
  // 
  // @return              The size of this JSON array 
  public native int GetArraySize();
};

methodmap WebSocket < Handle {
    public native WebSocket(const char[] address, const char[] path, int port);
    public native void Connect();
    public native void Close();
    public native void SetReadCallback(WebSocket_Protocol protocol, WebSocket_ReadCallback callback);
    public native void SetConnectCallback(WebSocket_ConnectCallback callback);
    public native void SetDisconnectCallback(WebSocket_ConnectCallback callback);
    public native void Write(JSON json);
}

public Extension:__ext_websocket = {
    name = "WebSocket",
    file = "websocket.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};
