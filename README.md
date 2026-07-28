# Narwal System Metrics
Narwal System Metrics is a library designed to provide a simple interface for retrieving system metrics exposed
by a Linux-based operating system.

The library could be used in small satellite missions. More specifically, it is intended to be used 
alongside NASA's Core Flight System.
## Currently supported metrics.
### Memory Data.
* Used Memory.
* Total amount of memory available for the OS.
### Thermal Zones Data.
Temperature sensors are available through the [Linux Thermal Subsystem](https://docs.kernel.org/admin-guide/thermal/index.html).
* Get available thermal zones.
* Get temperature of a given thermal zone.
### CPU Data.
