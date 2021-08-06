#include <stdio.h>
#include "CppUTest/TestHarness.h"

extern "C" {
#include "CppUTest/TestHarness_c.h"
//#include "messageHandlerTest.h"
//#include "../src/messageHandler.h"

// a bit of a hack to test private-ish functions
#include "../src/messageHandler.c" 
}


TEST_GROUP(MESSAGE_HANDLER) {
    void setup(){};
    void teardown(){};
};

TEST(MESSAGE_HANDLER, NormalValues) {
    uint8_t buf[] = {0x01, 0x02};
    Msg_Header_t msgHeader = {.cmd = 0x00, .len =0x00};
    CHECK(unpackMsgHeader(buf, &msgHeader) == 0);
    CHECK(msgHeader.cmd == buf[0]);
    CHECK(msgHeader.len == buf[1]);
};

TEST(MESSAGE_HANDLER, CatchNullPointer) {
    Msg_Header_t msgHeader = {.cmd = 0x00, .len =0x00};
    CHECK(unpackMsgHeader(NULL, &msgHeader) == -1);
};

TEST(MESSAGE_HANDLER, UnpackCmd1Normal) {
    uint8_t buf[] = {0x01, 0x02, 0x03};
    Cmd1_uMsg_t cmd1 = {.msg = {
                                .header = {.cmd = 0x00, .len = 0x00},
                                .payload = {.adcChannel = 0x00}
                                }
                        };

    int8_t rtn;
    rtn = unpackCmd1(buf, &cmd1);
    CHECK(rtn == 0);
    CHECK(cmd1.msg.header.cmd == buf[0]);
    CHECK(cmd1.msg.header.len == buf[1]);
    CHECK(cmd1.msg.payload.adcChannel == buf[2]);
}


TEST(MESSAGE_HANDLER, UnpackCmd1Null) {

    Cmd1_uMsg_t cmd1 = {.msg = {
                                .header = {.cmd = 0x00, .len = 0x00},
                                .payload = {.adcChannel = 0x00}
                                }
                        };
    int8_t rtn;
    rtn = unpackCmd1(NULL, &cmd1);
    CHECK(rtn == -1);
}



TEST(MESSAGE_HANDLER, UnpackCmd2Normal) {
    uint8_t buf[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    Cmd2_uMsg_t cmd2 = {.msg = {
                                .header = {.cmd = 0x00, .len = 0x00},
                                .payload = {.dacChannel = 0x00, .value = 0x00}
                                }
                        };

    int8_t rtn;
    rtn = unpackCmd2(buf, &cmd2);
    CHECK(rtn == 0);
    CHECK(cmd2.msg.header.cmd == buf[0]);
    CHECK(cmd2.msg.header.len == buf[1]);
    CHECK(cmd2.msg.payload.dacChannel == buf[2]);
    CHECK(cmd2.msg.payload.value == (((uint16_t)buf[3]) << 8) + (uint16_t)buf[4]);
    for(uint8_t i = 0; i < sizeof(Cmd2_uMsg_t)/sizeof(uint8_t); i++) {
        printf("byte %i = %x\n", i, cmd2.bytes[i]);
    }
    printf("value = %x, %x\n",cmd2.bytes[3],cmd2.bytes[4]);
    printf("value = %x\n", cmd2.msg.payload.value);
    buf[3] = 0x8;
    buf[4] = 0x9;
    printf("value = %x\n", cmd2.msg.payload.value);
}


TEST(MESSAGE_HANDLER, UnpackCmd2Null) {

    Cmd2_uMsg_t cmd2 = {.msg = {
                                .header = {.cmd = 0x00, .len = 0x00},
                                .payload = {.dacChannel = 0x00, .value = 0x00}
                                }
                        };
    int8_t rtn;
    rtn = unpackCmd2(NULL, &cmd2);
    CHECK(rtn == -1);
}
