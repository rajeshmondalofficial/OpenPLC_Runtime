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
} RYLR998_CONFIG;


// RYLR998_SEND
typedef struct
{
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL, EN)
  __DECLARE_VAR(BOOL, ENO)
  __DECLARE_VAR(BOOL, SEND)
  __DECLARE_VAR(STRING, DEVICE)
  __DECLARE_VAR(STRING, MSG)
  __DECLARE_VAR(STRING, SEND_COMMENT)
  __DECLARE_VAR(INT, SUCCESS_ID)
  __DECLARE_VAR(STRING, ADDRESS)
  __DECLARE_VAR(INT, PAYLOAD_LENGTH)
  __DECLARE_VAR(STRING, PAYLOAD_DATA)
  __DECLARE_VAR(INT, BYTES_SENT)
} RYLR998_SEND;

// RYLR998_RECEIVE
typedef struct {
  __DECLARE_VAR(BOOL, EN)
  __DECLARE_VAR(BOOL, ENO)
  __DECLARE_VAR(STRING, DEVICE)
  __DECLARE_VAR(BOOL, RECEIVE)
  __DECLARE_VAR(INT, BYTES_RECEIVED)
  __DECLARE_VAR(STRING, MESSAGE)
} RYLR998_RECEIVE;

int rylr998_config(uint8_t *device, int baud_rate, int frequency);

static void RYLR998_CONFIG_init__(RYLR998_CONFIG *data__, BOOL retain)
{
  __INIT_VAR(data__->EN, __BOOL_LITERAL(TRUE), retain)
  __INIT_VAR(data__->ENO, __BOOL_LITERAL(TRUE), retain)
  __INIT_VAR(data__->DEVICE, __STRING_LITERAL(0, ""), retain)
  __INIT_VAR(data__->FREQUENCY, 9600, retain)
  __INIT_VAR(data__->BAUD_RATE, 0, retain)
  __INIT_VAR(data__->SUCCESS, 0, retain)
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

  #define GetFbVar(var,...) __GET_VAR(data__->var,__VA_ARGS__)
  #define SetFbVar(var,val,...) __SET_VAR(data__->,var,__VA_ARGS__,val)

  int baud_rate = GetFbVar(BAUD_RATE);
  IEC_STRING device = GetFbVar(DEVICE);
  int frequency = GetFbVar(FREQUENCY);
  int config_response = rylr998_config(device.body, baud_rate, frequency);
  SetFbVar(SUCCESS, config_response);
  

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
  __INIT_VAR(data__->SEND, __BOOL_LITERAL(FALSE), retain)
  __INIT_VAR(data__->DEVICE, __STRING_LITERAL(0, ""), retain)
  __INIT_VAR(data__->MSG, __STRING_LITERAL(0, ""), retain)
  __INIT_VAR(data__->SEND_COMMENT, __STRING_LITERAL(0, ""), retain)
  __INIT_VAR(data__->SUCCESS_ID, 0, retain)
  __INIT_VAR(data__->ADDRESS, __STRING_LITERAL(0, ""), retain)
  __INIT_VAR(data__->PAYLOAD_LENGTH, 0, retain)
  __INIT_VAR(data__->PAYLOAD_DATA, __STRING_LITERAL(0, ""), retain)
  __INIT_VAR(data__->BYTES_SENT, 0, retain)
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

static void RYLR998_RECEIVE_init__(RYLR998_RECEIVE *data__, BOOL retain){
  __INIT_VAR(data__->EN, __BOOL_LITERAL(TRUE), retain)
  __INIT_VAR(data__->ENO, __BOOL_LITERAL(TRUE), retain)
  __INIT_VAR(data__->DEVICE, __STRING_LITERAL(0, ""), retain)
  __INIT_VAR(data__->RECEIVE, __BOOL_LITERAL(FALSE), retain)
  __INIT_VAR(data__->BYTES_RECEIVED, 0, retain)
  __INIT_VAR(data__->MESSAGE, __STRING_LITERAL(0, ""), retain)
}

static void RYLR998_RECEIVE_body__(RYLR998_RECEIVE *data__){
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

/************************************************************************
 *                END OF MODEM BLOCKS                          *
 *************************{***********************************************/
