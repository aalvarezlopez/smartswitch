#ifndef _CONF_SD_MMC_SPI_H_
#define _CONF_SD_MMC_SPI_H_

#include "conf_access.h"

#if SD_MMC_SPI_MEM == DISABLE
    #error conf_sd_mmc_spi.h is #included although SD_MMC_SPI_MEM is disabled
#endif

#include "sd_mmc_spi.h"

//_____ D E F I N I T I O N S ______________________________________________

#define SD_MMC_SPI_MASTER_SPEED     12000000

#define SD_MMC_SPI_BITS             8


#endif  // _CONF_SD_MMC_SPI_H_