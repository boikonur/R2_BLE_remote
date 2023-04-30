#ifndef SPHERO_PROTOCOL_API_H
#define SPHERO_PROTOCOL_API_H
// #pragma once

#include <Arduino.h>
#include <cstdint>
#include <array>

/*
| SOP:	| Start of Packet	| Control byte identifying the start of the packet |
| FLAGS: | Packet Flags	| Bit-flags that modify the behavior of the packet |
| TID:	| Target ID	| Address of the target, expressed as a port ID (upper nibble) and a node ID (lower nibble). (Optional) |
| SID:	| Source ID	| Address of the source, expressed as a port ID (upper nibble) and a node ID (lower nibble). (Optional) |
| DID:	| Device ID	| The command group ("virtual device") of the command being sent |
| CID:	| Command ID |	The command to execute |
| SEQ:	| Sequence Number |	The token used to link commands with responses |
| ERR:	| Error Code |	Command error code of the response packet (optional) |
| DATA: | Message Data |	Zero or more bytes of message data |
| CHK:	| Checksum	| The sum of all bytes (excluding SOP & EOP) mod 256, bit-inverted |
| EOP:	| End of Packet | 	Control byte identifying the end of the packet |
*/

// Packet structure:
//     ---------------------------------
//     - start      [1 byte]
//     - flags      [1 byte]
//     - source_id  [1 byte] (optional)
//     - target_id  [1 byte] (optional)
//     - device_id  [1 byte]
//     - command_id [1 byte]
//     - data       [n byte]
//     - checksum   [1 byte]
//     - end        [1 byte]
//     ---------------------------------

// MSG_INIT{ 0x0A, 0x13, 0x0D } =   requests_response &resets_inactivity_timeout (Flags), DeviceId.power (DID), PowerCommandIds.wake(CID)
// MSG_ROTATE[] = {0x0A,0x17,0x0F};   requests_response &resets_inactivity_timeout (Flags) , DeviceId.animatronics, AnimatronicsCommandIds.set_head_position(CID),
// const uint8_t MSG_CONNECTION[]= {0x75, 0x73, 0x65, 0x74, 0x68, 0x65, 0x66, 0x6F, 0x72, 0x63, 0x65, 0x2E, 0x2E, 0x2E, 0x62, 0x61, 0x6E, 0x64 }; //  string = "usetheforece...band"
// const uint8_t MSG_INIT[] =      {0x0A, 0x13, 0x0D }; //0x8D 0x0A 0x13 0x0D 0x00 0xD5 0xD8
// const uint8_t MSG_OFF[] =       {0x0A, 0x13, 0x01 }; //0x8D 0x0A 0x13 0x01 0x20 0xC1 0xD8
// const uint8_t MSG_ROTATE[] =    {0x0A, 0x17, 0x0F};
// const uint8_t MSG_ANIMATION[] = {0x0A, 0x17, 0x05}; //MSG_ANIMATION, [0x00, 7]), MSG_ANIMATION, [0x00, 13]
// const uint8_t MSG_CARRIAGE[] =  {0x0A, 0x17, 0x0D};

enum class Flag : int
{
  response = 0x1,
  requests_response = 0x2,
  requests_only_error_response = 0x4,
  resets_inactivity_timeout = 0x8,
  command_has_target_id = 0x10,
  command_has_source_id = 0x20
};

enum class DeviceId : uint8_t
{
  api_processor = 0x10,
  system_info = 0x11,
  system_modes = 0x12,
  power = 0x13,
  driving = 0x16,
  animatronics = 0x17,
  sensors = 0x18,
  peer_connection = 0x19,
  user_io = 0x1a,
  storage_command = 0x1b,
  secondary_mcu_firmware_update_command = 0x1d,
  wifi_command = 0x1e,
  factory_test = 0x1f,
  macro_system = 0x20,
  proto = 0xfe
}

// POWER
enum class PowerCommand : int
{
  enter_deep_sleep = 0x00,
  enter_soft_sleep = 0x01,
  get_usb_state = 0x02,
  get_battery_voltage = 0x03,
  get_battery_state_LMQ = 0x04, // value from Core library Used by LMQ
  enable_battery_state_change_notification = 0x05,
  battery_state_changed_LMQ = 0x06, // value from Core library used by LMQ
  wake = 0x0d,
  get_battery_percentage = 0x10,
  set_power_options = 0x12,
  get_power_options = 0x13,
  get_battery_state = 0x17,
  will_sleep_async = 0x19,
  sleep_async = 0x1a,
  battery_state_changed = 0x1f
}

enum class BatteryVoltageStates : int
{
  ok = 0x01,
  low = 0x02,
  critical = 0x03
}

enum class ChargerStates : int
{
  not_charging = 0x01,
  charging = 0x02,
  charged = 0x03
}
// ANIMATRONICS
enum class AnimatronicsCommandIds : int
{
  play_animation = 0x05,
  perform_leg_action = 0x0d,
  set_head_position = 0x0f,
  play_animation_complete_notify = 0x11,
  get_head_position = 0x14,
  set_leg_position = 0x15,
  get_leg_position = 0x16,
  get_leg_action = 0x25,
  leg_action_complete_notify = 0x26,
  stop_animation = 0x2b,
  get_trophy_mode_enabled = 0x2e
}

enum class UserIOCommand : int
{
  enable_gesture_event_notification = 0x00,
  gesture_event = 0x01,
  enable_button_event_notification = 0x02,
  button_event = 0x03,
  set_led = 0x04,
  release_led_request = 0x05,
  play_haptic_pattern = 0x06,
  play_audio_file = 0x07,
  set_audio_volume = 0x08,
  get_audio_volume = 0x09,
  stop_all_audio = 0x0a,
  cap_touch_enable = 0x0b,
  ambient_light_sensor_enable = 0x0c,
  enable_ir = 0x0d,
  set_all_leds = 0x0e, //<-- this one works for R2 and blacky
  set_backlight_intensity = 0x0f,
  cap_touch_indication = 0x10,
  enable_debug_data = 0x11,
  assert_lcd_reset_pin = 0x12,
  set_headlights = 0x13,
  set_taillights = 0x14,
  play_test_tone = 0x18,
  start_idle_led = 0x19,
  toy_commands = 0x20,
  toy_events = 0x21,
  set_user_profile = 0x22,
  get_user_profile = 0x23,
  set_all_leds_32_bit_mask = 0x1a,
  set_all_leds_64_bit_mask = 0x1b,
  set_all_leds_8_bit_mask = 0x1c,
  set_led_matrix_pixel = 0x2d,
  set_led_matrix_one_color = 0x2f,
  set_led_matrix_frame_rotation = 0x3a,
  set_led_matrix_text_scrolling = 0x3b,
  set_led_matrix_text_scrolling_notify = 0x3c,
  set_led_matrix_single_character = 0x42
}

enum class SystemModeCommand : int
{
   set_system_mode = 0x00,
    get_system_mode = 0x01,
    enable_system_mode_notification = 0x02,
    system_mode_changed = 0x03,
    set_max_speed = 0x04,
    get_max_speed = 0x05,
    set_current_weapon = 0x06,
    get_current_weapon = 0x07,
    set_aiming_mode = 0x08,
    get_playmode_unlock_mask = 0x09,
    set_playmode_unlock_mask = 0x0a,
    enable_menu_item_change_notification = 0x0b,
    menu_item_changed = 0x0c,
    get_enabled_weapons_mask = 0x0d,
    set_enabled_weapons_mask = 0x0e,
    get_holocron_counts = 0x10,
    dec_holocron_count = 0x13,
    clear_holocron_count = 0x14,
    find_holocron_now = 0x15,
    enable_force_awareness_reporting = 0x16,
    force_awareness_event_async = 0x17,
    set_category_rarity = 0x18,
    get_angle_to_holocron = 0x19,
    set_fa_disturbance_timeout = 0x1a,
    set_audio_header = 0x1b,
    set_audio_header_async = 0x1c,
    enable_robot_bas_app_report = 0x20,
    robot_bad_app_report = 0x21,
    get_pending_action_list = 0x22,
    get_audio_info = 0x23
} 

// AnimatronicsCommandIds + animation

enum class R2LegAction : int
{
  stop = 0x00,
  three_legs = 0x01,
  two_legs = 0x02,
  waddle = 0x03
}

enum class R2D2Animation : int
{
  charger_1 = 0x00,
  charger_2 = 0x01,
  charger_3 = 0x02,
  charger_4 = 0x03,
  charger_5 = 0x04,
  charger_6 = 0x05,
  charger_7 = 0x06,
  emote_alarm = 0x07,
  emote_angry = 0x08,
  emote_annoyed = 0x09,
  emote_chatty = 0x0a,
  emote_drive = 0x0b,
  emote_excited = 0x0c,
  emote_happy = 0x0d,
  emote_ion_blast = 0x0e,
  emote_laugh = 0x0f,
  emote_no = 0x10,
  emote_sad = 0x11,
  emote_sassy = 0x12,
  emote_scared = 0x13,
  emote_spin = 0x14,
  emote_yes = 0x15,
  emote_scan = 0x16,
  emote_sleep = 0x17,
  emote_surprised = 0x18,
  idle_1 = 0x19,
  idle_2 = 0x1a,
  idle_3 = 0x1b,
  patrol_alarm = 0x1c,
  patrol_hit = 0x1d,
  patrol_patrolling = 0x1e,
  wwm_angry = 0x1f,
  wwm_anxious = 0x20,
  wwm_bow = 0x21,
  wwm_concern = 0x22,
  wwm_curious = 0x23,
  wwm_double_take = 0x24,
  wwm_excited = 0x25,
  wwm_fiery = 0x26,
  wwm_frustrated = 0x27,
  wwm_happy = 0x28,
  wwm_jittery = 0x29,
  wwm_laugh = 0x2a,
  wwm_long_shake = 0x2b,
  wwm_no = 0x2c,
  wwm_ominous = 0x2d,
  wwm_relieved = 0x2e,
  wwm_sad = 0x2f,
  wwm_scared = 0x30,
  wwm_shake = 0x31,
  wwm_surprised = 0x32,
  wwm_taunting = 0x33,
  wwm_whisper = 0x34,
  wwm_yelling = 0x35,
  wwm_yoohoo = 0x36,
  motor = 0x37
}

enum class R2Q5Animation : int
{
  charger_1 = 0x00,
  charger_3 = 0x01,
  charger_2 = 0x02,
  charger_4 = 0x03,
  charger_5 = 0x04,
  charger_6 = 0x05,
  charger_7 = 0x06,
  emote_alarm = 0x07,
  emote_angry = 0x08,
  emote_attention = 0x09,
  emote_frustrated = 0x0a,
  emote_drive = 0x0b,
  emote_excited = 0x0c,
  emote_search = 0x0d,
  emote_short_circuit = 0x0e,
  emote_laugh = 0x0f,
  emote_no = 0x10,
  emote_retreat = 0x11,
  emote_fiery = 0x12,
  emote_understood = 0x13,
  emote_yes = 0x15,
  emote_scan = 0x16,
  emote_surprised = 0x18,
  idle_1 = 0x19,
  idle_2 = 0x1a,
  idle_3 = 0x1b,
  wwm_angry = 0x1f,
  wwm_anxious = 0x20,
  wwm_bow = 0x21,
  wwm_concern = 0x22,
  wwm_curious = 0x23,
  wwm_double_take = 0x24,
  wwm_excited = 0x25,
  wwm_fiery = 0x26,
  wmm_frustrated = 0x27,
  wwm_happy = 0x28,
  wwm_jittery = 0x29,
  wwm_laugh = 0x2a,
  wwm_long_shake = 0x2b,
  wwm_no = 0x2c,
  wwm_ominous = 0x2d,
  wwm_relieved = 0x2e,
  wwm_sad = 0x2f,
  wwm_scared = 0x30,
  wwm_shake = 0x31,
  wwm_surprised = 0x32,
  wwm_taunting = 0x33,
  wwm_whisper = 0x34,
  wwm_yelling = 0x35,
  wwm_yoohoo = 0x36
}

enum class BB9EAnimation : int
{
  EMOTE_ALARM = 0x00,
  EMOTE_NO = 0x01,
  EMOTE_SCAN_SWEEP = 0x02,
  EMOTE_SCARED = 0x03,
  EMOTE_YES = 0x04,
  EMOTE_AFFIRMATIVE = 0x05,
  EMOTE_AGITATED = 0x06,
  EMOTE_ANGRY = 0x07,
  EMOTE_CONTENT = 0x08,
  EMOTE_EXCITED = 0x09,
  EMOTE_FIERY = 0x0a,
  EMOTE_GREETINGS = 0x0b,
  EMOTE_NERVOUS = 0x0c,
  EMOTE_SLEEP = 0x0e,
  EMOTE_SURPRISED = 0x0f,
  EMOTE_UNDERSTOOD = 0x10,
  HIT = 0x11,
  WWM_ANGRY = 0x12,
  WWM_ANXIOUS = 0x13,
  WWM_BOW = 0x14,
  WWM_CURIOUS = 0x16,
  WWM_DOUBLE_TAKE = 0x17,
  WWM_EXCITED = 0x18,
  WWM_FIERY = 0x19,
  WWM_HAPPY = 0x1a,
  WWM_JITTERY = 0x1b,
  WWM_LAUGH = 0x1c,
  WWM_LONG_SHAKE = 0x1d,
  WWM_NO = 0x1e,
  WWM_OMINOUS = 0x1f,
  WWM_RELIEVED = 0x20,
  WWM_SAD = 0x21,
  WWM_SCARED = 0x22,
  WWM_SHAKE = 0x23,
  WWM_SURPRISED = 0x24,
  WWM_TAUNTING = 0x25,
  WWM_WHISPER = 0x26,
  WWM_YELLING = 0x27,
  WWM_YOOHOO = 0x28,
  WWM_FRUSTRATED = 0x29,
  IDLE_1 = 0x2a,
  IDLE_2 = 0x2b,
  IDLE_3 = 0x2c,
  EYE_1 = 0x2d,
  EYE_2 = 0x2e,
  EYE_3 = 0x2f,
  EYE_4 = 0x30
}

enum class Api2Error : int
{
  success = 0x00,
  bad_device_id = 0x01,
  bad_command_id = 0x02,
  not_yet_implemented = 0x03,
  command_is_restricted = 0x04,
  bad_data_length = 0x05,
  command_failed = 0x06,
  bad_parameter_value = 0x07,
  busy = 0x08,
  bad_target_id = 0x09,
  target_unavailable = 0x0a,
  unknown = 0xff
}


enum class Version : uint8_t{
  // major: int
  //   minor: int
  //   revision: int
}
  


enum class SystemInfoCommand :uint8_t{
    get_main_application_version = 0x00,
    get_bootloader_version = 0x01,
    get_main_application_build_info = 0x02,
    get_board_revision = 0x03,
    config_block_write = 0x04,
    config_block_write_complete = 0x05,
    get_mac_address = 0x06,
    get_user_configblock = 0x07,
    set_user_configblock = 0x08,
    set_user_configblock_complete = 0x09,
    disable_factory_mode = 0x0a,
    get_log_status = 0x0b,
    get_log_chunk = 0x0c,
    clear_log = 0x0d,
    get_nordic_temperature = 0x0e,
    startup_async = 0x0f,
    get_model_number = 0x12,
    get_stats_id = 0x13,
    set_device_mode = 0x14,
    get_device_mode = 0x15,
    get_device_mode_async = 0x16,
    get_secondary_mcu_version = 0x17,
    get_secondary_mcu_version_async = 0x18,
    get_animation_version = 0x19,
    get_animation_version_async = 0x1a,
    set_audio_crc = 0x22,
    get_audio_crc = 0x23,
    get_level_one_diagnostics = 0x26,
    level_one_diagnostics_async = 0x27,
    get_sku_LMQ = 0x28 // value from Core library used by LMQ,
    get_secondary_mcu_status = 0x29,
    secondary_mcu_status_async = 0x2a,
    get_sku = 0x38
}

  //  response = self.request(SystemInfoCommand.get_bootloader_version)
  //       return Version(
  //           major=int.from_bytes(response.data[:2], "big"),
  //           minor=int.from_bytes(response.data[2:4], "big"),
  //           revision=int.from_bytes(response.data[4:], "big"),
    
// enum class GenericCharacteristic : int {
//     device_name = 0x2a00,
//     client_characteristic_configuration = 0x2902,
//     peripheral_preferred_connection_parameters = 0x2a04
//}
// enum class SpheroCharacteristic : int{
//     force_band = "00020005-574f-4f20-5370-6865726f2121",
//     api_v2 = "00010002-574f-4f20-5370-6865726f2121",
//}


class SpheroProtocol
{
public:
  static constexpr size_t maxDataSize = 256;

  struct Packet
  {
    uint8_t flags;
    uint8_t tid;
    uint8_t sid;
    uint8_t did;
    uint8_t cid;
    uint8_t seq;
    uint8_t err;
    std::array<uint8_t, maxDataSize> data;
    size_t dataSize;
  };

  static std::array<uint8_t, maxDataSize * 2> createPacket(const Packet &packet, size_t &packetSize);
  static Packet parsePacket(const std::array<uint8_t, maxDataSize * 2> &rawData, size_t packetSize);

  // FLAGS related methods
  static bool isResponse(const Packet &packet);
  static void setResponseFlag(Packet &packet, bool value);
  static bool shouldRequestResponse(const Packet &packet);
  static void setRequestResponseFlag(Packet &packet, bool value);
  // ... other FLAGS related methods

  // Error code related methods
  static uint8_t getErrorCode(const Packet &packet);
  static void setErrorCode(Packet &packet, uint8_t errorCode);

private:
  static constexpr uint8_t ESC = 0xAB;
  static constexpr uint8_t SOP = 0x8D;
  static constexpr uint8_t EOP = 0xD8;
  static constexpr uint8_t ESC_ESC = 0x23;
  static constexpr uint8_t ESC_SOP = 0x05;
  static constexpr uint8_t ESC_EOP = 0x50;

  static std::array<uint8_t, maxDataSize * 2> encodePayload(const std::array<uint8_t, maxDataSize> &payload, size_t dataSize, size_t &encodedSize);
  static std::array<uint8_t, maxDataSize> decodePayload(const std::array<uint8_t, maxDataSize * 2> &encodedPayload, size_t encodedSize, size_t &dataSize);
  static uint8_t calculateChk(const Packet &packet);
};

#endif // end SPHERO_PROTOCOL_API_H
