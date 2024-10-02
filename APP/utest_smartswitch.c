/**
* @file utest_fluid_ctrl.c

* @brief Unit test for fluid control application
* @author Adrian Alvarez Lopez
* @version 1.0.0
* @date 2022-01-15
*/

#include <CUnit/CUnitCI.h>
#include "stdbool.h"
#include "string.h"
#include "smartswitch.h"

extern char SmartSwitch_cdc_buffer[];
extern char *SmartSwitch_cdc_buffer_head;
extern char *SmartSwitch_cdc_buffer_tail;
extern uint16_t mock_cdc_buff_len;
extern char mock_cdc_buff[];
extern uint8_t ipaddress[4];
extern uint16_t deviceid;

void checkValidCdcMessage(char * msg, uint8_t *ip, uint16_t id)
{
    strcpy(SmartSwitch_cdc_buffer, msg);
    SmartSwitch_cdc_buffer_head = SmartSwitch_cdc_buffer + strlen(msg);
    SmartSwitch_cdc_buffer_tail = SmartSwitch_cdc_buffer;
    SmartSwitch_Task();
    CU_ASSERT_EQUAL( mock_cdc_buff_len, 3);
    CU_ASSERT_STRING_EQUAL( mock_cdc_buff, "ACK" );
    CU_ASSERT_EQUAL( ip[0], ipaddress[0] );
    CU_ASSERT_EQUAL( ip[1], ipaddress[1] );
    CU_ASSERT_EQUAL( ip[2], ipaddress[2] );
    CU_ASSERT_EQUAL( ip[3], ipaddress[3] );
    CU_ASSERT_EQUAL( id, deviceid);

    mock_cdc_buff_len = 0;
    memset(mock_cdc_buff, 0, 64);
}

CU_SUITE_SETUP()
{
    return 0;
}

CI_SUITE_TEARDOWN()
{
    return 0;
}

CU_TEST_SETUP()
{
}

CU_TEST_TEARDOWN()
{
}

void testInit(void)
{
    CU_ASSERT_EQUAL( 0, 0);
}

void testCdcParsingMessage(void)
{
    char msg[100];
    uint8_t ip[4] = {192, 168, 1, 117};

    strcpy( msg, "#192168001117$001#");
    checkValidCdcMessage(msg, ip, 1);

    ip[1] = 203;
    strcpy( msg, "aa12wcasd908#192203001117$002#");
    SmartSwitch_Task();
    checkValidCdcMessage(msg, ip, 2);

    ip[2] = 109;
    strcpy( msg, "#####192203109117$259#####");
    checkValidCdcMessage(msg, ip, 259);

    ip[2] = 172;
    strcpy( msg, "#####192203172117$001#....asdfw");
    checkValidCdcMessage(msg, ip, 1);

    ip[0] = 199;
    ip[1] = 168;
    ip[2] = 1;
    ip[3] = 1;
    strcpy( msg, "#####192203172001$001######199168001001$999#");
    checkValidCdcMessage(msg, ip, 999);
    SmartSwitch_Task();
    CU_ASSERT_EQUAL( mock_cdc_buff_len, 3);
    CU_ASSERT_STRING_EQUAL( mock_cdc_buff, "ACK" );
}

void testCdcBufferOverflow(void)
{
    uint8_t ip[4] = {10, 12, 13, 14};
    mock_cdc_buff_len = 0;
    memset(mock_cdc_buff, 0, 64);

    strcpy(SmartSwitch_cdc_buffer + APP_MAX_BUFF_LEN - 8, "#010012");
    strcpy(SmartSwitch_cdc_buffer, "013014$703#");
    SmartSwitch_cdc_buffer_head = SmartSwitch_cdc_buffer + 12;
    SmartSwitch_cdc_buffer_tail = SmartSwitch_cdc_buffer + APP_MAX_BUFF_LEN - 8;
    SmartSwitch_Task();
    CU_ASSERT_EQUAL( mock_cdc_buff_len, 3);
    CU_ASSERT_STRING_EQUAL( mock_cdc_buff, "ACK" );
    CU_ASSERT_EQUAL( 10, ipaddress[0] );
    CU_ASSERT_EQUAL( 12, ipaddress[1] );
    CU_ASSERT_EQUAL( 13, ipaddress[2] );
    CU_ASSERT_EQUAL( 14, ipaddress[3] );
    CU_ASSERT_EQUAL( 703, deviceid);
}
void testCdcQuery(void)
{
    char msg[100];

    mock_cdc_buff_len = 0;
    memset(mock_cdc_buff, 0, 64);
    strcpy( SmartSwitch_cdc_buffer, "#145201122234$632#");
    SmartSwitch_cdc_buffer_head = SmartSwitch_cdc_buffer + strlen(SmartSwitch_cdc_buffer);
    SmartSwitch_cdc_buffer_tail = SmartSwitch_cdc_buffer;
    SmartSwitch_Task();

    strcpy(SmartSwitch_cdc_buffer, "#?");
    SmartSwitch_cdc_buffer_head = SmartSwitch_cdc_buffer+2;
    SmartSwitch_cdc_buffer_tail = SmartSwitch_cdc_buffer;
    SmartSwitch_Task();
    CU_ASSERT_EQUAL( mock_cdc_buff_len, 18);
    CU_ASSERT_STRING_EQUAL( mock_cdc_buff, "#145201122234$632#");
}

CUNIT_CI_RUN("smartswitchapp-suite",
                             CUNIT_CI_TEST(testInit),
                             CUNIT_CI_TEST(testCdcParsingMessage),
                             CUNIT_CI_TEST(testCdcBufferOverflow),
                             CUNIT_CI_TEST(testCdcQuery)
                             );
