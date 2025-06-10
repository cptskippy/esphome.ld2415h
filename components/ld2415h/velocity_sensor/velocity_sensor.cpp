#include "velocity_sensor.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ld2415h {

static const char *const TAG = "LD2415H.velocity";

void VelocitySensor::dump_config() {
  ESP_LOGCONFIG(TAG, "LD2415H Velocity:");
  LOG_SENSOR("  ", "Velocity", this->velocity_sensor_);
}

}  // namespace ld2415h
}  // namespace esphome
