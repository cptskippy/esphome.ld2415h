#include "speed_sensor.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ld2415h {

static const char *const TAG = "LD2415H.speed";

void SpeedSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "LD2415H Speed:");
  LOG_SENSOR("  ", "Speed", this->speed_sensor_);
}

}  // namespace ld2415h
}  // namespace esphome
