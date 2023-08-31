/**
* @file utest_fluid_ctrl.c

* @brief Unit test for fluid control application
* @author Adrian Alvarez Lopez
* @version 1.0.0
* @date 2022-01-15
*/

#include "CUnit/Basic.h"
#include "stdbool.h"
#include "fluid_ctrl.h"
#include "fluid_ctrl_pri.h"
#include "fluid_ctrl_cfg.h"

extern uint32_t raw_pulses_counter;
extern bool evalve_state;
extern uint32_t q_calibration_pulses_liter;
extern windows_et current_window;
extern uint8_t battery_percentage;
extern bool _mock_new_acq_triggered;
extern uint8_t _mock_last_color[];
extern uint16_t _mock_adc_value;
extern bool _mock_ad_aquired;
extern char _mok_printed_str[][128];
extern uint16_t _mok_printed_attr[][3];
extern uint8_t _mok_printed_str_counter;

int init_suite1(void)
{
    return 0;
}

int clean_suite1(void)
{
    return 0;
}

void testInit(void)
{
    _mock_new_acq_triggered = false;
    FluidCtrl_Init();
    CU_ASSERT_EQUAL( current_window, FLUID_CTRL_MAIN_WIN);
    CU_ASSERT_TRUE( _mock_new_acq_triggered );
    CU_ASSERT_EQUAL( _mock_last_color[0], 255);
    CU_ASSERT_EQUAL( _mock_last_color[1], 255);
    CU_ASSERT_EQUAL( _mock_last_color[2], 255);
}

void testTask(void)
{
    _mock_ad_aquired = false;
    _mock_new_acq_triggered = false;
    FluidCtrl_Task();
    CU_ASSERT_FALSE( _mock_new_acq_triggered );

    _mock_ad_aquired = true;
    _mock_adc_value = 3500;
    FluidCtrl_Task();
    CU_ASSERT_TRUE( _mock_new_acq_triggered );
    CU_ASSERT_EQUAL( battery_percentage , 100);

    _mock_adc_value = 3300;
    FluidCtrl_Task();
    CU_ASSERT_EQUAL( battery_percentage , 100);

    _mock_adc_value = 2700;
    FluidCtrl_Task();
    CU_ASSERT_EQUAL( battery_percentage , 0);

    _mock_adc_value = 2500;
    FluidCtrl_Task();
    CU_ASSERT_EQUAL( battery_percentage , 0);

    _mock_adc_value = 3000;
    FluidCtrl_Task();
    CU_ASSERT_EQUAL( battery_percentage , 50);

    _mock_adc_value = 3150;
    FluidCtrl_Task();
    CU_ASSERT_EQUAL( battery_percentage , 75);

    _mock_adc_value = 2850;
    FluidCtrl_Task();
    CU_ASSERT_EQUAL( battery_percentage , 25);
}

void testUserRequest(void)
{
    current_window = FLUID_CTRL_MAIN_WIN;
    FluidCtrl_UserPressedButton( UP_BUTTON );
    CU_ASSERT_EQUAL( current_window, FLUID_CTRL_MAIN_WIN);
    FluidCtrl_UserPressedButton( DOWN_BUTTON );
    CU_ASSERT_EQUAL( current_window, FLUID_CTRL_MAIN_WIN);

    current_window = FLUID_CTRL_HISTORY_WIN;
    FluidCtrl_UserPressedButton( UP_BUTTON );
    CU_ASSERT_EQUAL( current_window, FLUID_CTRL_HISTORY_WIN);
    FluidCtrl_UserPressedButton( DOWN_BUTTON );
    CU_ASSERT_EQUAL( current_window, FLUID_CTRL_HISTORY_WIN);

    current_window = FLUID_CTRL_CFG_WIN;
    FluidCtrl_UserPressedButton( UP_BUTTON );
    CU_ASSERT_EQUAL( current_window, FLUID_CTRL_CFG_WIN);
    FluidCtrl_UserPressedButton( DOWN_BUTTON );
    CU_ASSERT_EQUAL( current_window, FLUID_CTRL_CFG_WIN);

    current_window = FLUID_CTRL_MAIN_WIN;
    FluidCtrl_UserPressedButton( LEFT_BUTTON );
    CU_ASSERT_EQUAL( current_window, FLUID_CTRL_HISTORY_WIN);

    current_window = FLUID_CTRL_CFG_WIN;
    FluidCtrl_UserPressedButton( LEFT_BUTTON );
    CU_ASSERT_EQUAL( current_window, FLUID_CTRL_CFG_WIN);

    current_window = FLUID_CTRL_HISTORY_WIN;
    FluidCtrl_UserPressedButton( LEFT_BUTTON );
    CU_ASSERT_EQUAL( current_window, FLUID_CTRL_HISTORY_WIN);

    current_window = FLUID_CTRL_MAIN_WIN;
    FluidCtrl_UserPressedButton( RIGHT_BUTTON );
    CU_ASSERT_EQUAL( current_window, FLUID_CTRL_CFG_WIN);

    current_window = FLUID_CTRL_CFG_WIN;
    FluidCtrl_UserPressedButton( RIGHT_BUTTON );
    CU_ASSERT_EQUAL( current_window, FLUID_CTRL_MAIN_WIN);

    current_window = FLUID_CTRL_HISTORY_WIN;
    FluidCtrl_UserPressedButton( RIGHT_BUTTON );
    CU_ASSERT_EQUAL( current_window, FLUID_CTRL_MAIN_WIN);
}

void testControl(void)
{
    raw_pulses_counter = 100;
    evalve_state = false;
    FluidCtrl_set_evalve_enable(true);
    CU_ASSERT_EQUAL( raw_pulses_counter, 0);
    CU_ASSERT_TRUE(evalve_state);
    CU_ASSERT_TRUE( FluidCtrl_get_evalve_enable());

    raw_pulses_counter = 100;
    FluidCtrl_set_evalve_enable(true);
    CU_ASSERT_EQUAL( raw_pulses_counter, 100);
    CU_ASSERT_TRUE(evalve_state);
    CU_ASSERT_TRUE( FluidCtrl_get_evalve_enable());

    FluidCtrl_set_evalve_enable(false);
    CU_ASSERT_FALSE(evalve_state);
    CU_ASSERT_FALSE( FluidCtrl_get_evalve_enable());
    FluidCtrl_set_evalve_enable(false);
    CU_ASSERT_FALSE(evalve_state);
    CU_ASSERT_FALSE( FluidCtrl_get_evalve_enable());
}

void testQCounter(void)
{
    raw_pulses_counter = 800;
    q_calibration_pulses_liter = 1;
    CU_ASSERT_EQUAL( FluidCtrl_get_liters_0_10(),  800);

    raw_pulses_counter = 800;
    q_calibration_pulses_liter = 2;
    CU_ASSERT_EQUAL( FluidCtrl_get_liters_0_10(),  400);

    FluidCtr_refresh_calibration( 100 );
    CU_ASSERT_EQUAL( q_calibration_pulses_liter, 10);
    raw_pulses_counter = 100;
    CU_ASSERT_EQUAL( FluidCtrl_get_liters_0_10(),  10);
    for(uint8_t i = 0; i < 100; i++){
        FluidCtrl_increase_q_pulse();
    }
    CU_ASSERT_EQUAL( FluidCtrl_get_liters_0_10(),  20);

}

void testCentralWidget(void)
{
    raw_pulses_counter = 100;
    FluidCtr_refresh_calibration( 100 );

    _mok_printed_str_counter = 0;
    FluidCtrl_Task();

    CU_ASSERT_STRING_EQUAL( _mok_printed_str[0], "04/01/2022");
    CU_ASSERT_STRING_EQUAL( _mok_printed_str[1], "08:45");
    CU_ASSERT_STRING_EQUAL( _mok_printed_str[2], "1");
    CU_ASSERT_STRING_EQUAL( _mok_printed_str[3], "0");

    raw_pulses_counter = 150;
    FluidCtr_refresh_calibration( 100 );

    _mok_printed_str_counter = 0;
    FluidCtrl_Task();

    CU_ASSERT_STRING_EQUAL( _mok_printed_str[0], "04/01/2022");
    CU_ASSERT_STRING_EQUAL( _mok_printed_str[1], "08:45");
    CU_ASSERT_STRING_EQUAL( _mok_printed_str[2], "1");
    CU_ASSERT_STRING_EQUAL( _mok_printed_str[3], "5");

}

void testCallbackFunction(void)
{
    char msg[1024];
    char reply[1024];
    FluidCtr_new_message_cbk(msg, reply);
    CU_ASSERT_STRING_EQUAL( reply, "NACK");

    strcpy( msg, "FLD_CMD: STATUS");
    q_calibration_pulses_liter = 2;
    raw_pulses_counter = 50;
    FluidCtr_new_message_cbk(msg, reply);
    CU_ASSERT_STRING_EQUAL( reply, "ACK_L=25");

    q_calibration_pulses_liter = 1;
    raw_pulses_counter = 50;
    FluidCtr_new_message_cbk(msg, reply);
    CU_ASSERT_STRING_EQUAL( reply, "ACK_L=50");

    q_calibration_pulses_liter = 1;
    raw_pulses_counter = 10;
    FluidCtr_new_message_cbk(msg, reply);
    CU_ASSERT_STRING_EQUAL( reply, "ACK_L=10");

    strcpy( msg, "FLD_CMD: VALVE=ON");
    FluidCtr_new_message_cbk(msg, reply);
    CU_ASSERT_STRING_EQUAL( reply, "ACK");
    CU_ASSERT_TRUE( evalve_state );

    strcpy( msg, "FLD_CMD: VALVE=OFF");
    FluidCtr_new_message_cbk(msg, reply);
    CU_ASSERT_STRING_EQUAL( reply, "ACK");
    CU_ASSERT_FALSE( evalve_state );

    raw_pulses_counter = 100;
    strcpy( msg, "FLD_CMD: VALVE=ON");
    FluidCtr_new_message_cbk(msg, reply);
    CU_ASSERT_STRING_EQUAL( reply, "ACK");
    CU_ASSERT_TRUE( evalve_state );
    CU_ASSERT_EQUAL( raw_pulses_counter, 0 );

    strcpy( msg, "FLD_CMD: THD=");
    FluidCtr_new_message_cbk(msg, reply);
    CU_ASSERT_STRING_EQUAL( reply, "ACK");

    strcpy( msg, "FLD_CMD: THD");
    FluidCtr_new_message_cbk(msg, reply);
    CU_ASSERT_STRING_EQUAL( reply, "NACK");
}


/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main()
{
    CU_pSuite pSuite = NULL;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    /* add a suite to the registry */
    pSuite = CU_add_suite("Core suite case", init_suite1, clean_suite1);

    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "Fluid control initialization",
                             testInit)) ||
        (NULL == CU_add_test(pSuite, "Fluid control main task",
                             testTask)) ||
        (NULL == CU_add_test(pSuite, "Fluid control pulse counter behaviour",
                             testQCounter)) ||
        (NULL == CU_add_test(pSuite, "Fluid control pulse counter behaviour",
                             testCentralWidget)) ||
        (NULL == CU_add_test(pSuite, "Fluid control external requests",
                             testControl)) ||
        (NULL == CU_add_test(pSuite, "Fluid control message has been received",
                             testCallbackFunction)) ||
        (NULL == CU_add_test(pSuite, "User request such as button pressed",
                             testUserRequest))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
