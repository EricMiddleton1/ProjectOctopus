import json
import socket
import sys


def construct_packet(data):
    raw_data = json.dumps(data)
    print("Going to send " + str(len(raw_data)))
    to_send = len(raw_data).to_bytes(4, sys.byteorder) + bytes(raw_data, encoding='ASCII')
    return to_send

if __name__ == '__main__':
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        sock.connect(("127.0.0.1", 8080))
    except:
        print("Failed to connect to socket")
        exit(-1)
    test_data = {
        "thing": 123
    }
    packet = construct_packet(test_data)
    sock.sendall(packet)
