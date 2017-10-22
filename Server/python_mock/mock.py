import json
import socket
import sys
import math
import time


def construct_packet(data):
    raw_data = json.dumps(data)
    to_send = len(raw_data).to_bytes(4, sys.byteorder) + bytes(raw_data, encoding='ASCII')
    return to_send

class Environment:
    def __init__(self, res=100):
        self.res = res
        self.map = []
        for i in range(res):
            row = [255 * (j / res) for j in range(res)]
            self.map.append(row)

    def get_reading(self, location):
        x_idx = int(location[0] * self.res)
        y_idx = int(location[1] * self.res)
        return self.map[x_idx][y_idx]


if __name__ == '__main__':
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        sock.connect(("127.0.0.1", 8080))
    except:
        print("Failed to connect to socket")
        exit(-1)

    env = Environment()
    iteration = 0
    t_s = 0.5
    robot_x = 0.5
    while True:
        robot_dx = math.sin(iteration * 0.1) * 0.01
        robot_x += robot_dx
        reflectivity = int(env.get_reading((robot_x, 0.5)))
        print("x: " + str(robot_x) + ", refl: " + str(reflectivity))
        msg_data = {
            "dx": robot_dx,
            "dy": 0,
            "dphi": 0,
            "color": [reflectivity, reflectivity, reflectivity],
            "id": 0,
            "time": iteration * t_s * 1e6
        }

        packet = construct_packet(msg_data)
        sock.sendall(packet)

        iteration += 1
        time.sleep(t_s)
        
