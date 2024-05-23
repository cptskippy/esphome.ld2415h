import esphome.codegen as cg
from esphome.components import select
import esphome.config_validation as cv
from esphome.const import (
    ENTITY_CATEGORY_CONFIG,
    ICON_CLOCK_FAST,
    ICON_RADAR,
)
from .. import CONF_LD2415H_ID, LD2415HComponent, ld2415h_ns

CONF_SAMPLE_RATE = "sample_rate"
CONF_SAMPLE_RATE_SELECTS = [
    "~22 fps",
    "~11 fps",
    "~6 fps",
]
CONF_TRACKING_MODE = "tracking_mode"
CONF_TRACKING_MODE_SELECTS = [
    "Approaching and Restreating",
    "Approaching",
    "Restreating",
]

SampleRateSelect = ld2415h_ns.class_("SampleRateSelect", select.Select)
TrackingModeSelect = ld2415h_ns.class_("TrackingModeSelect", select.Select)

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_LD2415H_ID): cv.use_id(LD2415HComponent),
    cv.Optional(CONF_SAMPLE_RATE, default=1): select.select_schema(
        SampleRateSelect,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon=ICON_CLOCK_FAST,
    ),
    cv.Optional(CONF_TRACKING_MODE): select.select_schema(
        TrackingModeSelect,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon=ICON_RADAR,
    ),
}


async def to_code(config):
    ld2415h_component = await cg.get_variable(config[CONF_LD2415H_ID])
    if sample_rate_config := config.get(CONF_SAMPLE_RATE):
        s = await select.new_select(
            sample_rate_config,
            options=[CONF_SAMPLE_RATE_SELECTS],
        )
        await cg.register_parented(s, config[CONF_LD2415H_ID])
        cg.add(ld2415h_component.set_sample_rate_select(s))
    if tracking_mode_config := config.get(CONF_TRACKING_MODE):
        s = await select.new_select(
            tracking_mode_config,
            options=[CONF_TRACKING_MODE_SELECTS],
        )
        await cg.register_parented(s, config[CONF_LD2415H_ID])
        cg.add(ld2415h_component.set_tracking_mode_select(s))