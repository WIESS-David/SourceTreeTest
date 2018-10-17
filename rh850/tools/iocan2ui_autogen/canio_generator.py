import csv
import jinja2
import os
from jinja2 import Environment, FileSystemLoader

#RX buffer from w_sys_rscan.c
CAN_IDS = [0x287, 0x133, 0x134, 0x53E, 0x53F, 0x434, 0x340, 0x56B]

TEMPLATE_ENVIRONMENT = Environment(
    autoescape=False,
    loader=FileSystemLoader('.'),
    trim_blocks=False)
	
headers = None


def idx(col):
	return headers.index(col)

def process_row(item):
	#TODO
	return item

def read_tsv(filename='CAN_UI_MAPPING_MAG - can_master.tsv'):
	global headers
	
	data = []
	with open(filename, newline='') as f:
		reader = csv.reader(f, delimiter='\t', quoting=csv.QUOTE_NONE)
		for row in reader:
			if not headers:
				headers = row
			else:
				item = {h:row[idx(h)] for h in headers}
				pitem = process_row(item)
				data.append(pitem)
	return data
				

def generate(data):
	context = {'can_size':len(CAN_IDS), 'all_data':data, 'CAN_IDS':map(hex, CAN_IDS)}
	content = TEMPLATE_ENVIRONMENT.get_template('w_canio_template.h').render(context)
	with open(r'C:\git\rh850\src\app\gfx_apps\simple_draw\lib\w_canio_gen.h', 'w') as f:
		f.write(content)
		print('generate \src\app\gfx_apps\simple_draw\lib\w_canio_gen.h')
	content = TEMPLATE_ENVIRONMENT.get_template('w_canio_template.c').render(context)
	with open(r'C:\git\rh850\src\app\gfx_apps\simple_draw\src\w_canio_gen.c', 'w') as f:
		f.write(content)
		print('generate \src\app\gfx_apps\simple_draw\src\w_canio_gen.c')
	
	
def main():
	gen = generate(read_tsv())

if __name__ == '__main__':
	main()