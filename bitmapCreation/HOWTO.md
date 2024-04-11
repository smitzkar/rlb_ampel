# Create / convert pictures to hex for use in display

# update: found this tool: 

https://javl.github.io/image2cpp/

just use this :) 


## easy solution  

* 1. SAVE BITMAP AS 1BIT COLOUR in paint.net
* 2. Run: bmp2hex.py -i <BITMAP FILE>
* 3. Copy paste output into sketch.

## without paint.net (not available for macOS...)

### using Gimp: 
image > mode > indexed : black and white 1 bit, then export as bmp 

### using python script: 
(includes option to rotate 90°)

* 1. create png via pixelart creator of choice
* 2. run: python png-to-bmp.py (will tell you what to do)
* 3. run: python bmp2hex.py original.bmp > newHex.txt
* 4. Copy paste output into sketch.

(could rewrite this a little bit to make it really user friendly. maybe later)
