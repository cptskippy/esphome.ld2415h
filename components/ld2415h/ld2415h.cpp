#include "ld2415h.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ld2415h {

static const char *const TAG = "ld2415h";

LD2415HComponent::LD2415HComponent() {
  // Listen for parsed config so we can publish it to the number/select
  // entities once the sensor's state read comes back.
  this->radar_.registerListener(this);
}

void LD2415HComponent::setup() {
  this->config_pref_ =
      global_preferences->make_preference<PersistentConfig>(this->config_pref_key_);
  this->config_pref_ready_ = true;

  // Load the saved configuration. A missing record means first boot;
  // a record with valid=false means the user cleared it via reset.
  // Both use the built-in defaults, and neither writes flash.
  PersistentConfig stored;
  if (this->config_pref_.load(&stored) && stored.valid && stored.version == 1) {
    this->radar_.mutableConfiguration() = stored.config;
    ESP_LOGI(TAG, "Loaded saved configuration");
  } else {
    ESP_LOGI(TAG, "No saved configuration; using defaults");
  }

  // Publish the current configuration state to the number/select
  // entities; when the sensor's config read response arrives,
  // onConfig() republishes with the sensor's values.
  this->publish_config_state_();

  // Request the current configuration from the sensor; it arrives as a
  // response line and triggers onConfig() to publish entity state.
  this->radar_.requestConfig();
}

void LD2415HComponent::dump_config() {
  const auto &cfg = this->radar_.getConfiguration();

  ESP_LOGCONFIG(TAG, "LD2415H:");
  ESP_LOGCONFIG(TAG, "  Firmware: %s", this->radar_.getFirmwareVersion().c_str());
  ESP_LOGCONFIG(TAG, "  Minimum Speed Threshold: %u KPH", cfg.minSpeedThreshold);
  ESP_LOGCONFIG(TAG, "  Compensation Angle: %u", cfg.compensationAngle);
  ESP_LOGCONFIG(TAG, "  Sensitivity: %u", cfg.sensitivity);
  ESP_LOGCONFIG(TAG, "  Tracking Mode: %s", ::hlk::ld2415h::enumToString(::hlk::ld2415h::trackingModeStrings(), static_cast<uint8_t>(cfg.trackingMode)));
  ESP_LOGCONFIG(TAG, "  Sampling Rate: %s", ::hlk::ld2415h::enumToString(::hlk::ld2415h::sampleRateStrings(), cfg.sampleRate));
  ESP_LOGCONFIG(TAG, "  Unit of Measure: %s", ::hlk::ld2415h::enumToString(::hlk::ld2415h::unitOfMeasureStrings(), static_cast<uint8_t>(cfg.unitOfMeasure)));
  ESP_LOGCONFIG(TAG, "  Vibration Correction: %u", cfg.vibrationCorrection);
  ESP_LOGCONFIG(TAG, "  Relay Trigger Duration: %u", cfg.relayTriggerDuration);
  ESP_LOGCONFIG(TAG, "  Relay Trigger Speed: %u KPH", cfg.relayTriggerSpeed);
  ESP_LOGCONFIG(TAG, "  Negotiation Mode: %s", ::hlk::ld2415h::enumToString(::hlk::ld2415h::negotiationModeStrings(), static_cast<uint8_t>(cfg.negotiationMode)));
}

void LD2415HComponent::loop() { this->radar_.update(); }

// ::hlk::ld2415h::Transport

int LD2415HComponent::available() { return this->uart::UARTDevice::available(); }

int LD2415HComponent::read() { return this->uart::UARTDevice::read(); }

void LD2415HComponent::write(const uint8_t *data, uint8_t size) { this->write_array(data, size); }

// ::hlk::ld2415h::Logger

void LD2415HComponent::log(::hlk::ld2415h::LogLevel level, const char *, const char *message) {
  switch (level) {
    case ::hlk::ld2415h::LogLevel::DEBUG:
      ESP_LOGD(TAG, "%s", message);
      break;
    case ::hlk::ld2415h::LogLevel::INFO:
      ESP_LOGI(TAG, "%s", message);
      break;
    case ::hlk::ld2415h::LogLevel::WARN:
      ESP_LOGW(TAG, "%s", message);
      break;
    case ::hlk::ld2415h::LogLevel::ERROR:
      ESP_LOGE(TAG, "%s", message);
      break;
  }
}

// ::hlk::ld2415h::Listener

void LD2415HComponent::onConfig() { this->publish_config_state_(); }

void LD2415HComponent::set_tracking_mode(const std::string &state) {
  auto it = ::hlk::ld2415h::trackingModeStrings().find(state);
  if (it == ::hlk::ld2415h::trackingModeStrings().end()) {
    ESP_LOGE(TAG, "Invalid tracking mode: %s", state.c_str());
    return;
  }
  this->set_tracking_mode(static_cast<::hlk::ld2415h::TrackingMode>(it->second));
  if (this->tracking_mode_selector_ != nullptr)
    this->tracking_mode_selector_->publish_state(state);
}

void LD2415HComponent::set_sample_rate(const std::string &state) {
  auto it = ::hlk::ld2415h::sampleRateStrings().find(state);
  if (it == ::hlk::ld2415h::sampleRateStrings().end()) {
    ESP_LOGE(TAG, "Invalid sample rate: %s", state.c_str());
    return;
  }
  this->set_sample_rate(it->second);
  if (this->sample_rate_selector_ != nullptr)
    this->sample_rate_selector_->publish_state(state);
}

// Entity state publishing

void LD2415HComponent::publish_config_state_() {
  const auto &cfg = this->radar_.getConfiguration();

  #ifdef USE_NUMBER
  if (this->min_speed_threshold_number_ != nullptr)
    this->min_speed_threshold_number_->publish_state(cfg.minSpeedThreshold);
  if (this->compensation_angle_number_ != nullptr)
    this->compensation_angle_number_->publish_state(cfg.compensationAngle);
  if (this->sensitivity_number_ != nullptr)
    this->sensitivity_number_->publish_state(cfg.sensitivity);
  if (this->vibration_correction_number_ != nullptr)
    this->vibration_correction_number_->publish_state(cfg.vibrationCorrection);
  if (this->relay_trigger_duration_number_ != nullptr)
    this->relay_trigger_duration_number_->publish_state(cfg.relayTriggerDuration);
  if (this->relay_trigger_speed_number_ != nullptr)
    this->relay_trigger_speed_number_->publish_state(cfg.relayTriggerSpeed);
  #endif

  #ifdef USE_SELECT
  if (this->tracking_mode_selector_ != nullptr)
    this->tracking_mode_selector_->publish_state(
        ::hlk::ld2415h::enumToString(::hlk::ld2415h::trackingModeStrings(), static_cast<uint8_t>(cfg.trackingMode)));
  if (this->sample_rate_selector_ != nullptr)
    this->sample_rate_selector_->publish_state(
        ::hlk::ld2415h::enumToString(::hlk::ld2415h::sampleRateStrings(), cfg.sampleRate));
  #endif
}

// Configuration persistence

void LD2415HComponent::save_config_() {
  if (!this->config_pref_ready_)
    return;

  PersistentConfig stored;
  stored.version = 1;
  stored.valid = true;
  stored.config = this->radar_.getConfiguration();
  if (!this->config_pref_.save(&stored))
    ESP_LOGW(TAG, "Failed to save configuration");
}

void LD2415HComponent::reset_defaults() {
  ESP_LOGI(TAG, "Resetting to default configuration");
  this->radar_.resetConfiguration();
  this->publish_config_state_();

  // Mark the saved record as cleared. The record (rather than an
  // erase) keeps first boot distinguishable from an explicit reset.
  if (this->config_pref_ready_) {
    PersistentConfig cleared;
    cleared.version = 1;
    cleared.valid = false;
    if (!this->config_pref_.save(&cleared))
      ESP_LOGW(TAG, "Failed to clear saved configuration");
  }
}

}  // namespace ld2415h
}  // namespace esphome
