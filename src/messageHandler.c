#include <stdint.h>
//#include "messageHandler.h"
//#include "test/messageHandlerTest.h"

#define ACK 0x55
#define NACK 0xAA

// Want to put in ASSERT statements for this example.
// Define dummy ASSERT for now
#ifndef ASSERT
#define ASSERT(x)
#endif

#ifndef NULL
#define NULL 0x00
#endif

// ENUMS
typedef enum Cmd_Type_e {
    CMD1 = 0x01,
    CMD2 = 0x02
} Cmd_Type_t;

typedef enum Rply_Type_e {
    RPLY1 = 0x81,
    RPLY2 = 0x82
} Rply_Type_t;

typedef struct Msg_Handler_Ctx_s {
    uint8_t errCnt;
} Msg_Handler_Ctx_t;

/*
Command 1 Byte. 0x01 for A/D conversion
Length 1 B in hex (0x01 means one byte) (not including command and length bytes)
Payload 1 B. 0x01 means channel 0, 0x02 means channel 1, 0x07 means channels 1, 
2 and 3 and so on. Each bit position means one channel. We are expecting this 
byte to be 0x01 only.
*/

/*
Command 1 B. 0x02 for D/A write
Length 1 B
Payload 3 B.
Byte 1: 0x01 means channel 0, 0x02 means channel 1, 0x07 means channels 1, 2 and 
3 and so on. Each bit position means one channel. We are expecting this byte to 
be 0x01 only. Bytes 2 and 3: Right adjusted big endian value to be written to 
D/A converter.
*/

/**
 * Header
 * Command and Reply message have save header structure
 * 
*/
typedef struct Msg_Header_s {
    uint8_t cmd;
    uint8_t len;
} Msg_Header_t;

// CMD1
typedef struct Cmd1_Payload_s {
    uint8_t adcChannel;
} Cmd1_Payload_t;

typedef struct Cmd1_Msg_s {
    Msg_Header_t header;
    Cmd1_Payload_t payload;
} Cmd1_Msg_t;

// CMD2
typedef struct Cmd2_Payload_s {
    uint8_t dacChannel;
    uint16_t value;
} Cmd2_Payload_t;

typedef struct Cmd2_Msg_s {
    Msg_Header_t header;
    Cmd2_Payload_t payload;
} Cmd2_Msg_t;


/*
Reply:
Command 1 B. 0x81
Length 1 B (0x03)
Payload at least 3 bytes.
First byte represents A/D channels converted. 0X01 means channel 0, 0x03 means 
channels 0 and 1 and so on. It is expected to be 0x01 only.
The following 2 bytes are the results of A/D conversion right adjusted and in 
big endian.
*/

/*
Reply:
Command 1 B. 0x82
Length 1 B
Payload: ACK/NACK 1 B. 0x55/0xAA respectively.
*/

// RPLY1
typedef struct Rply1_Payload_s {
    uint8_t channels;
    uint16_t value;
} Rply1_Payload_t;

typedef struct Rply1_Msg_s {
    Msg_Header_t header;
    Rply1_Payload_t payload;
} Rply1_Msg_t;


// RPLY2
typedef struct Rply2_Payload_s {
    uint8_t value;
} Rply2_Payload_t;

typedef struct Rply2_Msg_s {
    Msg_Header_t header;
    Rply2_Payload_t payload;
} Rply2_Msg_t;


/*
Algorithm
Receive msg
Parse msg
if msg1, start AD conversion
when conversion complete, respond with reply1
if msg2, write to message to DAC and respond with rply2

if any other cmd msg log fault and send unknow/unimplimented command
*/

/***
 * 
 * Scope: Private
 * Return:   0 - Okay
 *          -1 - fail
*/
static int8_t unpackMsgHeader(uint8_t *buf, Msg_Header_t *msgHeader)
{
    ASSERT(buf != NULL);
    if (buf == NULL)
        return -1;

    msgHeader->cmd = buf[0];
    msgHeader->len = buf[1];

    return 0;
}

static int8_t unpackCmd1(uint8_t *buf_i, Cmd1_uMsg_t *msg_o)
{
    ASSERT(buf_i != NULL);
    if (buf_i == NULL)
        return -1;

    for (uint8_t i = 0; i < (sizeof(Cmd1_Msg_t) / sizeof(uint8_t)); i++)
    {
        msg_o->bytes[i] = buf_i[i];
    }

    return 0;
}

static int8_t unpackCmd2(uint8_t *buf_i, Cmd2_uMsg_t *msg_o)
{
    ASSERT(buf_i != NULL);
    if (buf_i == NULL)
        return -1;

    msg_o->msg.header.cmd = buf_i[0];
    msg_o->msg.header.len = buf_i[1];
    msg_o->msg.payload.dacChannel = buf_i[2];
    msg_o->msg.payload.value = ((uint16_t)buf_i[3] << 8) + (uint16_t)buf_i[4];

    return 0;
}

int8_t processCmd1(Cmd1_Msg_t msg)
{
    return 0;
}

int8_t processCmd2(Cmd2_Msg_t msg)
{
    return 0;
}

int8_t processMsg(uint8_t *buf)
{
    Msg_Header_t msgHeader = {.cmd = 0x0, .len = 0x0};
    Cmd1_uMsg_t cmd1;
    Cmd2_uMsg_t cmd2;
    if (unpackMsgHeader(buf, &msgHeader) == -1)
        return -1;

    // For this example code, assume message structure is correct
    // Actual production code would need message structure validation, like a
    // CRC check, and maybe value range check.
    switch (msgHeader.cmd)
    {
    case CMD1:
        unpackCmd1(buf, &cmd1);
        processCmd1(cmd1.msg);
        break;

    case CMD2:
        unpackCmd2(buf, &cmd2);
        processCmd2(cmd2.msg);
        break;

    default:
        // Unknown command
        // Currently this just fails silently. But sending an "unknown
        // command" response might be a good improvement. Should also log or
        // track number of unknown commands received.
        return -1;
        break;
    }
    return 0;
}
