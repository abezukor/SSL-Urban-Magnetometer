# Urban Magnetometer Station Code
This is the code to run an Urban Magnetometer station. It is written for an Arduino with a USB Host shield and an SD card module. The Magnetometer is a Twinleaf VMR with a Twinleaf Sync2. These are connected via the USB host shield to the Arduino.  The Arduino code saves the data from the Twinleaf sensors to files on the SD Card. There is also a python program to turn a folder of the raw data logs into a CSV file and an R script to plot the data.

By Default the VMR's data format is as follows: 

```Vector X, Vector Y, Vector Z, Accl.X, Accl.Y, Accl.Z, Gyro X, Gyro Y, Gyro Z, Bar, Therm```

The code as tested works with the Accelerometer, Gyroscope, Barometer, and Thermometer disabled and the magnetometer set to a decimation rate of 100. For instructions on configuring the VMR see [the Twinleaf VMR reference](https://twinleaf.com/vector/VMR/reference/R8/).

This project was done for the [UC Berkeley Space Sciences Lab](https://www.ssl.berkeley.edu/). 