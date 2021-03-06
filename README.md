Set (cards game) Solver
=======================
This is a Set game soler using OpenCV.

Intro
-----
This code is a result of me paying set, staring at the 12 cards and not finding any set, Thinking are there really no sets on the table or it is me that is really bad.
So I have decided to create an app that will solve the problem for me.

* At the moment I need to do a lot of code cleanup.

Games Rules
-----------
A set deck has 81 cards, varying in four features:

* number (one, two, or three)
* symbol (diamond, squiggle, oval)
* shading (solid, striped, or open)
* color (red, green, or purple).

A set consists of three cards which in each feature is either the same or different among the three cards

Algorithm
=========

Locating Cards
--------------
### Assumptions
1. Image contains 12 cards on a dark background.

### Flow
1. Convert RGB image to grayscale.
2. Threshold grayscale image to binary image.
3. Find all contours in binary image.
4. Find 12 contours that are about the same size and their size is greater than `X%` of the image.
5. Cut each contour and send it to card analysis.


Card Analysis
-------------
### Assumptions
1. Image contains only one card.
2. The card background is all most white.

### Preprossesing
1.Convert RGB image to grayscale.
2. Threshold grayscale image to binary image.
3. Find all contours in binary image.

### Number
1. The Number is determined by the number of contours.

### Color
1. Convert RGB image to HSV.
2. Mask the HSV image with a shape contours, this way we don't count background pixels.
3. The card color is determined by the number of pixels in each color range (red, green, or purple).

### Symbol
1. Crop one contour.
2. Calculate [Hu invariant moments][1] for the contour - rotation and scale invariant
3. The symbol is determined by Hu moment $\phi_1$

$$
\mu_{p,q}=\int_{-\infty}^{\infty}\int_{-\infty}^{\infty}(x-x_c)^p(y-y_c)^qI(x,y)dxdy
$$

$$
\phi_1 = \mu_{2,0}+\mu_{0,2}
$$
Where $x_c$ and $y_c$ are the contour center of mass $x_c=\frac{\mu_{1,0}}{\mu_{0,0}}$, $y_c=\frac{\mu_{0,1}}{\mu_{0,0}}$

Hu moment $ \phi_1 $ is analogous to the moment of inertia around the image's centroid, this tell us how much the shape is spread.

### Shading
Finding the shading has proved to be the most difficult task.

1. Crop one contour.
2. The Shading is determined by the ratio of pixels to the area of a full contour.
    * solid - high ratio
    * striped - mid
    * open - very low ratio

Another option:

1. Run an edge detection on the binary image
2. calculate the ratio
    * solid - only one line - very low ratio
    * striped - high ratio
    * open - 2 lines, inner and outer, low ratio

Finding Sets
------------
Now when we found all of features for each cards we can look for sets.
One way to do it is brute force (I'm lazy so for the moment I'm brute forcing)


Remarks
=======
An Android app is on the way.

Links
=====
[1]: http://en.wikipedia.org/wiki/Image_moment
