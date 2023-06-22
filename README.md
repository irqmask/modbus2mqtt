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

### Install as daemon

Edit modbus2mqtt.service and set appropriate username under which the application will be started. This user must have the config file modbus2mqtt in its /home/<user>/.local/ directory.

    sudo cp modbus2mqtt.service /etc/systemd/system/
    sudo systemctl daemon-reload
    sudo systemctl enable modbus2mqtt