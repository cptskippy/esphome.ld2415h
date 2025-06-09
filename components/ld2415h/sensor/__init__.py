import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    CONF_SPEED,
    DEVICE_CLASS_SPEED,
    STATE_CLASS_MEASUREMENT,
    UNIT_KILOMETER_PER_HOUR,
)
from .. import ld2415h_ns, LD2415HComponent, CONF_LD2415H_ID

SpeedSensor = ld2415h_ns.class_("SpeedSensor", sensor.Sensor, cg.Component)
VelocitySensor = ld2415h_ns.class_("VelocitySensor", sensor.Sensor, cg.Component)

ICON_SPEEDOMETER = "mdi:speedometer"

CONF_VELOCITY = "velocity"

CONFIG_SCHEMA = cv.All(
    cv.COMPONENT_SCHEMA.extend(
        {

            #cv.GenerateID(): cv.declare_id(LD2415HSpeedSensor),
            #cv.GenerateID(): cv.declare_id(LD2415HVelocitySensor),
            cv.GenerateID(CONF_LD2415H_ID): cv.use_id(LD2415HComponent),
            cv.Optional(CONF_SPEED): sensor.sensor_schema(
                SpeedSensor,
                device_class=DEVICE_CLASS_SPEED,
                state_class=STATE_CLASS_MEASUREMENT,
                unit_of_measurement=UNIT_KILOMETER_PER_HOUR,
                icon=ICON_SPEEDOMETER,
                accuracy_decimals=1,
            ),
            cv.Optional(CONF_VELOCITY): sensor.sensor_schema(
                VelocitySensor,
                device_class=DEVICE_CLASS_SPEED,
                state_class=STATE_CLASS_MEASUREMENT,
                unit_of_measurement=UNIT_KILOMETER_PER_HOUR,
                icon=ICON_SPEEDOMETER,
                accuracy_decimals=1,
            ),
        }
    ),
)


async def to_code(config):
    ld2415h_component = await cg.get_variable(config[CONF_LD2415H_ID])
        
    # var = cg.new_Pvariable(config[CONF_ID])
    # await cg.register_component(var, config)

    if CONF_SPEED in config:
        sens = await sensor.new_sensor(config[CONF_SPEED])
        await cg.register_parented(sens, config[CONF_LD2415H_ID])
        cg.add(ld2415h_component.set_speed_sensor(sens))

    if CONF_VELOCITY in config:
        sens = await sensor.new_sensor(config[CONF_VELOCITY])
        await cg.register_parented(sens, config[CONF_LD2415H_ID])
        cg.add(ld2415h_component.set_velocity_sensor(sens))

    #ld2415h = await cg.get_variable(config[CONF_LD2415H_ID])
    #cg.add(ld2415h.register_listener(var))
