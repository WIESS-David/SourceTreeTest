#define _PIC_B_0                                _PIC_START_ADDR + 0x00000000 

import click
import re



@click.command()
@click.option('--infile', default='fs_data.c', help='input fs data')
@click.option('--outfile', default='output.h', help='output header')
def main(infile, outfile):
    content = open(infile).read()
    pat = re.compile(r'{"([a-z_A-Z0-9\.]*).*?,\s*([0-9]*),')
    matches = re.findall(pat, content)
    total = 0
    lines = ''
    for m in matches:
        name = m[0].upper()
        total += int(m[1])
        line = '#define\t%s\t\t _PIC_START_ADDR + 0x%X\n' 
        lines += line % (name, total)
    print(lines)

    with open(outfile, 'w') as f:
        f.write(lines) 



if __name__ == '__main__':
    main()