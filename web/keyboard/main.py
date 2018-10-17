import threading
import time
import serial

from flask import Flask, render_template, request
from flask_socketio import SocketIO

DEBUG_MODE = True
DISPLAY_CLEAR_CHAR = '\xa0'

clients = []
current_display = DISPLAY_CLEAR_CHAR
com_status = 0
app = Flask(__name__)
app.debug = DEBUG_MODE
socketio = SocketIO(app, manage_session=False)


def send_message(client_id, data):
	socketio.emit('output', data, room=client_id)
	if DEBUG_MODE:
		print('sending message "{}" to client "{}".'.format(data, client_id))


def send_com_status(client_id, com_status):
	socketio.emit('com_status', com_status, room=client_id)
	if DEBUG_MODE:
		print('sending com status "{}" to client "{}".'.format(com_status, client_id))


@socketio.on('connect')
def handle_connect():
	clients.append(request.sid)

	if DEBUG_MODE:
		print('Client connected: ' + request.sid)
	send_message(request.sid, current_display)
	send_com_status(request.sid, com_status)


@socketio.on('disconnect')
def handle_disconnect():
	clients.remove(request.sid)

	if DEBUG_MODE:
		print('Client connected: ' + request.sid)


@socketio.on('display')
def handle_display(key):
	key = str(key)
	print('Key:' + key)
	if key in ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '*', '#', DISPLAY_CLEAR_CHAR]:
		global current_display
		current_display = key

		for user in clients:
			send_message(user, current_display)

		if DEBUG_MODE:
			print('Received key: ' + key + '\nBy: ' + request.sid)
	else:
		if DEBUG_MODE:
			print('ERROR Recived wrong key: ' + key + '\nBy: ' + request.sid)


@app.route('/')
def index():
	return render_template('index.html')


def com_was_enabled():
	global com_status
	com_status = 1
	for user in clients:

		send_com_status(user, com_status)

		if DEBUG_MODE:
			print('COM was enabled')


def com_was_disabled():
	global com_status
	com_status = 0
	for user in clients:
		send_com_status(user, com_status)

		if DEBUG_MODE:
			print('COM was disabled')

is_open = False
def print_isopen(ser):
	global is_open;
	if(is_open != ser.is_open):
		is_open=ser.is_open
		print(is_open)	
			
			
def com_worker():
	ser = serial.Serial()
	ser.port = 'COM4'
	while True:
		try:
			print_isopen(ser);
			#ser = serial.Serial('COM4', 9600, timeout=0)
			#ser.close()
			if  not ser.is_open:
				ser.close()
				ser.open()
			if(not com_status):
				com_was_enabled()			
			if current_display != DISPLAY_CLEAR_CHAR:
				ser.write(str.encode(current_display))
			else:
				ser.write(str.encode(' '))
			
			time.sleep(0.2)
		except serial.serialutil.SerialException as e:
			ser.close()
			com_was_disabled()
			if com_status == 1:
				com_was_disabled()
			if DEBUG_MODE:
				print('COM ERROR %s' % e)
			time.sleep(5)
			

if __name__ == "__main__":
	com_thread = threading.Thread(target=com_worker)
	com_thread.start()
	socketio.run(app, debug=True, host='0.0.0.0')
	