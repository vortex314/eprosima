## Target
Evaluate XRCE  DDS for my robotics project.
Port XRCE client to ESP32 , to use first across serial interface.


# Guide
## Install scripts
Install sripts can be found in the ./install directory to install the complete stack needed to run Eprosima Fast-DDS , XRCE-agent and clients.

# Miscellaneous brainstorming

My concepts start from micro-events that are emitted and received by micro-controller submodules. 

These micro-events are most of the time simple types, no complex structures. So IDL looks like an overhead.

The hierarchy of DDS : participant , subscriber/publisher and dataReader,dataWriter needs to be mapped.
The domain and topic approach also looks very coarse grained compared to my intentions.

Would the use of keys within topics help ? 

Mapping approach 1:

device => participant
object in device => topic
event => message Topic

Example an ESP32 that handles 2 objects : IMU and compass and GPS and device
and another ESP32 handling the steering and forward motion of the machine

esp32-1 => topic 'devices' in domain of the robot 'lawnmower' with key 'propertyName' and variant values 
esp32-2 => topic 'devices' in domain of the robot 'lawnmower' ...
imu => topic 'imu' with properties tilt,...
compass => topic 'compass' 
gps => topic 'gps' with properties latitude, longitude
steer => topic 'steer' with 'motor' properties + angleTarget and angleMeasured
drive => topic 'drive" with 'motor' properties + speedTarget and speedMeasured

the brain also generates on 'lawnmower' topic the overall system state 
The reflector can share the topic types and message types on a 'doc' topic

Preferrably the topic and message layout is defined by reflection ( an idea from the Mira project )
Example.
```
struct SystemMsg {
  uint32_t type=H("SystemMsg");
  string deviceName;
  uint64_t upTime;
  uint64_t utc;
  uint64_t heap;
  string cpu;
  
  Reflector &reflect(Reflector &r) { return r & type & deviceName & upTime & utc & heap & cpu ; };
};
```
Here the reflector can be used for serialization, deserialization and documentation.

