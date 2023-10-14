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

    /*nothing else to do,
     * scheduler will run from the SystemTick ISR
     */
    while (true) {
        #if 0
        if(OS_systemtick == true){
            SPI_Task();
            FluidCtrl_Task();
            //DS18B20_Task();
            ENC_Task();
            OS_systemtick = false;
        }
        #endif

    }

    return 0;
}
