#include <stdio.h>
#include <stdlib.h>
#include "hw2_file_ops.h"
#include "hw2_math_ops.h"


extern Camera cameras[100];
extern int numberOfCameras;

extern Model models[1000];
extern int numberOfModels;

extern Color colors[100000];
extern int numberOfColors;

extern Translation translations[1000];
extern int numberOfTranslations;

extern Rotation rotations[1000];
extern int numberOfRotations;

extern Scaling scalings[1000];
extern int numberOfScalings;

extern Vec3 vertices[100000];
extern int numberOfVertices;

extern Color backgroundColor;
extern int backfaceCullingSetting;

extern Color **image;

/*
 * Reads camera file.
 */
void readCameraFile(char *camFileName) {
    FILE *fp;
    int i = 0;
    char line[80] = {};
    fp = fopen(camFileName, "r");

    if (fp == NULL) {
        printf("ERROR: Can't open %s\n", camFileName);
        exit(1);
    }

    /* read number of cameras */
    fscanf(fp, "%d", &numberOfCameras);

    for (i = 0; i < numberOfCameras; i++) {
        /* skip line "#Camera n" */
        fscanf(fp, "%s %d", line, &(cameras[i].cameraId));
        fscanf(fp, "%lf %lf %lf", &(cameras[i].pos.x), &(cameras[i].pos.y), &(cameras[i].pos.z));
        fscanf(fp, "%lf %lf %lf", &(cameras[i].gaze.x), &(cameras[i].gaze.y), &(cameras[i].gaze.z));
        fscanf(fp, "%lf %lf %lf", &(cameras[i].v.x), &(cameras[i].v.y), &(cameras[i].v.z));

        cameras[i].gaze = normalizeVec3(cameras[i].gaze);
        cameras[i].u = crossProductVec3(cameras[i].gaze, cameras[i].v);
        cameras[i].u = normalizeVec3(cameras[i].u);

        cameras[i].w.x = -cameras[i].gaze.x;
        cameras[i].w.y = -cameras[i].gaze.y;
        cameras[i].w.z = -cameras[i].gaze.z;
        cameras[i].v = crossProductVec3(cameras[i].u, cameras[i].gaze);
        cameras[i].v = normalizeVec3(cameras[i].v);


        fscanf(fp, "%lf %lf %lf %lf", &(cameras[i].l), &(cameras[i].r), &(cameras[i].b), &(cameras[i].t));
        fscanf(fp, "%lf", &(cameras[i].n));
        fscanf(fp, "%lf", &(cameras[i].f));
        fscanf(fp, "%d %d", &(cameras[i].sizeX), &(cameras[i].sizeY));
        fscanf(fp, "%s", cameras[i].outputFileName);

    }
}

/*
 * Reads scene file.
 */
void readSceneFile(char *sceneFileName) {
    FILE *fp;
    char line[80];
    int i, j;
    char tmp[80];

    fp = fopen(sceneFileName, "r");

    if (fp == NULL) {
        printf("ERROR: Can't open %s\n", sceneFileName);
        exit(1);
    }

    /* read background color */
    fscanf(fp, "%lf %lf %lf", &(backgroundColor.r), &(backgroundColor.g), &(backgroundColor.b));

    /* read backface culling setting, enabled-disabled */
    fscanf(fp, "%d", &(backfaceCullingSetting));

    /* skip line "#Vertices" */
    fscanf(fp, "%s", line);

    /* read number of vertices */
    fscanf(fp, "%d", &numberOfVertices);
    numberOfColors = numberOfVertices;

    /* skip line "#Colors" */
    fscanf(fp, "%s", line);

    for (i = 1; i <= numberOfColors; i++) {
        fscanf(fp, "%lf %lf %lf", &(colors[i].r), &(colors[i].g), &(colors[i].b));
    }

    /* skip line "#Positions" */
    fscanf(fp, "%s", line);

    for (i = 1; i <= numberOfVertices; i++) {
        fscanf(fp, "%lf %lf %lf", &(vertices[i].x), &(vertices[i].y), &(vertices[i].z));
        vertices[i].colorId = i;
    }

    /* skip line "#Translations" */
    fscanf(fp, "%s", line);

    /* read number of translations */
    fscanf(fp, "%d", &numberOfTranslations);

    for (i = 1; i <= numberOfTranslations; i++) {
        fscanf(fp, "%lf %lf %lf", &(translations[i].tx), &(translations[i].ty), &(translations[i].tz));
    }

    /* skip line "#Scalings" */
    fscanf(fp, "%s", line);

    /* read number of scalings */
    fscanf(fp, "%d", &numberOfScalings);

    for (i = 1; i <= numberOfScalings; i++) {
        fscanf(fp, "%lf %lf %lf", &(scalings[i].sx), &(scalings[i].sy), &(scalings[i].sz));
    }

    /* skip line "#Rotations" */
    fscanf(fp, "%s", line);

    /* read number of rotations */
    fscanf(fp, "%d", &numberOfRotations);

    for (i = 1; i <= numberOfRotations; i++) {
        fscanf(fp, "%lf %lf %lf %lf", &(rotations[i].angle), &(rotations[i].ux), &(rotations[i].uy),
               &(rotations[i].uz));
    }

    /* skip line "#Models" */
    fscanf(fp, "%s", line);

    /* read number of models */
    fscanf(fp, "%d", &numberOfModels);

    for (i = 0; i < numberOfModels; i++) {
        /* read model id */
        fscanf(fp, "%d", &(models[i].modelId));

        /* read model type */
        fscanf(fp, "%d", &(models[i].type));

         /* read number of transformations */
        fscanf(fp, "%d", &(models[i].numberOfTransformations));

        for (j = 0; j < models[i].numberOfTransformations; j++) {
            fscanf(fp, "%s", tmp);
            models[i].transformationTypes[j] = tmp[0];
            fscanf(fp, "%d", &(models[i].transformationIDs[j]));
        }

        /* read number of triangles */
        fscanf(fp, "%d", &(models[i].numberOfTriangles));

        for (j = 0; j < models[i].numberOfTriangles; j++) {
            fscanf(fp, "%d %d %d", &(models[i].triangles[j].vertexIds[0]), &(models[i].triangles[j].vertexIds[1]),
                   &(models[i].triangles[j].vertexIds[2]));
        }
    }

    fclose(fp);

}

/*
 * If given value is less than 0, converts value to 0.
 * If given value is more than 255, converts value to 255.
 * Otherwise returns value itself.
 */
int make_between_0_255(double value) {
    if (value >= 255.0) return 255;
    if (value <= 0.0) return 0;
    return (int) (value);
}

/*
 * Writes contents of image (Color**) into a PPM file.
 */
void writeImageToPPMFile(Camera camera) {
    FILE *outputFile;
    int i, j;

    outputFile = fopen(camera.outputFileName, "w");

    fprintf(outputFile, "P3\n");
    fprintf(outputFile, "# %s\n", camera.outputFileName);

    fprintf(outputFile, "%d %d\n", camera.sizeX, camera.sizeY);

    fprintf(outputFile, "255\n");

    for (j = camera.sizeY - 1; j >= 0; j--) {
        for (i = 0; i < camera.sizeX; i++) {
            fprintf(outputFile, "%d %d %d ", make_between_0_255(image[i][j].r), make_between_0_255(image[i][j].g), make_between_0_255(image[i][j].b));
        }
        fprintf(outputFile, "\n");
    }
    fclose(outputFile);
}


/*
	Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
	os_type == 1 		-> Ubuntu
	os_type == 2 		-> Windows
	os_type == other	-> No conversion
*/
void convertPPMToPNG(char *ppmFileName, int os_type) {
    char command[100];

    // call command on Ubuntu
    if (os_type == 1) {
        sprintf(command, "convert %s %s.png", ppmFileName, ppmFileName);
        system(command);
    }

        // call command on Windows
    else if (os_type == 2) {
        sprintf(command, "magick convert %s %s.png", ppmFileName, ppmFileName);
        system(command);
    }

        // default action - don't do conversion
    else {

    }
}
