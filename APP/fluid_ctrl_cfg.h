/**
 * @file fluid_ctrl_cfg.h
 * @brief Configuration parameters for the fluid control application
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-01-19
 */

#ifndef FLUID_CTRL_CFG_H
#define FLUID_CTRL_CFG_H

/*******************************************************************************
 * LAYOUT definition
*******************************************************************************/
#define LAYOUT_TOP_LINE_START_X    0u
#define LAYOUT_TOP_LINE_START_Y    19u
#define LAYOUT_SCREEN_WIDTH        240u
#define LAYOUT_SCREEN_HEIGHT       320u
#define LAYOUT_BOTTOM_LINE_START_X 0u
#define LAYOUT_BOTTOM_HEIGHT       40u
#define LAYOUT_BOTTOM_LINE_START_Y (LAYOUT_SCREEN_HEIGHT - LAYOUT_BOTTOM_HEIGHT)

#define LAYOUT_MAIN_PADDING 5u

#define LAYOUT_WIDGET_HEIGHT (LAYOUT_BOTTOM_LINE_START_Y - LAYOUT_TOP_LINE_START_Y - (2*LAYOUT_MAIN_PADDING))

/*******************************************************************************
 * Main windows color setup
 *******************************************************************************/
#define MAIN_BKGD_RED    33u
#define MAIN_BKGD_GREEN  177u
#define MAIN_BKGD_BLUE   105

#define MAIN_OFF_BKGD_RED    96u
#define MAIN_OFF_BKGD_GREEN  96u
#define MAIN_OFF_BKGD_BLUE   96u

#define MAIN_FGD_RED     255u
#define MAIN_FGD_GREEN   255u
#define MAIN_FGD_BLUE    255u

#define ICON_CONNECTED_RED     255u
#define ICON_CONNECTED_GREEN   255u
#define ICON_CONNECTED_BLUE    255u

#define ICON_IDLE_RED     241u
#define ICON_IDLE_GREEN   196u
#define ICON_IDLE_BLUE    15u

#define FONT_FGD_VALVE_ON_RED 241u
#define FONT_FGD_VALVE_ON_GREEN 196u
#define FONT_FGD_VALVE_ON_BLUE 15u

#define FONT_WARNING_RED    232u
#define FONT_WARNING_GREEN  14u
#define FONT_WARNING_BLUE   15u


/*******************************************************************************
 * Menu and history widget color setup
 ******************************************************************************/
#define SECONDARY_WIDGET_BCGD_RED       146u
#define SECONDARY_WIDGET_BCGD_GREEN     250u
#define SECONDARY_WIDGET_BCGD_BLUE      198u

#define SECONDARY_WIDGET_FG_RED       51u
#define SECONDARY_WIDGET_FG_GREEN     51u
#define SECONDARY_WIDGET_FG_BLUE      255u

/*******************************************************************************
 * Battery widget color setup
 *******************************************************************************/
#define BATTERY_WIDGET_RED     4u
#define BATTERY_WIDGET_GREEN   102u
#define BATTERY_WIDGET_BLUE    204u

/*******************************************************************************
 * Top menu widget positions
 *******************************************************************************/
#define DATE_START_POINT_X 2u
#define DATE_START_POINT_Y 5u

#define HOUR_START_POINT_X 100u
#define HOUR_START_POINT_Y 5u

#define STATUS_ICON_START_POINT_X 180u
#define STATUS_ICON_START_POINT_Y 7u
#define STATUS_ICON_LENGTH  10u
#define STATUS_ICON_GAP_WIDTH  2u

#define BATTERY_START_POINT_X 200u
#define BATTERY_START_POINT_Y 3u
#define BATTERY_LEN           25u
#define BATTERY_HEIGHT        15u
#define BULLET_HEIGHT         6u
#define BULLET_WIDTH          2u
#define BATTERY_PADDING       2u

#define LEFT_MENU_START_X     20u
#define RIGHT_MENU_START_X    140u
#define BOTTOM_MENU_STR_Y     (LAYOUT_BOTTOM_LINE_START_Y + 5)

/*******************************************************************************
 * Bottom menu widget positions
 *******************************************************************************/
#define BOTTOM_MENU_ACTION_LEN 5u
#define LEFT_MENU_ACTION_MW  "LIST"
#define RIGHT_MENU_ACTION_MW "MENU"
#define RIGHT_MENU_ACTION_SD "BACK"
#define MENU_NO_ACTION       "----"
#define LEFT_MENU_ACTION_CFG "DECR"
#define RIGHT_MENU_ACTION_CFG "INCR"
#define BOTTOM_MENU_FONT_SIZE 3u

/*******************************************************************************
 * Main window widget positions
 *******************************************************************************/
#define LITERS_DISPLAY_START_X  5u
#define LITERS_DISPLAY_START_Y  60u
#define LITERS_FONT_SIZE        12u
#define DEC_LITERS_DISPLAY_START_X 20u
#define DEC_LITERS_DISPLAY_START_Y 160u
#define DEC_LITERS_FONT_SIZE       6u
#define DEC_POINT_START_X          5u
#define DEC_POINT_START_Y          188u
#define DEC_POINT_SIZE             8u
#define WARNING_DISPLAY_START_X    20u
#define WARNING_DISPLAY_START_Y    240u
#define WARNING_FONT_SIZE          2u

#define LITERS_UNITS_TEXT_START_POINT_X 125u
#define LITERS_UNITS_TEXT_START_POINT_Y 190u
#define LITERS_UNIT_FONT_TEXT           2u
#define LITERS_UNIT_TEXT                "liters"

/*******************************************************************************
 * Project name position and name
 ******************************************************************************/
#define PROJ_NAME_TEXT_START_POINT_X 40u
#define PROJ_NAME_TEXT_START_POINT_Y (LAYOUT_BOTTOM_LINE_START_Y - 15)
#define PROJ_NAME_FONT_TEXT           2u
#define PROJ_NAME_TEXT               "Viquina Fluid Control"

/*******************************************************************************
 * NETWORK parameters
 *******************************************************************************/
#define FLUIDCTRL_MISSING_TIMEOUT_WARNING 50
#define FLUIDCTRL_MISSING_TIMEOUT_ERROR   100

#endif
