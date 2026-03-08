import FreeSimpleGUI as sg
import random
import time

#Defining sent variables
killed = False
sock_1_value = 255
sock_2_value = 255
sock_3_value = 0
sock_4_value = 0
sock_5_value = 0
sock_6_value = 0
sock_7_value = 0
sock_8_value = 0
timers_running = 'No Timers Running'
printout = 'Estop Currently Enabled'

# Define the window's layout contents
# layout = [[sg.Text('Is the Robot Active?',size=(40,1)),sg.Text('Socket 1',size=(40,1)),sg.Text('Socket 2',size=(40,1)),sg.Text('Socket 3',size=(40,1)),sg.Text('Socket 4',size=(40,1))],
#             [sg.Text(size=(40,1), key='-OUTPUT1-'),sg.Text(size=(40,1), key='-OUTPUT2-'),sg.Text(size=(40,1), key='-OUTPUT3-'),sg.Text(size=(40,1), key='-OUTPUT4-'),sg.Text(size=(40,1), key='-OUTPUT5-')],
#             [sg.Text('Timers Running?',size=(40,1)),sg.Text('Socket 5',size=(40,1)),sg.Text('Socket 6',size=(40,1)),sg.Text('Socket 7',size=(40,1)),sg.Text('Socket 8',size=(40,1)),],
#             [sg.Text(size=(40,1), key='-OUTPUT6-'),sg.Text(size=(40,1), key='-OUTPUT7-'),sg.Text(size=(40,1), key='-OUTPUT8-'),sg.Text(size=(40,1), key='-OUTPUT9-'),sg.Text(size=(40,1), key='-OUTPUT10-')]]

# layout = [[sg.Text('Is the Robot Active?',expand_x=True, expand_y=True),sg.Text('Socket 1',expand_x=True,expand_y=True),sg.Text('Socket 2',expand_x=True,expand_y=True),sg.Text('Socket 3',expand_x=True,expand_y=True),sg.Text('Socket 4',expand_x=True,expand_y=True)],
#             [sg.Text(expand_x=True,expand_y=True, key='-OUTPUT1-'),sg.Text(expand_x=True,expand_y=True, key='-OUTPUT2-'),sg.Text(expand_x=True,expand_y=True, key='-OUTPUT3-'),sg.Text(expand_x=True,expand_y=True, key='-OUTPUT4-'),sg.Text(expand_x=True,expand_y=True, key='-OUTPUT5-')],
#             [sg.Text('Timers Running?',expand_x=True,expand_y=True),sg.Text('Socket 5',expand_x=True,expand_y=True),sg.Text('Socket 6',expand_x=True,expand_y=True),sg.Text('Socket 7',expand_x=True,expand_y=True),sg.Text('Socket 8',expand_x=True,expand_y=True),],
#             [sg.Text(expand_x=True,expand_y=True, key='-OUTPUT6-'),sg.Text(expand_x=True,expand_y=True, key='-OUTPUT7-'),sg.Text(expand_x=True,expand_y=True, key='-OUTPUT8-'),sg.Text(expand_x=True,expand_y=True, key='-OUTPUT9-'),sg.Text(expand_x=True,expand_y=True, key='-OUTPUT10-')]]

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
    event, values = window.read(50)

    if event == sg.WINDOW_CLOSED:
        break

    sock_1_value = random.randint(0,255)
    sock_2_value = random.randint(0,255)
    sock_3_value = random.randint(0,1)
    sock_4_value = random.randint(0,1)
    sock_5_value = random.randint(0,1)
    sock_6_value = random.randint(0,1)
    sock_7_value = random.randint(0,1)
    sock_8_value = random.randint(0,1)

    window['-OUTPUT1-'].update(str(killed))
    window['-OUTPUT2-'].update(str(sock_1_value))
    window['-OUTPUT3-'].update(str(sock_2_value))
    window['-OUTPUT4-'].update(str(sock_3_value))
    window['-OUTPUT5-'].update(str(sock_4_value))
    window['-OUTPUT6-'].update(str(timers_running))
    window['-OUTPUT7-'].update(str(sock_5_value))
    window['-OUTPUT8-'].update(str(sock_6_value))
    window['-OUTPUT9-'].update(str(sock_7_value))
    window['-OUTPUT10-'].update(str(sock_8_value))

    time.sleep(0.05)

window.close()