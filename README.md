Code for Arthur for the Ohio State University Production of After the Blast

# Setup Process
1. To start install Python 3.13 the most recent version of python should work as well but has not been tested.

2. For windows machines install the Playstation controller driver `PlayStationAccessoriesInstaller.exe`

3. Install pygame and other necessary requirements

    Open a terminal and navigate to the folder the code is located in then run
    ``` bash
    pip install -r requirements.txt
    ```
    This will install pygame, numpy and FreeSimpleGui

4. Files

The file `PC_TCP.py` runs python on your computer and contains the control code for the robot.

The file `ESP32_TCP.ino` runs on the ESP32 on the robot and will need to be reflashed if any changes are made.

The Parallelized version contains the GUI for running the robot and knowing the motor states. This only currently works on Windows.

5. If you need to change anything on the ESP32 you will need to install the ArduinoIDE to make modifications to that code and reflash it to the ESP32.

The Parallax servo library is not working in this code.

# Networking
The ASUS router is configured already 


# How to bring up the robot
This section shows you how to get the robot running
## Intial startup
1. Plug in and turn on the router
2. Connect computer to `ArthurNet`
3. Insert the 2 batteries into Arthur and ensure the power switches on the mounts are set to on.
4. The robot should automatically connect to the router at the IP Address `192.168.50.100` this can be changed in the `ESP32_TCP.ino` firmware. It should be noted that spare ESP32's have different IP Addresses so if a board swap must be made you need to update the python code.
5. Ensure that the IP address in the Python code is set to the same IP and start the program
6. We would run this in VS Code for simplicity but it can be run separately.

# Production Info
Created by

Matthew Fisher | fisher.matt10@gmail.com 

Gabe Willenberg | willenbg@gmail.com

For The Ohio State University Department of Theatre Film and Media Arts

After the Blast November 2025

Columbus, Ohio USA

See the Most Updated version on [Github](https://github.com/ImaginaryMatt/Arthur)