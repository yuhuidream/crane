#ifndef BBU_TEST_H
#define BBU_TEST_H

//#define BBU_TEST_PASS       (1<<0)    /* Test failed */
//#define BBU_TEST_FAIL       (1<<1)    /* Test pass */
//#define BBU_TEST_CHECK      (1<<2)    /* Test completed, and need people to check */
//#define BBU_TEST_TIMEOUT    (1<<3)    /* Do not receive ack from test owner in time */
//#define BBU_TEST_UNKNOWN    (1<<4)    /* Unknown test command */

#define BBU_TEST_INIT       0    /* Test init state */
#define BBU_TEST_PASS       1    /* Test pass */
#define BBU_TEST_FAIL       2    /* Test failed */
#define BBU_TEST_CHECK      3    /* Test completed, and need people to check */
#define BBU_TEST_TIMEOUT    4    /* Do not receive ack from test owner in time */
#define BBU_TEST_UNKNOWN    5    /* Unknown test command */

#define TEST_TRUE 1
#define TEST_FALSE 0

#define TEST_ACK_FROM_SCREEN BIT_7

typedef struct ATEReqEvtTag
{
	QEvent super;
	uint8_t type;  /* 1-start test, 0-clear status */
	uint8_t group_id;
	
} ATEReqEvt;

typedef struct ATEAckEvtTag
{
	QEvent super;
	uint8_t result;  
	uint8_t cmd_index;
	
} ATEAckEvt;

void screen_cmd_ack(uint8_t result, uint8_t cmd_index);

#endif



