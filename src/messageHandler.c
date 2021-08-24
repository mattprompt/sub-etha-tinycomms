#include <stdint.h>
//#include <stdio.h>
//#include "messageHandler.h"
//#include "test/messageHandlerTest.h"

#define ACK 0x55
#define NACK 0xAA

#define HIGH_BYTE_MASK  0xFF00
#define LOW_BYTE_MASK   0x00FF

// Want to put in ASSERT statements for this example.
// Define dummy ASSERT for now
#ifndef ASSERT
#define ASSERT(x)
#endif

#ifndef NULL
#define NULL 0x00
#endif

#define MSG_HEADER_SIZE     sizeof(Msg_Header_t)/sizeof(uint8_t)

#define CMD1_PAYLOAD_SIZE   sizeof(Cmd1_Payload_t)/sizeof(uint8_t)
#define CMD1_MSG_SIZE       sizeof(Cmd1_Msg_t)/sizeof(uint8_t)

#define CMD2_PAYLOAD_SIZE   sizeof(Cmd2_Payload_t)/sizeof(uint8_t)
#define CMD2_MSG_SIZE       MSG_HEADER_SIZE + CMD2_PAYLOAD_SIZE

#define RPLY1_PAYLOAD_SIZE  sizeof(Rply1_Payload_t)/sizeof(uint8_t)
#define RPLY1_MSG_SIZE      sizeof(Rply1_Msg_t)/sizeof(uint8_t)

#define RPLY2_PAYLOAD_SIZE  sizeof(Rply2_Payload_t)/sizeof(uint8_t)
#define RPLY2_MSG_SIZE      sizeof(Rply2_Msg_t)/sizeof(uint8_t)



// ENUMS
typedef enum Cmd_Type_e {
    CMD1 = 0x01,
    CMD2 = 0x02
} Cmd_Type_t;

typedef enum Rply_Type_e {
    RPLY1 = 0x81,
    RPLY2 = 0x82
} Rply_Type_t;

//https://stackoverflow.com/questions/11770451/what-is-the-meaning-of-attribute-packed-aligned4
#pragma pack(push,1)

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

#pragma push(pop)

/*
Algorithm
Receive msg
Parse msg
if msg1, start AD conversion
when conversion complete, respond with reply1
if msg2, write to message to DAC and respond with rply2

if any other cmd msg log fault and send unknow/unimplimented command
*/

/**
 * @brief 
 * 
 * @param buf 
 * @param msgHeader 
 * @return int8_t 
 */
static int8_t unpackMsgHeader(uint8_t *buf, Msg_Header_t *msgHeader) {
    ASSERT(buf != NULL);
    if (buf == NULL) {
        return -1;
    }

    msgHeader->cmd = buf[0];
    msgHeader->len = buf[1];

    return 0;
}

/**
 * @brief 
 * 
 * @param buf 
 * @param msgHeader 
 * @return int8_t 
 */
static int8_t packMsgHeader(uint8_t *buf, Msg_Header_t *msgHeader) {
    ASSERT(buf != NULL);
    if (buf == NULL) {
        return -1;
    }

    buf[0] = msgHeader->cmd;
    buf[1] = msgHeader->len;

    return 0;
}

/**
 * @brief 
 * 
 * @param buf 
 * @param msgPayload 
 * @return int8_t 
 */
static int8_t unpackCmd1Payload(uint8_t *buf, Cmd1_Payload_t *msgPayload) {
    ASSERT(buf != NULL);
    if (buf == NULL) {
        return -1;
    }
    
    msgPayload->adcChannel = buf[0];
    if (msgPayload->adcChannel != 0x01) {// Only expect adcChannel to be 0x01.
        return -1; // Invalid channel number. Would be good to send a message
    }              // indicating an invalid channel or failed command and not
                   // just fail silently. 

    return 0;
}

/**
 * @brief 
 * 
 * @param buf 
 * @param msgPayload 
 * @return int8_t 
 */
static int8_t unpackCmd2Payload(uint8_t *buf, Cmd2_Payload_t *msgPayload) {
    ASSERT(buf != NULL);
    if (buf == NULL) {
        return -1;
    }
    
    msgPayload->dacChannel = buf[0];
    if (msgPayload->dacChannel != 0x01) { // Only expect adcChannel to be 0x01.
        return -1; // Invalid channel number. Would be good to send a message
    }              // indicating an invalid channel or failed command and not
                   // just fail silently. 

    msgPayload->value = (uint16_t)(buf[1] << 8) + (uint16_t)buf[2];

    return 0;
}

/**
 * @brief 
 * 
 * @param buf 
 * @param msg 
 * @return int8_t 
 */
static int8_t unpackCmd1(uint8_t *buf, Cmd1_Msg_t *msg) {
    ASSERT(buf != NULL);
    if (buf == NULL) {
        return -1;
    }

    if (unpackMsgHeader(buf, &(msg->header)) == -1) {
        return -1;
    }

    if ( (msg->header.cmd != CMD1) || (msg->header.len != CMD1_MSG_SIZE) ) {
        return -1;
    }

    if (unpackCmd1Payload(buf + MSG_HEADER_SIZE, &(msg->payload)) == -1) {
        return -1;
    }

    return 0;
}

/**
 * @brief 
 * 
 * @param buf 
 * @param msg 
 * @return int8_t 
 */
static int8_t unpackCmd2(uint8_t *buf, Cmd2_Msg_t *msg) {
    ASSERT(buf != NULL);
    if (buf == NULL) {
        return -1;
    }

    if (unpackMsgHeader(buf, &(msg->header)) == -1) {
        return -1;
    }

    if ( (msg->header.cmd != CMD2) || (msg->header.len < CMD2_MSG_SIZE)) {
        return -1;
    }

    if (unpackCmd2Payload(buf + MSG_HEADER_SIZE, &(msg->payload)) == -1) {
        return -1;
    }

    return 0;
}

/**
 * @brief 
 * 
 * @param buf 
 * @param msgPayload 
 * @return int8_t 
 */
static int8_t packRply1Payload(uint8_t *buf, Rply1_Payload_t *msgPayload) {
    ASSERT(buf != NULL);
    if (buf == NULL) {
        return -1;
    }

    if (msgPayload->channels != 0x01) {
        return -1;
    }

    buf[0] = msgPayload->channels;
    buf[1] = (uint8_t)((HIGH_BYTE_MASK & msgPayload->value) >> 8);
    buf[2] = (uint8_t)(LOW_BYTE_MASK & msgPayload->value);

    return 0;
}

/**
 * @brief 
 * 
 * @param buf 
 * @param msgPayload 
 * @return int8_t 
 */
static int8_t packRply2Payload(uint8_t *buf, Rply2_Payload_t *msgPayload) {
    ASSERT(buf != NULL);
    if (buf == NULL) {
        return -1;
    }

    if ( (msgPayload->value != ACK) && (msgPayload->value != NACK) ) {
        return -1;
    }

    buf[0] = msgPayload->value;

    return 0;
}

/**
 * @brief 
 * 
 * @param buf 
 * @param msg 
 * @return int8_t 
 */
static int8_t packRply1(uint8_t *buf, Rply1_Msg_t *msg) {
    ASSERT(buf != NULL);
    if (buf == NULL) {
        return -1;
    }

    // Calling function should probably ensure 
    if ( (msg->header.cmd != RPLY1) || (msg->header.len != RPLY1_MSG_SIZE)) {
        return -1;
    }

    if (packMsgHeader(buf, &(msg->header)) == -1) {
        return -1;
    }

    if (packRply1Payload(buf + MSG_HEADER_SIZE, &(msg->payload)) == -1) {
        return -1;
    }
    
    return 0;
}
/**
 * @brief 
 * 
 * @param buf 
 * @param msg 
 * @return int8_t 
 */
static int8_t packRply2(uint8_t *buf, Rply2_Msg_t *msg) {
    ASSERT(buf != NULL);
    if (buf == NULL) {
        return -1;
    }

    if ( (msg->header.cmd != RPLY2) || (msg->header.len != RPLY2_MSG_SIZE)) {
        return -1;
    }

    if (packMsgHeader(buf, &(msg->header)) == -1) {
        return -1;
    }

    if (packRply2Payload(buf + MSG_HEADER_SIZE, &(msg->payload)) == -1) {
        return -1;
    }

    return 0;
}

/**
 * @brief 
 * 
 * @param buf 
 * @param len 
 * @return int8_t 
 */
static int8_t sendMsg(uint8_t *buf, uint8_t len) {
    // Send len number of bytes.
    // Track number of bytes sent, 
    // and any send errors.
    return 0;
}

/**
 * @brief 
 * 
 * @param buf_in 
 * @return int8_t 
 */
static int8_t processCmd1(uint8_t *buf_in) {
    Cmd1_Msg_t cmd1;
    Rply1_Msg_t rply1;
    uint8_t buf_out[RPLY1_MSG_SIZE];

    if (unpackCmd1(buf_in, &cmd1) == -1) {
        return -1;
    }

    // read AD

    rply1.header.cmd = RPLY1;
    rply1.header.len = RPLY1_MSG_SIZE;
    rply1.payload.channels = 0x01;
    rply1.payload.value = 0; // AD value

    if (packRply1(buf_out, &rply1) == -1) {
        return -1;
    }

    if (sendMsg(buf_out, RPLY1_MSG_SIZE) == -1) {
        return -1;
    }

    return 0;
}

/**
 * @brief 
 * 
 * @param buf_in 
 * @return int8_t 
 */
static int8_t processCmd2(uint8_t *buf_in) {
    Rply2_Msg_t rply2;
    Cmd2_Msg_t cmd2;
    uint8_t buf_out[RPLY2_MSG_SIZE];

    if (unpackCmd2(buf_in, &cmd2) == -1) {
        return -1;
    }

    // set DAC

    rply2.header.cmd = RPLY2;
    rply2.header.len = RPLY2_MSG_SIZE;
    rply2.payload.value = ACK; // or NACK if DAC write failed

    if ( packRply2(buf_out, &rply2) ==  -1) {
        return -1;
    }

    if (sendMsg(buf_out, RPLY2_MSG_SIZE) == -1) {
        return -1;
    }

    return 0;
}

/**
 * @brief 
 * 
 * @param buf 
 * @return int8_t 
 */
int8_t processMsg(uint8_t *buf) {
    Msg_Header_t msgHeader = {.cmd = 0x0, .len = 0x0};

    if (unpackMsgHeader(buf, &msgHeader) == -1) {
        return -1;
    }

    // For this example code, assume message structure is correct
    // Actual production code would need message structure validation, like a
    // CRC check, and maybe value range check.
    switch (msgHeader.cmd) {
    case CMD1:
        processCmd1(buf);
        break;

    case CMD2:
        processCmd2(buf);
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
