## Ground station software
### Command line interface version

This branch contains the commmand line version of the groundstation software.

### Modification of the console output

To modify the way packets are written to the console output, modify the [PacketDispatcher.cpp](ground_station/PacketDispatcher.cpp) `processDataFlows()` function.

### Selection of the COM port

You need to modify the `config.json` file located in the `groudsdtation/` folder and put it next to the `ground_station` binary.

The modification that need to be done concern the choice of the serial from which to read data.
You can either specify the *port* (e.g. `COM14`, `/dev/ttyS14`), or the *hardwareID* (e.g. `SB\\VID_1A86&PID_7523&REV_02630`)
The port parameter is read first.

_Example:_

```json
{
    "receiverID": {
        "900MHz": {
            "port": "COM1",
            "hardwareID": ""
        },
        "433MHz": {
            "hardwareID_YAGI": "USB\\VID_1A86&PID_7523&REV_0263"
        }
    }
}
```
