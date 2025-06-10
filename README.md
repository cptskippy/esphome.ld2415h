# LD2415H Velocity Radar Sensor
## Overview
The ld2415h sensor platform allows you to use the Hi-Link HLK-LD2415H velocity radar sensor (datasheet and user manual) with ESPHome to track the velocity of objects.  The sensor utilizes millimeter wave radar to measure the velocity of objects within it's field of view.

## Specifications
 The HLK-LD2415H is a velocity radar sensor module has the following specifications:
 * K-band RF circuit operating in the 24.125GHz frequency range (customizable)
 * Accurately measures object velocities from 1-240KM/H
 * A precision of less than 1KM/H
 * Range up to 180 meters.
 * Antenna angle is 40° horizontal with a 16° pitch
 * Capable of 22 samples per second
 * RS-485 and TTL Serial interfaces
 * Site specific angle and sensitivity configuration


## Product Images
![Frontside of Sensor](ld2415h.front.png "Frontside of HLK-LD2415H Sensor")
![Backside of Sensor](ld2415h.back.png "Backside of HLK-LD2415H Sensor")

## Sensor Communication
The UART is required to be set up in your configuration for this sensor to work. Use of hardware UART pins is recommended, however the HLK-LD2415H sensor is limited to a 9600 baud rate.

### Serial Interface Specification
The sensor has both RS-485 and TTL serial interfaces with a pair of RX and TX pins for each interface.  Either interface can be used to read or configure the sensor.

### Serial Interface Command Syntax
Commands are comprised of 8, 11, or 13 bytes that are transmitted over either RX pin and responses can be received on either TX pin:

#### Command Format:
  - Length: 8, 11, or 13 bytes
  - 1st Byte: 0x43
  - 2nd Byte: 0x46
  - 3rd Byte: Command identifier
  - Remaining Bytes: Command specific parameters

Example:

    0x43 0x46 0x01 0x01 0x00 0x05 0x0d 0x0a

#### Supported Commands:
  - **0x01** : Set speed threshold, angular compensation, and sensitivity

    | Param | Default | Min  | Max  | Description |
    | ----- | --------| ---- | ---- | ----------- |
    | 1     | 0x01    | 0x01 | ???? | Represents the minimum speed reported by the sensor. |
    | 2     | 0x00    | 0x00 | ???? | Compensation angle between the center of the radar beam axis and the travel direction of objects. |
    | 3     | 0x05    | 0x01 | 0x0f | Radar sensitivity.  Smaller values have higher sensitivity and more interference.  Larger values are less sensitive and more resilient to interference. |

    Example:

        >>> 0x43 0x46 0x01 0x01 0x00 0x05 0x0d 0x0a

  - **0x02** : Set tracking mode, sample rate, and unit of measure

    | Param | Default | Min  | Max  | Description |
    | ----- | --------| ---- | ---- | ----------- |
    | 1     | 0x01    | 0x00 | 0x02 | Tracking Mode: <br>**0x00** : Approaching and retreating <br>**0x01** : Approaching <br>**0x02** : Retreating
    | 2     | 0x01    | 0x00 | ???? | The sample rate, higher values lower the rate. A value of 0 is ~22 samples per second, 1 is ~11. |
    | 3     | 0x00    | 0x00 | 0x02 | Unit of Measure: <br>**0x00** : kph <br>**0x01** : mph <br>**0x02** : mps |

    Example:

        >>> 0x43 0x46 0x02 0x01 0x01 0x00 0x0d 0x0a

  - **0x03** : Set anti-vibration compensation

    | Param | Default | Min  | Max  | Description |
    | ----- | --------| ---- | ---- | ----------- |
    | 1     | 0x05    | 0x00 | 0x70 | Vibration anti-interference coefficient used to reject false positives.  For example if the sensor is mounted to gently swaying pole. |
    | 2     | 0x00    | ???? | ???? | Undefined |
    | 3     | 0x00    | ???? | ???? | Undefined |

    Example:

        >>> 0x43 0x46 0x03 0x05 0x00 0x00 0x0d 0x0a

  - **0x04** : Set relay trigger duration and trigger speed threshold (only when using the photocoupler function)

    | Param | Default | Min  | Max  | Description |
    | ----- | --------| ---- | ---- | ----------- |
    | 1     | 0x03    | 0x00 | 0xff | Relay closure duration when speed exceeds threshold. |
    | 2     | 0x01    | 0x00 | 0xff | The lowest detection value for the photocoupler pickup in kph. |
    | 3     | 0x00    | ???? | ???? | Undefined |

    Example:

        >>> 0x43 0x46 0x04 0x03 0x01 0x00 0x0d 0x0a

  - **0x05** : Changes the Negotiation Mode (unknown purpose)

    | Param | Default | Min  | Max  | Description |
    | ----- | --------| ---- | ---- | ----------- |
    | 1     | 0x01    | 0x01 | 0x02 | Negotiation Mode: <br>**0x01** : Custom Agreement <br>**0x02** : Standard Protocol |
    | 2-8   |         |      |      | Mode 1:<br>    **0x43 0x46 0x05 0x01 0x00 0x00 0x00 0x00 0x00 0x00**<br> Mode 2 first call:<br> **0xfa 0x31 0x30 0x3d 0xfb**<br> Mode 2 second call:<br> **0xfa 0x55 0xaa 0xff 0xfb** |

    Example:

        >>> 0x43 0x46 0x05 0x01 0x00 0x00 0x00 0x00 0x00 0x00
        <<< Switch to CSR Mode... Done.


  - **0x07** : Read sensor configuration when in Standard Protocol mode.

    | Param | Default | Min  | Max  | Description |
    | ----- | --------| ---- | ---- | ----------- |
    | 1-10  |         |      |      | Pass 0x00 for all parameters, a response will be returned with the current configuration of the sensor. |

    Example:

        >>> 0x43 0x46 0x07 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
        <<< xFFxFF\r\n
        <<< No.:20230801E v5.0\r\n
        <<< X1:01 X2:00 X3:05 X4:01 X5:00 X6:00 X7:05 X8:03 X9:01 X0:01\r\n

    Output parameters:
  
    | Param | Description |
    | ----- | ----------- |
    | X1    | Represents the minimum speed reported by the sensor. |
    | X2    | Compensation angle between the center of the radar beam axis and the travel direction of objects. |
    | X3    | Radar sensitivity. Smaller values have higher sensitivity and more interference. Larger values are less sensitive and more resilient to interference. |
    | X4    | Tracking Mode: <br>**0x00** : Approaching and retreating <br>**0x01** : Approaching <br>**0x02** : Retreating
    | X5    | The sample rate, higher values lower the rate. A value of 0 is ~22 samples per second, 1 is ~11. |
    | X6    | Unit of Measure: <br>**0x00** : kph <br>**0x01** : mph <br>**0x02** : mps |
    | X7    | Vibration anti-interference coefficient used to reject false positives.  For example if the sensor is mounted to gently swaying pole. |
    | X8    | Relay closure time when speed exceeds Param 9. |
    | X9    | The lowest detection value for the photocoupler pickup in kph. |
    | X0    | Negotiation Mode: <br>**0x01** : Custom Agreement <br>**0x02** : Standard Protocol |


## ESPHome Example Configuration

Example yaml to use in esphome device config:

```yaml
external_components:
  - source:
      url: https://github.com/cptskippy/esphome.ld2415h
      type: git
      ref: main
    components: ld2415h
    refresh: 0s

uart:
  tx_pin: 36
  rx_pin: 34
  baud_rate: 9600

sensor:
  - platform: ld2415h
    speed:
      name: Speed
      filters:
        # Sensor reports on speed down to 1km/h
        # we must zero out the speed manually
        # when the sensor stops reporting.
        - timeout:
            timeout: 0.1s
            value: 0
        # Sensor will constantly report speed
        # at the confgiured sample rate
        # this ensures we only report changes
        - delta: 0.1
```
