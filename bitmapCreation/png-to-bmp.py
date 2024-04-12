
# Annoyingly on Mac python is kind of weird to use without a virtual environment.
# -> use conda (don't forget to deactivate after :) )


# /* Alternatively (from urbanCompass):
#  *
#  * 1. SAVE BITMAP AS 1BIT COLOUR in paint.net
#  * 2. Run: bmp2hex.py -i <BITMAP FILE> // don't use -x as written in urbanCompass readme
#  * 3. Copy paste output into sketch.
#  * 

import os
from PIL import Image

# Get user input for filenames
png_filename_input = input("Enter the name of your png: ")
bmp_filename_input = input("Enter the name of the new bmp: ")
to_be_rotated = input("Do you want to rotate the image 90°? (y/n): ")

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
if to_be_rotated == 'y':
    image = image.rotate(90, expand=True)
image = image.convert('L')  # Convert image to grayscale
image = image.point(lambda x: 0 if x<128 else 255, '1')  # Apply threshold and convert to 1-bit bitmap

# Save it as a BMP
image.save(bmp_filename, format="BMP")

print("PNG converted to 1-bit BMP successfully!")