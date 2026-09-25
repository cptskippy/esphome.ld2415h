#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

#include "LD2415H.h"

#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif
#ifdef USE_SELECT
#include "esphome/components/select/select.h"
#endif

namespace esphome {
namespace ld2415h {

// Thin ESPHome wrapper around the transport-agnostic ld2415h::LD2415H
// protocol engine (ld2415h library).
//
// Owns the radar instance and adapts it to ESPHome: UART provides the
// transport, the logger forwards to ESP_LOG*, and parsed config state
// is published to the registered number/select entities.
class LD2415HComponent : public Component,
                         public uart::UARTDevice,
                         public ::ld2415h::Transport,
                         public ::ld2415h::Logger,
                         public ::ld2415h::Listener {
 public:
  LD2415HComponent();
  void setup() override;
  void dump_config() override;
  void loop() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }

  // ::ld2415h::Transport
  int available() override;
  int read() override;
  void write(const uint8_t *data, uint8_t size) override;

  // ::ld2415h::Logger
  void log(::ld2415h::LogLevel level, const char *tag, const char *message) override;

  // ::ld2415h::Listener
  void onSpeed(float speed) override {}
  void onVelocity(float velocity) override {}
  void onConfig() override;

  // Forward listener registration for entities (sensors).
  void register_listener(::ld2415h::Listener *listener) { this->radar_.registerListener(listener); }

  // Setters used by the number/select entities.
  void set_min_speed_threshold(uint8_t value) { this->radar_.setMinSpeedThreshold(value); }
  void set_compensation_angle(uint8_t value) { this->radar_.setCompensationAngle(value); }
  void set_sensitivity(uint8_t value) { this->radar_.setSensitivity(value); }
  void set_vibration_correction(uint8_t value) { this->radar_.setVibrationCorrection(value); }
  void set_relay_trigger_duration(uint8_t value) { this->radar_.setRelayTriggerDuration(value); }
  void set_relay_trigger_speed(uint8_t value) { this->radar_.setRelayTriggerSpeed(value); }
  void set_tracking_mode(::ld2415h::TrackingMode mode) { this->radar_.setTrackingMode(mode); }
  void set_tracking_mode(const std::string &state);
  void set_sample_rate(uint8_t rate) { this->radar_.setSampleRate(rate); }
  void set_sample_rate(const std::string &state);

#ifdef USE_NUMBER
  void set_min_speed_threshold_number(number::Number *number) { this->min_speed_threshold_number_ = number; }
  void set_compensation_angle_number(number::Number *number) { this->compensation_angle_number_ = number; }
  void set_sensitivity_number(number::Number *number) { this->sensitivity_number_ = number; }
  void set_vibration_correction_number(number::Number *number) { this->vibration_correction_number_ = number; }
  void set_relay_trigger_duration_number(number::Number *number) { this->relay_trigger_duration_number_ = number; }
  void set_relay_trigger_speed_number(number::Number *number) { this->relay_trigger_speed_number_ = number; }
#endif
#ifdef USE_SELECT
  void set_sample_rate_select(select::Select *selector) { this->sample_rate_selector_ = selector; }
  void set_tracking_mode_select(select::Select *selector) { this->tracking_mode_selector_ = selector; }
#endif

 protected:
  void publish_config_state_();

  ::ld2415h::LD2415H radar_{this, this};

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
};

}  // namespace ld2415h
}  // namespace esphome
