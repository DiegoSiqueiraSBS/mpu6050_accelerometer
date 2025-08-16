pkg load instrument-control

s = serialport("COM4", 115200); % Replace with your port and baud rate

data = read(s, 500); % Reads 10 bytes from the serial port
