#pragma once

#include "../ld2415h.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace ld2415h {

// Sensor entity that forwards parsed speed/velocity from the radar
// protocol engine to ESPHome sensor entities.
class LD2415HSensor : public Component, public ::hlk::ld2415h::Listener {
 public:
  void dump_config() override;
  void set_speed_sensor(sensor::Sensor *sensor) { this->speed_sensor_ = sensor; }
  void set_velocity_sensor(sensor::Sensor *velocity) { this->velocity_sensor_ = velocity; }

  // ::hlk::ld2415h::Listener
  void onSpeed(float speed) override;
  void onVelocity(float velocity) override;

 protected:
  sensor::Sensor *speed_sensor_{nullptr};
  sensor::Sensor *velocity_sensor_{nullptr};
};

}  // namespace ld2415h
}  // namespace esphome
