#include "stdint.h"
#include "stdbool.h"
#include "EcuM.h"
#include "enc28j60.h"

extern volatile bool OS_systemtick;

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
        if(OS_systemtick == true){
            SPI_Task();
            FluidCtrl_Task();
            //DS18B20_Task();
            ENC_Task();
            EtherCard_Task();
            TCPIP_Task();
            OS_systemtick = false;
        }
    }

    return 0;
}
