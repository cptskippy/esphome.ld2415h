#include "ld2415h.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ld2415h {

static const char *const TAG = "ld2415h";

LD2415HComponent::LD2415HComponent() {}

void LD2415HComponent::setup() {
  // This triggers current sensor configurations to be dumped
  this->update_config_ = true;
  // Setup the listener to publish so we can publish config state on connect.
  api::global_api_server->add_homeassistant_listener(this);
}

void LD2415HComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "LD2415H:");
  ESP_LOGCONFIG(TAG, "  Firmware: %s", this->firmware_);
  ESP_LOGCONFIG(TAG, "  Minimum Speed Threshold: %u KPH", this->min_speed_threshold_);
  ESP_LOGCONFIG(TAG, "  Compensation Angle: %u", this->compensation_angle_);
  ESP_LOGCONFIG(TAG, "  Sensitivity: %u", this->sensitivity_);
  ESP_LOGCONFIG(TAG, "  Tracking Mode: %s", this->i_to_s_(TRACKING_MODE_STR_TO_INT, this->tracking_mode_));
  ESP_LOGCONFIG(TAG, "  Sampling Rate: %s", this->i_to_s_(SAMPLE_RATE_STR_TO_INT, this->sample_rate_));
  ESP_LOGCONFIG(TAG, "  Unit of Measure: %s", this->i_to_s_(UNIT_OF_MEASURE_STR_TO_INT, this->unit_of_measure_));
  ESP_LOGCONFIG(TAG, "  Vibration Correction: %u", this->vibration_correction_);
  ESP_LOGCONFIG(TAG, "  Relay Trigger Duration: %u", this->relay_trigger_duration_);
  ESP_LOGCONFIG(TAG, "  Relay Trigger Speed: %u KPH", this->relay_trigger_speed_);
  ESP_LOGCONFIG(TAG, "  Negotiation Mode: %s", this->i_to_s_(NEGOTIATION_MODE_STR_TO_INT, this->negotiation_mode_));
}

void LD2415HComponent::loop() {
  // Process the stream from the sensor UART
  while (this->available()) {
    if (this->fill_buffer_(this->read())) {
      this->parse_buffer_();
    }
  }

  if (this->update_speed_angle_sense_) {
    ESP_LOGD(TAG, "LD2415H_CMD_SET_SPEED_ANGLE_SENSE: ");
    this->cmd_set_speed_angle_sense_[3] = this->min_speed_threshold_;
    this->cmd_set_speed_angle_sense_[4] = this->compensation_angle_;
    this->cmd_set_speed_angle_sense_[5] = this->sensitivity_;

    this->issue_command_(this->cmd_set_speed_angle_sense_, sizeof(this->cmd_set_speed_angle_sense_));
    this->update_speed_angle_sense_ = false;
    return;
  }

  if (this->update_mode_rate_uom_) {
    ESP_LOGD(TAG, "LD2415H_CMD_SET_MODE_RATE_UOM: ");
    this->cmd_set_mode_rate_uom_[3] = static_cast<uint8_t>(this->tracking_mode_);
    this->cmd_set_mode_rate_uom_[4] = this->sample_rate_;

    this->issue_command_(this->cmd_set_mode_rate_uom_, sizeof(this->cmd_set_mode_rate_uom_));
    this->update_mode_rate_uom_ = false;
    return;
  }

  if (this->update_anti_vib_comp_) {
    ESP_LOGD(TAG, "LD2415H_CMD_SET_ANTI_VIB_COMP: ");
    this->cmd_set_anti_vib_comp_[3] = this->vibration_correction_;

    this->issue_command_(this->cmd_set_anti_vib_comp_, sizeof(this->cmd_set_anti_vib_comp_));
    this->update_anti_vib_comp_ = false;
    return;
  }

  if (this->update_relay_duration_speed_) {
    ESP_LOGD(TAG, "LD2415H_CMD_SET_RELAY_DURATION_SPEED: ");
    this->cmd_set_relay_duration_speed_[3] = this->relay_trigger_duration_;
    this->cmd_set_relay_duration_speed_[4] = this->relay_trigger_speed_;

    this->issue_command_(this->cmd_set_relay_duration_speed_, sizeof(this->cmd_set_relay_duration_speed_));
    this->update_relay_duration_speed_ = false;
    return;
  }

  if (this->update_config_) {
    ESP_LOGD(TAG, "LD2415H_CMD_GET_CONFIG: ");

    this->issue_command_(this->cmd_get_config_, sizeof(this->cmd_get_config_));
    this->update_config_ = false;
    return;
  }
}

void LD2415HComponent::on_homeassistant_connected() {
  ESP_LOGI("ld2415h", "Connected to Home Assistant – pushing initial state");

  // Publish initial values or configuration state
  if (this->speed_sensor_ != nullptr)
    this->speed_sensor_->publish_state(this->speed_);

  if (this->velocity_sensor_ != nullptr)
    this->velocity_sensor_->publish_state(this->velocity_);

  #ifdef USE_NUMBER
  if (this->min_speed_threshold_number_ != nullptr)
      this->min_speed_threshold_number_->publish_state(this->min_speed_threshold_);
  if (this->compensation_angle_number_ != nullptr)
      this->compensation_angle_number_->publish_state(this->compensation_angle_);
  if (this->sensitivity_number_ != nullptr)
      this->sensitivity_number_->publish_state(this->sensitivity_);
  if (this->vibration_correction_number_ != nullptr)
      this->vibration_correction_number_->publish_state(this->vibration_correction_);
  if (this->relay_trigger_duration_number_ != nullptr)
      this->relay_trigger_duration_number_->publish_state(this->relay_trigger_duration_);
  if (this->relay_trigger_speed_number_ != nullptr)
      this->relay_trigger_speed_number_->publish_state(this->relay_trigger_speed_);
  #endif

  #ifdef USE_SELECT
  if (this->tracking_mode_selector_ != nullptr)
      this->tracking_mode_selector_->publish_state(this->i_to_s_(TRACKING_MODE_STR_TO_INT, this->tracking_mode_));
  if (this->sample_rate_selector_ != nullptr)
      this->sample_rate_selector_->publish_state(this->i_to_s_(SAMPLE_RATE_STR_TO_INT, this->sample_rate_));
  #endif

}


#ifdef USE_NUMBER
void LD2415HComponent::set_min_speed_threshold(uint8_t speed) {
  this->min_speed_threshold_ = speed;
  this->update_speed_angle_sense_ = true;
}

void LD2415HComponent::set_compensation_angle(uint8_t angle) {
  this->compensation_angle_ = angle;
  this->update_speed_angle_sense_ = true;
}

void LD2415HComponent::set_sensitivity(uint8_t sensitivity) {
  this->sensitivity_ = sensitivity;
  this->update_speed_angle_sense_ = true;
}

void LD2415HComponent::set_vibration_correction(uint8_t correction) {
  this->vibration_correction_ = correction;
  this->update_anti_vib_comp_ = true;
}

void LD2415HComponent::set_relay_trigger_duration(uint8_t duration) {
  this->relay_trigger_duration_ = duration;
  this->update_relay_duration_speed_ = true;
}

void LD2415HComponent::set_relay_trigger_speed(uint8_t speed) {
  this->relay_trigger_speed_ = speed;
  this->update_relay_duration_speed_ = true;
}
#endif

#ifdef USE_SELECT
void LD2415HComponent::set_tracking_mode(const std::string &state) {
  uint8_t mode = TRACKING_MODE_STR_TO_INT.at(state);
  this->set_tracking_mode(mode);
  this->tracking_mode_selector_->publish_state(state);
}

void LD2415HComponent::set_tracking_mode(TrackingMode mode) {
  this->tracking_mode_ = mode;
  this->update_mode_rate_uom_ = true;
}

void LD2415HComponent::set_tracking_mode(uint8_t mode) { this->set_tracking_mode(i_to_tracking_mode_(mode)); }

void LD2415HComponent::set_sample_rate(const std::string &state) {
  uint8_t rate = SAMPLE_RATE_STR_TO_INT.at(state);
  this->set_sample_rate(rate);
  this->sample_rate_selector_->publish_state(state);
}

void LD2415HComponent::set_sample_rate(uint8_t rate) {
  ESP_LOGD(TAG, "set_sample_rate: %i", rate);
  this->sample_rate_ = rate;
  this->update_mode_rate_uom_ = true;
}
#endif

void LD2415HComponent::issue_command_(const uint8_t cmd[], uint8_t size) {
  for (uint8_t i = 0; i < size; i++)
    ESP_LOGD(TAG, "  0x%02x", cmd[i]);

  // Don't assume the response buffer is empty, clear it before issuing a command.
  clear_remaining_buffer_(0);
  this->write_array(cmd, size);
}

bool LD2415HComponent::fill_buffer_(char c) {
  switch (c) {
    case 0x00:
    case 0xFF:
    case '\r':
      // Ignore these characters
      break;

    case '\n':
      // End of response
      if (this->response_buffer_index_ == 0)
        break;

      clear_remaining_buffer_(this->response_buffer_index_);
      ESP_LOGV(TAG, "Response Received:: %s", this->response_buffer_);
      return true;

    default:
      // Append to response
      this->response_buffer_[this->response_buffer_index_] = c;
      this->response_buffer_index_++;
      break;
  }

  return false;
}

void LD2415HComponent::clear_remaining_buffer_(uint8_t pos) {
  while (pos < sizeof(this->response_buffer_)) {
    this->response_buffer_[pos] = 0x00;
    pos++;
  }

  this->response_buffer_index_ = 0;
}

void LD2415HComponent::parse_buffer_() {
  char c = this->response_buffer_[0];

  switch (c) {
    case 'N':
      // Firmware Version
      this->parse_firmware_();
      break;
    case 'X':
      // Config Response
      this->parse_config_();
      break;
    case 'V':
      // Speed
      this->parse_speed_();
      break;

    default:
      ESP_LOGE(TAG, "Unknown Response: %s", this->response_buffer_);
      break;
  }
}

void LD2415HComponent::parse_config_() {
  // Example: "X1:01 X2:00 X3:05 X4:01 X5:00 X6:00 X7:05 X8:03 X9:01 X0:01"

  const char *delim = ": ";
  uint8_t token_len = 2;
  char *key;
  char *val;

  char *token = strtok(this->response_buffer_, delim);

  while (token != nullptr) {
    if (std::strlen(token) != token_len) {
      ESP_LOGE(TAG, "Configuration key length invalid.");
      break;
    }
    key = token;

    token = strtok(nullptr, delim);
    if (std::strlen(token) != token_len) {
      ESP_LOGE(TAG, "Configuration value length invalid.");
      break;
    }
    val = token;

    this->parse_config_param_(key, val);

    token = strtok(nullptr, delim);
  }

  ESP_LOGD(TAG, "Configuration received:");
  dump_config();
}

void LD2415HComponent::parse_firmware_() {
  // Example: "No.:20230801E v5.0"

  const char *fw = strchr(this->response_buffer_, ':');

  if (fw != nullptr) {
    // Move p to the character after ':'
    ++fw;

    // Copy string into firmware
    std::strncpy(this->firmware_, fw, sizeof(this->firmware_));
  } else {
    ESP_LOGE(TAG, "Firmware value invalid.");
  }
}

void LD2415HComponent::parse_speed_() {
  // Example: "V+001.9"

  const char *p = strchr(this->response_buffer_, 'V');

  if (p != nullptr) {
    ++p;
    this->velocity_ = strtod(p, nullptr);
    ++p;
    this->speed_ = strtod(p, nullptr);
    

    ESP_LOGV(TAG, "Speed updated: %f KPH", this->speed_);

    for (auto &listener : this->listeners_) {
      listener->on_speed(this->speed_);
      listener->on_velocity(this->velocity_);
    }

    if (this->speed_sensor_ != nullptr)
      this->speed_sensor_->publish_state(this->speed_);

    if (this->velocity_sensor_ != nullptr)
      this->velocity_sensor_->publish_state(this->velocity_);

  } else {
    ESP_LOGE(TAG, "Speed value invalid.");
  }
}

void LD2415HComponent::parse_config_param_(char *key, char *value) {
  if (std::strlen(key) != 2 || std::strlen(value) != 2 || key[0] != 'X') {
    ESP_LOGE(TAG, "Invalid Parameter %s:%s", key, value);
    return;
  }

  uint8_t v = std::stoi(value, nullptr, 16);

  switch (key[1]) {
    case '1':
      this->min_speed_threshold_ = v;
      #ifdef USE_NUMBER
      if (this->min_speed_threshold_number_ != nullptr)
        this->min_speed_threshold_number_->publish_state(this->min_speed_threshold_);
      #endif
      break;
    case '2':
      this->compensation_angle_ = std::stoi(value, nullptr, 16);
      #ifdef USE_NUMBER
      if (this->compensation_angle_number_ != nullptr)
        this->compensation_angle_number_->publish_state(this->compensation_angle_);
      #endif
      break;
    case '3':
      this->sensitivity_ = std::stoi(value, nullptr, 16);
      #ifdef USE_NUMBER
      if (this->sensitivity_number_ != nullptr)
        this->sensitivity_number_->publish_state(this->sensitivity_);
      #endif
      break;
    case '4':
      this->tracking_mode_ = this->i_to_tracking_mode_(v);
      #ifdef USE_SELECT
      if (this->tracking_mode_selector_ != nullptr)
        this->tracking_mode_selector_->publish_state(this->i_to_s_(TRACKING_MODE_STR_TO_INT, this->tracking_mode_));
      #endif
      break;
    case '5':
      this->sample_rate_ = v;
      #ifdef USE_SELECT
      if (this->sample_rate_selector_ != nullptr)
        this->sample_rate_selector_->publish_state(this->i_to_s_(SAMPLE_RATE_STR_TO_INT, this->sample_rate_));
      #endif
      break;
    case '6':
      this->unit_of_measure_ = this->i_to_unit_of_measure_(v);
      break;
    case '7':
      this->vibration_correction_ = v;
      #ifdef USE_NUMBER
      if (this->vibration_correction_number_ != nullptr)
        this->vibration_correction_number_->publish_state(this->vibration_correction_);
      #endif
      break;
    case '8':
      this->relay_trigger_duration_ = v;
      #ifdef USE_NUMBER
      if (this->relay_trigger_duration_number_ != nullptr)
        this->relay_trigger_duration_number_->publish_state(this->relay_trigger_duration_);
      #endif
      break;
    case '9':
      this->relay_trigger_speed_ = v;
      #ifdef USE_NUMBER
      if (this->relay_trigger_speed_number_ != nullptr)
        this->relay_trigger_speed_number_->publish_state(this->relay_trigger_speed_);
      #endif
      break;
    case '0':
      this->negotiation_mode_ = this->i_to_negotiation_mode_(v);
      break;
    default:
      ESP_LOGD(TAG, "Unknown Parameter %s:%s", key, value);
      break;
  }
}

TrackingMode LD2415HComponent::i_to_tracking_mode_(uint8_t value) {
  TrackingMode u = TrackingMode(value);
  switch (u) {
    case TrackingMode::APPROACHING_AND_RETREATING:
      return TrackingMode::APPROACHING_AND_RETREATING;
    case TrackingMode::APPROACHING:
      return TrackingMode::APPROACHING;
    case TrackingMode::RETREATING:
      return TrackingMode::RETREATING;
    default:
      ESP_LOGE(TAG, "Invalid TrackingMode:%u", value);
      return TrackingMode::APPROACHING_AND_RETREATING;
  }
}

UnitOfMeasure LD2415HComponent::i_to_unit_of_measure_(uint8_t value) {
  UnitOfMeasure u = UnitOfMeasure(value);
  switch (u) {
    case UnitOfMeasure::MPS:
      return UnitOfMeasure::MPS;
    case UnitOfMeasure::MPH:
      return UnitOfMeasure::MPH;
    case UnitOfMeasure::KPH:
      return UnitOfMeasure::KPH;
    default:
      ESP_LOGE(TAG, "Invalid UnitOfMeasure:%u", value);
      return UnitOfMeasure::KPH;
  }
}

NegotiationMode LD2415HComponent::i_to_negotiation_mode_(uint8_t value) {
  NegotiationMode u = NegotiationMode(value);

  switch (u) {
    case NegotiationMode::CUSTOM_AGREEMENT:
      return NegotiationMode::CUSTOM_AGREEMENT;
    case NegotiationMode::STANDARD_PROTOCOL:
      return NegotiationMode::STANDARD_PROTOCOL;
    default:
      ESP_LOGE(TAG, "Invalid NegotiationMode:%u", value);
      return NegotiationMode::CUSTOM_AGREEMENT;
  }
}

const char *LD2415HComponent::i_to_s_(const std::map<std::string, uint8_t> &map, uint8_t i) {
  for (const auto &pair : map) {
    if (pair.second == i) {
      return pair.first.c_str();
    }
  }
  return "Unknown";
}

}  // namespace ld2415h
}  // namespace esphome
