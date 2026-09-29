#include "reset_defaults_button.h"

namespace esphome {
namespace ld2415h {

void ResetDefaultsButton::press_action() { this->parent_->reset_defaults(); }

}  // namespace ld2415h
}  // namespace esphome
