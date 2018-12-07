#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "hw2_types.h"
#include "hw2_math_ops.h"
#include "hw2_file_ops.h"

Camera cameras[100];
int numberOfCameras = 0;

Model models[1000];
int numberOfModels = 0;

Color colors[100000];
int numberOfColors = 0;

Translation translations[1000];
int numberOfTranslations = 0;

Rotation rotations[1000];
int numberOfRotations = 0;

Scaling scalings[1000];
int numberOfScalings = 0;

Vec3 vertices[100000];
int numberOfVertices = 0;

Color backgroundColor;

// backface culling setting, default disabled
int backfaceCullingSetting = 0;

Color **image;

/*
	Initializes image with background color
*/
void initializeImage(Camera cam) {
    int i, j;

    for (i = 0; i < cam.sizeX; i++)
        for (j = 0; j < cam.sizeY; j++) {
            image[i][j].r = backgroundColor.r;
            image[i][j].g = backgroundColor.g;
            image[i][j].b = backgroundColor.b;

        }
}

/*
	Transformations, culling, rasterization are done here.
	You can define helper functions inside this file (rasterizer.c) only.
	Using types in "hw2_types.h" and functions in "hw2_math_ops.c" will speed you up while working.
*/
void forwardRenderingPipeline(Camera cam) {
    // TODO: IMPLEMENT HERE
}


int main(int argc, char **argv) {
    int i, j;

    if (argc < 2) {
        printf("Usage: ./rasterizer <scene file> <camera file>\n");
        return 1;
    }

    // read camera and scene files
    readSceneFile(argv[1]);
    readCameraFile(argv[2]);

    image = 0;

    for (i = 0; i < numberOfCameras; i++) {

        // allocate memory for image
        if (image) free(image);

        image = (Color **) malloc(sizeof(Color *) * (cameras[i].sizeX));

        if (image == NULL) {
            printf("ERROR: Cannot allocate memory for image.");
            exit(1);
        }

        for (j = 0; j < cameras[i].sizeX; j++) {
            image[j] = (Color *) malloc(sizeof(Color) * (cameras[i].sizeY));
            if (image[j] == NULL) {
                printf("ERROR: Cannot allocate memory for image.");
                exit(1);
            }
        }


        // initialize image with basic values
        initializeImage(cameras[i]);

        // do forward rendering pipeline operations
        forwardRenderingPipeline(cameras[i]);

        // generate PPM file
        writeImageToPPMFile(cameras[i]);

        // Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
        // Notice that os_type is not given as 1 (Ubuntu) or 2 (Windows), below call doesn't do conversion.
        // Change os_type to 1 or 2, after being sure that you have ImageMagick installed.
        convertPPMToPNG(cameras[i].outputFileName, 99);
    }

    return 0;

}
