import socket

HOST = "192.168.1.42"  # din Pi IP
PORT = 5000

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect((HOST, PORT))

data = client.recv(1024)
print(data.decode())

client.close()