#!/usr/bin/python
# -*- coding: UTF-8 -*-
# File name   : client.py
# Description : client  
# Website	 : www.adeept.com
# E-mail	  : support@adeept.com
# Author	  : William
# Date		: 2018/08/22

from socket import *
import sys
import time
import threading as thread
import tkinter as tk
import math

tcpClicSock = ''
stat = 0
ip_stu=1
stop_stu = 1
x_range = 1

def replace_num(initial,new_num):   #Call this function to replace data in '.txt' file
	newline=""
	str_num=str(new_num)
	with open("ip.txt","r") as f:
		for line in f.readlines():
			if(line.find(initial) == 0):
				line = initial+"%s" %(str_num)
			newline += line
	with open("ip.txt","w") as f:
		f.writelines(newline)	#Call this function to replace data in '.txt' file


def num_import(initial):			#Call this function to import data from '.txt' file
	with open("ip.txt") as f:
		for line in f.readlines():
			if(line.find(initial) == 0):
				r=line
	begin=len(list(initial))
	snum=r[begin:]
	n=snum
	return n	


def call_forward(event):		 #When this function is called,client commands the car to move forward
	global stop_stu
	if stop_stu:
		print("Forward")
		tcpClicSock.send(('1').encode())
		stop_stu = 0


def call_back(event):
	global stop_stu
	if stop_stu:
		print("Backward")
		tcpClicSock.send(('2').encode())
		stop_stu = 0


def call_right(event):
	global stop_stu
	if stop_stu:
		print("Right")
		tcpClicSock.send(('3').encode())
		stop_stu = 0


def call_left(event):
	global stop_stu
	if stop_stu:
		print("Left")
		tcpClicSock.send(('4').encode())
		stop_stu = 0


def call_steady(event):
	print("Steady")
	tcpClicSock.send(('5').encode())


def call_stop(event):
	global stop_stu
	print("Stop")
	tcpClicSock.send(('9').encode())
	stop_stu = 1


def call_attack(event):
	print("Attack")
	tcpClicSock.send(('10').encode())


def call_auto(event):
	print("Auto")
	tcpClicSock.send(('8').encode())


def call_ultra(event):
	print("Ultrasonic")
	tcpClicSock.send(('7').encode())


def new_number2view(info):
	dis_list=[]
	f_list=[]

	total_number = int(info[2:])

	can_scan_1 = tk.Canvas(root,bg=color_can,height=250,width=320,highlightthickness=0) #define a canvas
	can_scan_1.place(x=30,y=300) #Place the canvas
	line = can_scan_1.create_line(0,62,320,62,fill='darkgray')   #Draw a line on canvas
	line = can_scan_1.create_line(0,124,320,124,fill='darkgray') #Draw a line on canvas
	line = can_scan_1.create_line(0,186,320,186,fill='darkgray') #Draw a line on canvas
	line = can_scan_1.create_line(160,0,160,250,fill='darkgray') #Draw a line on canvas
	line = can_scan_1.create_line(80,0,80,250,fill='darkgray')   #Draw a line on canvas
	line = can_scan_1.create_line(240,0,240,250,fill='darkgray') #Draw a line on canvas

	for i in range (total_number,0,-1):   #Scale the result to the size as canvas
		dis_info_get = (tcpClicSock.recv(BUFSIZ)).decode()
		print(dis_info_get)
		if 'F' in dis_info_get:
			break

		try:
			dis_info_get = int(dis_info_get)/100
			if dis_info_get > 0:
				len_dis_1 = int((dis_info_get/x_range)*250)						  #600 is the height of canvas
				pos	 = int((i/total_number)*320)								#740 is the width of canvas
				pos_ra  = int(((i/total_number)*140)+20)						   #Scale the direction range to (20-160)
				len_dis = int(len_dis_1*(math.sin(math.radians(pos_ra))))		   #len_dis is the height of the line

				x0_l,y0_l,x1_l,y1_l=pos,(250-len_dis),pos,(250-len_dis)			 #The position of line
				x0,y0,x1,y1=(pos+3),(250-len_dis+3),(pos-3),(250-len_dis-3)		 #The position of arc

				if pos <= 160:													  #Scale the whole picture to a shape of sector
					pos = 160-abs(int(len_dis_1*(math.cos(math.radians(pos_ra)))))
					x1_l= (x1_l-math.cos(math.radians(pos_ra))*130)
				else:
					pos = abs(int(len_dis_1*(math.cos(math.radians(pos_ra)))))+160
					x1_l= x1_l+abs(math.cos(math.radians(pos_ra))*130)

				y1_l = y1_l-abs(math.sin(math.radians(pos_ra))*130)			  #Orientation of line

				line = can_scan_1.create_line(pos,y0_l,x1_l,y1_l,fill=color_line)   #Draw a line on canvas
				point_scan = can_scan_1.create_oval((pos+3),y0,(pos-3),y1,fill=color_oval,outline=color_oval) #Draw a arc on canvas

				can_tex_11=can_scan_1.create_text((27,178),text='%sm'%round((x_range/4),2),fill='#aeea00')	 #Create a text on canvas
				can_tex_12=can_scan_1.create_text((27,116),text='%sm'%round((x_range/2),2),fill='#aeea00')	 #Create a text on canvas
				can_tex_13=can_scan_1.create_text((27,54),text='%sm'%round((x_range*0.75),2),fill='#aeea00')  #Create a text on canvas
		except:
			pass


def connection_thread():
	global funcMode, Switch_3, Switch_2, Switch_1, SmoothMode
	while 1:
		car_info = (tcpClicSock.recv(BUFSIZ)).decode()
		print(car_info)
		if not car_info:
			continue

		elif 'V:' in car_info:
			#try:
			car_info = car_info[2:]
			print(car_info)
			vot=float(car_info)
			if vot > 6.8:
				l_ip.config(text='Battery:%.2f'%vot)
			else:
				l_ip.config(text='Battery State Low:%.2f'%vot)

			try:
				canvas_battery.delete(canvas_rec)
			except:
				pass

			if vot >= 7.3:
				canvas_rec=canvas_battery.create_rectangle(0,0,int((vot-6.7)/1.7*132),30,fill = '#558B2F',width=0)
			elif vot < 7.3:
				canvas_rec=canvas_battery.create_rectangle(0,0,int((vot-6.7)/1.7*132),30,fill = '#FF8F00',width=0)
			else:
				pass

		elif 'U:' in car_info:
			new_number2view(car_info)


		else:
			pass


def socket_connect():	 #Call this function to connect with the server
	global ADDR,tcpClicSock,BUFSIZ,ip_stu,ipaddr
	ip_adr=E1.get()	   #Get the IP address from Entry

	if ip_adr == '':	  #If no input IP address in Entry,import a default IP
		ip_adr=num_import('IP:')
		l_ip_4.config(text='Connecting')
		l_ip_4.config(bg='#FF8F00')
		l_ip_5.config(text='Default:%s'%ip_adr)
		pass
	
	SERVER_IP = ip_adr
	SERVER_PORT = 333   #Define port serial 
	BUFSIZ = 1024		 #Define buffer size
	ADDR = (SERVER_IP, SERVER_PORT)
	tcpClicSock = socket(AF_INET, SOCK_STREAM) #Set connection value for socket

	for i in range (1,6): #Try 5 times if disconnected
		#try:
		if ip_stu == 1:
			print("Connecting to server @ %s:%d..." %(SERVER_IP, SERVER_PORT))
			print("Connecting")
			tcpClicSock.connect(ADDR)		#Connection with the server
		
			print("Connected")
		
			l_ip_5.config(text='IP:%s'%ip_adr)
			l_ip_4.config(text='Connected')
			l_ip_4.config(bg='#558B2F')

			replace_num('IP:',ip_adr)
			E1.config(state='disabled')	  #Disable the Entry
			Btn14.config(state='disabled')   #Disable the Entry
			
			ip_stu=0						 #'0' means connected

			connection_threading=thread.Thread(target=connection_thread)		 #Define a thread for FPV and OpenCV
			connection_threading.setDaemon(True)							 #'True' means it is a front thread,it would close when the mainloop() closes
			connection_threading.start()									 #Thread starts

			break
		else:
			print("Cannot connecting to server,try it latter!")
			l_ip_4.config(text='Try %d/5 time(s)'%i)
			l_ip_4.config(bg='#EF6C00')
			print('Try %d/5 time(s)'%i)
			ip_stu=1
			time.sleep(1)
			continue

	if ip_stu == 1:
		l_ip_4.config(text='Disconnected')
		l_ip_4.config(bg='#F44336')


def connect(event):	   #Call this function to connect with the server
	if ip_stu == 1:
		sc=thread.Thread(target=socket_connect) #Define a thread for connection
		sc.setDaemon(True)					  #'True' means it is a front thread,it would close when the mainloop() closes
		sc.start()							  #Thread starts


def connect_click():	   #Call this function to connect with the server
	if ip_stu == 1:
		sc=thread.Thread(target=socket_connect) #Define a thread for connection
		sc.setDaemon(True)					  #'True' means it is a front thread,it would close when the mainloop() closes
		sc.start()							  #Thread starts


def set_R(event):
	time.sleep(0.03)
	tcpClicSock.send(('wsR %s'%var_R.get()).encode())


def set_G(event):
	time.sleep(0.03)
	tcpClicSock.send(('wsG %s'%var_G.get()).encode())


def set_B(event):
	time.sleep(0.03)
	tcpClicSock.send(('wsB %s'%var_B.get()).encode())


def loop():					  #GUI
	global color_can,color_line,target_color,color_oval,tcpClicSock,root,E1,connect,l_ip,l_ip_4,l_ip_5,color_btn,color_text,Btn14,CPU_TEP_lab,CPU_USE_lab,RAM_lab,canvas_ultra,color_text,var_R,var_B,var_G,Btn_Steady,Btn_FindColor,Btn_WatchDog,Btn_Fun4,Btn_Fun5,Btn_Fun6,Btn_Switch_1,Btn_Switch_2,Btn_Switch_3,Btn_Smooth,canvas_battery   #The value of tcpClicSock changes in the function loop(),would also changes in global so the other functions could use it.
	while True:
		color_bg='#000000'		#Set background color
		color_text='#E1F5FE'	  #Set text color
		color_btn='#0277BD'	   #Set button color
		color_line='#01579B'	  #Set line color
		color_can='#212121'	   #Set canvas color
		color_oval='#2196F3'	  #Set oval color
		target_color='#FF6D00'

		root = tk.Tk()			#Define a window named root
		root.title('Adeept Arduino Robot')	  #Main window title
		root.geometry('380x580')  #Main window size, middle of the English letter x.
		root.config(bg=color_bg)  #Set the background color of root window

		try:
			logo =tk.PhotoImage(file = 'logo.png')		 #Define the picture of logo,but only supports '.png' and '.gif'
			l_logo=tk.Label(root,image = logo,bg=color_bg) #Set a label to show the logo picture
			l_logo.place(x=30,y=13)						#Place the Label in a right position
		except:
			pass

		canvas_battery=tk.Canvas(root,bg=color_btn,height=23,width=132,highlightthickness=0)
		canvas_battery.place(x=30,y=145)

		l_ip=tk.Label(root,width=18,text='Status',fg=color_text,bg=color_btn)
		l_ip.place(x=30,y=110)						   #Define a Label and put it in position

		l_ip_4=tk.Label(root,width=18,text='Disconnected',fg=color_text,bg='#F44336')
		l_ip_4.place(x=218,y=110)						 #Define a Label and put it in position

		l_ip_5=tk.Label(root,width=18,text='Use default IP',fg=color_text,bg=color_btn)
		l_ip_5.place(x=218,y=145)						 #Define a Label and put it in position

		E1 = tk.Entry(root,show=None,width=16,bg="#37474F",fg='#eceff1')
		E1.place(x=148,y=40)							 #Define a Entry and put it in position

		l_ip_3=tk.Label(root,width=10,text='IP Address:',fg=color_text,bg='#000000')
		l_ip_3.place(x=143,y=15)						 #Define a Label and put it in position

		Btn_Steady = tk.Button(root, width=8, text='Steady',fg=color_text,bg=color_btn,relief='ridge')
		Btn_Steady.place(x=284,y=195)
		Btn_Steady.bind('<ButtonPress-1>', call_steady)
		root.bind('<KeyPress-f>', call_steady)

		Btn_Ultra = tk.Button(root, width=8, text='Ultrasonic',fg=color_text,bg=color_btn,relief='ridge')
		Btn_Ultra.place(x=284,y=230)
		Btn_Ultra.bind('<ButtonPress-1>', call_ultra)
		root.bind('<KeyPress-x>', call_ultra)

		Btn0 = tk.Button(root, width=8, text='Forward',fg=color_text,bg=color_btn,relief='ridge')
		Btn1 = tk.Button(root, width=8, text='Backward',fg=color_text,bg=color_btn,relief='ridge')
		Btn2 = tk.Button(root, width=8, text='Left',fg=color_text,bg=color_btn,relief='ridge')
		Btn3 = tk.Button(root, width=8, text='Right',fg=color_text,bg=color_btn,relief='ridge')

		Btn_LeftSide = tk.Button(root, width=8, text='Auto',fg=color_text,bg=color_btn,relief='ridge')
		Btn_LeftSide.place(x=30,y=195)
		Btn_LeftSide.bind('<ButtonPress-1>', call_auto)

		Btn_RightSide = tk.Button(root, width=8, text='Attack',fg=color_text,bg=color_btn,relief='ridge')
		Btn_RightSide.place(x=170,y=195)
		Btn_RightSide.bind('<ButtonPress-1>', call_attack)

		Btn0.place(x=100,y=195)
		Btn1.place(x=100,y=230)
		Btn2.place(x=30,y=230)
		Btn3.place(x=170,y=230)

		Btn0.bind('<ButtonPress-1>', call_forward)
		Btn1.bind('<ButtonPress-1>', call_back)
		Btn2.bind('<ButtonPress-1>', call_left)
		Btn3.bind('<ButtonPress-1>', call_right)

		Btn0.bind('<ButtonRelease-1>', call_stop)
		Btn1.bind('<ButtonRelease-1>', call_stop)
		Btn2.bind('<ButtonRelease-1>', call_stop)
		Btn3.bind('<ButtonRelease-1>', call_stop)

		root.bind('<KeyPress-w>', call_forward) 
		root.bind('<KeyPress-a>', call_left)
		root.bind('<KeyPress-d>', call_right)
		root.bind('<KeyPress-s>', call_back)

		root.bind('<KeyPress-q>', call_auto)
		root.bind('<KeyPress-e>', call_attack)

		root.bind('<KeyRelease-w>', call_stop)
		root.bind('<KeyRelease-a>', call_stop)
		root.bind('<KeyRelease-d>', call_stop)
		root.bind('<KeyRelease-s>', call_stop)

		Btn14= tk.Button(root, width=8,height=2, text='Connect',fg=color_text,bg=color_btn,command=connect_click,relief='ridge')
		Btn14.place(x=283,y=15)						  #Define a Button and put it in position

		root.bind('<Return>', connect)

		var_R = tk.StringVar()
		var_R.set(0)

		can_scan = tk.Canvas(root,bg=color_can,height=250,width=320,highlightthickness=0) #define a canvas
		can_scan.place(x=30,y=300) #Place the canvas
		line = can_scan.create_line(0,62,320,62,fill='darkgray')   #Draw a line on canvas
		line = can_scan.create_line(0,124,320,124,fill='darkgray') #Draw a line on canvas
		line = can_scan.create_line(0,186,320,186,fill='darkgray') #Draw a line on canvas
		line = can_scan.create_line(160,0,160,250,fill='darkgray') #Draw a line on canvas
		line = can_scan.create_line(80,0,80,250,fill='darkgray')   #Draw a line on canvas
		line = can_scan.create_line(240,0,240,250,fill='darkgray') #Draw a line on canvas

		can_tex_11=can_scan.create_text((27,178),text='%sm'%round((x_range/4),2),fill='#aeea00')	 #Create a text on canvas
		can_tex_12=can_scan.create_text((27,116),text='%sm'%round((x_range/2),2),fill='#aeea00')	 #Create a text on canvas
		can_tex_13=can_scan.create_text((27,54),text='%sm'%round((x_range*0.75),2),fill='#aeea00')  #Create a text on canvas

		global stat
		if stat==0:			  # Ensure the mainloop runs only once
			root.mainloop()  # Run the mainloop()
			stat=1		   # Change the value to '1' so the mainloop() would not run again.


if __name__ == '__main__':
	try:
		loop()				   # Load GUI
	except:
		tcpClicSock.close()		  # Close socket or it may not connect with the server again
		pass
