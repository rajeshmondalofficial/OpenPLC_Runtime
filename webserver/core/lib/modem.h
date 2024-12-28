/************************************************************************
 *                DECLARATION OF MODEM BLOCKS                          *
 ************************************************************************/

// RYLR998_CONFIG
// Data part
typedef struct
{
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL, EN)
  __DECLARE_VAR(BOOL, ENO)
  __DECLARE_VAR(STRING, DEVICE)
  __DECLARE_VAR(INT, FREQUENCY)
  __DECLARE_VAR(INT, BAUD_RATE)
  __DECLARE_VAR(INT, SUCCESS)
  __DECLARE_VAR(BOOL, TRIGGER)
} RYLR998_CONFIG;

// RYLR998_SEND
typedef struct
{
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL, EN)
  __DECLARE_VAR(BOOL, ENO)
  __DECLARE_VAR(BOOL, TRIGGER)
  __DECLARE_VAR(INT, CONNECTION_ID)
  __DECLARE_VAR(INT, ADDRESS)
  __DECLARE_VAR(STRING, PAYLOAD_DATA)
  __DECLARE_VAR(INT, BYTES_SENT)
  __DECLARE_VAR(BOOL, SUCCESS)
  __DECLARE_VAR(STRING, RESPONSE)
} RYLR998_SEND;

// RYLR998_RECEIVE
typedef struct
{
  __DECLARE_VAR(BOOL, EN)
  __DECLARE_VAR(BOOL, ENO)
  __DECLARE_VAR(INT, CONNECTION_ID)
  __DECLARE_VAR(STRING, ADDRESS)
  __DECLARE_VAR(STRING, LENGTH)
  __DECLARE_VAR(INT, BYTES_RECEIVED)
  __DECLARE_VAR(STRING, MESSAGE)
} RYLR998_RECEIVE;

int rylr998_config(uint8_t *device, int baud_rate, int frequency);
char *rylr_receive();

static void RYLR998_CONFIG_init__(RYLR998_CONFIG *data__, BOOL retain)
{
  __INIT_VAR(data__->EN, __BOOL_LITERAL(TRUE), retain)
  __INIT_VAR(data__->ENO, __BOOL_LITERAL(TRUE), retain)
  __INIT_VAR(data__->DEVICE, __STRING_LITERAL(0, ""), retain)
  __INIT_VAR(data__->FREQUENCY, 9600, retain)
  __INIT_VAR(data__->BAUD_RATE, 0, retain)
  __INIT_VAR(data__->SUCCESS, 0, retain)
  __INIT_VAR(data__->TRIGGER, __BOOL_LITERAL(FALSE), retain)
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

  int baud_rate = GetFbVar(BAUD_RATE);
  IEC_STRING device = GetFbVar(DEVICE);
  int frequency = GetFbVar(FREQUENCY);
  bool trigger = GetFbVar(TRIGGER);
  if (trigger)
  {
    int config_response = rylr998_config(device.body, baud_rate, frequency);
    SetFbVar(SUCCESS, config_response);
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
  __INIT_VAR(data__->CONNECTION_ID, 0, retain)
  __INIT_VAR(data__->TRIGGER, __BOOL_LITERAL(FALSE), retain)
  __INIT_VAR(data__->ADDRESS, 0, retain)
  __INIT_VAR(data__->PAYLOAD_DATA, __STRING_LITERAL(0, ""), retain)
  __INIT_VAR(data__->BYTES_SENT, 0, retain)
  __INIT_VAR(data__->SUCCESS, __BOOL_LITERAL(FALSE), retain)
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

  goto __end;

__end:
  return;
}

static void RYLR998_RECEIVE_init__(RYLR998_RECEIVE *data__, BOOL retain)
{
  __INIT_VAR(data__->EN, __BOOL_LITERAL(TRUE), retain)
  __INIT_VAR(data__->ENO, __BOOL_LITERAL(TRUE), retain)
  __INIT_VAR(data__->CONNECTION_ID, 0, retain)
  __INIT_VAR(data__->ADDRESS, __STRING_LITERAL(0, ""), retain)
  __INIT_VAR(data__->LENGTH, __STRING_LITERAL(0, ""), retain)
  __INIT_VAR(data__->BYTES_RECEIVED, 0, retain)
  __INIT_VAR(data__->MESSAGE, __STRING_LITERAL(0, ""), retain)
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
  IEC_STRING message = GetFbVar(MESSAGE);
  IEC_STRING address = GetFbVar(ADDRESS);
  IEC_STRING length = GetFbVar(LENGTH);

  char *receive_message = rylr_receive();
  if (strlen(receive_message) > 0)
  {
    char *saveptr1, *saveptr2, *saveptr3, *saveptr4;
    // Step 1: Split by '='
    char *firstPart = strtok_r(receive_message, "=", &saveptr1);
    char *secondPart = strtok_r(NULL, "=", &saveptr1);

    if (secondPart != NULL)
    {
      char *address_payload = strtok_r(secondPart, ",", &saveptr2);
      char *rest_after_address = strtok_r(NULL, ",", &saveptr2);
      char *length_payload = strtok_r(rest_after_address, ",", &saveptr2);
      char *rest_after_length = strtok_r(NULL, ",", &saveptr2);
      // char *message_payload = strtok_r(rest_after_length, ",", &saveptr4);
      // char *rest_after_message = strtok_r(NULL, ",", &saveptr4);
      // Set Message
      strncpy((char *)message.body, receive_message, strlen(receive_message)); // Copy data to body
      message.body[strlen(receive_message)] = '\0';                            // Null-terminate
      message.len = (uint8_t)strlen(receive_message);

      // Set Address
      strncpy((char *)address.body, address_payload, strlen(address_payload)); // Copy data to body
      address.body[strlen(address_payload)] = '\0';                            // Null-terminate
      address.len = (uint8_t)strlen(address_payload);

      // Set Length
      strncpy((char *)length.body, length_payload, strlen(length_payload)); // Copy data to body
      length.body[strlen(length_payload)] = '\0';                            // Null-terminate
      length.len = (uint8_t)strlen(rest_after_length);
    }
  }

  SetFbVar(MESSAGE, message);
  SetFbVar(ADDRESS, address);
  SetFbVar(LENGTH, length);

#undef GetFbVar
#undef SetFbVar

  goto __end;

__end:
  return;
}

/************************************************************************
 *                END OF MODEM BLOCKS                          *
 *************************{***********************************************/
