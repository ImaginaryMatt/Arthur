# This is a python script that sends strings to a tcp server
# It sends values from a game controller
 
from ast import Or
import socket
import sys
import time
from tracemalloc import start
from turtle import left, right, st
# from xml.etree.ElementTree import tostring
import pygame
 
 # Defining kill state boolean
killed = False

# Sends a command to the TCP server
def send_command(sock, motor_id, value):
    message = "{}{}\n".format(motor_id, value)
    sock.sendall(message.encode('utf-8'))
    print("Sent: {}".format(message))
 
  # Moves the robotic arm to a specified position based on Trigger input
def move_arm(sock, arm, position):
    position = max(0, min(255, position))  # Ensure position is within valid range
    send_command(sock, arm, position)

# Emergency stop function to halt all motors immediately
# 
def emergency_stop(sock):
    send_command(sock, 8, 1) #Triggering halt command
    print("Emergency stop activated: All motors halted.")




 
if __name__ == "__main__":
    # Initialize pygame and the joystick
    pygame.init()
    pygame.joystick.init()
    # Prints the number of Joystick Need to include a check for if the controller is not connected
    num = pygame.joystick.get_count()
    print("There are ",num, " Joysticks connected!")
 
    if pygame.joystick.get_count() == 0:
        print("No joystick connected.")
        sys.exit(1)
 
    joystick = pygame.joystick.Joystick(0)
    joystick.init()
 
    # Set up TCP connection
    TCP_IP = '192.168.50.200'  # Replace with your server's IP address
    TCP_PORT = 80              # Replace with your server's port

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((TCP_IP, TCP_PORT))
    print("Connected to server at {}:{}".format(TCP_IP, TCP_PORT))
 


    try:
        while True:
            # Get the joystick input
            pygame.event.pump()
 
            # Send motor commands based on joystick position
            left_x_axis = joystick.get_axis(0) * 126 + 128 # Normalize to 0-255
            left_y_axis = joystick.get_axis(1) * 255 + 255 # Normalize to 0-255
            right_x_axis = joystick.get_axis(2) * 126 + 128 # Normalize to 0-255
            right_y_axis = joystick.get_axis(3) * 255 + 255 # Normalize to 0-255
            trigger_l = joystick.get_axis(4) * 126 + 128 # Normalize to 0-255
            trigger_r = joystick.get_axis(5) * 126 + 128 # Normalize to 0-255
            
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

            send_command(sock, 1, left_y_axis)
            send_command(sock, 2, right_y_axis)
            # send_command(sock, 3, right_x_axis)
            # send_command(sock, 4, right_y_axis)
            # send_command(sock, 5, trigger_l)
            # send_command(sock, 6, trigger_r)
            # send_command(sock, 7, b0)
            # send_command(sock, 8, b1)
            # send_command(sock, 9, b2)
            # send_command(sock, 10, b3)
            # send_command(sock, 11, b4)
            # send_command(sock, 12, b5)
            # send_command(sock, 13, b6)
            # send_command(sock, 14, b7)
            # send_command(sock, 15, b8)  
            
            
            # Head control Commands
            if joystick.get_button(2):  # If button 2 is pressed
                send_command(sock, 3, 1)  # Run motor 3 CCW (head left)
            elif joystick.get_button(1):  # If button 1 is pressed
                send_command(sock, 3, 2)  # Run motor 3 
            elif joystick.get_button(3):  # If button 3 is pressed
                send_command(sock, 3, 3)  # Stop motor 3 head
            
            # Arm Control Commands
            # move_arm(sock, 4, int(trigger_l))  # Move arm based on left trigger
            # move_arm(sock, 5, int(trigger_r))  # Move arm based on right trigger

            #Left arm plan B
            if joystick.get_button(9):          # If button 9 is pressed
                send_command(sock, 4, 1)        # Run left arm motor CW 
            elif joystick.get_axis(4) >= 0.5:   # If left trigger is pressed
                send_command(sock, 4, 2)        # Run left arm motor CCW
            else:
                send_command(sock, 4, 0)        # Stop left arm movement

            #Right arm plan B
            if joystick.get_button(10):         # If button 10 is pressed
                send_command(sock, 5, 1)        # Run right arm motor CCW
            elif joystick.get_axis(5) >= 0.5:   # If right trigger is pressed
                send_command(sock, 5, 2)        # Run left arm motor CCW
            else:
                send_command(sock, 5, 0)        # Stop right arm movement

            #Extending and Retracting Arm
            if joystick.get_button(11):  # If button 11 is pressed
                send_command(sock, 6, 1)  # Extend arm
            elif joystick.get_button(12):  # If button 12 is pressed
                send_command(sock, 6, 2)  # Retract arm
            else:
                send_command(sock, 6, 0)  # Stop arm movement

            # Opening Claw
            if joystick.get_button(14):  # If button 0 is pressed
                send_command(sock, 7, 1)  # Close claw
            elif joystick.get_button(13):  # If button 1 is pressed
                send_command(sock, 7, 2)  # Open claw
            else:
                send_command(sock, 7, 0)  # Stop claw movement
            
            # Mode Change to press button 1 the drive motors are disabled and the head motor is moved with the x axis of the left joystick
            # while joystick.get_button(1):  # If button 1 is pressed
            #     send_command(sock, 1, 0)  # Disable left drive motor
            #     send_command(sock, 2, 0)  # Disable right drive motor
            #     # Interpreted left joystick x-axis to move head stepper motor
            #     if int(left_x_axis) > 128:
            #         send_command(sock, 3, 2)  # Move head in one direction
            #     elif int(left_x_axis) < 128:
            #         send_command(sock, 3, 1)  # Move head in the opposite direction
            #     else:

            #         send_command(sock, 3, int(left_x_axis))  # Move head with left joystick x-axis


            # Emergency Stop Logic
    
            # RESET_HOLD_TIME = 3  # seconds
            # # Emergency stop if both either button 4 and 6 are pressed
            # if joystick.get_button(4): # If both start and option have been pressed
            #     emergency_stop(sock) # Send the emergency stop command to the robot
            #     killed = True # Mark the robot as killed on the driver's station
            #     print("Emergency stop activated. Press both buttons 4 and 6 for 3 seconds to resume.")

            # # Wait until both buttons are pressed for 3 seconds to resume operations then unkill the robot
            # if joystick.get_button(6): # Read whether both buttons have been pressed
            #     start_time = time.time(); # Set a reference time
            #     while joystick.get_button(6) and killed == True: # While both buttons are being pressed and driver's station marks the robot has been killed
            #         timer = time.time() - start_time # Set an internal timer that counts up
            #         print(timer)
            #         if timer >= RESET_HOLD_TIME: # When this internal timer reaches 3 seconds
            #             send_command(sock,8,2) #Re-enable command for the robot
            #             killed = False # Mark the robot as un-killed to the driver's station
            #             print("Robot Alive")

            
            # while robot_killed:
            #         pygame.event.pump()
            #         # Wait until both buttons are pressed for 3 seconds to resume operations
            #         share_Button = joystick.get_button(4)
            #         option_Button = joystick.get_button(6)

            #         if (joystick.get_button(4) and joystick.get_button(6)):
            #             startTime = time.time()
            #             killed = False
            #             print("Resuming operations...")
            #         currentTime = time.time()
            #         timeElapsed = currentTime - startTime

            #         #Reset logic
            #         if robot_killed:
            #             if share_Button and option_Button:
            #                 if reset_start_time is None:
            #                     reset_start_time = time.time()
            #                 elif time.time() - reset_start_time >= RESET_HOLD_TIME:
            #                     reset_robot()
            #                     reset_start_time = None
            #             else:
            #                 reset_start_time = None  # buttons not both held → reset timer
            


            # Wait for a short period
            time.sleep(0.1)
 
    except KeyboardInterrupt:
        print("Exiting...")
    finally:
        sock.close()
        exit(0)