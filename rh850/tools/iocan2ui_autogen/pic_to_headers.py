import os, io
from PIL import Image, ImageOps
import struct
import math
import bincopy
from logo_rle import gen_rle
from jinja2 import Environment, FileSystemLoader

TEMPLATE_ENVIRONMENT = Environment(
    autoescape=False,
    loader=FileSystemLoader('.'),
    trim_blocks=False)
	
	

bmp_dir = '../../src/app/image_bin/'
rle_dir = bmp_dir + 'rle/'
bin_dir = bmp_dir + 'bin/'

def get_bmp_files():
	files = os.listdir(bmp_dir)
	return  [f for f in files if f.endswith('bmp')]
	
def get_rle_files():
	files = os.listdir(rle_dir)
	return  [f for f in files if f.endswith('bmp')]
		
def get_rs_bin_data(file):
	return open(bin_dir + file + '.bin', 'rb').read()
	

def get_bmp_data(bmp_file, directory=bmp_dir, outfile=None):
	img = Image.open(directory + bmp_file)
	mirror_img = ImageOps.mirror(img)
	bmp_width, bmp_height = mirror_img.size
	imgByteArr = io.BytesIO()
	mirror_img.save(imgByteArr, format='BMP')
	if outfile:
		mirror_img.save(directory + outfile, format='BMP')
	data = imgByteArr.getvalue()
	bmp_bits = None
	
	return data, bmp_width, bmp_height, bmp_bits

	offset = 0
	bmp_width = 0
	bmp_height = 0
	bmp_bits = 0
	data_length = 0
	data = None
	with open(directory + bmp_file, 'rb') as bmp:
		type = bmp.read(2).decode()
		size = struct.unpack('I', bmp.read(4))
		#print('Type:', type)
		#print('Size:', size )
		struct.unpack('H', bmp.read(2)) #reserved
		struct.unpack('H', bmp.read(2)) #reserved
		offset = struct.unpack('I', bmp.read(4))
		struct.unpack('I', bmp.read(4)) #DIB header
		bmp_width = struct.unpack('I', bmp.read(4))
		#print('Width:', bmp_width)
		bmp_height = struct.unpack('I', bmp.read(4))
		#print('Height:', bmp_height)
		struct.unpack('H', bmp.read(2)) #colour planes
		bmp_bits = struct.unpack('H', bmp.read(2))
		#print('Bits per Pixel: %s' % bmp_bits)
		struct.unpack('I', bmp.read(4)) # Compression
		data_length = struct.unpack('I', bmp.read(4))
		print('Raw Image Size: %s' % data_length )
		assert(data_length[0] % 4 == 0)

	with open(directory + bmp_file, 'rb') as f:
		data = f.read()
		data = data[offset[0]:]
	#print(len(data), data_length[0])
	assert(len(data) == data_length[0])
	return data, bmp_width, bmp_height, bmp_bits
		
def gen_headers(pics):
	context = {'pic_size':len(pics), 'pics':pics}
	content = TEMPLATE_ENVIRONMENT.get_template('templates/w_canio_pics_template.h').render(context)
	with open(r'C:\git\rh850\src\app\gfx_apps\simple_draw\lib\w_canio_pics_gen.h', 'w') as f:
		f.write(content)
		print('generated \src\app\gfx_apps\simple_draw\lib\w_canio_pics_gen.h')
	
	content = TEMPLATE_ENVIRONMENT.get_template('templates/w_canio_pics_template.c').render(context)
	with open(r'C:\git\rh850\src\app\gfx_apps\simple_draw\src\w_canio_pics_gen.c', 'w') as f:
		f.write(content)
		print('generated \src\app\gfx_apps\simple_draw\src\w_canio_pics_gen.c')
	
	with open('w_pics_old.h', 'w') as f:
		f.write('\n'.join(['#define\t_PIC_%s\t\t_PIC_BASE + 0x%0X'% (p[0].upper(), p[4]) for p in pics]))
		
	
ALIGN = 512
def gen_bincopy(files, rle_files):
	binfile = bincopy.BinFile()
	address = 0
	binfile.add_binary([i for i in range(256)], address)
	address = ALIGN
	pics = []
	for f in files: #bmp
		fname = f.split('.')[0]
		data, w, h, d = get_bmp_data(f)
		#if fname == 'eric_test': 
		#	open(fname + '.bin', 'wb').write(data) #TODO: bin need to be SAME as RGBench
		data = get_rs_bin_data(fname)
		data_len = len(data)
		print(fname, '='*30)
		print(fname, 'len', data_len, 'address', hex(address), 'block', address/ALIGN)
		pics.append([fname.upper(), w, h, data_len, address])
		binfile.add_binary(data, address)
		address += math.ceil(data_len / ALIGN) * ALIGN
		
	for f in rle_files:
		fname = f.split('.')[0]
		if fname.endswith('_w'): continue
		
		fout = fname + '.img'
		_, w, h, d = get_bmp_data(f, directory=rle_dir)
		gen_rle(rle_dir + f, rle_dir + fout)
		data = open(rle_dir + fout, 'rb').read()
		data = data[512:] #skip header
		data_len = len(data)
		print(fname, '='*30)
		print(fname, 'len', data_len, 'address', hex(address), 'block', address/ALIGN)
		pics.append([fname.upper(), w, h, data_len, address])
		binfile.add_binary(data, address)
		address += math.ceil(data_len / ALIGN) * ALIGN
		
	gen_headers(pics)
	print('generating images_flash.srec...')
	with open('images_flash.srec', 'w') as f:
		f.write(binfile.as_srec())
		
	print('generating image_flash_dump.txt...')
	with open('image_flash_dump.txt', 'w') as f:
		f.write(binfile.as_hexdump())
	
#gen_bincopy(['eric_test.bmp'],[])		
gen_bincopy(get_bmp_files(), get_rle_files())