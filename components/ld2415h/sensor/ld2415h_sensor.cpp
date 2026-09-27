#include "ld2415h_sensor.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ld2415h {

static const char *const TAG = "LD2415H.sensor";

void LD2415HSensor::setup() {
  // Publish initial values so the entities are initialized immediately
  // rather than showing as uninitialized until the first speed line.
  if (this->speed_sensor_ != nullptr)
    this->speed_sensor_->publish_state(0.0f);
  if (this->velocity_sensor_ != nullptr)
    this->velocity_sensor_->publish_state(0.0f);
}

void LD2415HSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "LD2415H Sensor:");
  LOG_SENSOR("  ", "Speed", this->speed_sensor_);
  LOG_SENSOR("  ", "Velocity", this->velocity_sensor_);
}

void LD2415HSensor::onSpeed(float speed) {
  if (this->speed_sensor_ != nullptr && this->speed_sensor_->get_state() != speed) {
    this->speed_sensor_->publish_state(speed);
  }
}

void LD2415HSensor::onVelocity(float velocity) {
  if (this->velocity_sensor_ != nullptr && this->velocity_sensor_->get_state() != velocity) {
    this->velocity_sensor_->publish_state(velocity);
  }
}

}  // namespace ld2415h
}  // namespace esphome
