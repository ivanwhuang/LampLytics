# LampLytics

The goal of LampLytics is to automate the process of creating and maintaining a comfortable desk environment.  

LampLytics is a prototype of a personal desk assistant system that utilizes data from multiple sensors in order to automatically turn on a lamp and fan with just the right amount of power in order to create a comfortable desk environment. Usage rates and power consumption for both the light and fan will be recorded and sent up to be stored on the cloud via a WiFi connection. These analytics are able to be viewed on an end user interface for the user to monitor. 

[Live Demo](https://youtu.be/SsZZQ1udUtw)

### Motivation

Everytime we’re right by our desk with the hopes of preparing to get some work done, sometimes it takes a bit of time for us to focus and adjust to our work environment. Maybe you turn on your lamp and you realize it’s too bright and you spend a few seconds adjusting its position. Or maybe it’s been annoyingly hot recently, and so you spend time to get a fan and power it on. Or whatever the case, every tiny issue requires time to solve and it prevents us from being able to sit down and start working immediately. Even if it’s just a matter of seconds, time is our most precious resource. With LampLytics, the faster we can get comfortable, the less time it will take for us to be productive.

### System Architecture

**Sensors:** Temperature sensor, Light sensor, Ultrasonic Distance Sensor, Touch Button

Measured data from the Light and Temperature sensor is used to determine how much power should be supplied to the fan and lamp

**Actuators:** LED (Lamp), Fan 

This system is a **state machine** that can be in the following **three states**:

`OFF`
If you’re not within a certain distance from this ultrasonic distance sensor, the system recognizes that you are not at your desk and will not power on anything. 

`Active`
If you are within a certain distance from the ultrasonic distance sensor, the system recognizes that you are at your desk and will power on the lamp, fan, and RGB display. 

`Standby` 
If you are at your desk, but you are actually pretty comfortable already and you decide it’s probably better to just save power, you can press the button to have the system go on standby. When the system is on standby, all the components will be powered off but it will be ready to turn everything back on and go back to being active if you decide to push the button again. 

**Communication Protocol and Cloud**

The system uses WiFi in order to communicate and send collected data to an Amazon EC2 Cloud Server. In the cloud, we use Flask to create a web server to host an end user interface to display recent sensor and usage data and analytics. 
