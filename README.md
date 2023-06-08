<img src="https://github.com/irqmask/modbus2mqtt/actions/workflows/build.yml/badge.svg"/>

# modbus2mqtt

Query Modbus registers and publish values via MQTT

## Build and install

### Prerequisites

External libraries for Modbus, MQTT and config-files are needed.
On Debian based systems:

    sudo apt install cmake git libmodbus-dev libmosquitto-dev libconfig++-dev

### Build

    mkdir build && cd build
    cmake ..
    make

### Install

    sudo make install 
