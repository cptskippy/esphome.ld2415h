#pragma once

#include "esphome/components/button/button.h"
#include "../ld2415h.h"

namespace esphome {
namespace ld2415h {

class ResetDefaultsButton : public button::Button, public Parented<LD2415HComponent> {
 public:
  ResetDefaultsButton() = default;

 protected:
  void press_action() override;
};

}  // namespace ld2415h
}  // namespace esphome
