#!/usr/local/bin/python3.13
# This is a python script that sends strings to a tcp server
# It sends values from a game controller

from ast import Or
import socket
import sys
import time
import threading
import FreeSimpleGUI as sg
import queue
from tracemalloc import start
from turtle import left, right, st
# from xml.etree.ElementTree import tostring
import pygame

# Defining necessary state booleans
killed = True
extend_timer_running = False
grasp_timer_running = False
estop_timer_running = False
button_press_time = None

# Defining output array for GUI
output_values = [0,0,0,0,0,0,0,0,None, None] 


# Sends a command to the TCP server
def send_command(sock, motor_id, value):
    global output_values
    if motor_id == 1 or motor_id == 2:
        output_values[motor_id-1] = int(value * (10 ** 4)) / (10 ** 4)
    else:
        output_values[motor_id-1] = value
    message = "{}{}\n".format(motor_id, value)
    sock.sendall(message.encode('utf-8'))
    #print("Sent: {}".format(message))

def smooth_joystick(raw_input, deadzone=0.1, expo=2.0, max_output=0.8):
    # Step 1: apply deadzone
    if abs(raw_input) < deadzone:
        return 0.0

    # Step 2: rescale after deadzone
    scaled = (raw_input - deadzone * (1 if raw_input > 0 else -1)) / (1 - deadzone)

    # Step 3: apply throttle curve
    curved = (abs(scaled) ** expo) * (1 if scaled >= 0 else -1)

    # Step 4: limit maximum speed
    return curved * max_output

# Moves the robotic arm to a specified position based on Trigger input
def move_arm(sock, arm, position):
    position = max(0, min(255, position))  # Ensure position is within valid range
    send_command(sock, arm, position)

# Emergency stop function to halt all motors immediately
# 
def emergency_stop(sock):
    send_command(sock, 8, 1) #Triggering halt command
    print("Emergency stop activated: All motors halted.")

def extend_halt():
    send_command(sock,6,0)
    print("Extend timer has finished")
    global extend_timer_running 
    extend_timer_running = False

def grasp_halt():
    send_command(sock,7,0)
    print("Grasp timer has finished")
    global grasp_timer_running
    grasp_timer_running = False

def estop_reset():
    send_command(sock,8,2)
    print("Estop Reset has finished")
    global killed
    killed = False

def enable_on_press():
    global button_press_time
    if button_press_time is None:
        button_press_time = time.time()
        print ("Beginning Re-Enable Attempt")

def enable_on_release():
    global button_press_time
    if button_press_time is not None:
        hold_duration = time.time() - button_press_time
        print(hold_duration)
        if hold_duration >= 3:
            estop_reset()
            button_press_time = None
        else:
            button_press_time = None
            print("Re-Enable Aborted")

#Defining timers
class Timer:
    def __init__(self, duration):
        self._timer = None
        self.timer_duration = duration

    def start_timer(self, func):
        self._timer = threading.Timer(self.timer_duration, func)
        self._timer.start()
        print ("timer started")

    def stop_timer(self):
        self._timer.cancel()
        print ("timer cancelled")

extend_timer = Timer(3)
grasp_timer = Timer(1)
estop_timer = Timer(3)

#Starting communication queue
data_queue = queue.Queue()

def run_pygame():

    # Defining necessary state booleans
    global killed
    global extend_timer_running
    global grasp_timer_running
    global estop_timer_running

    # Defining output array for GUI
    global output_values

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
    TCP_IP = '192.168.50.100'  # IP Adress for Arthur main ESP32
    #TCP_IP = '192.168.50.62'   # IP Adress for Arthur spare ESP32
    TCP_PORT = 80              # Replace with your server's port

    global sock     
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((TCP_IP, TCP_PORT))
    print("Connected to server at {}:{}".format(TCP_IP, TCP_PORT))

    # print("No. of  active threads: ", threading.active_count())
    # print("Timers for grabber use initialized")

    try:
        while True:

            # Get the joystick input
            pygame.event.pump()
            
            # Drive Motor Commands
            left_y_axis = smooth_joystick(joystick.get_axis(1), deadzone=0.0, expo=3.0, max_output=1) * 255 + 255
            right_y_axis = smooth_joystick(joystick.get_axis(3), deadzone=0.0, expo=3.0, max_output=0.8) * 255 + 255
            send_command(sock, 1, left_y_axis)
            send_command(sock, 2, right_y_axis)

            
            # Head control Commands
            if joystick.get_button(2):  # If button 2 is pressed
                send_command(sock, 3, 1)  # Run motor 3 CCW (head left)
            elif joystick.get_button(1):  # If button 1 is pressed
                send_command(sock, 3, 2)  # Run motor 3 
            elif joystick.get_button(3):  # If button 3 is pressed
                send_command(sock, 3, 3)  # Stop motor 3 head
            else:
                send_command(sock,3,0)
            
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
            if joystick.get_button(11) and extend_timer_running == False:  # If button 11 is pressed
                send_command(sock, 6, 1)  # Extend arm
                extend_timer_running = True
                extend_timer.start_timer(extend_halt)
            elif joystick.get_button(12) and extend_timer_running == False:  # If button 12 is pressed
                send_command(sock, 6, 2)  # Retract arm
                extend_timer_running = True
                extend_timer.start_timer(extend_halt)
            elif extend_timer_running == False:
            #else:
                send_command(sock, 6, 0)  # Stop arm movement
                #extend_timer.stop_timer()

            # Opening Claw
            if joystick.get_button(14) and grasp_timer_running == False:  # If button 0 is pressed
                send_command(sock, 7, 1)  # Close claw
                grasp_timer_running = True
                grasp_timer.start_timer(grasp_halt)
            elif joystick.get_button(13) and grasp_timer_running == False:  # If button 1 is pressed
                send_command(sock, 7, 2)  # Open claw
                grasp_timer_running = True
                grasp_timer.start_timer(grasp_halt)
            elif grasp_timer_running == False:
                send_command(sock, 7, 0)  # Stop arm movement

            # Emergency Stop Logic
    
            # Emergency stop if button 4 is pressed
            if joystick.get_button(4) and killed == False: # If both start and option have been pressed
                emergency_stop(sock) # Send the emergency stop command to the robot
                killed = True # Mark the robot as killed on the driver's station
                print("Emergency stop activated. Press button 6 for 3 seconds to resume.")
            #Begin Re-Enable attempt when button 6 is pressed
            elif joystick.get_button(6) and estop_timer_running == False:
                estop_timer_running = True
                enable_on_press()
            #Evaluate Re-Enable attempt when button 6 is released
            elif joystick.get_button(6) == False and estop_timer_running == True:
                estop_timer_running = False
                enable_on_release()
            else:
                send_command(sock,8,0) #This does nothing but provides an output for our GUI

            # Conditional statements for window output
            if extend_timer_running == True:
                output_values[9] = 'Extend Timer is Running'
            elif grasp_timer_running == True:
                output_values[9] = 'Grasp Timer Running'
            elif estop_timer_running == True:
                output_values[9] = 'Re-Enable Timer Running'
            else:
                output_values[9] = 'No Timers Running'

            if killed == True:
                output_values[8] = 'Robot Killed'
            elif killed == False:
                output_values[8] = 'Robot Active'

            gui_data = output_values[:]

            if not data_queue.empty():
                try:
                    data_queue.get_nowait() #Clear old data if not read in time
                except queue.Empty:
                    pass

            data_queue.put(gui_data)
            # Wait for a short period
            time.sleep(0.05)

    except KeyboardInterrupt:
        print("Exiting...")
    finally:
        sock.close()
        pygame.quit()
        data_queue.put("QUIT")


def run_gui():

    #Building GUI Window
    column_1 = [
                [sg.Text('Is the Robot Active?',expand_x=True, expand_y=True,font=('Calibri',24))],
                [sg.Text(key='-OUTPUT1-',expand_x=True, expand_y=True,font=('Calibri',24))],
                [sg.Text('Any Timers Running?',expand_x=True, expand_y=True,font=('Calibri',24))],
                [sg.Text(key='-OUTPUT6-',expand_x=True, expand_y=True,font=('Calibri',24))]]

    column_2 = [
                [sg.Text('Socket 1',expand_x=True, expand_y=True,font=('Calibri',24))],
                [sg.Text(key='-OUTPUT2-',expand_x=True, expand_y=True,font=('Calibri',24))],
                [sg.Text('Socket 5',expand_x=True, expand_y=True,font=('Calibri',24))],
                [sg.Text(key='-OUTPUT7-',expand_x=True, expand_y=True,font=('Calibri',24))]]

    column_3 = [
                [sg.Text('Socket 2',expand_x=True, expand_y=True,font=('Calibri',24))],
                [sg.Text(key='-OUTPUT3-',expand_x=True, expand_y=True,font=('Calibri',24))],
                [sg.Text('Socket 6',expand_x=True, expand_y=True,font=('Calibri',24))],
                [sg.Text(key='-OUTPUT8-',expand_x=True, expand_y=True,font=('Calibri',24))]]

    column_4 = [
                [sg.Text('Socket 3',expand_x=True, expand_y=True,font=('Calibri',24))],
                [sg.Text(key='-OUTPUT4-',expand_x=True, expand_y=True,font=('Calibri',24))],
                [sg.Text('Socket 7',expand_x=True, expand_y=True,font=('Calibri',24))],
                [sg.Text(key='-OUTPUT9-',expand_x=True, expand_y=True,font=('Calibri',24))]]

    column_5 = [
                [sg.Text('Socket 4',expand_x=True, expand_y=True,font=('Calibri',24))],
                [sg.Text(key='-OUTPUT5-',expand_x=True, expand_y=True,font=('Calibri',24))],
                [sg.Text('Socket 8',expand_x=True, expand_y=True,font=('Calibri',24))],
                [sg.Text(key='-OUTPUT10-',expand_x=True, expand_y=True,font=('Calibri',24))]]

    layout = [[sg.Column(column_1,expand_x=True, expand_y=True), sg.Column(column_2,expand_x=True, expand_y=True), sg.Column(column_3,expand_x=True, expand_y=True), sg.Column(column_4,expand_x=True, expand_y=True), sg.Column(column_5,expand_x=True, expand_y=True)]]

    window = sg.Window('Robot Status', layout, resizable = True)

    while True:

        event, values = window.read(timeout=10)
        if event == sg.WINDOW_CLOSED:
            break

        # Printing updated outputs to GUI
        try:
            data = data_queue.get_nowait()
            if data == 'QUIT':
                break

            if isinstance(data,list) and len(data) == 10:
                window['-OUTPUT1-'].update(data[8])
                window['-OUTPUT2-'].update(data[0])
                window['-OUTPUT3-'].update(data[1])
                window['-OUTPUT4-'].update(data[2])
                window['-OUTPUT5-'].update(data[3])
                window['-OUTPUT6-'].update(data[9])
                window['-OUTPUT7-'].update(data[4])
                window['-OUTPUT8-'].update(data[5])
                window['-OUTPUT9-'].update(data[6])
                window['-OUTPUT10-'].update(data[7]) 
        
        except queue.Empty:
            pass
        
    window.close()


if __name__ == "__main__":
    
    #Setting up and starting thread for GUI
    pygame_thread = threading.Thread(target=run_pygame, daemon=True)
    pygame_thread.start()

    #running pygame code
    run_gui()

    #Joining gui thread
    pygame_thread.join()


    exit(0)