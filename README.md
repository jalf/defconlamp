# DefconLamp
DefconLamp is an over-engineered 3D-printed RGB lamp controlled by a REST API that can be used to illuminate a room but also as a warning device for alarms and events.

## [The API]
* GET /status - Get the current status
* POST /stop - Stop the patterns cycle
* POST /cycle - Start the patterns cycle
* POST /random - Set an random order to patterns cycle
* GET /patterns - Get the available patterns list
* POST /patter/{pattern_name} - Set the active pattern
* POST /delay/{delay} - Delay (in seconds) to change a pattern in cycle mode
* POST /on - Turn on the lights
* POST /off - Turn off the lights
* POST /reset - Reset wifi settings
* POST /color/{r}/{g}/{b} - Set a solid RGB color

## [Sensors]
For no specific reason, the lamp also has **temperature**, **humidity** and **light** sensors. Use them wisely.

