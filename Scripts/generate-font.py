
from io import BytesIO
from argparse import ArgumentParser

from wand.color import Color
from wand.image import Image
from wand.drawing import Drawing
from wand.font import Font

parser = ArgumentParser()

parser.add_argument('--font-size', type=int)
parser.add_argument('--ttf-file', type=str)

args = parser.parse_args()

font_size = args.font_size
ttf_file = args.ttf_file

glyphs = '''
    abcdefghijklmnopqrstuvwxyz
    ABCDEFGHIJKLMNOPQRSTUVWXYZ
    0123456789
    !@#$%^&*
    ()-_=+
    []{}:;'"
    ,.<>/?|\\
'''

glyphs = ''.join(glyphs.split())

buffer = BytesIO()

font_line_height = 0
font_glyph_width = 0

with Image(width=len(glyphs) * font_size, height=font_size, background=Color('transparent')) as image:
    with Drawing() as drawing:
        drawing.font = ttf_file
        drawing.fill_color = Color('black')

        metrics = drawing.get_font_metrics(image, ' ')
        font_glyph_width = int(metrics.text_width)

        metrics = drawing.get_font_metrics(image, glyphs)
        font_line_height = int(metrics.text_height)
        
        image.resize(len(glyphs) * font_glyph_width, font_line_height)

        x = 0
        for glyph in glyphs:
            # TODO: Remove the need for the + 2
            drawing.text(x, int(metrics.y2) + 2, glyph)
            x += font_glyph_width

        drawing.draw(image)

    image.format = 'BMP'
    image.compression = 'rle'
    image.save(file=buffer)
    # image.save(filename='font.png')

glyphs = glyphs.replace('\\', '\\\\')
glyphs = glyphs.replace('"', '\\"')

data = buffer.getvalue()

data_xxd = ''
for i in range(0, len(data), 32):
    hex = [ f'0x{b:02X}' for b in data[i : i + 32] ]
    data_xxd += '    ' + ', '.join(hex) + ',\n'

with open('Source/Font.hpp', 'wt') as file:
    code = f'''
#ifndef FONT_HPP
#define FONT_HPP

#include <cstdint>

static const int FONT_SIZE = {font_size};

static const int FONT_GLYPH_WIDTH = {font_glyph_width};

static const int FONT_LINE_HEIGHT = {font_line_height};

static const char * FONT_GLYPH_LOOKUP = "{glyphs}";

static const uint8_t FONT_BMP[] = {{
{data_xxd}}};

#endif // FONT_HPP
'''

    file.write(code.strip())