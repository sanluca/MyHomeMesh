#!/usr/bin/python3
# -*- coding: UTF-8 -*-
# File name   : client.py
# Description : client
# Author      : William
# Date      : 2018/08/22

from socket import *
import sys
import time
import threading as thread
import tkinter as tk
import json
import os

import serial

ser = 0


ip_stu = 1

init_1 = 99;
init_2 = 84;
init_3 = 57;
init_4 = 90;
init_5 = 84;
init_6 = 102;
init_7 = 96;
init_8 = 99;
init_9 = 114;
init_10 = 90;
init_11 = 96;
init_12 = 93;
init_13 = 90;

servoSpeed = 500

def global_init():
    global color_bg, color_text, color_btn, color_line, color_can, color_oval
    color_bg='#000000'      #Set background color
    color_text='#E1F5FE'      #Set text color
    color_btn='#0277BD'    #Set button color
    color_line='#01579B'      #Set line color
    color_can='#212121'    #Set canvas color
    color_oval='#2196F3'      #Set oval color

global_init()


def replace_num(initial,new_num):   #Call this function to replace data in '.txt' file
    newline=""
    str_num=str(new_num)
    try:
        with open("ip.txt","r") as f:
            for line in f.readlines():
                if(line.find(initial) == 0):
                    line = initial+"%s" %(str_num)
                newline += line
        with open("ip.txt","w") as f:
            f.writelines(newline)   #Call this function to replace data in '.txt' file
    except:
        pass


def replace_init(initial,new_num):   #Call this function to replace data in '.txt' file
    newline=""
    # try:
    # os.rename("AlterGo\\AlterGo.ino","AlterGo\\AlterGo.txt")
    with open("angle.h","r") as f:
        for line in f.readlines():
            if(line.find(initial) == 0):
                line = initial+"%d;\n"%(new_num)
            newline += line
    with open("angle.h","w") as f:
        f.writelines(newline)   #Call this function to replace data in '.txt' file
    # os.rename("AlterGo\\AlterGo.txt","AlterGo\\AlterGo.ino")
    
    
def my_replace_init(initial,new_num):   #Call this function to replace data in '.txt' file
    newline=""
    # try:
    # os.rename("AlterGo\\AlterGo.ino","AlterGo\\AlterGo.txt")
    with open("servosGUI.py","r") as f:
        for line in f.readlines():
            if(line.find(initial) == 0):
                line = initial+"%d;\n"%(new_num)
            newline += line
    with open("servosGUI.py","w") as f:
        f.writelines(newline)   #Call this function to replace data in '.txt' file
    # os.rename("AlterGo\\AlterGo.txt","AlterGo\\AlterGo.ino")
    
    


def num_import(initial):            #Call this function to import data from '.txt' file
    with open("ip.txt") as f:
        for line in f.readlines():
            if(line.find(initial) == 0):
                r=line
    begin=len(list(initial))
    snum=r[begin:]
    n=snum
    return n    


def pwm_show():
    # show_input = show_input.split()
    # L0.config(text=show_input[1])
    L1.config(text=init_1)
    L2.config(text=init_2)
    L3.config(text=init_3)
    L4.config(text=init_4)
    L5.config(text=init_5)
    L6.config(text=init_6)
    L7.config(text=init_7)
    L8.config(text=init_8)
    L9.config(text=init_9)
    L10.config(text=init_10)
    L11.config(text=init_11)
    L12.config(text=init_12)
    L13.config(text=init_13)
    # L14.config(text=init_14)
    # L15.config(text=init_15)


def normal_function_button():
    Btn_function_1.config(bg=color_btn)
    Btn_function_2.config(bg=color_btn)
    Btn_function_3.config(bg=color_btn)
    Btn_function_4.config(bg=color_btn)
    Btn_function_5.config(bg=color_btn)
    Btn_function_6.config(bg=color_btn)
    Btn_function_7.config(bg=color_btn)


def connection_thread():
    while 1:
        car_info = (tcpClicSock.recv(BUFSIZ)).decode()
        if not car_info:
            continue
        elif 'PWM' in car_info:
            pwm_show(car_info)


        elif '1 Selected' == car_info:
            normal_select_button()
            Btn_S1.config(bg='#4CAF50')

        elif '2 Selected' == car_info:
            normal_select_button()
            Btn_S2.config(bg='#4CAF50')

        elif '3 Selected' == car_info:
            normal_select_button()
            Btn_S3.config(bg='#4CAF50')

        elif '4 Selected' == car_info:
            normal_select_button()
            Btn_S4.config(bg='#4CAF50')

        elif '5 Selected' == car_info:
            normal_select_button()
            Btn_S5.config(bg='#4CAF50')

        elif '6 Selected' == car_info:
            normal_select_button()
            Btn_S6.config(bg='#4CAF50')

        elif '7 Selected' == car_info:
            normal_select_button()
            Btn_S7.config(bg='#4CAF50')

        elif '8 Selected' == car_info:
            normal_select_button()
            Btn_S8.config(bg='#4CAF50')

        elif '9 Selected' == car_info:
            normal_select_button()
            Btn_S9.config(bg='#4CAF50')

        elif '10 Selected' == car_info:
            normal_select_button()
            Btn_S10.config(bg='#4CAF50')

        elif '11 Selected' == car_info:
            normal_select_button()
            Btn_S11.config(bg='#4CAF50')

        elif '12 Selected' == car_info:
            normal_select_button()
            Btn_S12.config(bg='#4CAF50')

        elif '13 Selected' == car_info:
            normal_select_button()
            Btn_S13.config(bg='#4CAF50')

        elif '14 Selected' == car_info:
            normal_select_button()
            Btn_S14.config(bg='#4CAF50')

        elif '15 Selected' == car_info:
            normal_select_button()
            Btn_S15.config(bg='#4CAF50')

        elif '16 Selected' == car_info:
            normal_select_button()
            Btn_S16.config(bg='#4CAF50')


        elif 'function_1_on' in car_info:
            normal_function_button()
            Btn_function_1.config(bg='#4CAF50')

        elif 'function_2_on' in car_info:
            normal_function_button()
            Btn_function_2.config(bg='#4CAF50')

        elif 'function_3_on' in car_info:
            normal_function_button()
            Btn_function_3.config(bg='#4CAF50')

        elif 'function_4_on' in car_info:
            normal_function_button()
            Btn_function_4.config(bg='#4CAF50')

        elif 'function_5_on' in car_info:
            normal_function_button()
            Btn_function_5.config(bg='#4CAF50')

        elif 'function_6_on' in car_info:
            normal_function_button()
            Btn_function_6.config(bg='#4CAF50')

        elif 'function_7_on' in car_info:
            normal_function_button()
            Btn_function_7.config(bg='#4CAF50')

        elif 'perform_1_on' in car_info:
            normal_function_button()
            normal_select_button()
            Btn_P1.config(bg='#4CAF50')
        elif 'perform_2_on' in car_info:
            normal_function_button()
            normal_select_button()
            Btn_P2.config(bg='#4CAF50')
        elif 'perform_3_on' in car_info:
            normal_function_button()
            normal_select_button()
            Btn_P3.config(bg='#4CAF50')
        elif 'perform_4_on' in car_info:
            normal_function_button()
            normal_select_button()
            Btn_P4.config(bg='#4CAF50')
        elif 'perform_5_on' in car_info:
            normal_function_button()
            normal_select_button()
            Btn_P5.config(bg='#4CAF50')
        elif 'perform_6_on' in car_info:
            normal_function_button()
            normal_select_button()
            Btn_P6.config(bg='#4CAF50')
        elif 'perform_7_on' in car_info:
            normal_function_button()
            normal_select_button()
            Btn_P7.config(bg='#4CAF50')
        elif 'perform_8_on' in car_info:
            normal_function_button()
            normal_select_button()
            Btn_P8.config(bg='#4CAF50')

        elif 'perform_end' in car_info:
            normal_perform_button()

        print(car_info)

def wiat_connect():
    global ser
    while 1:
        ser.write("{'start':['setup']}\n".encode("gbk"))
        #print(11)
        line = ser.readline()
        if line:
            break
            
def serial_connect():    #Call this function to connect with the server
    global ADDR,tcpClicSock,BUFSIZ,ip_stu,ipaddr,ser
    com=E1.get()       #Get the IP address from Entry
    ser = serial.Serial(com,115200,timeout = 1,writeTimeout = 10)
    wiat_connect()
    print(com+':Succes')


def connect(event):    #Call this function to connect with the server
    if ip_stu == 1:
        sc=thread.Thread(target=serial_connect) #Define a thread for connection
        sc.setDaemon(True)                    #'True' means it is a front thread,it would close when the mainloop() closes
        sc.start()                            #Thread starts


def three_function(a,b,c):
    global ser
    a = str(a)
    b = str(b)
    c = str(c)
    p1 = "{'start':"+'['+a+','+b+','+c+']'+'}'+'\n'
    #print(p1)
    ser.write(p1.encode("gbk"))
    
def jsonDS(numInput, adjustInput):
    global init_1, init_2, init_3, init_4, init_5, init_6, init_7, init_8, init_9, init_10, init_11, init_12,ser,init_13
    if numInput == 1:
        init_1 += adjustInput
        posInput = init_1
    elif numInput == 2:
        init_2 += adjustInput
        posInput = init_2
    elif numInput == 3:
        init_3 += adjustInput
        posInput = init_3
    elif numInput == 4:
        init_4 += adjustInput
        posInput = init_4
    elif numInput == 5:
        init_5 += adjustInput
        posInput = init_5
    elif numInput == 6:
        init_6 += adjustInput
        posInput = init_6
    elif numInput == 7:
        init_7 += adjustInput
        posInput = init_7
    elif numInput == 8:
        init_8 += adjustInput
        posInput = init_8
    elif numInput == 9:
        init_9 += adjustInput
        posInput = init_9
    elif numInput == 10:
        init_10 += adjustInput
        posInput = init_10
    elif numInput == 11:
        init_11 += adjustInput
        posInput = init_11
    elif numInput == 12:
        init_12 += adjustInput
        posInput = init_12
    elif numInput == 13:
        init_13 += adjustInput
        posInput = init_13

    dumpsInput = {'angle':[numInput, posInput]}
    c2json = json.dumps(dumpsInput)
    three_function("'angle'",numInput-1,posInput);
    pwm_show()


def servo_buttons(x,y):
    global L0, L1, L2, L3, L4, L5, L6, L7, L8, L9, L10, L11, L12, L13, L14, L15
    # def call_pwm0_up(event):
    #   tcpClicSock.send(('0+').encode())

    # def call_pwm0_down(event):
    #   tcpClicSock.send(('0-').encode())

    def call_pwm1_up(event):
        jsonDS(1, 3)

    def call_pwm1_down(event):
        jsonDS(1, -3)

    def call_pwm2_up(event):
        jsonDS(2, 3)

    def call_pwm2_down(event):
        jsonDS(2, -3)

    def call_pwm3_up(event):
        jsonDS(3, 3)

    def call_pwm3_down(event):
        jsonDS(3, -3)

    def call_pwm4_up(event):
        jsonDS(4, 3)

    def call_pwm4_down(event):
        jsonDS(4, -3)

    def call_pwm5_up(event):
        jsonDS(5, 3)

    def call_pwm5_down(event):
        jsonDS(5, -3)

    def call_pwm6_up(event):
        jsonDS(6, 3)

    def call_pwm6_down(event):
        jsonDS(6, -3)

    def call_pwm7_up(event):
        jsonDS(7, 3)

    def call_pwm7_down(event):
        jsonDS(7, -3)

    def call_pwm8_up(event):
        jsonDS(8, 3)

    def call_pwm8_down(event):
        jsonDS(8, -3)

    def call_pwm9_up(event):
        jsonDS(9, 3)

    def call_pwm9_down(event):
        jsonDS(9, -3)

    def call_pwm10_up(event):
        jsonDS(10, 3)

    def call_pwm10_down(event):
        jsonDS(10, -3)

    def call_pwm11_up(event):
        jsonDS(11, 3)

    def call_pwm11_down(event):
        jsonDS(11, -3)

    def call_pwm12_up(event):
        jsonDS(12, 3)

    def call_pwm12_down(event):
        jsonDS(12, -3)
    def call_pwm13_up(event):
        jsonDS(13, 3)   
    def call_pwm13_down(event):
        jsonDS(13, -3)

    # def call_pwm13_up(event):
    #   tcpClicSock.send(('13+').encode())

    # def call_pwm13_down(event):
    #   tcpClicSock.send(('13-').encode())

    # def call_pwm14_up(event):
    #   tcpClicSock.send(('14+').encode())

    # def call_pwm14_down(event):
    #   tcpClicSock.send(('14-').encode())

    # def call_pwm15_up(event):
    #   tcpClicSock.send(('15+').encode())

    # def call_pwm15_down(event):
    #   tcpClicSock.send(('15-').encode())

    # L0 = tk.Label(root,width=8,text='PWM0',fg=color_text,bg='#212121')
    # L0.place(x=x,y=y-25)
    # Btn_0i = tk.Button(root, width=8, text='PWM0+',fg=color_text,bg=color_btn,relief='ridge')
    # Btn_0i.place(x=x,y=y)
    # Btn_0i.bind('<ButtonPress-1>', call_pwm0_up)

    # Btn_0d = tk.Button(root, width=8, text='PWM0-',fg=color_text,bg=color_btn,relief='ridge')
    # Btn_0d.place(x=x,y=y+35)
    # Btn_0d.bind('<ButtonPress-1>', call_pwm0_down)

    L1 = tk.Label(root,width=8,text=init_1,fg=color_text,bg='#212121')
    L1.place(x=x+100,y=y-15)
    Btn_1i = tk.Button(root, width=8, text='angle0+',fg=color_text,bg=color_btn,relief='ridge')
    Btn_1i.place(x=x+100,y=y+10)
    Btn_1i.bind('<ButtonPress-1>', call_pwm1_up)

    Btn_1d = tk.Button(root, width=8, text='angle0-',fg=color_text,bg=color_btn,relief='ridge')
    Btn_1d.place(x=x+100,y=y+45)
    Btn_1d.bind('<ButtonPress-1>', call_pwm1_down)

    leg1 = tk.Label(root,width=8,text='Leg1',fg='#FF1493',bg='#212121')
    leg1.place(x=x+150,y=y-35)
    
    L2 = tk.Label(root,width=8,text=init_2,fg=color_text,bg='#212121')
    L2.place(x=x+200,y=y-15)
    Btn_2i = tk.Button(root, width=8, text='angle1+',fg=color_text,bg=color_btn,relief='ridge')
    Btn_2i.place(x=x+200,y=y+10)
    Btn_2i.bind('<ButtonPress-1>', call_pwm2_up)

    Btn_2d = tk.Button(root, width=8, text='angle1-',fg=color_text,bg=color_btn,relief='ridge')
    Btn_2d.place(x=x+200,y=y+45)
    Btn_2d.bind('<ButtonPress-1>', call_pwm2_down)

    L3 = tk.Label(root,width=8,text=init_3,fg=color_text,bg='#212121')
    L3.place(x=x+300,y=y-15)
    Btn_3i = tk.Button(root, width=8, text='angle2+',fg=color_text,bg=color_btn,relief='ridge')
    Btn_3i.place(x=x+300,y=y+10)
    Btn_3i.bind('<ButtonPress-1>', call_pwm3_up)

    Btn_3d = tk.Button(root, width=8, text='angle2-',fg=color_text,bg=color_btn,relief='ridge')
    Btn_3d.place(x=x+300,y=y+45)
    Btn_3d.bind('<ButtonPress-1>', call_pwm3_down)
    leg2 = tk.Label(root,width=8,text='Leg2',fg='#FF1493',bg='#212121')
    leg2.place(x=x+350,y=y-35)
    L4 = tk.Label(root,width=8,text=init_4,fg=color_text,bg='#212121')
    L4.place(x=x+400,y=y-15)
    Btn_4i = tk.Button(root, width=8, text='angle3+',fg=color_text,bg=color_btn,relief='ridge')
    Btn_4i.place(x=x+400,y=y+10)
    Btn_4i.bind('<ButtonPress-1>', call_pwm4_up)

    Btn_4d = tk.Button(root, width=8, text='angle3-',fg=color_text,bg=color_btn,relief='ridge')
    Btn_4d.place(x=x+400,y=y+45)
    Btn_4d.bind('<ButtonPress-1>', call_pwm4_down)

    
    L5 = tk.Label(root,width=8,text=init_5,fg=color_text,bg='#212121')
    L5.place(x=x+500,y=y-15)
    Btn_5i = tk.Button(root, width=8, text='angle4+',fg=color_text,bg=color_btn,relief='ridge')
    Btn_5i.place(x=x+500,y=y+10)
    Btn_5i.bind('<ButtonPress-1>', call_pwm5_up)

    Btn_5d = tk.Button(root, width=8, text='angle4-',fg=color_text,bg=color_btn,relief='ridge')
    Btn_5d.place(x=x+500,y=y+45)
    Btn_5d.bind('<ButtonPress-1>', call_pwm5_down)

    leg2 = tk.Label(root,width=8,text='Leg3',fg='#FF1493',bg='#212121')
    leg2.place(x=x+550,y=y-35)
    
    L6 = tk.Label(root,width=8,text=init_6,fg=color_text,bg='#212121')
    L6.place(x=x+600,y=y-15)
    Btn_6i = tk.Button(root, width=8, text='angle5+',fg=color_text,bg=color_btn,relief='ridge')
    Btn_6i.place(x=x+600,y=y+10)
    Btn_6i.bind('<ButtonPress-1>', call_pwm6_up)

    Btn_6d = tk.Button(root, width=8, text='angle5-',fg=color_text,bg=color_btn,relief='ridge')
    Btn_6d.place(x=x+600,y=y+45)
    Btn_6d.bind('<ButtonPress-1>', call_pwm6_down)

    L7 = tk.Label(root,width=8,text=init_7,fg=color_text,bg='#212121')
    L7.place(x=x+100,y=y+100)
    Btn_7i = tk.Button(root, width=8, text='angle6+',fg=color_text,bg=color_btn,relief='ridge')
    Btn_7i.place(x=x+100,y=y+125)
    Btn_7i.bind('<ButtonPress-1>', call_pwm7_up)

    Btn_7d = tk.Button(root, width=8, text='angle6-',fg=color_text,bg=color_btn,relief='ridge')
    Btn_7d.place(x=x+100,y=y+150)
    Btn_7d.bind('<ButtonPress-1>', call_pwm7_down)
    leg2 = tk.Label(root,width=8,text='Leg4',fg='#FF1493',bg='#212121')
    leg2.place(x=x+150,y=y+75)
    
    L8 = tk.Label(root,width=8,text=init_8,fg=color_text,bg='#212121')
    L8.place(x=x+200,y=y+100)
    Btn_8i = tk.Button(root, width=8, text='angle7+',fg=color_text,bg=color_btn,relief='ridge')
    Btn_8i.place(x=x+200,y=y+125)
    Btn_8i.bind('<ButtonPress-1>', call_pwm8_up)

    Btn_8d = tk.Button(root, width=8, text='angle7-',fg=color_text,bg=color_btn,relief='ridge')
    Btn_8d.place(x=x+200,y=y+150)
    Btn_8d.bind('<ButtonPress-1>', call_pwm8_down)

    L9 = tk.Label(root,width=8,text=init_9,fg=color_text,bg='#212121')
    L9.place(x=x+300,y=y+100)
    Btn_9i = tk.Button(root, width=8, text='angle8+',fg=color_text,bg=color_btn,relief='ridge')
    Btn_9i.place(x=x+300,y=y+125)
    Btn_9i.bind('<ButtonPress-1>', call_pwm9_up)

    Btn_9d = tk.Button(root, width=8, text='angle8-',fg=color_text,bg=color_btn,relief='ridge')
    Btn_9d.place(x=x+300,y=y+150)
    Btn_9d.bind('<ButtonPress-1>', call_pwm9_down)
    
    leg2 = tk.Label(root,width=8,text='Leg5',fg='#FF1493',bg='#212121')
    leg2.place(x=x+350,y=y+75)
    
    L10 = tk.Label(root,width=8,text=init_10,fg=color_text,bg='#212121')
    L10.place(x=x+400,y=y+100)
    Btn_10i = tk.Button(root, width=8, text='angle9+',fg=color_text,bg=color_btn,relief='ridge')
    Btn_10i.place(x=x+400,y=y+125)
    Btn_10i.bind('<ButtonPress-1>', call_pwm10_up)

    Btn_10d = tk.Button(root, width=8, text='angle9-',fg=color_text,bg=color_btn,relief='ridge')
    Btn_10d.place(x=x+400,y=y+150)
    Btn_10d.bind('<ButtonPress-1>', call_pwm10_down)

    
    L11 = tk.Label(root,width=8,text=init_11,fg=color_text,bg='#212121')
    L11.place(x=x+500,y=y+100)
    Btn_11i = tk.Button(root, width=8, text='angle10+',fg=color_text,bg=color_btn,relief='ridge')
    Btn_11i.place(x=x+500,y=y+125)
    Btn_11i.bind('<ButtonPress-1>', call_pwm11_up)

    
    Btn_11d = tk.Button(root, width=8, text='angle10-',fg=color_text,bg=color_btn,relief='ridge')
    Btn_11d.place(x=x+500,y=y+150)
    Btn_11d.bind('<ButtonPress-1>', call_pwm11_down)

    leg2 = tk.Label(root,width=8,text='Leg6',fg='#FF1493',bg='#212121')
    leg2.place(x=x+550,y=y+75)
    L12 = tk.Label(root,width=8,text=init_12,fg=color_text,bg='#212121')
    L12.place(x=x+600,y=y+100)
    Btn_12i = tk.Button(root, width=8, text='angle11+',fg=color_text,bg=color_btn,relief='ridge')
    Btn_12i.place(x=x+600,y=y+125)
    Btn_12i.bind('<ButtonPress-1>', call_pwm12_up)

    Btn_12d = tk.Button(root, width=8, text='angle11-',fg=color_text,bg=color_btn,relief='ridge')
    Btn_12d.place(x=x+600,y=y+150)
    Btn_12d.bind('<ButtonPress-1>', call_pwm12_down)
    
    leg2 = tk.Label(root,width=8,text='Head',fg='#FF1493',bg='#212121')
    leg2.place(x=x+700,y=y)
    L13 = tk.Label(root,width=8,text=init_13,fg=color_text,bg='#212121')
    L13.place(x=x+700,y=y+25)
    
    Btn_12i = tk.Button(root, width=8, text='angle12+',fg=color_text,bg=color_btn,relief='ridge')
    Btn_12i.place(x=x+700,y=y+50)
    Btn_12i.bind('<ButtonPress-1>', call_pwm13_up)
    Btn_12d = tk.Button(root, width=8, text='angle12-',fg=color_text,bg=color_btn,relief='ridge')
    Btn_12d.place(x=x+700,y=y+85)
    Btn_12d.bind('<ButtonPress-1>', call_pwm13_down)

    # L13 = tk.Label(root,width=8,text='PWM13',fg=color_text,bg='#212121')
    # L13.place(x=x+500,y=y+75)
    # Btn_13i = tk.Button(root, width=8, text='PWM13+',fg=color_text,bg=color_btn,relief='ridge')
    # Btn_13i.place(x=x+500,y=y+100)
    # Btn_13i.bind('<ButtonPress-1>', call_pwm13_up)

    # Btn_13d = tk.Button(root, width=8, text='PWM13-',fg=color_text,bg=color_btn,relief='ridge')
    # Btn_13d.place(x=x+500,y=y+135)
    # Btn_13d.bind('<ButtonPress-1>', call_pwm13_down)

    # L14 = tk.Label(root,width=8,text='PWM14',fg=color_text,bg='#212121')
    # L14.place(x=x+600,y=y+75)
    # Btn_14i = tk.Button(root, width=8, text='PWM14+',fg=color_text,bg=color_btn,relief='ridge')
    # Btn_14i.place(x=x+600,y=y+100)
    # Btn_14i.bind('<ButtonPress-1>', call_pwm14_up)

    # Btn_14d = tk.Button(root, width=8, text='PWM14-',fg=color_text,bg=color_btn,relief='ridge')
    # Btn_14d.place(x=x+600,y=y+135)
    # Btn_14d.bind('<ButtonPress-1>', call_pwm14_down)


    # L15 = tk.Label(root,width=8,text='PWM15',fg=color_text,bg='#212121')
    # L15.place(x=x+700,y=y+75)
    # Btn_15i = tk.Button(root, width=8, text='PWM15+',fg=color_text,bg=color_btn,relief='ridge')
    # Btn_15i.place(x=x+700,y=y+100)
    # Btn_15i.bind('<ButtonPress-1>', call_pwm15_up)

    # Btn_15d = tk.Button(root, width=8, text='PWM15-',fg=color_text,bg=color_btn,relief='ridge')
    # Btn_15d.place(x=x+700,y=y+135)
    # Btn_15d.bind('<ButtonPress-1>', call_pwm15_down)


def posSelect_buttons(x,y):
    global Btn_S1, Btn_S2, Btn_S3, Btn_S4, Btn_S5, Btn_S6, Btn_S7, Btn_S8, Btn_S9, Btn_S10, Btn_S11, Btn_S12, Btn_S13, Btn_S14, Btn_S15, Btn_S16
    def select_1(event):
        tcpClicSock.send(('Select 1').encode())

    def select_2(event):
        tcpClicSock.send(('Select 2').encode())

    def select_3(event):
        tcpClicSock.send(('Select 3').encode())

    def select_4(event):
        tcpClicSock.send(('Select 4').encode())

    def select_5(event):
        tcpClicSock.send(('Select 5').encode())

    def select_6(event):
        tcpClicSock.send(('Select 6').encode())

    def select_7(event):
        tcpClicSock.send(('Select 7').encode())

    def select_8(event):
        tcpClicSock.send(('Select 8').encode())


    def select_9(event):
        tcpClicSock.send(('Select 9').encode())

    def select_10(event):
        tcpClicSock.send(('Select 10').encode())

    def select_11(event):
        tcpClicSock.send(('Select 11').encode())

    def select_12(event):
        tcpClicSock.send(('Select 12').encode())

    def select_13(event):
        tcpClicSock.send(('Select 13').encode())

    def select_14(event):
        tcpClicSock.send(('Select 14').encode())

    def select_15(event):
        tcpClicSock.send(('Select 15').encode())

    def select_16(event):
        tcpClicSock.send(('Select 16').encode())


    Btn_S1 = tk.Button(root, width=8, text='Q',fg=color_text,bg=color_btn,relief='ridge')
    Btn_S1.place(x=x,y=y)
    Btn_S1.bind('<ButtonPress-1>', select_1)
    root.bind('<KeyPress-q>', select_1) 

    Btn_S2 = tk.Button(root, width=8, text='W',fg=color_text,bg=color_btn,relief='ridge')
    Btn_S2.place(x=x+100,y=y)
    Btn_S2.bind('<ButtonPress-1>', select_2)
    root.bind('<KeyPress-w>', select_2)

    Btn_S3 = tk.Button(root, width=8, text='E',fg=color_text,bg=color_btn,relief='ridge')
    Btn_S3.place(x=x+200,y=y)
    Btn_S3.bind('<ButtonPress-1>', select_3)
    root.bind('<KeyPress-e>', select_3) 

    Btn_S4 = tk.Button(root, width=8, text='R',fg=color_text,bg=color_btn,relief='ridge')
    Btn_S4.place(x=x+300,y=y)
    Btn_S4.bind('<ButtonPress-1>', select_4)
    root.bind('<KeyPress-r>', select_4) 

    Btn_S5 = tk.Button(root, width=8, text='T',fg=color_text,bg=color_btn,relief='ridge')
    Btn_S5.place(x=x+400,y=y)
    Btn_S5.bind('<ButtonPress-1>', select_5)
    root.bind('<KeyPress-t>', select_5) 

    Btn_S6 = tk.Button(root, width=8, text='Y',fg=color_text,bg=color_btn,relief='ridge')
    Btn_S6.place(x=x+500,y=y)
    Btn_S6.bind('<ButtonPress-1>', select_6)
    root.bind('<KeyPress-y>', select_6) 

    Btn_S7 = tk.Button(root, width=8, text='U',fg=color_text,bg=color_btn,relief='ridge')
    Btn_S7.place(x=x+600,y=y)
    Btn_S7.bind('<ButtonPress-1>', select_7)
    root.bind('<KeyPress-u>', select_7) 

    Btn_S8 = tk.Button(root, width=8, text='I',fg=color_text,bg=color_btn,relief='ridge')
    Btn_S8.place(x=x+700,y=y)
    Btn_S8.bind('<ButtonPress-1>', select_8)
    root.bind('<KeyPress-i>', select_8) 


    Btn_S9 = tk.Button(root, width=8, text='A',fg=color_text,bg=color_btn,relief='ridge')
    Btn_S9.place(x=x,y=y+35)
    Btn_S9.bind('<ButtonPress-1>', select_9)
    root.bind('<KeyPress-a>', select_9)

    Btn_S10 = tk.Button(root, width=8, text='S',fg=color_text,bg=color_btn,relief='ridge')
    Btn_S10.place(x=x+100,y=y+35)
    Btn_S10.bind('<ButtonPress-1>', select_10)
    root.bind('<KeyPress-s>', select_10) 

    Btn_S11 = tk.Button(root, width=8, text='D',fg=color_text,bg=color_btn,relief='ridge')
    Btn_S11.place(x=x+200,y=y+35)
    Btn_S11.bind('<ButtonPress-1>', select_11)
    root.bind('<KeyPress-d>', select_11) 

    Btn_S12 = tk.Button(root, width=8, text='F',fg=color_text,bg=color_btn,relief='ridge')
    Btn_S12.place(x=x+300,y=y+35)
    Btn_S12.bind('<ButtonPress-1>', select_12)
    root.bind('<KeyPress-f>', select_12) 

    Btn_S13 = tk.Button(root, width=8, text='G',fg=color_text,bg=color_btn,relief='ridge')
    Btn_S13.place(x=x+400,y=y+35)
    Btn_S13.bind('<ButtonPress-1>', select_13)
    root.bind('<KeyPress-g>', select_13) 

    Btn_S14 = tk.Button(root, width=8, text='H',fg=color_text,bg=color_btn,relief='ridge')
    Btn_S14.place(x=x+500,y=y+35)
    Btn_S14.bind('<ButtonPress-1>', select_14)
    root.bind('<KeyPress-h>', select_14) 

    Btn_S15 = tk.Button(root, width=8, text='J',fg=color_text,bg=color_btn,relief='ridge')
    Btn_S15.place(x=x+600,y=y+35)
    Btn_S15.bind('<ButtonPress-1>', select_15)
    root.bind('<KeyPress-j>', select_15) 

    Btn_S16 = tk.Button(root, width=8, text='K',fg=color_text,bg=color_btn,relief='ridge')
    Btn_S16.place(x=x+700,y=y+35)
    Btn_S16.bind('<ButtonPress-1>', select_16)
    root.bind('<KeyPress-k>', select_16) 


def perform_buttons(x,y):
    global Btn_P1, Btn_P2, Btn_P3, Btn_P4, Btn_P5, Btn_P6, Btn_P7, Btn_P8
    def perform_1(event):
        tcpClicSock.send(('perform_1').encode())
    def perform_2(event):
        tcpClicSock.send(('perform_2').encode())
    def perform_3(event):
        tcpClicSock.send(('perform_3').encode())
    def perform_4(event):
        tcpClicSock.send(('perform_4').encode())
    def perform_5(event):
        tcpClicSock.send(('perform_5').encode())
    def perform_6(event):
        tcpClicSock.send(('perform_6').encode())
    def perform_7(event):
        tcpClicSock.send(('perform_7').encode())
    def perform_8(event):
        tcpClicSock.send(('perform_8').encode())

    Btn_P1 = tk.Button(root, width=8, text='Perform_1',fg=color_text,bg=color_btn,relief='ridge')
    Btn_P1.place(x=x,y=y)
    Btn_P1.bind('<ButtonPress-1>', perform_1)
    root.bind('<KeyPress-z>', perform_1)

    Btn_P2 = tk.Button(root, width=8, text='Perform_2',fg=color_text,bg=color_btn,relief='ridge')
    Btn_P2.place(x=x+100,y=y)
    Btn_P2.bind('<ButtonPress-1>', perform_2)
    root.bind('<KeyPress-x>', perform_2)

    Btn_P3 = tk.Button(root, width=8, text='Perform_3',fg=color_text,bg=color_btn,relief='ridge')
    Btn_P3.place(x=x+200,y=y)
    Btn_P3.bind('<ButtonPress-1>', perform_3)
    root.bind('<KeyPress-c>', perform_3)

    Btn_P4 = tk.Button(root, width=8, text='Perform_4',fg=color_text,bg=color_btn,relief='ridge')
    Btn_P4.place(x=x+300,y=y)
    Btn_P4.bind('<ButtonPress-1>', perform_4)
    root.bind('<KeyPress-v>', perform_4)

    Btn_P5 = tk.Button(root, width=8, text='Perform_5',fg=color_text,bg=color_btn,relief='ridge')
    Btn_P5.place(x=x+400,y=y)
    Btn_P5.bind('<ButtonPress-1>', perform_5)
    root.bind('<KeyPress-b>', perform_5)

    Btn_P6 = tk.Button(root, width=8, text='Perform_6',fg=color_text,bg=color_btn,relief='ridge')
    Btn_P6.place(x=x+500,y=y)
    Btn_P6.bind('<ButtonPress-1>', perform_6)
    root.bind('<KeyPress-n>', perform_6)

    Btn_P7 = tk.Button(root, width=8, text='Perform_7',fg=color_text,bg=color_btn,relief='ridge')
    Btn_P7.place(x=x+600,y=y)
    Btn_P7.bind('<ButtonPress-1>', perform_7)
    root.bind('<KeyPress-m>', perform_7)

    Btn_P8 = tk.Button(root, width=8, text='Perform_8',fg=color_text,bg=color_btn,relief='ridge')
    Btn_P8.place(x=x+700,y=y)
    Btn_P8.bind('<ButtonPress-1>', perform_8)
    root.bind('<KeyPress-,>', perform_8)


def normal_perform_button():
    Btn_P1.config(bg=color_btn)
    Btn_P2.config(bg=color_btn)
    Btn_P3.config(bg=color_btn)
    Btn_P4.config(bg=color_btn)
    Btn_P5.config(bg=color_btn)
    Btn_P6.config(bg=color_btn)
    Btn_P7.config(bg=color_btn)
    Btn_P8.config(bg=color_btn)


def normal_select_button():
    Btn_S1.config(bg=color_btn)
    Btn_S2.config(bg=color_btn)
    Btn_S3.config(bg=color_btn)
    Btn_S4.config(bg=color_btn)
    Btn_S5.config(bg=color_btn)
    Btn_S6.config(bg=color_btn)
    Btn_S7.config(bg=color_btn)
    Btn_S8.config(bg=color_btn)
    Btn_S9.config(bg=color_btn)
    Btn_S10.config(bg=color_btn)
    Btn_S11.config(bg=color_btn)
    Btn_S12.config(bg=color_btn)
    Btn_S13.config(bg=color_btn)
    Btn_S14.config(bg=color_btn)
    Btn_S15.config(bg=color_btn)
    Btn_S16.config(bg=color_btn)


def connent_input(x,y):
    global E1, Btn14
    E1 = tk.Entry(root,show=None,width=16,bg="#37474F",fg='#eceff1')
    E1.place(x=x+5,y=y+25)                           #Define a Entry and put it in position

    l_ip_3=tk.Label(root,width=10,text='Port:',fg=color_text,bg='#000000')
    l_ip_3.place(x=x,y=y)                        #Define a Label and put it in position

    Btn14= tk.Button(root, width=8,height=2, text='Connect',fg=color_text,bg=color_btn,relief='ridge')
    Btn14.place(x=x+130,y=y)                          #Define a Button and put it in position

    root.bind('<Return>', connect)
    Btn14.bind('<ButtonPress-1>', connect)


def initSet(event):
    replace_init("int angle0 = ", init_1)
    replace_init("int angle1 = ", init_2)
    replace_init("int angle2 = ", init_3)
    replace_init("int angle3 = ", init_4)
    replace_init("int angle4 = ", init_5)

    replace_init("int angle5 = ", init_6)
    replace_init("int angle6 = ", init_7)
    replace_init("int angle7 = ", init_8)
    replace_init("int angle8 = ", init_9)
    replace_init("int angle9 = ", init_10)

    replace_init("int angle10 = ", init_11)
    replace_init("int angle11 = ", init_12)
    replace_init("int angle12 = ", init_13)
    
    
    my_replace_init("init_1 = ", init_1)
    my_replace_init("init_2 = ", init_2)
    my_replace_init("init_3 = ", init_3)
    
    my_replace_init("init_4 = ", init_4)
    my_replace_init("init_5 = ", init_5)
    my_replace_init("init_6 = ", init_6)
    
    my_replace_init("init_7 = ", init_7)
    my_replace_init("init_8 = ", init_8)
    my_replace_init("init_9 = ", init_9)
    
    my_replace_init("init_10 = ", init_10)
    my_replace_init("init_11 = ", init_11)
    my_replace_init("init_12 = ", init_12)
    my_replace_init("init_13 = ", init_13)

def set_button(x,y):
    global L_select_feedback
    L_select_feedback = tk.Label(root,width=23,text='Click here after debugging',fg=color_text,bg='#000000')
    L_select_feedback.place(x=x-200,y=y-25)

    Btn_Switch_1 = tk.Button(root, width=8, text='SET',fg=color_text,bg=color_btn,relief='ridge')
    Btn_Switch_1.place(x=x-100,y=y)
    Btn_Switch_1.bind('<ButtonPress-1>', initSet)


def scale(x,y,w):
    def pix_send(event):
        time.sleep(0.03)
        tcpClicSock.send(('pix %s'%var_B.get()).encode())


    def time_send(event):
        time.sleep(0.03)
        tcpClicSock.send(('time %s'%var_C.get()).encode())

    global var_B, var_C
    var_B = tk.StringVar()
    var_B.set(10)

    var_C = tk.StringVar()
    var_C.set(0.03)

    Scale_B = tk.Scale(root,label=None,
    from_=0,to=50,orient=tk.HORIZONTAL,length=w,
    showvalue=1,tickinterval=None,resolution=5,variable=var_B,troughcolor='#448AFF',command=pix_send,fg=color_text,bg=color_bg,highlightthickness=0)
    Scale_B.place(x=x,y=y)                          #Define a Scale and put it in position

    Scale_B = tk.Scale(root,label=None,
    from_=0.00,to=0.15,orient=tk.HORIZONTAL,length=w,
    showvalue=1,tickinterval=None,resolution=0.01,variable=var_C,troughcolor='#448AFF',command=time_send,fg=color_text,bg=color_bg,highlightthickness=0)
    Scale_B.place(x=x,y=y+30)                           #Define a Scale and put it in position

    canvas_cover=tk.Canvas(root,bg=color_bg,height=30,width=510,highlightthickness=0)
    canvas_cover.place(x=x,y=y+60)


def speed_set(x,y,w):
    def speed_send(event):
        time.sleep(0.03)
        tcpClicSock.send(('speed %s'%var_speed.get()).encode())
    global var_speed
    var_speed = tk.StringVar()
    var_speed.set(1)

    Scale_speed = tk.Scale(root,label=None,
    from_=1,to=50,orient=tk.HORIZONTAL,length=w,
    showvalue=1,tickinterval=None,resolution=1,variable=var_speed,troughcolor='#448AFF',command=speed_send,fg=color_text,bg=color_bg,highlightthickness=0)
    Scale_speed.place(x=x,y=y)                          #Define a Scale and put it in position


def function_buttons(x,y):
    global function_stu, Btn_function_1, Btn_function_2, Btn_function_3, Btn_function_4, Btn_function_5, Btn_function_6, Btn_function_7
    def call_function_1(event):
        tcpClicSock.send(('function_1_on').encode())

    def call_function_2(event):
        tcpClicSock.send(('function_2_on').encode())

    def call_function_3(event):
        tcpClicSock.send(('function_3_on').encode())

    def call_function_4(event):
        tcpClicSock.send(('function_4_on').encode())

    def call_function_5(event):
        tcpClicSock.send(('function_5_on').encode())

    def call_function_6(event):
        tcpClicSock.send(('function_6_on').encode())

    def call_function_7(event):
        tcpClicSock.send(('function_7_on').encode())

    Btn_function_1 = tk.Button(root, width=8, text='FastMove',fg=color_text,bg=color_btn,relief='ridge')
    Btn_function_2 = tk.Button(root, width=8, text='Smooth',fg=color_text,bg=color_btn,relief='ridge')
    Btn_function_3 = tk.Button(root, width=8, text='Bézier',fg=color_text,bg=color_btn,relief='ridge')
    Btn_function_4 = tk.Button(root, width=8, text='Config',fg=color_text,bg=color_btn,relief='ridge')
    Btn_function_5 = tk.Button(root, width=8, text='-',fg=color_text,bg=color_btn,relief='ridge')
    Btn_function_6 = tk.Button(root, width=8, text='-',fg=color_text,bg=color_btn,relief='ridge')
    Btn_function_7 = tk.Button(root, width=8, text='-',fg=color_text,bg=color_btn,relief='ridge')

    Btn_function_1.place(x=x,y=y)
    Btn_function_2.place(x=x+70,y=y)
    Btn_function_3.place(x=x+140,y=y)
    Btn_function_4.place(x=x+210,y=y)
    Btn_function_5.place(x=x+280,y=y)
    Btn_function_6.place(x=x+350,y=y)
    Btn_function_7.place(x=x+420,y=y)

    Btn_function_1.bind('<ButtonPress-1>', call_function_1)
    Btn_function_2.bind('<ButtonPress-1>', call_function_2)
    Btn_function_3.bind('<ButtonPress-1>', call_function_3)
    Btn_function_4.bind('<ButtonPress-1>', call_function_4)
    Btn_function_5.bind('<ButtonPress-1>', call_function_5)
    Btn_function_6.bind('<ButtonPress-1>', call_function_6)
    Btn_function_7.bind('<ButtonPress-1>', call_function_7)


def loop():
    global root
    root = tk.Tk()          
    root.title('Quadruped Leg Init')
    root.geometry('725x320')
    root.config(bg=color_bg)

    try:
        logo =tk.PhotoImage(file = 'logo.png')
        l_logo=tk.Label(root,image = logo,bg=color_bg)
        l_logo.place(x=30,y=13)
    except:
        pass

    connent_input(25,15)
    set_button(630,35)
    #set_button1(630,35)
    #speed_set(350,25,280)
    
    servo_buttons(-70,120)

    # posSelect_buttons(30,385)

    # perform_buttons(30,465)

    # scale(530,290,260)

    # function_buttons(30,320)

    root.mainloop()


if __name__ == '__main__':
    loop()
