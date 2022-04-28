# pool-controller
Simple Arduino-based system to control solar heating for swimming pools. The purpose of this controller is to save energy, by not letting the circulation-pump run permanently. 

All you need is an Arduino Uno, a relay (that can handle a pump) and two temperature sensors.

The pump for the solar-panels is turned on every few minutes and runs for 10 seconds to move heated water from the panels to the "hotend" temperature sensor. If the temperature is 1°C warmer than the water on the cold side, it will continue to run for another 2 minutes and then check the temperature again. If the temperature isn't warmer, the pump will turn off.

All intervals and values can be changed in code.