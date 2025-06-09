#pragma once

#include "../ld2415h.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace ld2415h {

class VelocitySensor : public Component, sensor::Sensor {
 public:
  void dump_config() override;
  void set_velocity_sensor(sensor::Sensor *sensor) { this->velocity_sensor_ = sensor; }
  /*
  void on_velocity(double velocity) override {
    if (this->velocity_sensor_ != nullptr) {
      if (this->velocity_sensor_->get_state() != velocity) {
        this->velocity_sensor_->publish_state(velocity);
      }
    }
  }
*/

 protected:
  sensor::Sensor *velocity_sensor_{nullptr};
};

}  // namespace ld2415h
}  // namespace esphome
