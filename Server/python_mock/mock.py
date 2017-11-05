import json
import socket
import sys
import math
import time
import random

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import matplotlib.colors

import graphics as g


def construct_packet(data):
    raw_data = json.dumps(data)
    to_send = len(raw_data).to_bytes(4, sys.byteorder) + bytes(raw_data, encoding='ASCII')
    return to_send

# class Environment:
#     def __init__(self, res=100):
#         self.res = res
#         self.map = []
#         for i in range(res):
#             row = [255 * (j / res) for j in range(res)]
#             self.map.append(row)

#     def get_reading(self, location):
#         x_idx = int(location[0] * self.res)
#         y_idx = int(location[1] * self.res)
#         return self.map[y_idx][x_idx]

class Robot:
    # boundary = amt +/- in x & y to deviate from origin
    def __init__(self, boundary_len):
        self.pos = np.array([0, 0], dtype='float')
        self.boundary_len = boundary_len
        self.direction = np.array([1, 0], dtype='float')
        self.deviate_order = 4
        self.speed = 0.03
    
    def simulate(self):
        # normalized coordinates (+/- 1)
        norm_coords = self.pos / self.boundary_len
        # map the [-1, 1] plane to the unit circle
        theta = math.atan2(norm_coords[1], norm_coords[0])
        horiz_dist = float('inf') if math.sin(theta) == 0 else 1 / math.sin(theta)
        vert_dist = float('inf') if math.cos(theta) == 0 else 1 / math.cos(theta)
        scale_factor = min(abs(vert_dist), abs(horiz_dist) )
        remapped_norm = norm_coords / scale_factor
        remapped_coords = remapped_norm * self.boundary_len

        # probability is parabola
        prob_turn_back = (remapped_coords[0]**self.deviate_order + remapped_coords[1]**self.deviate_order) / 2
        interp_factor = 0.1
        if random.random() < prob_turn_back:
            # Need to turn back towards center
            new_vec = -self.pos    
        else:
            # choose random direction
            rand_angle = (2*random.random() - 1) * math.pi
            new_vec = np.array([math.cos(rand_angle), math.sin(rand_angle)])

        old_angle = math.atan2(self.direction[1], self.direction[0])
        summed_vecs = self.direction + (interp_factor * new_vec)
        self.direction = summed_vecs / np.linalg.norm(summed_vecs)
        new_angle = math.atan2(self.direction[1], self.direction[0])
        angle_change = new_angle - old_angle
        if angle_change > math.pi:
            angle_change -= math.pi * 2
        elif angle_change < -math.pi:
            angle_change += math.pi * 2

        delta_vec = [self.speed * math.cos(angle_change),
                     self.speed * math.sin(angle_change)]
        self.pos += self.direction * self.speed
        return delta_vec, angle_change


class Environment:
    def __init__(self, img_path, res=0.05):
        self.img = mpimg.imread(img_path)
        self.res = res

    def get_reading(self, location):
        x_pixels = location[0] / self.res
        y_pixels = -location[1] / self.res
        # Center of image is (0, 0)
        x_pixels += self.img.shape[1] / 2
        y_pixels += self.img.shape[0] / 2
        return np.copy(self.img[int(y_pixels), int(x_pixels)])

class Drawer:
    def __init__(self, quad, environment, win_real_size=1, win_size=500, pause_on_start=False):
        self.win_size = win_size # Size of square window in
        self.win_real_size = win_real_size # Width of window in meters
        self.quad = quad
        self.map_img = g.Image(g.Point(win_size/2, win_size/2), win_size, win_size)
        # copy image out
        x_scale = environment.img.shape[1] / win_size
        y_scale = environment.img.shape[0] / win_size
        rows = []
        for y in range(win_size):
            pixel_y = int(y * y_scale)
            this_line = '{' + ' '.join([matplotlib.colors.to_hex(environment.img[pixel_y, int(x*x_scale)]) for x in range(win_size)] ) + '}'
            rows.append(this_line)
        self.map_img.setPixels(' '.join(rows))

        self.win = g.GraphWin("Thing", win_size, win_size)
        # self.quad_size = (0.05 / self.win_real_size) * self.win_size
        self.quad_size = 15
        self.markers = [g.Circle(g.Point(0, 0), 0) for i in range(1)]
        # Draw background map
        self.map_img.draw(self.win)
        if pause_on_start:
            self.win.getMouse()

    def draw(self):
        self.win.flush()

    def update_marker(self, i, pos, color):
        self.markers[i].undraw()
        scaled_pos = pos * self.win_size / self.win_real_size
        scaled_pos[1] *= -1 # Flip so +y is up
        scaled_pos += self.win_size / 2 # Center 0 point
        self.markers[i] = g.Circle(g.Point(*scaled_pos), self.quad_size)
        # self.markers[i].setOutline(color)
        self.markers[i].setFill(color)
        self.markers[i].setWidth(3)
        self.markers[i].draw(self.win)

    def update_real(self):
        self.update_marker(0, self.quad.pos, 'yellow')


    def close(self):
        self.win.close()

if __name__ == '__main__':
    networking_enabled = True
    if networking_enabled:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            sock.connect(("127.0.0.1", 8080))
        except:
            print("Failed to connect to socket")
            exit(-1)

    env = Environment("../color_map.png", 0.05)
    robot = Robot(2)

    d = Drawer(robot, env, 4, 200)

    iteration = 1
    t_s = 0.2
    while True:
        # robot_dx = math.sin(iteration * 0.1) * 0.02
        # robot_x += robot_dx
        # robot_x = min(max(robot_x, 0), 1.0)
        movement, rotation = robot.simulate()
        color = env.get_reading(robot.pos)
        color *= 255
        # reflectivity = int(env.get_reading((robot_x, 0.5)))
        # reflectivity = max(1, reflectivity)
        print("pos: " + str(robot.pos) + ", color: " + str(color))
        print("dphi; " + str(rotation))
        # pos_delta = robot.pos - last_pos
        # last_pos = np.copy(robot.pos)
        msg_data = {
            "dx": movement[0],
            "dy": movement[1],
            "dphi": rotation,
            "color": [int(val) for val in color],
            "id": 0,
            "time": iteration * t_s * 1e6
        }

        packet = construct_packet(msg_data)
        if networking_enabled:
            sock.sendall(packet)

        d.update_real()

        iteration += 1
        time.sleep(t_s)
        
