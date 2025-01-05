/************************************************************************
 *                DECLARATION OF MODEM BLOCKS                          *
 ************************************************************************/

// RYLR998_CONFIG
// Data part
typedef struct
{
  // FB Interface - IN, OUT, IN_OUT variables
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL, EN)
  __DECLARE_VAR(BOOL, ENO)
  __DECLARE_VAR(BOOL, ENABLE)
  __DECLARE_VAR(BOOL, READ_TRIGGER)
  __DECLARE_VAR(BOOL, WRITE_TRIGGER)
  __DECLARE_VAR(INT, MODE)
  __DECLARE_VAR(STRING, DEVICE)
  __DECLARE_VAR(INT, BAUD_RATE)
  __DECLARE_VAR(STRING, PAYLOAD)
  __DECLARE_VAR(STRING, RESPONSE)
  __DECLARE_VAR(INT, CONNECTION_ID)
} RYLR998_CONFIG;

// RYLR998_SEND
typedef struct
{
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL, EN)
  __DECLARE_VAR(BOOL, ENO)
  __DECLARE_VAR(BOOL, ENABLE)
  __DECLARE_VAR(BOOL, TRIGGER)
  __DECLARE_VAR(INT, CONNECTION_ID)
  __DECLARE_VAR(INT, DESTINATION_ADDRESS)
  __DECLARE_VAR(STRING, MESSAGE)
  __DECLARE_VAR(INT, MSG_LEN)
  __DECLARE_VAR(INT, MSG_COUNTER)
  __DECLARE_VAR(STRING, RESPONSE)
} RYLR998_SEND;

// RYLR998_RECEIVE
typedef struct
{
  __DECLARE_VAR(BOOL, EN)
  __DECLARE_VAR(BOOL, ENO)
  __DECLARE_VAR(INT, CONNECTION_ID)
  __DECLARE_VAR(BOOL, ENABLE)
  __DECLARE_VAR(INT, MESSAGE_COUNTER)
  __DECLARE_VAR(STRING, SOURCE_ADDRESS)
  __DECLARE_VAR(STRING, LENGTH)
  __DECLARE_VAR(STRING, MESSAGE)
  __DECLARE_VAR(STRING, RSSI)
  __DECLARE_VAR(STRING, SNR)
} RYLR998_RECEIVE;

int rylr998_config(uint8_t *device, int baud_rate, bool read_trigger, bool write_trigger, uint8_t *payload, int mode);
int rylr_send(int connection_id, bool trigger, int address, uint8_t *payload_data);
char *rylr_receive(int connection_id);
int get_rylr_msg_counter();
int get_rylr_send_msg_counter();
char *get_send_resp();
char *get_config_response();


static void RYLR998_CONFIG_init__(RYLR998_CONFIG *data__, BOOL retain)
{
  __INIT_VAR(data__->EN, __BOOL_LITERAL(TRUE), retain)
  __INIT_VAR(data__->ENO, __BOOL_LITERAL(TRUE), retain)
  __INIT_VAR(data__->ENABLE, __BOOL_LITERAL(FALSE), retain)
  __INIT_VAR(data__->READ_TRIGGER, __BOOL_LITERAL(FALSE), retain)
  __INIT_VAR(data__->WRITE_TRIGGER, __BOOL_LITERAL(FALSE), retain)
  __INIT_VAR(data__->MODE, 0, retain)
  __INIT_VAR(data__->DEVICE, __STRING_LITERAL(0, ""), retain)
  __INIT_VAR(data__->BAUD_RATE, 0, retain)
  __INIT_VAR(data__->PAYLOAD, __STRING_LITERAL(0, ""), retain)
  __INIT_VAR(data__->RESPONSE, __STRING_LITERAL(0, ""), retain)
  __INIT_VAR(data__->CONNECTION_ID, 0, retain)
}

static void RYLR998_CONFIG_body__(RYLR998_CONFIG *data__)
{
  // Control execution
  if (!__GET_VAR(data__->EN))
  {
    __SET_VAR(data__->, ENO, , __BOOL_LITERAL(FALSE));
    return;
  }
  else
  {
    __SET_VAR(data__->, ENO, , __BOOL_LITERAL(TRUE));
  }

#define GetFbVar(var, ...) __GET_VAR(data__->var, __VA_ARGS__)
#define SetFbVar(var, val, ...) __SET_VAR(data__->, var, __VA_ARGS__, val)
  bool enable = GetFbVar(ENABLE);
  int baud_rate = GetFbVar(BAUD_RATE);
  IEC_STRING device = GetFbVar(DEVICE);
  IEC_STRING payload = GetFbVar(PAYLOAD);
  IEC_STRING response = GetFbVar(RESPONSE);
  bool read_trigger = GetFbVar(READ_TRIGGER);
  bool write_trigger = GetFbVar(WRITE_TRIGGER);
  int mode = GetFbVar(MODE);

  if (enable)
  {
    int config_response = rylr998_config(device.body, baud_rate, read_trigger, write_trigger, payload.body, mode);
    char *config_response_msg = get_config_response();

    strncpy((char *)response.body, config_response_msg, strlen(config_response_msg)); // Copy data to body
    response.body[strlen(config_response_msg)] = '\0';                            // Null-terminate
    response.len = (uint8_t)strlen(config_response_msg);
    SetFbVar(RESPONSE, response);
    SetFbVar(CONNECTION_ID, config_response);
  }

#undef GetFbVar
#undef SetFbVar

  goto __end;

__end:
  return;
}

static void RYLR998_SEND_init__(RYLR998_SEND *data__, BOOL retain)
{
  __INIT_VAR(data__->EN, __BOOL_LITERAL(TRUE), retain)
  __INIT_VAR(data__->ENO, __BOOL_LITERAL(TRUE), retain)
  __INIT_VAR(data__->ENABLE, __BOOL_LITERAL(FALSE), retain)
  __INIT_VAR(data__->TRIGGER, __BOOL_LITERAL(FALSE), retain)
  __INIT_VAR(data__->CONNECTION_ID, 0, retain)
  __INIT_VAR(data__->DESTINATION_ADDRESS, 0, retain)
  __INIT_VAR(data__->MESSAGE, __STRING_LITERAL(0, ""), retain)
  __INIT_VAR(data__->MSG_LEN, 0, retain)
  __INIT_VAR(data__->MSG_COUNTER, 0, retain)
  __INIT_VAR(data__->RESPONSE, __STRING_LITERAL(0, ""), retain)
}

static void RYLR998_SEND_body__(RYLR998_SEND *data__)
{
  // Control execution
  if (!__GET_VAR(data__->EN))
  {
    __SET_VAR(data__->, ENO, , __BOOL_LITERAL(FALSE));
    return;
  }
  else
  {
    __SET_VAR(data__->, ENO, , __BOOL_LITERAL(TRUE));
  }

#define GetFbVar(var, ...) __GET_VAR(data__->var, __VA_ARGS__)
#define SetFbVar(var, val, ...) __SET_VAR(data__->, var, __VA_ARGS__, val)
  int connection_id = GetFbVar(CONNECTION_ID);
  bool trigger = GetFbVar(TRIGGER);
  int address = GetFbVar(DESTINATION_ADDRESS);
  IEC_STRING payload_data = GetFbVar(MESSAGE);
  IEC_STRING response = GetFbVar(RESPONSE);
  bool enable = GetFbVar(ENABLE);

  if (enable)
  {
    int send_response = rylr_send(connection_id, trigger, address, payload_data.body);
    int msg_counter = get_rylr_send_msg_counter();
    char *send_response_msg = get_send_resp();

    strncpy((char *)response.body, send_response_msg, strlen(send_response_msg)); // Copy data to body
    response.body[strlen(send_response_msg)] = '\0';                            // Null-terminate
    response.len = (uint8_t)strlen(send_response_msg);

    SetFbVar(MSG_COUNTER, msg_counter);
    // SetFbVar(MSG_LEN, strlen(payload_data.body));
    SetFbVar(RESPONSE, response);
  }

  // SetFbVar(BYTES_SENT, send_response);
  // SetFbVar(SUCCESS, send_response == 0);
#undef GetFbVar
#undef SetFbVar

  goto __end;

__end:
  return;
}

static void RYLR998_RECEIVE_init__(RYLR998_RECEIVE *data__, BOOL retain)
{
  __INIT_VAR(data__->EN, __BOOL_LITERAL(TRUE), retain)
  __INIT_VAR(data__->ENO, __BOOL_LITERAL(TRUE), retain)
  __INIT_VAR(data__->CONNECTION_ID, 0, retain)
  __INIT_VAR(data__->ENABLE, __BOOL_LITERAL(FALSE), retain)
  __INIT_VAR(data__->MESSAGE_COUNTER, 0, retain)
  __INIT_VAR(data__->SOURCE_ADDRESS, __STRING_LITERAL(0, ""), retain)
  __INIT_VAR(data__->LENGTH, __STRING_LITERAL(0, ""), retain)
  __INIT_VAR(data__->MESSAGE, __STRING_LITERAL(0, ""), retain)
  __INIT_VAR(data__->RSSI, __STRING_LITERAL(0, ""), retain)
  __INIT_VAR(data__->SNR, __STRING_LITERAL(0, ""), retain)
}

static void RYLR998_RECEIVE_body__(RYLR998_RECEIVE *data__)
{
  // Control execution
  if (!__GET_VAR(data__->EN))
  {
    __SET_VAR(data__->, ENO, , __BOOL_LITERAL(FALSE));
    return;
  }
  else
  {
    __SET_VAR(data__->, ENO, , __BOOL_LITERAL(TRUE));
  }

#define GetFbVar(var, ...) __GET_VAR(data__->var, __VA_ARGS__)
#define SetFbVar(var, val, ...) __SET_VAR(data__->, var, __VA_ARGS__, val)
  bool enable = GetFbVar(ENABLE);
  IEC_STRING message = GetFbVar(MESSAGE);
  IEC_STRING address = GetFbVar(SOURCE_ADDRESS);
  IEC_STRING length = GetFbVar(LENGTH);
  IEC_STRING rssi = GetFbVar(RSSI);
  IEC_STRING snr = GetFbVar(SNR);

  if (enable)
  {
    int connection_id = GetFbVar(CONNECTION_ID);

    if (connection_id > 0)
    {
      char *receive_message = rylr_receive(connection_id);
      int msg_counter = get_rylr_msg_counter();
      if (strlen(receive_message) > 0)
      {
        SetFbVar(MESSAGE_COUNTER, msg_counter);
        char *saveptr1, *saveptr2;
        // Step 1: Split by '='
        char *firstPart = strtok_r(receive_message, "=", &saveptr1);
        char *secondPart = strtok_r(NULL, "=", &saveptr1);

        if (secondPart != NULL)
        {
          char *address_payload = strtok_r(secondPart, ",", &saveptr2);
          char *length_payload = strtok_r(NULL, ",", &saveptr2);
          char *message_payload = strtok_r(NULL, ",", &saveptr2);
          char *rssi_payload = strtok_r(NULL, ",", &saveptr2);
          char *snr_payload = strtok_r(NULL, ",", &saveptr2);

          // Set Message
          strncpy((char *)message.body, message_payload, strlen(message_payload)); // Copy data to body
          message.body[strlen(message_payload)] = '\0';                            // Null-terminate
          message.len = (uint8_t)strlen(message_payload);

          // Set Address
          strncpy((char *)address.body, address_payload, strlen(address_payload)); // Copy data to body
          address.body[strlen(address_payload)] = '\0';                            // Null-terminate
          address.len = (uint8_t)strlen(address_payload);

          // Set Length
          strncpy((char *)length.body, length_payload, strlen(length_payload)); // Copy data to body
          length.body[strlen(length_payload)] = '\0';                           // Null-terminate
          length.len = (uint8_t)strlen(length_payload);

          // Set RSSI
          strncpy((char *)rssi.body, rssi_payload, strlen(rssi_payload)); // Copy data to body
          rssi.body[strlen(rssi_payload)] = '\0';                         // Null-terminate
          rssi.len = (uint8_t)strlen(rssi_payload);

          // Set SNR
          strncpy((char *)snr.body, snr_payload, strlen(snr_payload)); // Copy data to body
          snr.body[strlen(snr_payload)] = '\0';                        // Null-terminate
          snr.len = (uint8_t)strlen(snr_payload);
        }
      }

      SetFbVar(MESSAGE, message);
      SetFbVar(SOURCE_ADDRESS, address);
      SetFbVar(LENGTH, length);
      SetFbVar(RSSI, rssi);
      SetFbVar(SNR, snr);
    }
  }

#undef GetFbVar
#undef SetFbVar

  goto __end;

__end:
  return;
}

/************************************************************************
 *                END OF MODEM BLOCKS                          *
 *************************{***********************************************/
