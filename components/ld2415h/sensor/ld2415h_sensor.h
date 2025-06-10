#pragma once

#include "../ld2415h.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace ld2415h {

class LD2415HSensor : public LD2415HListener, public Component, sensor::Sensor {
 public:
  void dump_config() override;
  void set_speed_sensor(sensor::Sensor *sensor) { this->speed_sensor_ = sensor; }
  void set_velocity_sensor(sensor::Sensor *velocity) { this->velocity_sensor_ = velocity; }
  
  void on_speed(double  speed) override {
    if (this->speed_sensor_ != nullptr) {
      if (this->speed_sensor_->get_state() != speed) {
        this->speed_sensor_->publish_state(speed);
      }
    }
  }
  void on_velocity(double  velocity) override {
    if (this->velocity_sensor_ != nullptr) {
      if (this->velocity_sensor_->get_state() != velocity) {
        this->velocity_sensor_->publish_state(velocity);
      }
    }
  }

 protected:
  sensor::Sensor *speed_sensor_{nullptr};
  sensor::Sensor *velocity_sensor_{nullptr};
};

}  // namespace ld2415h
}  // namespace esphome
