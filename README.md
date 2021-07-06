# Centering

## 1. Purpose

The centers of images of every fisheye cameras are a little different. To automatically find 
the location of the image center. we put a translucent tube in front of the camera to producy 
roughly a white circle.

![alt text](screen1.png "screen1")

Our experiment has been run on Raspberry Pi. The resolution of the input fisheye image is 
2592x1944, the marked green point is the result center. 

![alt text](screen2.png "screen2")


Here are our procedures,

1.1. Make upper and lower borders

1.2. Turn to grayscale image

1.3. Adjust brightness and contrast

1.4. Gaussian Blur

1.5. Apply the Hough Transform to find the circle





