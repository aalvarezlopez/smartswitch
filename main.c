#include "stdint.h"
#include "stdbool.h"
#include "EcuM.h"
#include "enc28j60.h"
#include "io.h"

extern volatile bool OS_Task_A;
extern volatile bool OS_Task_B;
extern volatile bool OS_Task_C;

void SystemInit( void );


int main(void)
{
    SystemInit();
    EcuM_Startup_one();

    EcuM_Startup_two();
    SysTickStart();

    /*nothing else to do,
     * scheduler will run from the SystemTick ISR
     */
    while (true) {
        if(OS_Task_A == true){
            SPI_Task();
            SmartSwitch_Task();
            ADC_Task();
            OS_Task_A = false;
        }
        if(OS_Task_B == true){
            ENC_Task();
            EtherCard_Task();
            TCPIP_Task();
            OS_Task_B = false;
        }
        if(OS_Task_C == true){
            DS18B20_Task();
            SmartSwitch_SlowTask();
            OS_Task_C = false;
        }

    }

    return 0;
}
