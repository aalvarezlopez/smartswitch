/**
 * @file fluid_ctrl_pri.h
 * @brief Private header for Fluid Control application
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-01-18
 */

#ifndef FLUID_CTRL_PRI_H
#define FLUID_CTRL_PRI_H


/**
 * @brief Each of the possible windows the device
 * can display
 */
typedef enum windows_e {
    FLUID_CTRL_MAIN_WIN,
    FLUID_CTRL_HISTORY_WIN,
    FLUID_CTRL_CFG_WIN,
    FLUID_CTRL_NWINDOWS
} windows_et;


/* Connect to the RTC to get date and time*/
#define FLUID_CTRL_DATE(y, m, d, out) { out[0] = '0' + (d >>4); out[1] = '0' + (d & 0xF);\
	out[3] = '0' + (m >> 4); out[4] = '0' + (m & 0xF);\
	out[6] =  '2'; out[7] =  '0'; out[8] = '0' + (y >> 4); out[9] = '0' + (y & 0xF);\
	out[2] = '/'; out[5] = '/'; out[10] = 0;}
#define FLUID_CTRL_HOUR(h, m, s, out) { out[0] = '0' + (h >>4); out[1] = '0' + (h & 0xF);\
	out[3] = '0' + (m >> 4); out[4] = '0' + (m & 0xF); out [2] = ':'; out[5] = 0;}

/* Connect to a valid algorithm which will calculate the memory level percentage*/
#define BATTERY_FILL_PERCENTAGE(x)  \
    ((uint32_t)(BATTERY_LEN - (2 * BATTERY_PADDING)) * 100) / x

/* Temporary used for calculating the battery level*/
#define BATTERY_LEVEL_HIGHEST_LEVEL 3300u
#define BATTERY_LEVEL_LOWEST_LEVEL 2700u

static inline void fluid_ctrl_refresh_top_menu(void);
static inline void fluid_ctrl_refresh_bottom_menu(void);
static inline void fluid_ctrl_refresh_main_central_widget(bool);
static inline void fluid_ctrl_refresh_history_widget(bool);
static inline void fluid_ctrl_refresh_config_widget(bool);
static inline void fluid_ctrl_print_layout(void);
static inline void fluid_ctrl_print_conn_icon(const uint8_t * const rgb);
static inline void fluid_ctrl_report_warning(const char * str);
static inline void fluid_ctrl_clean_warning(void);
static inline void fluid_ctrl_ip_string(char * const ip);
static inline void fluid_ctrl_configure_parameter(void);
static inline void fluid_ctrl_update_calibration(void);
static inline void fluid_ctrl_increase_current_parameter(void);
static inline void fluid_ctrl_decrease_current_parameter(void);
static inline void fluid_ctrl_set_current_parameter(void);
void fluidctrl_refreshBattery_percentage(uint16_t mv);
void fluid_ctrl_refresh_central_widget(void);

#endif
