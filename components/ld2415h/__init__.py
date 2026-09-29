import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID
from esphome.helpers import fnv1_hash_object_id

CODEOWNERS = ["@cptskippy"]

DEPENDENCIES = ["uart"]

MULTI_CONF = True

# The protocol engine lives in the standalone ld2415h library.
# Pulled from git for now; once the library is published to the
# PlatformIO registry this becomes:
#   cg.add_library("cptskippy/LD2415H", "<version>")
cg.add_library("LD2415H", None, "https://github.com/cptskippy/ld2415h.git#9a245e10f47356ca90f449cee46af32f8af3dad5")

ld2415h_ns = cg.esphome_ns.namespace("ld2415h")
LD2415HComponent = ld2415h_ns.class_("LD2415HComponent", cg.Component, uart.UARTDevice)

CONF_LD2415H_ID = "ld2415h_id"

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(LD2415HComponent),
        }
    )
    .extend(uart.UART_DEVICE_SCHEMA)
    .extend(cv.COMPONENT_SCHEMA)
)

FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    "ld2415h_uart",
    require_tx=True,
    require_rx=True,
    parity="NONE",
    stop_bits=1,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    # NVS key derived from the component id so multiple instances do
    # not collide on the same flash entry.
    cg.add(var.set_config_pref_key(fnv1_hash_object_id(config[CONF_ID].id)))
