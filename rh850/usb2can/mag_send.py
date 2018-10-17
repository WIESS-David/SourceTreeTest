#pip install -r requirements.txt --upgrade

import keyboard as kb
import usb_can
import time
import datetime
import math
import time
import threading
import click
import sys
import pandas as pd

millis = lambda: int(round(time.time() * 1000))
send_count = 0
recorded_key_strokes = []
replay_key_strokes = []
last_key_ts = millis()
can_data = []
exit_flag = 0
looping = 1
sent_messages = []

def add_key(key):
    global last_key_ts
    now = millis()
    recorded_key_strokes.append((now - last_key_ts, key))
    last_key_ts = now

def save_key_csv(filename):
    df = pd.DataFrame(recorded_key_strokes, columns=['TIMESTAMP', 'KEYSTROKE'])
    df.to_csv(filename, index=False)

def read_key_csv(filename):
    global replay_key_strokes
    print('read_key_csv')
    df = pd.read_csv(filename, header=None)
    replay_key_strokes = map(list, df.values)
    return replay_key_strokes

def set_bit(byte, bit):
    return byte | (1<<bit)

def clear_bit(byte, bit):
    return byte & ~(1<<bit)

def read_bits(byte, bit, width=1):
    mask = sum([2**(i) for i in range(width)]) << bit
    return byte & mask

def set_bits(byte, bit, value, width=1): 
    mask = sum([2**(i) for i in range(width)]) << bit
    byte = byte & (~mask)
    return byte | (value << bit)

def set_byte(byte ,value):
    set_bits(byte, 0, value, width=8)

def to_hex(data):
    return ' '.join(['%02X' % d for d in data])

def save_xml(filename):
    lines = ''
    line = """<TaskObj Frames="1" Interval="0" Times="1" IdIncrease="0" DataIncrease="0" SendType="0" RemoteFlag="0" ExternFlag="0" 
    ID="%s" DataLen="8" Data="%s"/>
"""
    for s in sent_messages:
        lines += line % (s[1], to_hex(s[2]) )
    f = open(filename, 'w')
    f.write("""<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE xml>
<xml version="1.0">""")
    f.write(lines)
    f.write('</xml>')


class CAN_msg():
    def __init__(self, id, period=20):
        self.id = id
        self.data = [0]*8
        self.period = period
        self.last_send_ts = millis()

    def set_bits(self, index, bit, value, width=1):
        self.data[index] = set_bits(self.data[index], bit, value, width)

    def set_byte(self, index, value):
        assert(self.data[index]!=None)
        self.data[index] = set_bits(self.data[index], 0, value, width=8)

    def hex(self):
        return ' '.join(['%02X' % d for d in self.data])
        
    def send(self):
        global send_count
        if (millis() - self.last_send_ts) > self.period or self.last_send_ts is 0:
             sent_messages.append([millis(), self.id, self.data])
             usb_can.send(int(self.id, 0), self.data)
             self.last_send_ts = millis()
             #print('sent', send_count, self)
             send_count += 1
             return True

        return False

    def __str__(self):
        return self.id + ' ' + ' '.join(map(str, self.data))

class VCU():
    def __init__(self):
        ids = ['0x287', '0x133', '0x134', '0x53E', '0x53F', '0x56B', '0x434', '0x340']
        period = [100]*8
        self.messages = { id:CAN_msg(id, p) for id, p in zip(ids, period)}
        #print(self.messages)
        self.count = 0
        self.enter_park_mode()

    def enter_park_mode(self):
        print('enter_park_mode')
        # shifter P
        msg = self.messages['0x134']
        msg.set_bits(1, 0, 1, 3)
        # door close
        msg = self.messages['0x434']
        msg.set_bits(0, 0, 0, 3)
        # key
        msg = self.messages['0x434']
        msg.set_bits(7, 5, 1, 1)
        # belt
        msg = self.messages['0x434']
        msg.set_bits(3, 0, 7, 2)

        self.attributes = {}
        #TODO check battery_low

    def get_mode(self, item):
        return self.attributes.get(item, '')

    def set_mode(self, item, mode):
        print('set_mode: ', item, mode)
        self.attributes[item] = mode
        if item == 'init':
            self.enter_park_mode()
        elif item == 'shifter':
            idx = ['r', 'n', 'd', 'p'].index(mode)
            msg = self.messages['0x134']
            msg.set_bits(1, 0, idx+1, 3)
        elif item == 'car_mode':
            #idx = ['turtle', 'ECO', 'Normal', 'Sport', 'Ludicrous', 'Insane'].index(mode)
            idx = ['eco', 'normal', 'sport'].index(mode)
            msg = self.messages['0x134']
            msg.set_bits(1, 4, idx+1, 3)
            print(msg.hex())
        elif item == 'cruise':
            idx = ['off', 'fixed', 'adaptive'].index(mode)
            msg = self.messages['0x134']
            msg.set_bits(2, 0, idx, 2)
        elif item == 'accel_pedal':
            msg = self.messages['0x133']
            msg.set_bits(2, 0, int(mode), 8)
            msg.set_bits(3, 0, 0, 8) #regen map
        elif item == 'regen_map':
            msg = self.messages['0x133']
            msg.set_bits(3, 0, int(mode), 8)
            msg.set_bits(2, 0, 0, 8) #acc pedal
        elif item == 'battery':
            msg = self.messages['0x53F']
            msg.set_byte(0, int(mode)&0xFF)
            msg.set_byte(1, (int(mode)&0xFF00) >> 8)
        elif item == 'service_timing':
            msg = self.messages['0x53E']
            msg.set_byte(5, int(mode)&0xFF)
            msg.set_byte(6, (int(mode)&0xFF00) >> 8)
            msg.set_byte(7, (int(mode)&0xFF0000) >> 16)
        elif item == 'ready_to_drive':
            msg = self.messages['0x53E']
            value = 1 if mode == 'on' else 0
            msg.set_bits(7, 0, value, 1)
        elif item == 'smart_key': 
            msg = self.messages['0x434']
            value = 1 if mode == 'on' else 0
            msg.set_bits(7, 5, value, 1)
        elif item == 'belt':
            msg = self.messages['0x434']
            msg.set_bits(3, 0, int(mode), 2)
        #TODO: eps, esp, airbag
        elif item == 'low_voltage':
            msg = self.messages['0x134']
            value = 1 if mode == 'on' else 0
            msg.set_bits(6, 0, value, 1)
        elif item == 'coolant_temp':
            msg = self.messages['0x133']
            value = 1 if mode == 'on' else 0
            msg.set_bits(6, 6, value, 1)
        elif item == 'high_voltage':
            msg = self.messages['0x133']
            value = 1 if mode == 'on' else 0
            msg.set_bits(6, 2, value, 1)
        elif item == 'drive_system':
            msg = self.messages['0x133']
            value = 1 if mode == 'on' else 0
            msg.set_bits(6, 0, value, 1)
        elif item == 'transmission':
            msg = self.messages['0x133']
            value = 1 if mode == 'on' else 0
            msg.set_bits(6, 5, value, 1)
        elif item == 'charging_system':
            msg = self.messages['0x133']
            value = 1 if mode == 'on' else 0
            msg.set_bits(6, 7, value, 1)
        elif item == 'dc_dc':
            msg = self.messages['0x134']
            value = 1 if mode == 'on' else 0
            msg.set_bits(7, 1, value, 1)
        elif item == 'tire_pressure':
            msg = self.messages['0x340']
            value = 1 if mode == 'on' else 0
            msg.set_bits(7, 0, value, 1)
        elif item == 'lamp_malfunction': 
            msg = self.messages['0x434']
            value = 1 if mode == 'on' else 0
            msg.set_bits(7, 0, value, 1)
        elif item == 'acc_warning':
            msg = self.messages['0x133']
            value = 1 if mode == 'on' else 0
            msg.set_bits(6, 1, value, 1)
        elif item == 'acc_on':
            msg = self.messages['0x134']
            value = 1 if mode == 'on' else 0
            msg.set_bits(2, 1, value, 2)
        elif item == 'door_open':
            msg = self.messages['0x434']
            msg.set_bits(0, 0, int(mode), 5)
        elif item == 'front_trunk_open':
            msg = self.messages['0x434']
            value = 1 if mode == 'on' else 0
            msg.set_bits(0, 5, value, 1)
        elif item == 'tailgate_open':
            msg = self.messages['0x434']
            value = 1 if mode == 'on' else 0
            msg.set_bits(0, 4, value, 1)
        elif item == 'brake_warning':
            msg = self.messages['0x133']
            value = 1 if mode == 'on' else 0
            msg.set_bits(6, 4, value, 1)
        elif item == 'parking_brake':
            msg = self.messages['0x134']
            value = 1 if mode == 'on' else 0
            msg.set_bits(6, 1, value, 1)
        elif item == 'menu_key': #TODO CAN 287
            msg = self.messages['0x287']
            msg.set_byte(0, 87)
            if mode == 'none':  
                msg.set_bits(6, 0, 0, 3)
            if mode == 'up':  
                msg.set_bits(6, 0, 1, 3)
            if mode == 'down':  
                msg.set_bits(6, 0, 2, 3)
            if mode == 'left':  
                msg.set_bits(6, 0, 3, 3)
            if mode == 'right':  
                msg.set_bits(6, 0, 4, 3)
        elif item == 'turn': #TODO CAN 287
            msg = self.messages['0x287']
            if mode == 'left':  
                msg.set_bits(7, 0, 1, 2)
            if mode == 'right':  
                msg.set_bits(7, 0, 2, 2)
            if mode == 'none':  
                msg.set_bits(7, 0, 0, 2)
        elif item == 'tail_lamp': #TODO CAN
            msg = self.messages['0x287']
            value = 1 if mode == 'on' else 0
            msg.set_bits(7, 2, value, 1)
        elif item == 'head_lamp_auto': #TODO CAN
            msg = self.messages['0x434']
            value = 0
            if mode == 'on':  value = 1
            msg.set_bits(2, 7, value, 1)
        elif item == 'head_lamp_high_beam': #TODO CAN
            msg = self.messages['0x287']
            value = 1 if mode == 'on' else 0
            msg.set_bits(7, 4, value, 1)
        elif item == 'head_lamp_high_low': #TODO CAN
            msg = self.messages['0x287']
            value = 1 if mode == 'on' else 0
            msg.set_bits(7, 5, value, 1)
        elif item == 'wiper': 
            msg = self.messages['0x434']
            msg.set_bits(4, 0, int(mode), 5)
        elif item == 'wiper_warning': 
            msg = self.messages['0x434']
            value = 1 if mode == 'on' else 0
            msg.set_bits(7, 1, value, 1)
        elif item == 'front_fog': #TODO CAN
            msg = self.messages['0x287']
            value = 1 if mode == 'on' else 0
            msg.set_bits(7, 6, value, 1)
        elif item == 'rear_fog': #TODO CAN
            msg = self.messages['0x287']
            value = 1 if mode == 'on' else 0
            msg.set_bits(7, 7, value, 1)
        elif item == 'buzzer': #for buzz test
            if mode == 'on':
                usb_can.send(0x287, [87, 12, 3, 4, 0, 0, 0, 1], dlc=8)
                time.sleep(0.5)
            else: 
                usb_can.send(0x287, [87, 12, 3, 4, 0, 0, 0, 0], dlc=8)
                time.sleep(0.5)
        elif item == 'speed':
            speed = int(mode)
            msg = self.messages['0x133']
            msg.set_byte(0, speed)

    def populate_data(self):
        self.count += 1
        #datetime
        n = datetime.datetime.now()
        msg = self.messages['0x56B']
        msg.set_bits(5, 0, n.year & 0xFF, 8)
        msg.set_bits(6, 0, (n.year & 0xFF00) >> 8, 8)
        msg.set_bits(4, 0, n.month, 8)
        msg.set_bits(3, 0, n.day, 8)
        second = n.hour*3600 +  n.minute*60 +  n.second
        msg.set_bits(0, 0, second & 0xFF, 8)
        msg.set_bits(1, 0, (second & 0xFF00) >> 8, 8)
        msg.set_bits(2, 0, (second & 0xFF0000) >> 16, 8)
        #temp
        outside_temp = 10 + self.count / 100.0
        msg = self.messages['0x340']
        msg.set_bits(2, 0, int(outside_temp + 80), 8)
        
    def step(self):
        self.populate_data()
        if any([c.send() for c in self.messages.values()]):
            pass


def next_mode(modes, mode):
    if not modes: 
        modes = ['on', 'off']
    
    if mode:
        nxt = (modes.index(mode) + 1) % len(modes)
        #print('next', nxt)
        return modes[nxt]
    else:
        return modes[0]

def shift_mode(attr, modes):
    #print('shift mode', attr, modes)
    m = next_mode(modes, vcu.get_mode(attr))
    vcu.set_mode(attr, m)

vcu = VCU()
usb_can.open()

conditions = [["`", 'init', ['on']],
    ['1', 'shifter', ['r', 'n', 'd', 'p']],
    ['2', 'car_mode', ['eco', 'normal', 'sport']],
    ['3', 'ready_to_drive', ['on', 'off']],
    ['4', 'speed', ['0', '10', '20', '40', '80', '100', '125', '200']],
    ['5', 'accel_pedal', ['10', '20', '30', '40', '50', '60', '70', '80', '90', '100', '0']],
    ['6', 'regen_map', ['10', '20', '30', '40', '50', '60', '70', '80', '90', '100', '0']],
    ['7', 'door_open', ['1', '2', '4', '8', '16', '0']],
    ['8', 'menu_key', ['none', 'up', 'down', 'left', 'right']],
    ['9', 'charging', ['charging_10', 'charging_20', 'no_charging']],
    ['0', 'wiper', ['0', '1', '2', '4', '8', '64', '128']],
    
    ['q', 'cruise', ['off', 'fixed']],
    ['w', 'belt', ['0', '1', '7']],
    ['e', 'low_voltage', ['on', 'off']],
    ['r', 'coolant_temp', ['on', 'off']],
    ['t', 'high_voltage', ['on', 'off']],
    ['y', 'drive_system', ['on', 'off']],
    ['u', 'transmission', ['on', 'off']],
    ['o', 'tire_pressure', ['on', 'off']],
    ['p', 'lamp_malfunction', ['on', 'off']],
    ['a', 'head_lamp_auto', ['on', 'off']],
    ['s', 'brake_warning', ['on', 'off']], 
    ['d', 'parking_brake', ['on', 'off']],
    ['f', 'wiper_warning', ['on', 'off']],
    ['g', 'acc_warning', ['on', 'off']],
    ['h', 'cruise', ['off', 'fixed', 'adaptive']],
    
    #['j', 'battery', ['0', '100', '200', '300', '400', '500', '600']], #TODO
    #['k', 'buzzer',  ['on',  'off']],
    #['l', 'service_timing', ['0', '5000', '10000', '20000']], #TODO
    #[';', 'dc_dc', ['on', 'off']], #TODO
    #['z', 'charging_system', ['on', 'off']],
    #['x', 'smart_key', ['on', 'off']], #TODO
    #['c', 'head_lamp_high_beam', ['on', 'off']],
    #['v', 'head_lamp_high_low', ['on', 'off']],
    #['b', 'tail_lamp', ['on', 'off']], #GPIO
    #['n', 'acc_on', ['on', 'off']], #GPIO
    #['m', 'front_trunk_open', ['on', 'off']], # #46 issue
    #[',', 'tail_trunk_open', ['on', 'off']], #47 issue
    
    #['.', 'turn', ['left', 'right', 'none']], #GPIO
    #['.', 'front_fog', ['on', 'off']], #GPIO
    #['.', 'rear_fog', ['on', 'off']] #GPIO
    #['.', 'eps', ['on', 'off']],
    #['.', 'esc', ['on', 'off']],
    #['.', 'airbag', ['on', 'off']],
]

def auto_play_key_strokes():
    print('auto_play_key_strokes...')
    for i in range(looping):
        for ts, r_key in replay_key_strokes:
            if r_key.find('KEY') >=0: 
                continue

            time.sleep(float(ts)/1000)
            for key, attr, modes in conditions:
                if key ==  r_key:
                    shift_mode(attr, modes)
    exit_flag = True
        
    
@click.command()
@click.option('--indata', default='', help='output csv files.')
@click.option('--inkey', default='', help='output key files.')
@click.option('--outxml', default='', help='output xml files.')
@click.option('--outkey', default='', help='output key files.')
@click.option('--loop', default='1', help='output key files.')
def main(indata, inkey, outkey, loop, outxml):
    global looping
    last_key = None
    key_count = 0

    if loop:
        looping = int(loop)
    if inkey: 
        replay_key_strokes = read_key_csv(inkey)
        task = threading.Thread(target = auto_play_key_strokes)
        task.start()

    while True:
        for key, attr, modes in conditions:
                if kb.is_pressed(key):
                    if last_key == key and key_count < 10:
                        key_count += 1
                    else:
                        print('got key', key)
                        add_key(key)
                        key_count = 0
                        last_key = key
                        shift_mode(attr, modes)

        if kb.is_pressed('+'): 
            # for i in range(8):
            #     send_count += 1
            #     print('send 0x287')
            #     usb_can.send(0x287, [87, 12, 3, 4, 0, 0, 0, 0], dlc=8)
            #     print('send 0x286')
            #     usb_can.send(0x286, [80, 0, 0, 0, 0, 0, 0, 0], dlc=8)
            break
        if kb.is_pressed(' '): 
            break

        vcu.step()
        time.sleep(0.01)
        
        if exit_flag:
            print('exit..')
            break 

    usb_can.close()
    if outkey:
        save_key_csv(outkey)
    if outxml:
        save_xml(outxml)

if __name__ == '__main__':
    main()
		