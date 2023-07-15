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

# 5. box filter
boxf = make_box_filter(7)
blur = convolve_image(im, boxf, 1)
save_image(blur, "dog-box7")

# 6. Blurred + Resized
thumb = nn_resize(blur, blur.w//7, blur.h//7)
save_image(thumb, "dogthumb")

#im2 = load_image("data/dog.jpg")
im2 = load_image("testimg1.jpg")
# 7. High-pass, Sharpen & Emboss
highpassf = make_highpass_filter()
highpass = convolve_image(im2, highpassf, 0)
save_image(highpass, "bldg-highpass")

sharpenf = make_sharpen_filter()
sharpen = convolve_image(im2, sharpenf, 1)
save_image(sharpen, "bldg-sharpen")

embossf = make_emboss_filter()
emboss = convolve_image(im2, embossf, 1)
save_image(emboss, "bldg-emboss")

cannyf = make_cannyedge_filter()
canny = convolve_image(im2, cannyf, 1)
save_image(canny, "bldg-canny")

# 8. Gaussian filter
gaussf = make_gaussian_filter(2)
gblur = convolve_image(im2, gaussf, 1)
save_image(gblur, "dog-gauss2")

im3 = load_image("data/dog.jpg")
gblur2 = convolve_image(im3, gaussf, 1)
save_image(gblur2, "dog-gauss2-big")

# 9. Hybrid image
lfreq = convolve_image(im3, gaussf, 1)
hfreq = im3 - lfreq
reconstruct = lfreq + hfreq
save_image(lfreq, "low-frequency")
save_image(hfreq, "high-frequency")
save_image(reconstruct, "reconstruct") 
