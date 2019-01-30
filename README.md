# CENG477

Repository that contains all of the course related work (i.e. programming assignments) that are done through the **CENG477 - Introduction to Computer Graphics** course.

## [Homework 1](/Homework1)

The aim of this assignment to implement a basic ray tracer which simulates the propagation of the light in the real world.

## [Homework 2](/Homework2)
The aim of this asignment is to implement Modeling Transformation, Viewing Transformation,
and Rasterization stages of the Forward Rendering Pipeline.
 
* At the first stage, the modeling transformations had been implemented which constructed the tiny little pixels in the image given below which represents the vertices:
![Empty Box Modeling Transformations](images/empty_box_modeling_transformations.jpeg)

* Then, we switched to the implementation of the Rasterization stage. The Midpoint algorithm had been implemented for this purpose. This algorithm is used to draw the lines which combines the vertices that are shown in the image above. 

The results of the algorithm for the empty box scene is given below:

![Empty Box Midpoint](images/empty_box_midpoint.png)

Also, the image below represents the results of the algorithm for the horse and mug scene:

![Horse and Mug Midpoint](images/horse_and_mug_midpoint.png)

* A sample for the final version of the output images can be found below:

![Horse and Mug](images/horse_and_mug.png)

## [Homework 3](/Homework3)

The aim of this assignment is to implement an OpenGL program to render a given scene with the help of the vertex and fragment shaders in order to display a texture image as a height map to fly through the scene in an interactive manner by processing the keyboard input.
