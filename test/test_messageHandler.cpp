#include <stdio.h>
#include "CppUTest/TestHarness.h"

extern "C" {
#include "CppUTest/TestHarness_c.h"
//#include "messageHandlerTest.h"
//#include "../src/messageHandler.h"

// a bit of a hack to test private-ish functions
#include "../src/messageHandler.c" 
}

/**
 * @brief Construct a new test group object
 * 
 */
TEST_GROUP(MESSAGE_HANDLER) {
    void setup(){};
    void teardown(){};
};

/**
 * @brief Construct a new TEST object
 * 
 */
TEST(MESSAGE_HANDLER, UnpackMsgHdr) {
    uint8_t buf[] = {0x01, 0x02};
    Msg_Header_t msgHeader = {.cmd = 0x00, .len =0x00};
    CHECK(unpackMsgHeader(buf, &msgHeader) == 0);
    CHECK(msgHeader.cmd == buf[0]);
    CHECK(msgHeader.len == buf[1]);

    // check NULL point is caught
    CHECK(unpackMsgHeader(NULL, &msgHeader) == -1);
};

/**
 * @brief Construct a new TEST object
 * 
 */
TEST(MESSAGE_HANDLER, PackMsgHdr) {
    uint8_t buf[] = {0x00, 0x00};
    Msg_Header_t msgHeader = {.cmd = 0x01, .len =0x02};
    CHECK(packMsgHeader(buf, &msgHeader) == 0);
    CHECK(msgHeader.cmd == buf[0]);
    CHECK(msgHeader.len == buf[1]);

    // check NULL point is caught
    CHECK(packMsgHeader(NULL, &msgHeader) == -1);
};


/**
 * @brief Test unpacking of CMD1 payload.
 * 
 */
TEST(MESSAGE_HANDLER, UnpackCmd1Payload) {
    Cmd1_Payload_t msgPayload = {.adcChannel = 0x00};

    // check valid value, 0x01
    uint8_t buf[] = {0x01};
    CHECK(unpackCmd1Payload(buf, &msgPayload) == 0);
    CHECK(msgPayload.adcChannel == buf[0]);

    // check invalid values, 0x00, 0x02, 0xFF
    buf[0] = 0x00;
    CHECK(unpackCmd1Payload(buf, &msgPayload) == -1);

    buf[0] = 0x02;
    CHECK(unpackCmd1Payload(buf, &msgPayload) == -1);

    buf[0] = 0xFF;
    CHECK(unpackCmd1Payload(buf, &msgPayload) == -1);

    // check NULL point is caught
    CHECK(unpackCmd1Payload(NULL, &msgPayload) == -1);
};


/**
 * @brief Test unpacking of CMD2 payload.
 * 
 */
TEST(MESSAGE_HANDLER, UnpackCmd2Payload) {
    Cmd2_Payload_t msgPayload = {
                                    .dacChannel = 0x00,
                                    .value = 0x00
                                };

    // check valid value, 0x01
    uint8_t buf[] = {0x01, 0x02, 0x03};
    CHECK(unpackCmd2Payload(buf, &msgPayload) == 0);
    CHECK(msgPayload.dacChannel == buf[0]);
    CHECK(msgPayload.value = (buf[1] << 8) + buf[2]);

    // check invalid values, 0x00, 0x02, 0xFF
    buf[0] = 0x00;
    CHECK(unpackCmd2Payload(buf, &msgPayload) == -1);

    buf[0] = 0x02;
    CHECK(unpackCmd2Payload(buf, &msgPayload) == -1);

    buf[0] = 0xFF;
    CHECK(unpackCmd2Payload(buf, &msgPayload) == -1);

    // check NULL point is caught
    CHECK(unpackCmd2Payload(NULL, &msgPayload) == -1);
};

/**
 * @brief Construct a new TEST object
 * 
 */
TEST(MESSAGE_HANDLER, UnpackCmd1) {
    uint8_t buf[] = {0x01, 0x03, 0x01};
    Cmd1_Msg_t cmd1 = {
                        .header = {.cmd = 0x00, .len = 0x00},
                        .payload = {.adcChannel = 0x00}
                        };

    CHECK(unpackCmd1(buf, &cmd1) == 0);
    CHECK(cmd1.header.cmd == buf[0]);
    CHECK(cmd1.header.len == buf[1]);
    CHECK(cmd1.payload.adcChannel == buf[2]);
    
    // check NULL point is caught
    CHECK(unpackCmd1(NULL, &cmd1) == -1);

    // check invalid header cmd number
    buf[0] = 0x02;
    CHECK(unpackCmd1(buf, &cmd1) == -1);

    buf[0] = 0x00;
    CHECK(unpackCmd1(buf, &cmd1) == -1);

    buf[0] = 0xFF;
    CHECK(unpackCmd1(buf, &cmd1) == -1);

    // check invalid header len
    buf[0] = 0x01;
    buf[1] = 0x00;
    CHECK(unpackCmd1(buf, &cmd1) == -1);

    buf[1] = 0x02;
    CHECK(unpackCmd1(buf, &cmd1) == -1);

    buf[1] = 0xFF;
    CHECK(unpackCmd1(buf, &cmd1) == -1);
}

/**
 * @brief Construct a new TEST object
 * 
 */
TEST(MESSAGE_HANDLER, UnpackCmd2) {
    uint8_t buf[] = {0x02, 0x05, 0x01, 0x02, 0x03};
    Cmd2_Msg_t cmd2 = {
                        .header = {.cmd = 0x00, .len = 0x00},
                        .payload = {.dacChannel = 0x00, .value = 0x00}
                        };

    CHECK(unpackCmd2(buf, &cmd2) == 0);
    CHECK(cmd2.header.cmd == buf[0]);
    CHECK(cmd2.header.len == buf[1]);
    CHECK(cmd2.payload.dacChannel == buf[2]);
    CHECK(cmd2.payload.value = (uint16_t)(buf[3] << 8) + (uint16_t)buf[4]);

    // check NULL point is caught
    CHECK(unpackCmd2(NULL, &cmd2) == -1);

    // check invalid header cmd
    buf[0] = 0x01;
    CHECK(unpackCmd2(buf, &cmd2) == -1);

    buf[0] = 0x00;
    CHECK(unpackCmd2(buf, &cmd2) == -1);

    buf[0] = 0xFF;
    CHECK(unpackCmd2(buf, &cmd2) == -1);

    // check invalid header len
    buf[0] = 0x02;
    buf[1] = 0x00;
    CHECK(unpackCmd2(buf, &cmd2) == -1);

    buf[1] = 0x02;
    CHECK(unpackCmd2(buf, &cmd2) == -1);

}

/**
 * @brief Construct a new TEST object
 * 
 */
TEST(MESSAGE_HANDLER, PackReply1Payload) {
    uint8_t buf[] = {0x00, 0x00, 0x00};
    Rply1_Payload_t rply1 = {.channels = 0x01, .value = 0x0405};

    CHECK(packRply1Payload(buf, &rply1) == 0);
    CHECK(rply1.channels == buf[0]);
    CHECK((uint8_t)((0xFF00 & rply1.value) >> 8) == buf[1]);
    CHECK((uint8_t)(0x00FF & rply1.value) == buf[2]);

    // check NULL point is caught
    CHECK(packRply1Payload(NULL, &rply1) == -1);

    // check invalid values, 0x00, 0x02, 0xFF
    rply1.channels = 0x00;
    CHECK(packRply1Payload(buf, &rply1) == -1);

    rply1.channels = 0x02;
    CHECK(packRply1Payload(buf, &rply1) == -1);

    rply1.channels = 0xFF;
    CHECK(packRply1Payload(buf, &rply1) == -1);
}

/**
 * @brief Construct a new TEST object
 * 
 */
TEST(MESSAGE_HANDLER, PackReply2Payload) {

    // Check ACK
    uint8_t buf[] = {0x00};
    Rply2_Payload_t rply2 = {.value = 0x55};
    CHECK(packRply2Payload(buf, &rply2) == 0);
    CHECK(rply2.value == buf[0]);

    // Check NACK
    buf[0] = 0x00;
    rply2.value = 0xAA;
    CHECK(packRply2Payload(buf, &rply2) == 0);
    CHECK(rply2.value == buf[0]);

    // check NULL point is caught
    CHECK(packRply2Payload(NULL, &rply2) == -1);

    // check invalid values, 0x00, 0x02, 0xFF
    rply2.value = 0x00;
    CHECK(packRply2Payload(buf, &rply2) == -1);

    rply2.value = 0x02;
    CHECK(packRply2Payload(buf, &rply2) == -1);

    rply2.value = 0xFF;
    CHECK(packRply2Payload(buf, &rply2) == -1);
}

/**
 * @brief Construct a new TEST object
 * 
 */
TEST(MESSAGE_HANDLER, PackReply1) {
    uint8_t buf[] = {0x00, 0x00, 0x00, 0x00, 0x00};
    Rply1_Msg_t rply1 = {
                         .header = {.cmd = 0x81, .len = 0x05},
                         .payload = {.channels = 0x01, .value = 0x0405}
                        };
    CHECK(packRply1(buf, &rply1) == 0);
    CHECK(rply1.header.cmd == buf[0]);
    CHECK(rply1.header.len == buf[1]);
    CHECK(rply1.payload.channels = buf[2]);
    CHECK((uint8_t)((0xFF00 & rply1.payload.value) >> 8) == buf[3]);
    CHECK((uint8_t)(0x00FF & rply1.payload.value) == buf[4]);

    // check NULL pointer is caught
    CHECK(packRply1(NULL, &rply1) == -1);

    // Check bad command
    rply1.header.cmd = 0x00;
    CHECK(packRply1(buf, &rply1) == -1);

    rply1.header.cmd = 0x01;
    CHECK(packRply1(buf, &rply1) == -1);

    rply1.header.cmd = 0xFF;
    CHECK(packRply1(buf, &rply1) == -1);

    // Check bad length
    rply1.header.cmd = 0x81;
    rply1.header.len = 0x00;
    CHECK(packRply1(buf, &rply1) == -1);

    rply1.header.len = 0x01;
    CHECK(packRply1(buf, &rply1) == -1);

    rply1.header.len = 0xFF;
    CHECK(packRply1(buf, &rply1) == -1);

}

/**
 * @brief Construct a new TEST object
 * 
 */
TEST(MESSAGE_HANDLER, PackReply2) {
    uint8_t buf[] = {0x00, 0x00, 0x00};
    Rply2_Msg_t rply2 = {
                         .header = {.cmd = 0x82, .len = 0x03},
                         .payload = {.value = 0x00}
                        };
    rply2.payload.value = ACK;
    CHECK(packRply2(buf, &rply2) == 0);
    CHECK(rply2.header.cmd == buf[0]);
    CHECK(rply2.header.len == buf[1]);
    CHECK(rply2.payload.value = buf[2]);

    rply2.payload.value = NACK;
    CHECK(packRply2(buf, &rply2) == 0);
    CHECK(rply2.header.cmd == buf[0]);
    CHECK(rply2.header.len == buf[1]);
    CHECK(rply2.payload.value = buf[2]);

    // check NULL pointer is caught
    CHECK(packRply2(NULL, &rply2) == -1);

    // Check bad command
    rply2.header.cmd = 0x00;
    CHECK(packRply2(buf, &rply2) == -1);

    rply2.header.cmd = 0x01;
    CHECK(packRply2(buf, &rply2) == -1);

    rply2.header.cmd = 0xFF;
    CHECK(packRply2(buf, &rply2) == -1);

    // Check bad length
    rply2.header.cmd = 0x82;
    rply2.header.len = 0x00;
    CHECK(packRply2(buf, &rply2) == -1);

    rply2.header.len = 0x01;
    CHECK(packRply2(buf, &rply2) == -1);

    rply2.header.len = 0xFF;
    CHECK(packRply2(buf, &rply2) == -1);
}
