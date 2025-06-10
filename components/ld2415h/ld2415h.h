#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif
#ifdef USE_SELECT
#include "esphome/components/select/select.h"
#endif
#include <map>

namespace esphome {
namespace ld2415h {

enum NegotiationMode : uint8_t { CUSTOM_AGREEMENT = 0x01, STANDARD_PROTOCOL = 0x02 };
enum SampleRateStructure : uint8_t { SAMPLE_RATE_22FPS = 0x00, SAMPLE_RATE_11FPS = 0x01, SAMPLE_RATE_6FPS = 0x02 };
enum TrackingMode : uint8_t { APPROACHING_AND_RETREATING = 0x00, APPROACHING = 0x01, RETREATING = 0x02 };
enum UnitOfMeasure : uint8_t { KPH = 0x00, MPH = 0x01, MPS = 0x02 };

static const std::map<std::string, uint8_t> NEGOTIATION_MODE_STR_TO_INT{
    {"Custom Agreement", CUSTOM_AGREEMENT}, {"Standard Protocol", STANDARD_PROTOCOL}};

static const std::map<std::string, uint8_t> SAMPLE_RATE_STR_TO_INT{
    {"~22 fps", SAMPLE_RATE_22FPS}, {"~11 fps", SAMPLE_RATE_11FPS}, {"~6 fps", SAMPLE_RATE_6FPS}};

static const std::map<std::string, uint8_t> TRACKING_MODE_STR_TO_INT{
    {"Approaching and Retreating", APPROACHING_AND_RETREATING},
    {"Approaching", APPROACHING},
    {"Retreating", RETREATING}};

static const std::map<std::string, uint8_t> UNIT_OF_MEASURE_STR_TO_INT{
    {"km/h", KPH}, {"mph", MPH}, {"m/s", MPS}};


class LD2415HListener {
 public:
  virtual void on_speed(double speed){};
  virtual void on_velocity(double velocity){};
};

class LD2415HComponent : public Component, public uart::UARTDevice {
 public:
  // Constructor declaration
  LD2415HComponent();
  void setup() override;
  void dump_config() override;
  void loop() override;

#ifdef USE_NUMBER
  void set_min_speed_threshold_number(number::Number *number) { this->min_speed_threshold_number_ = number; };
  void set_compensation_angle_number(number::Number *number) { this->compensation_angle_number_ = number; };
  void set_sensitivity_number(number::Number *number) { this->sensitivity_number_ = number; };
  void set_vibration_correction_number(number::Number *number) { this->vibration_correction_number_ = number; };
  void set_relay_trigger_duration_number(number::Number *number) { this->relay_trigger_duration_number_ = number; };
  void set_relay_trigger_speed_number(number::Number *number) { this->relay_trigger_speed_number_ = number; };
#endif
#ifdef USE_SELECT
  void set_sample_rate_select(select::Select *selector) { this->sample_rate_selector_ = selector; };
  void set_tracking_mode_select(select::Select *selector) { this->tracking_mode_selector_ = selector; };
#endif

  float get_setup_priority() const override { return setup_priority::HARDWARE; }
  void register_listener(LD2415HListener *listener) { this->listeners_.push_back(listener); }
  // void set_speed_sensor(sensor::Sensor *sensor) { this->speed_sensor_ = sensor; };
  // void set_velocity_sensor(sensor::Sensor *sensor) { this->velocity_sensor_ = sensor; };

  void set_min_speed_threshold(uint8_t speed);
  void set_compensation_angle(uint8_t angle);
  void set_sensitivity(uint8_t sensitivity);
  void set_tracking_mode(const std::string &state);
  void set_tracking_mode(TrackingMode mode);
  void set_tracking_mode(uint8_t mode);
  void set_sample_rate(const std::string &state);
  void set_sample_rate(uint8_t rate);
  void set_vibration_correction(uint8_t correction);
  void set_relay_trigger_duration(uint8_t duration);
  void set_relay_trigger_speed(uint8_t speed);

#ifdef USE_NUMBER
  number::Number *min_speed_threshold_number_{nullptr};
  number::Number *compensation_angle_number_{nullptr};
  number::Number *sensitivity_number_{nullptr};
  number::Number *vibration_correction_number_{nullptr};
  number::Number *relay_trigger_duration_number_{nullptr};
  number::Number *relay_trigger_speed_number_{nullptr};
#endif
#ifdef USE_SELECT
  select::Select *sample_rate_selector_{nullptr};
  select::Select *tracking_mode_selector_{nullptr};
#endif

 protected:
  sensor::Sensor *speed_sensor_{nullptr};
  sensor::Sensor *velocity_sensor_{nullptr};

  // Configuration
  uint8_t min_speed_threshold_ = 1;
  uint8_t compensation_angle_ = 0;
  uint8_t sensitivity_ = 10;
  TrackingMode tracking_mode_ = TrackingMode::APPROACHING_AND_RETREATING;
  uint8_t sample_rate_ = 1;
  UnitOfMeasure unit_of_measure_ = UnitOfMeasure::KPH;
  uint8_t vibration_correction_ = 18;
  uint8_t relay_trigger_duration_ = 0;
  uint8_t relay_trigger_speed_ = 1;
  NegotiationMode negotiation_mode_ = NegotiationMode::CUSTOM_AGREEMENT;

  // State
  uint8_t cmd_set_speed_angle_sense_[8] = {0x43, 0x46, 0x01, 0x01, 0x00, 0x05, 0x0d, 0x0a};
  uint8_t cmd_set_mode_rate_uom_[8] = {0x43, 0x46, 0x02, 0x01, 0x01, 0x00, 0x0d, 0x0a};
  uint8_t cmd_set_anti_vib_comp_[8] = {0x43, 0x46, 0x03, 0x05, 0x00, 0x00, 0x0d, 0x0a};
  uint8_t cmd_set_relay_duration_speed_[8] = {0x43, 0x46, 0x04, 0x03, 0x01, 0x00, 0x0d, 0x0a};
  uint8_t cmd_get_config_[13] = {0x43, 0x46, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  bool update_speed_angle_sense_ = true;
  bool update_mode_rate_uom_ = true;
  bool update_anti_vib_comp_ = true;
  bool update_relay_duration_speed_ = true;
  bool update_config_ = false;

  char firmware_[20] = "";
  double speed_ = 0;
  double velocity_ = 0;
  char response_buffer_[64];
  uint8_t response_buffer_index_ = 0;
  std::vector<LD2415HListener *> listeners_{};

  // Processing
  void issue_command_(const uint8_t cmd[], uint8_t size);
  bool fill_buffer_(char c);
  void clear_remaining_buffer_(uint8_t pos);
  void parse_buffer_();
  void parse_config_();
  void parse_firmware_();
  void parse_speed_();
  void parse_config_param_(char *key, char *value);

  // Helpers
  TrackingMode i_to_tracking_mode_(uint8_t value);
  UnitOfMeasure i_to_unit_of_measure_(uint8_t value);
  NegotiationMode i_to_negotiation_mode_(uint8_t value);
  const char *i_to_s_(const std::map<std::string, uint8_t> &map, uint8_t i);

};

}  // namespace ld2415h
}  // namespace esphome
