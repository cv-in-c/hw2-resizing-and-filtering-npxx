from uwimg import *

im = load_image("data/dogsmall.jpg")

# 1. nn-resize
a = nn_resize(im, im.w*4, im.h*4)
save_image(a, "dog4x-nn")

# 2. bilinear_resize
b = bilinear_resize(im, im.w*4, im.h*4)
save_image(b, "dog4x-bl")

# 3. nn-resize for 7th of the size
c = nn_resize(im, im.w//7, im.h//7)
save_image(c, "dog7th-nn")

# 4. bilinear_resize for 7th of the size
d = bilinear_resize(im, im.w//7, im.h//7)
save_image(d, "dog7th-bl")