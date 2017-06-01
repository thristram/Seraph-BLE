/******************************************************************************
*  Copyright 2015 - 2016 Qualcomm Technologies International, Ltd.
*  Bluetooth Low Energy CSRmesh 2.1
*  CSRmesh is a product of Qualcomm Technologies International Ltd.
*/
/*! \file csr_mesh_model_common.h
*
*  \brief This files defines data types and constants that are commonly used
*         across models.
*/
/**************************************************************************** */

#ifndef __CSR_MESH_MODEL_COMMON_H__
#define __CSR_MESH_MODEL_COMMON_H__

#include "csr_types.h"
#include "csr_mesh_result.h"
#include "sensor_types.h"

/*! \addtogroup Models
 * @{
 */
#define CSR_MESH_DEFAULT_NETID          (0)
#define CSR_MESH_DEFAULT_TTL            (50)

/* Macros for accessing model bit mask */
#define MCP_MODELS_FIELD_SIZE_IN_BYTES  (16)
#define MCP_MODEL_BYTE_MASK(model)      (0x1 << ((model) & 0x07))
#define MCP_MODEL_BYTE_INDEX(model)     ((model) >> 3)

#define MCP_MODELS_FIELD_SIZE_IN_WORDS  (MCP_MODELS_FIELD_SIZE_IN_BYTES/2)
#define MCP_MODEL_WORD_MASK(model)      (0x1 << ((model) & 0x0F))
#define MCP_MODEL_WORD_INDEX(model)     ((model) >> 4)

typedef CsrUint8 * CsrUint8Ptr;

/* The CSR101x XAP processor can access a minimum of 16bit. The CsrUint8 
 * type actually takes 16 bits on memory and the sizeof(CsrUint16) returns 1
 * This definition will be used in array declarations of 16 bit types.
 */
#if (defined CSR101x) || (defined CSR101x_A05)
#define size_in_bytes(type) SIZE_IN_BYTES_##type

#define SIZE_IN_BYTES_CsrUint8       1
#define SIZE_IN_BYTES_CsrUint16      2
#define SIZE_IN_BYTES_sensor_type_t  2
#else
#define size_in_bytes(type) sizeof(type)
#endif

typedef CsrUint16 csr_mesh_temperature_kelvin_t;
typedef CsrUint16 csr_mesh_temperature_kelvin_range_t;
typedef CsrUint16 csr_mesh_percentage_t;
typedef CsrUint16 csr_mesh_watts_per_square_metre_t;
typedef CsrUint16 csr_mesh_16_bit_counter_t;
typedef CsrUint16 csr_mesh_parts_per_million_t;
typedef CsrUint16 csr_mesh_micrograms_per_cubic_metre_t;
typedef CsrUint16 csr_mesh_minutes_of_the_day_t;
typedef CsrUint16 csr_mesh_water_flow_rate_t;
typedef CsrUint16 csr_mesh_decibel_t;
typedef CsrUint16 csr_mesh_centimetres_per_second_t;
typedef CsrUint16 csr_mesh_millimetres_t;
typedef CsrUint16 csr_mesh_air_pressure_t;
typedef CsrUint16 csr_mesh_generic_1_byte_t;
typedef CsrUint16 csr_mesh_generic_2_byte_t;
typedef CsrUint16 csr_mesh_generic_1_byte_typed_t;
typedef CsrUint16 csr_mesh_generic_2_byte_typed_t;
typedef CsrUint16 csr_mesh_generic_3_byte_typed_t;

/*! \brief CSRmesh boolean type */
typedef enum
{
    csr_mesh_boolean_false = 0, /*!< \brief false */
    csr_mesh_boolean_true = 1 /*!< \brief true */
}csr_mesh_boolean_t;
/*! \brief CSRmesh power_state type */
typedef enum
{
    csr_mesh_power_state_off = 0, /*!< \brief off */
    csr_mesh_power_state_on = 1, /*!< \brief on */
    csr_mesh_power_state_standby = 2, /*!< \brief standby */
    csr_mesh_power_state_onfromstandby = 3 /*!< \brief onfromstandby */
}csr_mesh_power_state_t;
/*! \brief CSRmesh device_information type */
typedef enum
{
    csr_mesh_device_information_uuid_low = 0, /*!< \brief uuid_low */
    csr_mesh_device_information_uuid_high = 1, /*!< \brief uuid_high */
    csr_mesh_device_information_model_low = 2, /*!< \brief model_low */
    csr_mesh_device_information_model_high = 3, /*!< \brief model_high */
    csr_mesh_device_information_vid_pid_version = 4, /*!< \brief vid_pid_version */
    csr_mesh_device_information_appearance = 5, /*!< \brief appearance */
    csr_mesh_device_information_lastetag = 6 /*!< \brief lastetag */
}csr_mesh_device_information_t;
/*! \brief CSRmesh key_properties type */
typedef enum
{
    csr_mesh_key_properties_administrator = 0, /*!< \brief administrator */
    csr_mesh_key_properties_user = 1, /*!< \brief user */
    csr_mesh_key_properties_guest = 2, /*!< \brief guest */
    csr_mesh_key_properties_relayonly = 3 /*!< \brief relayonly */
}csr_mesh_key_properties_t;
/*! \brief CSRmesh month_of_year type */
typedef enum
{
    csr_mesh_month_of_year_unknown = 0, /*!< \brief unknown */
    csr_mesh_month_of_year_january = 1, /*!< \brief january */
    csr_mesh_month_of_year_february = 2, /*!< \brief february */
    csr_mesh_month_of_year_march = 3, /*!< \brief march */
    csr_mesh_month_of_year_april = 4, /*!< \brief april */
    csr_mesh_month_of_year_may = 5, /*!< \brief may */
    csr_mesh_month_of_year_june = 6, /*!< \brief june */
    csr_mesh_month_of_year_july = 7, /*!< \brief july */
    csr_mesh_month_of_year_august = 8, /*!< \brief august */
    csr_mesh_month_of_year_september = 9, /*!< \brief september */
    csr_mesh_month_of_year_october = 10, /*!< \brief october */
    csr_mesh_month_of_year_november = 11, /*!< \brief november */
    csr_mesh_month_of_year_december = 12 /*!< \brief december */
}csr_mesh_month_of_year_t;
/*! \brief CSRmesh timer_mode type */
typedef enum
{
    csr_mesh_timer_mode_programming = 0, /*!< \brief programming */
    csr_mesh_timer_mode_active = 1, /*!< \brief active */
    csr_mesh_timer_mode_partly_random = 2, /*!< \brief partly_random */
    csr_mesh_timer_mode_completely_random = 3 /*!< \brief completely_random */
}csr_mesh_timer_mode_t;
/*! \brief CSRmesh remote_code type */
typedef enum
{
    csr_mesh_remote_code_number_0 = 0, /*!< \brief number_0 */
    csr_mesh_remote_code_number_1 = 1, /*!< \brief number_1 */
    csr_mesh_remote_code_number_2 = 2, /*!< \brief number_2 */
    csr_mesh_remote_code_number_3 = 3, /*!< \brief number_3 */
    csr_mesh_remote_code_number_4 = 4, /*!< \brief number_4 */
    csr_mesh_remote_code_number_5 = 5, /*!< \brief number_5 */
    csr_mesh_remote_code_number_6 = 6, /*!< \brief number_6 */
    csr_mesh_remote_code_number_7 = 7, /*!< \brief number_7 */
    csr_mesh_remote_code_number_8 = 8, /*!< \brief number_8 */
    csr_mesh_remote_code_number_9 = 9, /*!< \brief number_9 */
    csr_mesh_remote_code_direction_n = 16, /*!< \brief direction_n */
    csr_mesh_remote_code_direction_e = 17, /*!< \brief direction_e */
    csr_mesh_remote_code_direction_s = 18, /*!< \brief direction_s */
    csr_mesh_remote_code_direction_w = 19, /*!< \brief direction_w */
    csr_mesh_remote_code_direction_ne = 20, /*!< \brief direction_ne */
    csr_mesh_remote_code_direction_se = 21, /*!< \brief direction_se */
    csr_mesh_remote_code_direction_sw = 22, /*!< \brief direction_sw */
    csr_mesh_remote_code_direction_nw = 23, /*!< \brief direction_nw */
    csr_mesh_remote_code_direction_nne = 24, /*!< \brief direction_nne */
    csr_mesh_remote_code_direction_ene = 25, /*!< \brief direction_ene */
    csr_mesh_remote_code_direction_ese = 26, /*!< \brief direction_ese */
    csr_mesh_remote_code_direction_sse = 27, /*!< \brief direction_sse */
    csr_mesh_remote_code_direction_ssw = 28, /*!< \brief direction_ssw */
    csr_mesh_remote_code_direction_wsw = 29, /*!< \brief direction_wsw */
    csr_mesh_remote_code_direction_wnw = 30, /*!< \brief direction_wnw */
    csr_mesh_remote_code_direction_nnw = 31, /*!< \brief direction_nnw */
    csr_mesh_remote_code_select = 32, /*!< \brief select */
    csr_mesh_remote_code_channel_up = 33, /*!< \brief channel_up */
    csr_mesh_remote_code_channel_down = 34, /*!< \brief channel_down */
    csr_mesh_remote_code_volume_up = 35, /*!< \brief volume_up */
    csr_mesh_remote_code_volume_down = 36, /*!< \brief volume_down */
    csr_mesh_remote_code_volume_mute = 37, /*!< \brief volume_mute */
    csr_mesh_remote_code_menu = 38, /*!< \brief menu */
    csr_mesh_remote_code_back = 39, /*!< \brief back */
    csr_mesh_remote_code_guide = 40, /*!< \brief guide */
    csr_mesh_remote_code_play = 41, /*!< \brief play */
    csr_mesh_remote_code_pause = 42, /*!< \brief pause */
    csr_mesh_remote_code_stop = 43, /*!< \brief stop */
    csr_mesh_remote_code_fast_forward = 44, /*!< \brief fast_forward */
    csr_mesh_remote_code_fast_rewind = 45, /*!< \brief fast_rewind */
    csr_mesh_remote_code_skip_forward = 46, /*!< \brief skip_forward */
    csr_mesh_remote_code_skip_backward = 47 /*!< \brief skip_backward */
}csr_mesh_remote_code_t;
/*! \brief CSRmesh sensor_type type */
typedef enum
{
    csr_mesh_sensor_type_unknown = 0, /*!< \brief unknown with values in the format csr_mesh__t*/
    csr_mesh_sensor_type_internal_air_temperature = 1, /*!< \brief internal_air_temperature with values in the format csr_mesh_temperature_kelvin_t*/
    csr_mesh_sensor_type_external_air_temperature = 2, /*!< \brief external_air_temperature with values in the format csr_mesh_temperature_kelvin_range_t*/
    csr_mesh_sensor_type_desired_air_temperature = 3, /*!< \brief desired_air_temperature with values in the format csr_mesh_temperature_kelvin_t*/
    csr_mesh_sensor_type_internal_humidity = 4, /*!< \brief internal_humidity with values in the format csr_mesh_percentage_t*/
    csr_mesh_sensor_type_external_humidity = 5, /*!< \brief external_humidity with values in the format csr_mesh_percentage_t*/
    csr_mesh_sensor_type_external_dewpoint = 6, /*!< \brief external_dewpoint with values in the format csr_mesh_temperature_kelvin_t*/
    csr_mesh_sensor_type_internal_door = 7, /*!< \brief internal_door with values in the format csr_mesh_door_state_t*/
    csr_mesh_sensor_type_external_door = 8, /*!< \brief external_door with values in the format csr_mesh_door_state_t*/
    csr_mesh_sensor_type_internal_window = 9, /*!< \brief internal_window with values in the format csr_mesh_window_state_t*/
    csr_mesh_sensor_type_external_window = 10, /*!< \brief external_window with values in the format csr_mesh_window_state_t*/
    csr_mesh_sensor_type_solar_energy = 11, /*!< \brief solar_energy with values in the format csr_mesh_watts_per_square_metre_t*/
    csr_mesh_sensor_type_number_of_activations = 12, /*!< \brief number_of_activations with values in the format csr_mesh_16_bit_counter_t*/
    csr_mesh_sensor_type_fridge_temperature = 13, /*!< \brief fridge_temperature with values in the format csr_mesh_temperature_kelvin_t*/
    csr_mesh_sensor_type_desired_fridge_temperature = 14, /*!< \brief desired_fridge_temperature with values in the format csr_mesh_temperature_kelvin_t*/
    csr_mesh_sensor_type_freezer_temperature = 15, /*!< \brief freezer_temperature with values in the format csr_mesh_temperature_kelvin_t*/
    csr_mesh_sensor_type_desired_freezer_temperature = 16, /*!< \brief desired_freezer_temperature with values in the format csr_mesh_temperature_kelvin_t*/
    csr_mesh_sensor_type_oven_temperature = 17, /*!< \brief oven_temperature with values in the format csr_mesh_temperature_kelvin_t*/
    csr_mesh_sensor_type_desired_oven_temperature = 18, /*!< \brief desired_oven_temperature with values in the format csr_mesh_temperature_kelvin_t*/
    csr_mesh_sensor_type_seat_occupied = 19, /*!< \brief seat_occupied with values in the format csr_mesh_seat_state_t*/
    csr_mesh_sensor_type_washing_machine_state = 20, /*!< \brief washing_machine_state with values in the format csr_mesh_appliance_state_t*/
    csr_mesh_sensor_type_dish_washer_state = 21, /*!< \brief dish_washer_state with values in the format csr_mesh_appliance_state_t*/
    csr_mesh_sensor_type_clothes_dryer_state = 22, /*!< \brief clothes_dryer_state with values in the format csr_mesh_appliance_state_t*/
    csr_mesh_sensor_type_toaster_state = 23, /*!< \brief toaster_state with values in the format csr_mesh_appliance_state_t*/
    csr_mesh_sensor_type_carbon_dioxide = 24, /*!< \brief carbon_dioxide with values in the format csr_mesh_parts_per_million_t*/
    csr_mesh_sensor_type_carbon_monoxide = 25, /*!< \brief carbon_monoxide with values in the format csr_mesh_parts_per_million_t*/
    csr_mesh_sensor_type_smoke = 26, /*!< \brief smoke with values in the format csr_mesh_micrograms_per_cubic_metre_t*/
    csr_mesh_sensor_type_water_level = 27, /*!< \brief water_level with values in the format csr_mesh_percentage_t*/
    csr_mesh_sensor_type_hot_water_temperature = 28, /*!< \brief hot_water_temperature with values in the format csr_mesh_temperature_kelvin_t*/
    csr_mesh_sensor_type_cold_water_temperature = 29, /*!< \brief cold_water_temperature with values in the format csr_mesh_temperature_kelvin_t*/
    csr_mesh_sensor_type_desired_water_temperature = 30, /*!< \brief desired_water_temperature with values in the format csr_mesh_temperature_kelvin_t*/
    csr_mesh_sensor_type_cooker_hob_back_left_state = 31, /*!< \brief cooker_hob_back_left_state with values in the format csr_mesh_cooker_hob_state_t*/
    csr_mesh_sensor_type_desired_cooker_hob_back_left_state = 32, /*!< \brief desired_cooker_hob_back_left_state with values in the format csr_mesh_cooker_hob_state_t*/
    csr_mesh_sensor_type_cooker_hob_front_left_state = 33, /*!< \brief cooker_hob_front_left_state with values in the format csr_mesh_cooker_hob_state_t*/
    csr_mesh_sensor_type_desired_cooker_hob_front_left_state = 34, /*!< \brief desired_cooker_hob_front_left_state with values in the format csr_mesh_cooker_hob_state_t*/
    csr_mesh_sensor_type_cooker_hob_back_middle_state = 35, /*!< \brief cooker_hob_back_middle_state with values in the format csr_mesh_cooker_hob_state_t*/
    csr_mesh_sensor_type_desired_cooker_hob_back_middle_state = 36, /*!< \brief desired_cooker_hob_back_middle_state with values in the format csr_mesh_cooker_hob_state_t*/
    csr_mesh_sensor_type_cooker_hob_front_middle_state = 37, /*!< \brief cooker_hob_front_middle_state with values in the format csr_mesh_cooker_hob_state_t*/
    csr_mesh_sensor_type_desired_cooker_hob_front_middle_state = 38, /*!< \brief desired_cooker_hob_front_middle_state with values in the format csr_mesh_cooker_hob_state_t*/
    csr_mesh_sensor_type_cooker_hob_back_right_state = 39, /*!< \brief cooker_hob_back_right_state with values in the format csr_mesh_cooker_hob_state_t*/
    csr_mesh_sensor_type_desired_cooker_hob_back_right_state = 40, /*!< \brief desired_cooker_hob_back_right_state with values in the format csr_mesh_cooker_hob_state_t*/
    csr_mesh_sensor_type_cooker_hob_front_right_state = 41, /*!< \brief cooker_hob_front_right_state with values in the format csr_mesh_cooker_hob_state_t*/
    csr_mesh_sensor_type_desired_cooker_hob_front_right_state = 42, /*!< \brief desired_cooker_hob_front_right_state with values in the format csr_mesh_cooker_hob_state_t*/
    csr_mesh_sensor_type_desired_wakeup_alarm_time = 43, /*!< \brief desired_wakeup_alarm_time with values in the format csr_mesh_minutes_of_the_day_t*/
    csr_mesh_sensor_type_desired_second_wakeup_alarm_time = 44, /*!< \brief desired_second_wakeup_alarm_time with values in the format csr_mesh_minutes_of_the_day_t*/
    csr_mesh_sensor_type_passive_infrared_state = 45, /*!< \brief passive_infrared_state with values in the format csr_mesh_movement_state_t*/
    csr_mesh_sensor_type_water_flowing = 46, /*!< \brief water_flowing with values in the format csr_mesh_water_flow_rate_t*/
    csr_mesh_sensor_type_desired_water_flow = 47, /*!< \brief desired_water_flow with values in the format csr_mesh_water_flow_rate_t*/
    csr_mesh_sensor_type_audio_level = 48, /*!< \brief audio_level with values in the format csr_mesh_decibel_t*/
    csr_mesh_sensor_type_desired_audio_level = 49, /*!< \brief desired_audio_level with values in the format csr_mesh_decibel_t*/
    csr_mesh_sensor_type_fan_speed = 50, /*!< \brief fan_speed with values in the format csr_mesh_percentage_t*/
    csr_mesh_sensor_type_desired_fan_speed = 51, /*!< \brief desired_fan_speed with values in the format csr_mesh_forward_backward_t*/
    csr_mesh_sensor_type_wind_speed = 52, /*!< \brief wind_speed with values in the format csr_mesh_centimetres_per_second_t*/
    csr_mesh_sensor_type_wind_speed_gust = 53, /*!< \brief wind_speed_gust with values in the format csr_mesh_centimetres_per_second_t*/
    csr_mesh_sensor_type_wind_direction = 54, /*!< \brief wind_direction with values in the format csr_mesh_direction_t*/
    csr_mesh_sensor_type_wind_direction_gust = 55, /*!< \brief wind_direction_gust with values in the format csr_mesh_direction_t*/
    csr_mesh_sensor_type_rain_fall_last_hour = 56, /*!< \brief rain_fall_last_hour with values in the format csr_mesh_millimetres_t*/
    csr_mesh_sensor_type_rain_fall_today = 57, /*!< \brief rain_fall_today with values in the format csr_mesh_millimetres_t*/
    csr_mesh_sensor_type_barometric_pressure = 58, /*!< \brief barometric_pressure with values in the format csr_mesh_air_pressure_t*/
    csr_mesh_sensor_type_soil_temperature = 59, /*!< \brief soil_temperature with values in the format csr_mesh_temperature_kelvin_t*/
    csr_mesh_sensor_type_soil_moisure = 60, /*!< \brief soil_moisure with values in the format csr_mesh_percentage_t*/
    csr_mesh_sensor_type_window_cover_position = 61, /*!< \brief window_cover_position with values in the format csr_mesh_percentage_t*/
    csr_mesh_sensor_type_desired_window_cover_position = 62, /*!< \brief desired_window_cover_position with values in the format csr_mesh_percentage_t*/
    csr_mesh_sensor_type_generic_1_byte = 63, /*!< \brief generic_1_byte with values in the format csr_mesh_generic_1_byte_t*/
    csr_mesh_sensor_type_generic_2_byte = 64, /*!< \brief generic_2_byte with values in the format csr_mesh_generic_2_byte_t*/
    csr_mesh_sensor_type_generic_1_byte_typed = 250, /*!< \brief generic_1_byte_typed with values in the format csr_mesh_generic_1_byte_typed_t*/
    csr_mesh_sensor_type_generic_2_byte_typed = 251, /*!< \brief generic_2_byte_typed with values in the format csr_mesh_generic_2_byte_typed_t*/
    csr_mesh_sensor_type_generic_3_byte_typed = 252 /*!< \brief generic_3_byte_typed with values in the format csr_mesh_generic_3_byte_typed_t*/
}csr_mesh_sensor_type_t;
/*! \brief CSRmesh beacon_type type */
typedef enum
{
    csr_mesh_beacon_type_csr = 0, /*!< \brief csr */
    csr_mesh_beacon_type_ibeacon = 1, /*!< \brief ibeacon */
    csr_mesh_beacon_type_eddystone_url = 2, /*!< \brief eddystone_url */
    csr_mesh_beacon_type_eddystone_uid = 3, /*!< \brief eddystone_uid */
    csr_mesh_beacon_type_lte_direct = 4, /*!< \brief lte_direct */
    csr_mesh_beacon_type_lumicast = 5 /*!< \brief lumicast */
}csr_mesh_beacon_type_t;

/*! \brief CSRmesh door_state type */
typedef enum
{
    csr_mesh_door_state_open_unlocked = 2, /*!< \brief open_unlocked */
    csr_mesh_door_state_closed_unlocked = 1, /*!< \brief closed_unlocked */
    csr_mesh_door_state_closed_locked = 0, /*!< \brief closed_locked */
}csr_mesh_door_state_t;
/*! \brief CSRmesh window_state type */
typedef enum
{
    csr_mesh_window_state_closed_insecure = 1, /*!< \brief closed_insecure */
    csr_mesh_window_state_open_insecure = 3, /*!< \brief open_insecure */
    csr_mesh_window_state_open_secure = 2, /*!< \brief open_secure */
    csr_mesh_window_state_closed_secure = 0, /*!< \brief closed_secure */
}csr_mesh_window_state_t;
/*! \brief CSRmesh seat_state type */
typedef enum
{
    csr_mesh_seat_state_empty = 0, /*!< \brief empty */
    csr_mesh_seat_state_continuously_occupied = 2, /*!< \brief continuously_occupied */
    csr_mesh_seat_state_occupied = 1, /*!< \brief occupied */
}csr_mesh_seat_state_t;
/*! \brief CSRmesh appliance_state type */
typedef enum
{
    csr_mesh_appliance_state_pre_wash = 3, /*!< \brief pre_wash */
    csr_mesh_appliance_state_idle = 0, /*!< \brief idle */
    csr_mesh_appliance_state_post_wash = 5, /*!< \brief post_wash */
    csr_mesh_appliance_state_washing = 4, /*!< \brief washing */
    csr_mesh_appliance_state_heating_up = 1, /*!< \brief heating_up */
    csr_mesh_appliance_state_cooling_down = 2, /*!< \brief cooling_down */
}csr_mesh_appliance_state_t;
/*! \brief CSRmesh cooker_hob_state type */
typedef enum
{
    csr_mesh_cooker_hob_state_level_2 = 2, /*!< \brief level_2 */
    csr_mesh_cooker_hob_state_off_hot = 10, /*!< \brief off_hot */
    csr_mesh_cooker_hob_state_level_1 = 1, /*!< \brief level_1 */
    csr_mesh_cooker_hob_state_level_7 = 7, /*!< \brief level_7 */
    csr_mesh_cooker_hob_state_level_9 = 9, /*!< \brief level_9 */
    csr_mesh_cooker_hob_state_off_cold = 0, /*!< \brief off_cold */
    csr_mesh_cooker_hob_state_level_8 = 8, /*!< \brief level_8 */
    csr_mesh_cooker_hob_state_level_5 = 5, /*!< \brief level_5 */
    csr_mesh_cooker_hob_state_level_4 = 4, /*!< \brief level_4 */
    csr_mesh_cooker_hob_state_level_3 = 3, /*!< \brief level_3 */
    csr_mesh_cooker_hob_state_level_6 = 6, /*!< \brief level_6 */
}csr_mesh_cooker_hob_state_t;
/*! \brief CSRmesh movement_state type */
typedef enum
{
    csr_mesh_movement_state_no_movement = 0, /*!< \brief no_movement */
    csr_mesh_movement_state_movement_detected = 1, /*!< \brief movement_detected */
}csr_mesh_movement_state_t;
/*! \brief CSRmesh forward_backward type */
typedef enum
{
    csr_mesh_forward_backward_forward = 1, /*!< \brief forward */
    csr_mesh_forward_backward_backwards = 2, /*!< \brief backwards */
}csr_mesh_forward_backward_t;
/*! \brief CSRmesh direction type */
typedef enum
{
    csr_mesh_direction_north_of_north_east = 16, /*!< \brief north_of_north_east */
    csr_mesh_direction_north = 0, /*!< \brief north */
    csr_mesh_direction_north_east = 32, /*!< \brief north_east */
    csr_mesh_direction_east = 64, /*!< \brief east */
    csr_mesh_direction_north_west = 224, /*!< \brief north_west */
    csr_mesh_direction_north_of_north_west = 240, /*!< \brief north_of_north_west */
    csr_mesh_direction_east_of_north_east = 48, /*!< \brief east_of_north_east */
    csr_mesh_direction_west = 192, /*!< \brief west */
    csr_mesh_direction_south_east = 96, /*!< \brief south_east */
    csr_mesh_direction_east_of_south_east = 80, /*!< \brief east_of_south_east */
    csr_mesh_direction_south_of_south_east = 112, /*!< \brief south_of_south_east */
    csr_mesh_direction_south = 128, /*!< \brief south */
    csr_mesh_direction_west_of_north_west = 208, /*!< \brief west_of_north_west */
    csr_mesh_direction_south_of_south_west = 144, /*!< \brief south_of_south_west */
    csr_mesh_direction_west_of_south_west = 176, /*!< \brief west_of_south_west */
    csr_mesh_direction_south_west = 160, /*!< \brief south_west */
}csr_mesh_direction_t;

/*! \brief CSRmesh Model types */
typedef enum
{
    CSRMESH_WATCHDOG_MODEL = 0, /*!< \brief Type Watchdog model */
    CSRMESH_CONFIG_MODEL = 1, /*!< \brief Type Config model */
    CSRMESH_GROUP_MODEL = 2, /*!< \brief Type Group model */
    CSRMESH_SENSOR_MODEL = 4, /*!< \brief Type Sensor model */
    CSRMESH_ACTUATOR_MODEL = 5, /*!< \brief Type Actuator model */
    CSRMESH_DATA_MODEL = 8, /*!< \brief Type Data model */
    CSRMESH_BEARER_MODEL = 11, /*!< \brief Type Bearer model */
    CSRMESH_PING_MODEL = 12, /*!< \brief Type Ping model */
    CSRMESH_BATTERY_MODEL = 13, /*!< \brief Type Battery model */
    CSRMESH_ATTENTION_MODEL = 14, /*!< \brief Type Attention model */
    CSRMESH_POWER_MODEL = 19, /*!< \brief Type Power model */
    CSRMESH_LIGHT_MODEL = 20, /*!< \brief Type Light model */
    CSRMESH_ASSET_MODEL = 6, /*!< \brief Type Asset model */
    CSRMESH_TRACKER_MODEL = 7, /*!< \brief Type Tracker model */
    CSRMESH_TIME_MODEL = 16, /*!< \brief Type Time model */
    CSRMESH_SWITCH_MODEL = 21, /*!< \brief Type Switch model */
    CSRMESH_TUNING_MODEL = 28, /*!< \brief Type Tuning model */
    CSRMESH_EXTENSION_MODEL = 29, /*!< \brief Type Extension model */
    CSRMESH_LARGEOBJECTTRANSFER_MODEL = 30, /*!< \brief Type LargeObjectTransfer model */
    CSRMESH_FIRMWARE_MODEL = 9, /*!< \brief Type Firmware model */
    CSRMESH_DIAGNOSTIC_MODEL = 10, /*!< \brief Type Diagnostic model */
    CSRMESH_ACTION_MODEL = 34, /*!< \brief Type Action model */
    CSRMESH_BEACON_MODEL = 32, /*!< \brief Type Beacon model */
    CSRMESH_BEACONPROXY_MODEL = 33, /*!< \brief Type BeaconProxy model */
    CSRMESH_ALL_MODELS = 255 /*!< \brief Type All models */
} CSRMESH_MODEL_TYPE_T;

/*! \brief CSRmesh Model Event Code types */
typedef enum
{
    /* Watchdog model messages */
    CSRMESH_WATCHDOG_MESSAGE = 0x00,/*!< \brief Upon receiving a WATCHDOG_MESSAGE message, if the RspSize field is set to a non-zero value, then the device shall respond with a WATCHDOG_MESSAGE with the RspSize field set to zero, and RspSize -1 octets of additional RandomData. */
    CSRMESH_WATCHDOG_SET_INTERVAL = 0x01,/*!< \brief Upon receiving a WATCHDOG_SET_INTERVAL message, the device shall save the Interval and ActiveAfterTime fields into the Interval and ActiveAfterTime variables and respond with a WATCHDOG_INTERVAL message with the current variable values. */
    CSRMESH_WATCHDOG_INTERVAL = 0x02,/*!< \brief Watchdog interval state */

    /* Config model messages */
    CSRMESH_CONFIG_LAST_SEQUENCE_NUMBER = 0x03,/*!< \brief Upon receiving a CONFIG_LAST_SEQUENCE_NUMBER message from a trusted device, the local device updates the SequenceNumber to at least one higher than the LastSequenceNumber in the message. Note: A trusted device means a device that is not only on the same CSRmesh network, having the same network key, but also interacted with in the past. This message is most useful to check if a device has been reset, for example when the batteries of the device are changed, but it does not remember its last sequence number in non-volatile memory. */
    CSRMESH_CONFIG_RESET_DEVICE = 0x04,/*!< \brief Upon receiving a CONFIG_RESET_DEVICE message from a trusted device directed at only this device, the local device sets the DeviceID to zero, and forgets all network keys, associated NetworkIVs and other configuration information. The device may act as if it is not associated and use MASP to re-associate with a network. Note: If the CONFIG_RESET_DEVICE message is received on any other destination address than the DeviceID of the local device, it is ignored. This is typically used when selling a device, to remove the device from the network of the seller so that the purchaser can associate the device with their network. */
    CSRMESH_CONFIG_SET_DEVICE_IDENTIFIER = 0x05,/*!< \brief When the device with a DeviceID of 0x0000 receives a CONFIG_SET_DEVICE_IDENTIFIER message and the DeviceHash of the message matches the DeviceHash of this device, the DeviceID of this device is set to the DeviceID field of this message. Then the device responds with the DEVICE_CONFIG_IDENTIFIER message using the new DeviceID as the source address. Note: This function is not necessary in normal operation of a CSRmesh network as DeviceID is distributed as part of the MASP protocol in the MASP_ID_DISTRIBUTION message. */
    CSRMESH_CONFIG_SET_PARAMETERS = 0x06,/*!< \brief Upon receiving a CONFIG_SET_PARAMETERS message, where the destination address is the DeviceID of this device, the device saves the TxInterval, TxDuration, RxDutyCycle, TxPower and TTL fields into the TransmitInterval, TransmitDuration, ReceiverDutyCycle, TransmitPower and DefaultTimeToLive state respectively. Then the device responds with a CONFIG_PARAMETERS message with the current configuration model state information. */
    CSRMESH_CONFIG_GET_PARAMETERS = 0x07,/*!< \brief Upon receiving a CONFIG_GET_PARAMETERS message, where the destination address is the DeviceID of this device, the device will respond with a CONFIG_PARAMETERS message with the current config model state information. */
    CSRMESH_CONFIG_PARAMETERS = 0x08,/*!< \brief Configuration parameters */
    CSRMESH_CONFIG_DISCOVER_DEVICE = 0x09,/*!< \brief Upon receiving a CONFIG_DISCOVER_DEVICE message directed at the 0x0000 group identifier or to DeviceID of this device, the device responds with a CONFIG_DEVICE_IDENTIFIER message. */
    CSRMESH_CONFIG_DEVICE_IDENTIFIER = 0x0A,/*!< \brief Device identifier */
    CSRMESH_CONFIG_GET_INFO = 0x0B,/*!< \brief Upon receiving a CONFIG_GET_INFO message, directed at the DeviceID of this device, the device responds with a CONFIG_INFO message. The Info field of the CONFIG_GET_INFO message determines the information to be included in the CONFIG_INFO message. The following information values are defined: DeviceUUIDLow (0x00) contains the least significant eight octets of the DeviceUUID state value. DeviceUUIDHigh (0x01) contains the most significant eight octets of the DeviceUUID state value. ModelsLow (0x02) contains the least significant eight octets of the ModelsSupported state value. ModelsHigh (0x03) contains the most significant eight octets of the ModelsSupported state value. */
    CSRMESH_CONFIG_INFO = 0x0C,/*!< \brief Current device information */
    CSRMESH_CONFIG_SET_MESSAGE_PARAMS = 0x12,/*!< \brief  */
    CSRMESH_CONFIG_GET_MESSAGE_PARAMS = 0x13,/*!< \brief  */
    CSRMESH_CONFIG_MESSAGE_PARAMS = 0x14,/*!< \brief  */

    /* Group model messages */
    CSRMESH_GROUP_GET_NUMBER_OF_MODEL_GROUPIDS = 0x0D,/*!< \brief Getting Number of Group IDs: Upon receiving a GROUP_GET_NUMBER_OF_MODEL_GROUPS message, where the destination address is the DeviceID of this device, the device responds with a GROUP_NUMBER_OF_MODEL_GROUPS message with the number of Group IDs that the given model supports on this device. */
    CSRMESH_GROUP_NUMBER_OF_MODEL_GROUPIDS = 0x0E,/*!< \brief Get number of groups supported by the model */
    CSRMESH_GROUP_SET_MODEL_GROUPID = 0x0F,/*!< \brief Setting Model Group ID: Upon receiving a GROUP_SET_MODEL_GROUPID message, where the destination address is the DeviceID of this device, the device saves the Instance and GroupID fields into the appropriate state value determined by the Model and GroupIndex fields. It then responds with a GROUP_MODEL_GROUPID message with the current state information held for the given model and the GroupIndex values. */
    CSRMESH_GROUP_GET_MODEL_GROUPID = 0x10,/*!< \brief Getting Model Group ID: Upon receiving a GROUP_GET_MODEL_GROUPID message, where the destination address is the DeviceID of this device, the device responds with a GROUP_MODEL_GROUPID message with the current state information held for the given Model and GroupIndex values. */
    CSRMESH_GROUP_MODEL_GROUPID = 0x11,/*!< \brief GroupID of a model */

    /* Sensor model messages */
    CSRMESH_SENSOR_GET_TYPES = 0x20,/*!< \brief Upon receiving a SENSOR_GET_TYPES message, the device responds with a SENSOR_TYPES message with the list of supported types greater than or equal to the FirstType field. If the device does not support any types greater than or equal to the FirstType field, then it sends a SENSOR_TYPES message with zero-length Types field. */
    CSRMESH_SENSOR_TYPES = 0x21,/*!< \brief Sensor types */
    CSRMESH_SENSOR_SET_STATE = 0x22,/*!< \brief Setting Sensor State: Upon receiving a SENSOR_SET_STATE message, where the destination address is the device ID of this device and the Type field is a supported sensor type, the device saves the RxDutyCycle field and responds with a SENSOR_STATE message with the current state information of the sensor type. If the Type field is not a supported sensor type, the device ignores the message. */
    CSRMESH_SENSOR_GET_STATE = 0x23,/*!< \brief Getting Sensor State: Upon receiving a SENSOR_GET_STATE message, where the destination address is the deviceID of this device and the Type field is a supported sensor type, the device shall respond with a SENSOR_STATE message with the current state information of the sensor type. Upon receiving a SENSOR_GET_STATE message, where the destination address is the device ID of this device but the Type field is not a supported sensor type, the device shall ignore the message. */
    CSRMESH_SENSOR_STATE = 0x24,/*!< \brief Current sensor state */
    CSRMESH_SENSOR_WRITE_VALUE_NO_ACK = 0x26,/*!< \brief Writing Sensor Value: Upon receiving a SENSOR_WRITE_VALUE message, where the Type field is a supported sensor type, the device saves the value into the current value of the sensor type on this device and responds with a SENSOR_VALUE message with the current value of this sensor type. */
    CSRMESH_SENSOR_WRITE_VALUE = 0x25,/*!< \brief Writing Sensor Value: Upon receiving a SENSOR_WRITE_VALUE message, where the Type field is a supported sensor type, the device saves the value into the current value of the sensor type on this device and responds with a SENSOR_VALUE message with the current value of this sensor type. */
    CSRMESH_SENSOR_READ_VALUE = 0x27,/*!< \brief Getting Sensor Value: Upon receiving a SENSOR_READ_VALUE message, where the Type field is a supported sensor type, the device responds with a SENSOR_VALUE message with the value of the sensor type. Proxy Behaviour: Upon receiving a SENSOR_GET_STATE where the destination of the message and the sensor type correspond to a previously received SENSOR_BROADCAST_VALUE or SENSOR_BROADCAST_NEW message, the device responds with a SENSOR_VALUE message with the remembered values. */
    CSRMESH_SENSOR_VALUE = 0x28,/*!< \brief Current sensor value */
    CSRMESH_SENSOR_MISSING = 0x29,/*!< \brief Sensor data is missing. Proxy Behaviour: Upon receiving a SENSOR_MISSING message, the proxy determines if it has remembered this type and value and then writes that type and value to the device that sent the message. */

    /* Actuator model messages */
    CSRMESH_ACTUATOR_GET_TYPES = 0x30,/*!< \brief Upon receiving an ACTUATOR_GET_TYPES message, the device responds with an ACTUATOR_TYPES message with a list of supported types greater than or equal to the FirstType field. If the device does not support any types greater than or equal to FirstType, it sends an ACTUATOR_TYPES message with a zero length Types field. */
    CSRMESH_ACTUATOR_TYPES = 0x31,/*!< \brief Actuator types */
    CSRMESH_ACTUATOR_SET_VALUE_NO_ACK = 0x32,/*!< \brief Get sensor state. Upon receiving an ACTUATOR_SET_VALUE_NO_ACK message, where the destination address is the device ID of this device and the Type field is a supported actuator type, the device shall immediately use the Value field for the given Type field. The meaning of this actuator value is not defined in this specification. Upon receiving an ACTUATOR_SET_VALUE_NO_ACK message, where the destination address is is the device ID of this device but the Type field is not a supported actuator type, the device shall ignore the message. */
    CSRMESH_ACTUATOR_SET_VALUE = 0x33,/*!< \brief Get sensor state. Upon receiving an ACTUATOR_SET_VALUE_NO_ACK message, where the destination address is the device ID of this device and the Type field is a supported actuator type, the device shall immediately use the Value field for the given Type field. The meaning of this actuator value is not defined in this specification. Upon receiving an ACTUATOR_SET_VALUE_NO_ACK message, where the destination address is is the device ID of this device but the Type field is not a supported actuator type, the device shall ignore the message. */
    CSRMESH_ACTUATOR_VALUE_ACK = 0x35,/*!< \brief Current sensor state */
    CSRMESH_ACTUATOR_GET_VALUE_ACK = 0x34,/*!< \brief Get Current sensor state */

    /* Data model messages */
    CSRMESH_DATA_STREAM_FLUSH = 0x70,/*!< \brief Flushing Data: Upon receiving a DATA_STREAM_FLUSH message, the device saves the StreamSN field into the StreamSequenceNumber model state and responds with DATA_STREAM_RECEIVED with the StreamNESN field set to the value of the StreamSequenceNumber model state. The device also flushes all partially received stream data from this peer device. */
    CSRMESH_DATA_STREAM_SEND = 0x71,/*!< \brief Sending Data: Upon receiving a DATA_STREAM_SEND message, the device first checks if the StreamSN field is the same as the StreamSequenceNumber model state. If these values are the same, the device passes the StreamOctets field up to the application for processing, and increments StreamSequenceNumber by the length of the StreamOctets field. It then responds with a DATA_STREAM_RECEIVED message with the current value of the StreamSequenceNumber. Note: The DATA_STREAM_RECEIVED message is sent even if the StreamSN received is different from the StreamSequenceNumber state. This allows missing packets to be detected and retransmitted by the sending device. */
    CSRMESH_DATA_STREAM_RECEIVED = 0x72,/*!< \brief Acknowledgement of data received. */
    CSRMESH_DATA_BLOCK_SEND = 0x73,/*!< \brief A block of data, no acknowledgement. Upon receiving a DATA_BLOCK_SEND message, the device passes the DatagramOctets field up to the application for processing. */

    /* Bearer model messages */
    CSRMESH_BEARER_SET_STATE = 0x8100,/*!< \brief Setting Bearer State: Upon receiving a BEARER_SET_STATE message, where the destination address is the device ID of this device, the device saves the BearerRelayActive, BearerEnabled, and BearerPromiscuous fields into the appropriate state value. Then the device responds with a BEARER_STATE message with the current state information. */
    CSRMESH_BEARER_GET_STATE = 0x8101,/*!< \brief Getting Bearer State: Upon receiving a BEARER_GET_STATE message, where the destination address is the device ID of this device, the device responds with a BEARER_STATE message with the current state information. */
    CSRMESH_BEARER_STATE = 0x8102,/*!< \brief Set bearer state */

    /* Ping model messages */
    CSRMESH_PING_REQUEST = 0x8200,/*!< \brief Ping Request: Upon receiving a PING_REQUEST message, the device responds with a PING_RESPONSE message with the TTLAtRx field set to the TTL value from the PING_REQUEST message, and the RSSIAtRx field set to the RSSI value of the PING_REQUEST message. If the bearer used to receive the PING_REQUEST message does not have an RSSI value, then the value 0x00 is used. */
    CSRMESH_PING_RESPONSE = 0x8201,/*!< \brief Ping response */

    /* Battery model messages */
    CSRMESH_BATTERY_GET_STATE = 0x8300,/*!< \brief Getting Battery State: Upon receiving a BATTERY_GET_STATE message, the device responds with a BATTERY_STATE message with the current state information. */
    CSRMESH_BATTERY_STATE = 0x8301,/*!< \brief Current battery state */

    /* Attention model messages */
    CSRMESH_ATTENTION_SET_STATE = 0x8400,/*!< \brief Setting Flashing State: Upon receiving an ATTENTION_SET_STATE message, the device saves the AttractAttention and AttentionDuration fields into the appropriate state value. It then responds with an ATTENTION_STATE message with the current state information. If the AttractAttention field is set to 0x01 and the AttentionDuration is not 0xFFFF, then any existing attention timer is cancelled and a new attention timer is started that will expire after AttentionDuration milliseconds. If the AttractAttention field is set to 0x01 and the AttentionDuration field is 0xFFFF, then the attention timer is ignored. If the AttractAttention field is set to 0x00, then the attention timer is cancelled if it is already running. */
    CSRMESH_ATTENTION_STATE = 0x8401,/*!< \brief Current battery state */

    /* Power model messages */
    CSRMESH_POWER_SET_STATE_NO_ACK = 0x8900,/*!< \brief Setting Power State: Upon receiving a POWER_SET_STATE_NO_ACK message, the device sets the PowerState state value to the PowerState field. It then responds with a POWER_STATE message with the current state information. */
    CSRMESH_POWER_SET_STATE = 0x8901,/*!< \brief Setting Power State: Upon receiving a POWER_SET_STATE_NO_ACK message, the device sets the PowerState state value to the PowerState field. It then responds with a POWER_STATE message with the current state information. */
    CSRMESH_POWER_TOGGLE_STATE_NO_ACK = 0x8902,/*!< \brief Toggling Power State: Upon receiving a POWER_Toggle_STATE_NO_ACK message, the device sets the PowerState state value as defined: 1.If the current PowerState is 0x00, Off, then PowerState should be set to 0x01, On. 2.If the current PowerState is 0x01, On, then PowerState should be set to 0x00, Off. 3.If the current PowerState is 0x02, Standby, then PowerState should be set to 0x03, OnFromStandby. 4.If the current PowerState is 0x03, OnFromStandby, then PowerState should be set to 0x02, Standby. Then the device responds with a POWER_STATE message with the current state information. */
    CSRMESH_POWER_TOGGLE_STATE = 0x8903,/*!< \brief Toggling Power State: Upon receiving a POWER_Toggle_STATE_NO_ACK message, the device sets the PowerState state value as defined: 1.If the current PowerState is 0x00, Off, then PowerState should be set to 0x01, On. 2.If the current PowerState is 0x01, On, then PowerState should be set to 0x00, Off. 3.If the current PowerState is 0x02, Standby, then PowerState should be set to 0x03, OnFromStandby. 4.If the current PowerState is 0x03, OnFromStandby, then PowerState should be set to 0x02, Standby. Then the device responds with a POWER_STATE message with the current state information. */
    CSRMESH_POWER_GET_STATE = 0x8904,/*!< \brief Getting Power State: Upon receiving a POWER_GET_STATE message, the device responds with a POWER_STATE message with the current state information. */
    CSRMESH_POWER_STATE_NO_ACK = 0x8906,/*!< \brief Current power state */
    CSRMESH_POWER_STATE = 0x8905,/*!< \brief Current power state */

    /* Light model messages */
    CSRMESH_LIGHT_SET_LEVEL_NO_ACK = 0x8A00,/*!< \brief Setting Light Level: Upon receiving a LIGHT_SET_LEVEL_NO_ACK message, the device saves the Level field into the CurrentLevel model state. LevelSDState should be set to Idle. If ACK is requested, the device should respond with a LIGHT_STATE message. */
    CSRMESH_LIGHT_SET_LEVEL = 0x8A01,/*!< \brief Setting Light Level: Upon receiving a LIGHT_SET_LEVEL_NO_ACK message, the device saves the Level field into the CurrentLevel model state. LevelSDState should be set to Idle. If ACK is requested, the device should respond with a LIGHT_STATE message. */
    CSRMESH_LIGHT_SET_RGB_NO_ACK = 0x8A02,/*!< \brief Setting Light Colour:  Upon receiving a LIGHT_SET_RGB_NO_ACK message, the device saves the Level, Red, Green, and Blue fields into the TargetLevel, TargetRed, TargetGreen, and TargetBlue variables respectively. LevelSDState should be set to Attacking. If the Duration field is zero, then the device saves the Level, Red, Green, and Blue fields into the CurrentLevel, CurrentRed, CurrentGreen and CurrentBlue variables, and sets the DeltaLevel, DeltaRed, DeltaGreen, and DeltaBlue variables to zero. If the Duration field is greater than zero, then the device calculates the DeltaLevel, DeltaRed, DeltaGreen, and DeltaBlue levels from the differences between the Current values and the Target values divided by the Duration field, so that over Duration seconds, the CurrentLevel, CurrentRed, CurrentGreen, and CurrentBlue variables are changed smoothly to the TargetLevel, TargetRed, TargetGreen and TargetBlue values. If ACK is requested, the device responds with a LIGHT_STATE message. */
    CSRMESH_LIGHT_SET_RGB = 0x8A03,/*!< \brief Setting Light Colour:  Upon receiving a LIGHT_SET_RGB_NO_ACK message, the device saves the Level, Red, Green, and Blue fields into the TargetLevel, TargetRed, TargetGreen, and TargetBlue variables respectively. LevelSDState should be set to Attacking. If the Duration field is zero, then the device saves the Level, Red, Green, and Blue fields into the CurrentLevel, CurrentRed, CurrentGreen and CurrentBlue variables, and sets the DeltaLevel, DeltaRed, DeltaGreen, and DeltaBlue variables to zero. If the Duration field is greater than zero, then the device calculates the DeltaLevel, DeltaRed, DeltaGreen, and DeltaBlue levels from the differences between the Current values and the Target values divided by the Duration field, so that over Duration seconds, the CurrentLevel, CurrentRed, CurrentGreen, and CurrentBlue variables are changed smoothly to the TargetLevel, TargetRed, TargetGreen and TargetBlue values. If ACK is requested, the device responds with a LIGHT_STATE message. */
    CSRMESH_LIGHT_SET_POWER_LEVEL_NO_ACK = 0x8A04,/*!< \brief Setting Light Power and Light Level: Upon receiving a LIGHT_SET_POWER_LEVEL_NO_ACK message, the device sets the current PowerState to the Power field, the TargetLevel variable to the Level field, the DeltaLevel to the difference between TargetLevel and CurrentLevel divided by the LevelDuration field, saves the Sustain and Decay fields into the LevelSustain and LevelDecay variables, and sets LevelSDState to the Attacking state. If ACK is requested, the device should respond with a LIGHT_STATE message. */
    CSRMESH_LIGHT_SET_POWER_LEVEL = 0x8A05,/*!< \brief Setting Light Power and Light Level: Upon receiving a LIGHT_SET_POWER_LEVEL_NO_ACK message, the device sets the current PowerState to the Power field, the TargetLevel variable to the Level field, the DeltaLevel to the difference between TargetLevel and CurrentLevel divided by the LevelDuration field, saves the Sustain and Decay fields into the LevelSustain and LevelDecay variables, and sets LevelSDState to the Attacking state. If ACK is requested, the device should respond with a LIGHT_STATE message. */
    CSRMESH_LIGHT_SET_COLOR_TEMP = 0x8A06,/*!< \brief Setting Light Colour Temperature: Upon receiving a LIGHT_SET_COLOR_TEMP message, the device saves the ColorTemperature field into the TargetColorTemperature state variable. If the TempDuration field is zero, the CurrentColorTemperature variable is set to TargetColorTemperature and DeltaColorTemperature is set to zero. If the TempDuration field is greater than zero, then the device calculates the difference between TargetColorTemperature and CurrentColorTemperature, over the TempDuration field and store this into a DeltaColorTemperature state variable, so that over TempDuration seconds, CurrentColorTemperature changes smoothly to TargetColorTemperature. The device then responds with a LIGHT_STATE message. */
    CSRMESH_LIGHT_GET_STATE = 0x8A07,/*!< \brief Getting Light State: Upon receiving a LIGHT_GET_STATE message, the device responds with a LIGHT_STATE message. */
    CSRMESH_LIGHT_STATE_NO_ACK = 0x8A09,/*!< \brief Current light state */
    CSRMESH_LIGHT_STATE = 0x8A08,/*!< \brief Current light state */
    CSRMESH_LIGHT_SET_WHITE_NO_ACK = 0x8A0B,/*!< \brief Setting Light White level. */
    CSRMESH_LIGHT_SET_WHITE = 0x8A0A,/*!< \brief Setting Light White level. */
    CSRMESH_LIGHT_GET_WHITE = 0x8A0C,/*!< \brief Setting Light White level. */
    CSRMESH_LIGHT_WHITE_NO_ACK = 0x8A0E,/*!< \brief Setting Light White level. */
    CSRMESH_LIGHT_WHITE = 0x8A0D,/*!< \brief Setting Light White level. */

    /* Asset model messages */
    CSRMESH_ASSET_SET_STATE = 0x40,/*!< \brief Setting Asset State: Upon receiving an ASSET_SET_STATE message, the device saves the Interval, SideEffects, ToDestination, TxPower, Number of Announcements and AnnounceInterval  fields into the appropriate state values. It then responds with an ASSET_STATE message with the current state information. */
    CSRMESH_ASSET_GET_STATE = 0x41,/*!< \brief Getting Asset State: Upon receiving an ASSET_GET_STATE message, the device responds with an ASSET_STATE message with the current state information. */
    CSRMESH_ASSET_STATE = 0x42,/*!< \brief Current asset state */
    CSRMESH_ASSET_ANNOUNCE = 0x43,/*!< \brief Asset announcement */

    /* Tracker model messages */
    CSRMESH_TRACKER_FIND = 0x44,/*!< \brief Finding an Asset:  Upon receiving a TRACKER_FIND message, the server checks its tracker cache to see if it has received an ASSET_ANNOUNCE message recently that has the same DeviceID. If it finds one, it will send a TRACKER_FOUND message with the cached information. */
    CSRMESH_TRACKER_FOUND = 0x45,/*!< \brief Asset found */
    CSRMESH_TRACKER_REPORT = 0x46,/*!< \brief Asset report */
    CSRMESH_TRACKER_CLEAR_CACHE = 0x47,/*!< \brief Clear tracker cache */
    CSRMESH_TRACKER_SET_PROXIMITY_CONFIG = 0x48,/*!< \brief Set tracker proximity config */

    /* Time model messages */
    CSRMESH_TIME_SET_STATE = 0x75,/*!< \brief Setting Time Broadcast Interval: Upon receiving a TIME_SET_STATE message, the device saves the TimeInterval field into the appropriate state value. It then responds with a TIME_STATE message with the current state information. */
    CSRMESH_TIME_GET_STATE = 0x76,/*!< \brief Getting Time Broadcast Interval: Upon receiving a TIME_GET_STATE message, the device responds with a TIME_STATE message with the current state information. */
    CSRMESH_TIME_STATE = 0x77,/*!< \brief Set time broadcast interval */
    CSRMESH_TIME_BROADCAST = 0x7F,/*!< \brief Synchronise wall clock time from client device: This message is always sent with TTL=0. This message is sent at intervals by the clock master. It is always sent with TTL=0. It is repeated, but the time is updated before each repeat is sent. The clock master repeats the message 5 times, relaying stations repeat it 3 times. When a node receives a clock broadcast its behaviour depends on the current clock state: n MASTER: Ignore broadcasts.n INIT: Start the clock; relay this message. Set state to NO_RELAY if MasterFlag set, otherwise RELAY_MASTER. Start relay timer.n RELAY: Correct clock if required. Relay this message. Set state to NO_RELAY if MasterFlag set, otherwise RELAY_MASTER. Start relay timer.n NO_RELAY: Ignore. State will be reset to RELAY when the relay timer goes off.n  RELAY_MASTER: Relay message only if it is from the clock master and set state to NO_RELAY.n The relay timer is by default 1/4 of the clock broadcast interval (15 seconds if the interval is 60 seconds). This means that each node will relay a message only once, and will give priority to messages from the clock master (which always causes the clock to be corrected). Messages from other nodes will only cause clock correction if they exceed the max clock skew (250ms). */

    /* Switch model messages */

    /* Tuning model messages */
    CSRMESH_TUNING_PROBE = 0xEF01,/*!< \brief Tuning Probe: The Tuning Probe message is sent to discover neighbours. This messages is issued by devices wanting to determine their density metrics.n The message is sent in two forms. A short form omitting both ScanDutyCycle and BatteryState with a TTL=0. This allows immediate neighbours to perform various calculations and in turn provide their own PROBE messages. The long version is only provided with TTL>0. This cannot be used for immediate neighbour density determination, but can be used to determine the overall network density. The ability to identify if a node is a potential pinch-point in the Mesh network can be achieved through the comparison of immediate and average number of neighbours within the network. The usage of the PROBE message with TTL=0 or TTL>0 is a way to perform these computations.  It is worth noting that the periodicity of these two types of messages are different; messages with TTL>0 is much more infrequent than messages with TTL=0. Furthermore, it is wise not to use messages for TTL>0 and embedded values in the determination of the average values. The AverageNumNeighbour field is fixed point 6.2 format encoded. The ScanDutyCycle is expressing percentage for numbers from 1 to 100 and (x-100)/10 percentage for numbers from 101 to 255. */
    CSRMESH_TUNING_GET_STATS = 0xEF02,/*!< \brief Getting Tuning Stats: These messages are aimed at collecting statistics from specific nodes. This message allows for the request of all information or for some of its parts. Responses are multi-parts, each identified with an index (combining a continuation flag - top bit). MissingReplyParts for the required field serves at determining the specific responses one would like to collect. If instead all the information is requested, setting this field to zero will inform the destination device to send all messages. Importantly, response (STATS_RESPONSE) messages will not necessarily come back in order, or all reach the requestor. It is essential to handle these cases in the treatment of the collected responses. */
    CSRMESH_TUNING_STATS = 0xEF03,/*!< \brief Current Asset State: Response to the request. The PartNumber indicates the current index of the message, the top bit of this field is used to indicate that more messages are available after this part. For example, a response made up of three messages will have part numbers 0x80, 0x81 and 0x02. Each message has a maximum of  two neighbours. The combination of these responses and PROBE (TTL>0) are a means to establish an overall perspective of the entire Mesh Network. */
    CSRMESH_TUNING_ACK_CONFIG = 0xEF04,/*!< \brief Current tuning config for a device: This message comes as a response to a SET_CONFIG. Encoding its various fields follow the same convention as the ones exposed in SET_CONFIG. */
    CSRMESH_TUNING_SET_CONFIG = 0xEF05,/*!< \brief Setting (or reading) tuning config: Omitted or zero fields mean do not change. This message enforces the state of the recipient. The state is  defined by two goals, normal and high traffic, and their associated number of neighbour to decide which cases to follow. Goals are expressed with unit-less values ranging from 0 to 255. These goals relate to metrics calculated on the basis of density computed at the node and across the network. The expectation is for these goals to be maintained through modification of the receive duty cycle. The average of number of neighbours for high and normal traffic is expressed as a ratio, both numbers sharing the same denominator and each representing their respective numerators. The duty cycle encoding follows the same rules as per duty cycle encoding encountered in PROBE message. This message comes in two formats. A fully truncated form containing only the OpCode (thus of length 2) is used to indicate a request for information. This message should be answered by the appropriate ACK_CONFIG. Further interpretations of this message are: 1. Missing ACK field implies that a request for ACK_CONFIG is made. Thus, this is a special case of the fully truncated mode. However, the provided fields are meant to be used in the setting of goals. 2. Individual fields with zero value are meant NOT to be changed in the received element. Same as for missing fields in truncated messages. Furthermore, in order to improve testing, a combination of values for main and high goals are conventionally expected to be used for defining two behaviours: 1. Suspended: Tuning Probe messages (TTL=0) should be sent and statistics maintained, but the duty cycle should not be changed - thus goals will never be achieved. The encoding are: Main Goal = 0x00 and High Goal = 0xFE. 2. Disable: No Tuning Probe message should be sent and statistics should not be gathered - averaged values should decay. The encoding are: Main Goal = 0x00 and High Goal = 0xFF. */

    /* Extension model messages */
    CSRMESH_EXTENSION_REQUEST = 0xEF10,/*!< \brief Request for Extension OpCode to be approved by the whole Mesh. A device wanting to use an OpCode, makes a request to the entire Mesh Network. This message is issued to target identity 0. The device waits some time, proportional to the size of the Mesh network and only after this period, messages using these proposed OpCode are used. Device receiving this message and wanting to oppose the usage of such code will respond to the source node with a CONFLICT. In case no conflict is known and the OpCode is for a message the node is interested in implementing (through comparison with hash value), a record of the OpCode and its mapping is kept.  Request messages are relayed in cases of absence of conflict. The hash function is SHA-256, padded as per SHA-256 specifications2, for which the least significant 6 bytes will be used in the message. The range parameter indicates the maximum number of OpCode reserved from the based provided in the Proposed OpCode field. The last OpCode reserved is determined through the sum of the Proposed OpCode with the range value. This range parameter varies from 0 to 127, leaving the top bit free. */
    CSRMESH_EXTENSION_CONFLICT = 0xEF11,/*!< \brief Response to a REQUEST - only issued if a conflict is noticed. This message indicates that the proposed OpCode is already in use within the node processing the request message. Nodes receiving conflict extension will process this message and remove the conflicting OpCode from the list of OpCodes to handle. All conflict messages are relayed, processed or not. If a node receiving a REQUEST is able to match the hash of the provider previously assigned to an existing OpCode, but different to the proposed one, it responds with a CONFLICT with a reason combining the top bit with the previously associated range (0x80 | <old range>). In such cases, the previously used OpCode (start of range) will be placed in the ProposedOpCode. Nodes receiving this conflict message with the top bit raised, will discard the initially proposed OpCode and replace it with the proposed code supplied in the conflict message. */

    /* LargeObjectTransfer model messages */
    CSRMESH_LARGEOBJECTTRANSFER_ANNOUNCE = 0x1A,/*!< \brief A node wanting to provide a large object to neighbouring Mesh Nodes issues an ANNOUNCE with the associated content type. This message will have TTL=0, thus will only be answered by its immediate neighbours. The ANNOUNCE has the total size of the packet to be issued. The format and encoding of the large object is subject to the provided type and is out of scope of this document. The destination ID can either be 0, a group or a specific Device ID. In case the destination ID is not zero, only members of the group (associated with the LOT model) or the device with the specified Device ID responds with the intent to download the object for their own consumption. Every other node either ignores or accepts the offer for the purpose of relaying the packet. */
    CSRMESH_LARGEOBJECTTRANSFER_INTEREST = 0xEF30,/*!< \brief In case a node is ready to receive the proposed object, it responds with this message. The intended behaviour of the Large Object Transfer is to allow a Peer-to-Peer connection between the consumer and the producer. The consumer uses a ServiceID, part of which is randomly selected. The top 64 bits are 0x1122334455667788, the least significant 63 bits are randomly selected by the consumer node. The most significant bit of the least significant 64 bits is an encoding of the intent to relay the received data. Once this message has been issued, the consumer node starts advertising a connectable service with the 128-bits service composed through the concatenation of the fixed 64 bits and the randomly selected 63 bits. The duration of the advertisement is an implementation decision. */

    /* Firmware model messages */
    CSRMESH_FIRMWARE_GET_VERSION = 0x78,/*!< \brief Get firmwre verison: Upon receiving a FIRMWARE_GET_VERSION the device reponds with a FIRMWARE_VERSION message containing the current firmware version */
    CSRMESH_FIRMWARE_VERSION = 0x79,/*!< \brief Firmware version information */
    CSRMESH_FIRMWARE_UPDATE_REQUIRED = 0x7A,/*!< \brief Requesting a firmware update. Upon receiving this message, the device moves to a state where it is ready for receiving a firmware update */
    CSRMESH_FIRMWARE_UPDATE_ACKNOWLEDGED = 0x7b,/*!< \brief Acknowledgement message to the firmware update request */

    /* Diagnostic model messages */
    CSRMESH_DIAGNOSTIC_STATE = 0xEF40,/*!< \brief When received this message is interpreted as to reconfigure the set of information collected. Statistics gathering can be turned on/off ? in the off mode no measurement of messages count and RSSI measurements will be made. RSSI binning can be stored, such that collection ALL messages? RSSI (MASP/MCP, irrespective of encoding) are split between a given number of bin, each of equal dimensions. Masking of individual broadcast channel can be specified, resulting in the collection of information specifically on the selected channels. A REST bit is also available. When present all the accumulated data will be cleared and all counters restarted. Note that it is possible to change various configurations without the RESET flag, this will result in the continuation of accumulation and therefore incoherent statistics. */
    CSRMESH_DIAGNOSTIC_GET_STATS = 0xEF41,/*!< \brief  */
    CSRMESH_DIAGNOSTIC_STATS = 0xEF42,/*!< \brief  */

    /* Action model messages */
    CSRMESH_ACTION_SET_ACTION = 0x50,/*!< \brief This message defines the action which needs to be taken. This is a multiple parts message, each part will be acknowledged through an ACTION_SET_ACK. The message�s target will store this payload and upon defined trigger, will generate a mesh message using the payload. ACTION_SET_ACTION has several parameters, in particular the destination�s device ID. This Device ID can be the device itself, in which case the execution of the payload will be equivalent for this device to receive the message itself, it is up to the implementation to decide whether or not the message should be made visible to the rest of the Mesh. A Transaction ID is also provided. Since an ACTION_SET_ACTION may be composed of several parts, this transaction ID could be incremented with every parts � however, since the ACTION_SET_ACTION message has within its ActionID field a part number, the same transaction ID can be re-uses and the combination of both part-number and transaction ID can be used to identify each individual parts. Whilst there is no restriction in the type of Message one can include in the payload, the current implementation does not handle messages which would come as multiple parts and therefore does not handle the embedding of the ACTION_SET_ACTION!  The ActionID will be used by the target and node initiating this message as a mean to identify the action in future exchange. Note that there are no inherent mechanisms protecting against clashes if more than one device was elected to be the source of Action provision. */
    CSRMESH_ACTION_SET_ACTION_ACK = 0x51,/*!< \brief Every parts received is acknowledged, providing both Transaction ID and ActionID (which includes Part Number) */
    CSRMESH_ACTION_GET_ACTION_STATUS = 0x52,/*!< \brief Request towards a node about the various Actions currently handled. This will be answered through an ACTION_STATUS. A Transaction ID is provided to ensure identification of response with request. */
    CSRMESH_ACTION_ACTION_STATUS = 0x53,/*!< \brief This provides a bitmask identifying all the currently allocated ActionID. The Action Supported field provides an indication on how many actions this device has capacity for. Note that a device will never have more than 32 actions. */
    CSRMESH_ACTION_DELETE = 0x54,/*!< \brief This message allows a set of actions to be removed. Actions are identified through a bitmask reflecting the ActionID one wishes to delete. This message will be acknowledged through ACTION_DELETE_ACK. */
    CSRMESH_ACTION_DELETE_ACK = 0x55,/*!< \brief This message confirms the delete commands and report which actions have been removed. The provided transaction ID will match the one supplied by ACTION_DELETE. */
    CSRMESH_ACTION_GET = 0x56,/*!< \brief Using this message, it is possible to interrogate a node about the specific payload associated with an Action ID. This will be answered by and ACTION_SET_ACTION message, ACTION_SET_ACTION_ACK will need to be issued in order to collect all parts. */

    /* Beacon model messages */
    CSRMESH_BEACON_SET_STATUS = 0x60,/*!< \brief May be sent to a beacon to set its status. More than one such command can be sent to set the intervals for different beacon types, which need not be the same. This message also allows for the wakeup intervals to be set if the beacon elects to only be on the mesh sporadically. The time is always with respect to the beacon�s internal clock and has no absolute meaning. This message will be answered through BEACON_STATUS. */
    CSRMESH_BEACON_GET_BEACON_STATUS = 0x62,/*!< \brief Message to be sent to a beacon in order to recover its current status. This message fetch information pertinent to a particular type. The transaction ID will help matching the pertinent BEACON_STATUS message � a different transaction ID shall be used for a different type (although the BEACON_STATUS message has the type pertinent to the status and thus could be used in conjunction to the Transaction ID for disambiguation). */
    CSRMESH_BEACON_BEACON_STATUS = 0x63,/*!< \brief This message issue issued by a Beacon as response to BEACON_SET_STATUS or BEACON_GET_STATUS. Furthermore, a Beacon appearing on the mesh sporadically, will issue such message (with destination ID set to 0) as soon as it re-joins the mesh. In this case, one message per active beacon type should be issued. This message will provide the Payload ID currently in used for the associated type. */
    CSRMESH_BEACON_GET_TYPES = 0x64,/*!< \brief Message allowing a node to fetch all supported types and associated information by a given Beacon. */
    CSRMESH_BEACON_TYPES = 0x65,/*!< \brief Provides information on the set of beacon supported by the node. The battery level is reported as well as time since last message. */
    CSRMESH_BEACON_SET_PAYLOAD = 0x66,/*!< \brief One or more of these packets may be sent to a beacon to set its payload. The content is either the raw advert data, or extra information (such as crypto cycle) which a beacon may require. The payload data is sent as a length and an offset, so the whole payload need not be sent if it has not changed. A beacon can support many beacon types - it can be sent as many different payloads as needed, one for each type. The first byte of the first payload packet contains the length and offset of the payload and the payload ID; this allows a beacon which already has the payload to send an immediate acknowledgement, saving traffic. This ID will be sent back in the 'Payload ACK' message if the beacon has received the whole payload. The payload may have to be split in several parts, in which case only the last part shall be acknowledged. Upon receiving a BEACON_SET_PAYLOAD, the beacon will update its corresponding beacon data � if data was previously available, it will be replaced by the provided payload, thus a beacon can only hold one payload per beacon type. */
    CSRMESH_BEACON_PAYLOAD_ACK = 0x67,/*!< \brief Only one Acknowledgement message will occur for multiple BEACON_SET_PAYLOAD messages sharing the same transaction ID. Only when the last segment is received that such acknowledgement will be issued. Where missing payload messages exist, the list of their indices will be provided in the Ack field. */
    CSRMESH_BEACON_GET_PAYLOAD = 0x68,/*!< \brief Message allowing a node to retrieve the current payload held on a given beacon. This message shall be answered by one or more BEACON_SET_PAYLOAD messages. */

    /* BeaconProxy model messages */
    CSRMESH_BEACONPROXY_ADD = 0x69,/*!< \brief Message can be sent to a beacon-proxy to add to the list of devices it manages. This request will be acknowledged using a Proxy Command Status Devices. Up to four device ID can be specified in this message � Group can be defined. This message also permits the flushing of pending messages for managed IDs. */
    CSRMESH_BEACONPROXY_REMOVE = 0x6A,/*!< \brief May be sent to a proxy to remove from the list of devices it manages. Any message queued for those devices will be cleared. This request will be acknowledged using a Proxy Command Status Devices. Groups can be used, implying that all its members will be removed from the proxy management. Specifying 0 in the Device Addresses will be interpreted as stopping all Proxy activities on the targeted proxy. */
    CSRMESH_BEACONPROXY_COMMAND_STATUS_DEVICES = 0x6B,/*!< \brief Generic acknowledgement - the transaction ID permits reconciliation with sender. */
    CSRMESH_BEACONPROXY_GET_STATUS = 0x6C,/*!< \brief Fetch the current status - this will be answered by a BEACON_PROXY_STATUS */
    CSRMESH_BEACONPROXY_PROXY_STATUS = 0x6D,/*!< \brief Provides generic information on the internal states of a Proxy. Including number of managed nodes, groups, states of the various queues. */
    CSRMESH_BEACONPROXY_GET_DEVICES = 0x6E,/*!< \brief Fetch the current set of devices ID managed by a given proxy. This will be answered by one or more BEACON_PROXY_DEVICES messages. */
    CSRMESH_BEACONPROXY_DEVICES = 0x6F,/*!< \brief Provide the list of Devices and Groups currently managed by a given Proxy. This will be a multiple parts message. */

}CSRMESH_MODEL_EVENT_T;

/*! \brief CSRmesh Event Data  */
typedef struct
{
    CsrUint8 nw_id; /* \brief CSRmesh network ID over which the message is received */
    CsrUint32 seq_num; /* \brief MCP sequence number of message */
    CsrUint16 src_id;  /* \brief 16 bit ID of the device which sent the message */
    CsrUint16 dst_id;  /* \brief 16 bit ID of the device/group to which the message was sent */
    CsrUint8  rx_ttl;  /* \brief TTL of the received message */
    CsrUint8  rsp_ttl; /* \brief TTL to be set for the response message. Default \ref CSR_MESH_DEFAULT_TTL. The application can change this value in the model event callback. */
    CsrInt8   rx_rssi; /* \brief RSSI of the received message */
    CsrBool   *allow_relay; /* \brief Flag to specify whether the msg should be relayed by the stack or not */
    void   *data; /* \brief Pointer to the event specific data structure */
}CSRMESH_EVENT_DATA_T;

/*! \brief MCP Model handler function type */
typedef CSRmeshResult (*CSRMESH_MODEL_CALLBACK_T)(CSRMESH_MODEL_EVENT_T event_code,
                       CSRMESH_EVENT_DATA_T * data,
                       CsrUint16 length, void **state_data);

/*! @} */
#endif /* __CSR_MESH_MODEL_COMMON_H__ */

