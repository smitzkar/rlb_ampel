# Create / convert pictures to hex for use in display


## easy solution  

* 1. SAVE BITMAP AS 1BIT COLOUR in paint.net
* 2. Run: bmp2hex.py -i -x <BITMAP FILE>
* 3. Copy paste output into sketch.

## without paint.net (not available for macOS...)

hm... don't use this yet. bmp2hex.py creates a much bigger object. need to fiddle with parameters. 

* 1. create png via pixelart creator of choice
* 2. run: python png-to-bmp.py (will tell you what to do)
* 3. run: python bmp2hex.py original.bmp > newHex.txt
* 4. Copy paste output into sketch.


(could rewrite this a little bit to make it really user friendly. maybe later)
