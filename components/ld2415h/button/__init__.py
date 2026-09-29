import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import CONF_ID, ENTITY_CATEGORY_DIAGNOSTIC

from .. import CONF_LD2415H_ID, LD2415HComponent, ld2415h_ns

CONF_RESET_DEFAULTS = "reset_defaults"

ResetDefaultsButton = ld2415h_ns.class_("ResetDefaultsButton", button.Button)

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_ID): cv.declare_id(cg.EntityBase),
    cv.GenerateID(CONF_LD2415H_ID): cv.use_id(LD2415HComponent),
    cv.Optional(CONF_RESET_DEFAULTS): button.button_schema(
        ResetDefaultsButton,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon="mdi:restore",
    ),
}


async def to_code(config):
    ld2415h_component = await cg.get_variable(config[CONF_LD2415H_ID])
    if reset_defaults_config := config.get(CONF_RESET_DEFAULTS):
        b = await button.new_button(reset_defaults_config)
        await cg.register_parented(b, config[CONF_LD2415H_ID])
        cg.add(ld2415h_component.set_reset_defaults_button(b))
