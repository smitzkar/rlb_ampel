
# Annoyingly on Mac python is kind of weird to use without a virtual environment.
# -> use conda (don't forget to deactivate after :) )


# /* Alternatively (from urbanCompass):
#  *
#  * 1. SAVE BITMAP AS 1BIT COLOUR in paint.net
#  * 2. Run: bmp2hex.py -i -x <BITMAP FILE>
#  * 3. Copy paste output into sketch.
#  * 

import os
from PIL import Image

# Get user input for filenames
png_filename_input = input("Enter the name of your png: ")
bmp_filename_input = input("Enter the name of the new bmp: ")

# Remove file extension if present
png_filename = os.path.splitext(png_filename_input)[0] + '.png'
bmp_filename = os.path.splitext(bmp_filename_input)[0] + '.bmp'

# If no path is given, assume the file is in the current directory
if not os.path.dirname(png_filename):
    png_filename = os.path.join(os.getcwd(), png_filename)
if not os.path.dirname(bmp_filename):
    bmp_filename = os.path.join(os.getcwd(), bmp_filename)

# Open the image file
image = Image.open(png_filename)
# Save it as a BMP
image.save(bmp_filename, format="BMP")

print("PNG converted to BMP successfully!")