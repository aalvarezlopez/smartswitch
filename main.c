#include "stdint.h"
#include "stdbool.h"
#include "EcuM.h"
#include "enc28j60.h"

extern volatile bool OS_Task_A;
extern volatile bool OS_Task_B;

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
            FluidCtrl_Task();
            DS18B20_Task();
            OS_Task_A = false;
        }
        if(OS_Task_B == true){
            ENC_Task();
            EtherCard_Task();
            TCPIP_Task();
            OS_Task_B = false;
        }
    }

    return 0;
}
