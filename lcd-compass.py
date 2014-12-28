#!/usr/bin/env python
# encoding: utf-8

import sys
import struct
import math
import visual

from pyBusPirateLite.I2C import *
from pyBusPirateLite.I2Chigh import *


from font8x8 import font8x8

def bit_slice(arr, bit):
    assert len(arr) == 4
    bit = 7 - bit
    ret = 0
    for v in reversed(arr):
        ret = (ret << 2) | (0b11 * ((v >> bit) & 1))
    return ret

# Oh my god... Python sucks.
def to_16bit_signed(v):
    return struct.unpack_from("h", struct.pack("H", v))[0]

class DogXL:
    def bulk_write(self, data):
        self.i2c.send_start_bit()
	self.i2c.bulk_trans(len(data),data)
	self.i2c.send_stop_bit()

    def power_on(self):
        for c in [ 0xf1, 0x67, 0xc0, 0x40, 0x50, 0x2b, 0xeb, 0x81, 0x5f, 0x89, 0xaf ]:
            self.i2c.command(self.write_cmd_addr, c)

    def set_page(self, page):
        self.i2c.command(self.write_cmd_addr, 0b01100000 | page)

    def set_column(self, column):
        self.i2c.command(self.write_cmd_addr, 0b00000000 | column & 0xF)
        self.i2c.command(self.write_cmd_addr, 0b00010000 | (column >> 4) & 0xF)

    def set_ac(self, val):
        self.i2c.command(self.write_cmd_addr, 0b10001000 | val)
        
    def write_byte(self, byte):
        self.i2c.command(self.write_data_addr, byte)

    def put_char(self, row, col, char):
        off = ord(char) * 8
        char_data = font8x8[off:off+8]

        # Automatically increase columns
        self.set_ac(0b000)

        # Write top half of character
        self.set_column(col * 8)
        self.set_page(row * 2)
        for char_col in range(8):
            self.write_byte(bit_slice(char_data[0:4], char_col))

        # Write bottom half of character
        self.set_column(col * 8)            
        self.set_page(row * 2 + 1)
        for char_col in range(8):
            self.write_byte(bit_slice(char_data[4:8], char_col))

    def put_string(self, row, col, string):
        for i in range(len(string)):
            self.put_char(self, row, col + i, string[i])
            
    def __init__(self, i2c):
        self.i2c = i2c
        self.write_cmd_addr = 0x3c
        self.write_data_addr = 0x3d


class Compass:

    def ident(self):
        return chr(self.i2c.get_byte(self.addr, 10)) + \
               chr(self.i2c.get_byte(self.addr, 11)) + \
               chr(self.i2c.get_byte(self.addr, 12))               
    
    def read(self):
        return (to_16bit_signed(self.i2c.get_word(self.addr, 0x03)),
                to_16bit_signed(self.i2c.get_word(self.addr, 0x05)),
                to_16bit_signed(self.i2c.get_word(self.addr, 0x07)))

    def init(self):
        # Continuous measurements at 10Hz
        self.i2c.set_byte(self.addr, 0x02, 0x00)
        # Updates at 2 Hz
        #self.i2c.set_byte(self.addr, 0x00, 0b00001000)

    def __init__(self, i2c):
        self.i2c = i2c
        self.addr = 0x1e
        
if __name__ == '__main__':
	i2c = I2Chigh("/dev/ttyACM0", 115200)
        try:
            print "Entering binmode: ",
            if i2c.BBmode():
                print "OK."
            else:
		print "failed."
		sys.exit()

            print "Entering raw I2C mode: ",
            if i2c.enter_I2C():
		print "OK."
            else:
		print "failed."
		sys.exit()
		
            if not i2c.cfg_pins(I2CPins.POWER):
		print "Failed to set I2C peripherals."
		sys.exit()

            if not i2c.set_speed(I2CSpeed._400KHZ):
		print "Failed to set I2C Speed."
		sys.exit()

            i2c.timeout(0.2)

            dogxl = DogXL(i2c)
            dogxl.power_on()

            compass = Compass(i2c)
            compass.init()
            print compass.ident()

            visual.arrow(pos = visual.vector(0, 0, 0), axis = visual.vector(512, 0, 0), shaftwidth = 1)
            visual.arrow(pos = visual.vector(0, 0, 0), axis = visual.vector(0, 512, 0), shaftwidth = 1, color = visual.color.blue)
            visual.arrow(pos = visual.vector(0, 0, 0), axis = visual.vector(0, 0, 512), shaftwidth = 1, color = visual.color.red)

            comp_arrow = visual.arrow(pos = visual.vector(0, 0, 0), axis = visual.vector(0, 0, 0))

            while True:
                x, y, z = compass.read()
                comp_arrow.axis = visual.vector(x, 0, z)

                ang = math.atan2(x * 0.92, z * 0.92)
                if ang < 0:
                    ang += 2 * math.pi

                print "X=%.2f Y=%.2f Z=%.2f: %.2f" % (x, y, z, ang * 180 / math.pi)
                i2c.timeout(0.2)

            
            # s = "Ich glaube es geht!"
            # for o in range(len(s)):
            #     dogxl.put_char(2, o, s[o])
        
            i2c.timeout(100)

        finally:
            print "Resetting..."
            i2c.resetBP()

# EOF
