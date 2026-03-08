# PC UDP Controller

from xml.etree.ElementTree import tostring
import pygame
import socket
import time
import numpy as np

# Initialize controller
pygame.init()
pygame.joystick.init()
joystick = pygame.joystick.Joystick(0)
joystick.init()
print(pygame.joystick.get_count()) # Prints the number of Joystick Need to include a check for if the controller is not connected

# Set ESP32 IP and port
ESP32_IP = "192.168.50.200"  # change this to your ESP32's IP
ESP32_PORT = 1234

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def get_control():
    pygame.event.pump()
    axis_ly = joystick.get_axis(1)  # Left stick vertical
    axis_lx = joystick.get_axis(0)  # Left stick horizontal
    
    axis_ry = joystick.get_axis(2)  # Right stick horizontal
    axis_rx = joystick.get_axis(3)  # Right stick vertical

    trigger_l = joystick.get_axis(4)  # ZL on Switch Controller, left trigger (L2?) on PS4 controller
    trigger_r = joystick.get_axis(5)  # ZR on Switch Controller, right trigger (R2?) on PS4 controller
    # On Switch Pro Controller, these (above) output 0 before being pressed, then 0.9999 when pressed, then -1 when released and for all times button is not pressed after
    # Convert values above into 0 for not pressed, 1 for pressed
    #
    # if (trigger_l < 0.5): # Left trigger not pressed
    #    trigger_l = 0
    #else: # Left trigger pressed
    #    trigger_l = 1
    #if (trigger_r < 0.5): # Right trigger not pressed
    #    trigger_r = 0
    #else: # Right trigger pressed
    #    trigger_r = 1
   #
    # Both Switch and PS4 controllers have 16 buttons each
    #                                    Switch          PS4 (according to website)
    b0 = joystick.get_button(0)   # a                 |  cross
    b1 = joystick.get_button(1)   # b                 |  circle
    b2 = joystick.get_button(2)   # x                 |  square
    b3 = joystick.get_button(3)   # y                 |  triangle
    b4 = joystick.get_button(4)   # minus             |  share
    b5 = joystick.get_button(5)   # home              |  PS button
    b6 = joystick.get_button(6)   # plus              |  options
    b7 = joystick.get_button(7)   # left stick press  |  left stick press
    b8 = joystick.get_button(8)   # right stick press |  right stick press
    b9 = joystick.get_button(9)   # L                 |  left bumper (L1?)
    b10 = joystick.get_button(10) # R                 |  right bumper (R1?)
    b11 = joystick.get_button(11) # d-pad up          |  d-pad up
    b12 = joystick.get_button(12) # d-pad down        |  d-pad down
    b13 = joystick.get_button(13) # d-pad left        |  d-pad left
    b14 = joystick.get_button(14) # d-pad right       |  d-pad right
    b15 = joystick.get_button(15) # screenshot        |  touch pad click

    return axis_lx, axis_ly, axis_rx, axis_ry, trigger_l, trigger_r, b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13, b14, b15

try:
    while True:
        l_joy_x, l_joy_y, r_joy_x, r_joy_y, trig_l, trig_r, b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13, b14, b15 = get_control()
        # Convert to motor power scale, say -100 to 100
        motor_l = int((-l_joy_y + l_joy_x) * 100)
        motor_r = int((-r_joy_y - r_joy_x) * 100)
        motor_l = max(-100, min(100, motor_l))
        motor_r = max(-100, min(100, motor_r))

        # Right joystick - no need for conversion yet, since the stick may be used for something other than motors
        joy_z = int(r_joy_y * 100)
        joy_w = int(r_joy_x * 100)
        joy_z = max(-100, min(100, joy_z))
        joy_w = max(-100, min(100, joy_w))

        # Triggers
        trigModR = max(0, min(100, int(trig_r * 100)))
        trigModL = max(0, min(100, int(trig_l * 100)))

        message = f"{motor_l},{motor_r},{joy_z},{joy_w},{trigModL},{trigModR},{b0},{b1},{b2},{b3},{b4},{b5},{b6},{b7},{b8},{b9},{b10},{b11},{b12},{b13},{b14},{b15}"
        print(l_joy_x)
        print(message) # Shows the Joystick reading to send to the motor
        sock.sendto(message.encode(), (ESP32_IP, ESP32_PORT))
        time.sleep(0.05)

except KeyboardInterrupt:
    print("Exiting...")
finally:
    pygame.quit()
