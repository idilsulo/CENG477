#include "hw2_types.h"

#ifndef HW2_FILE_OPS_H
#define HW2_FILE_OPS_H

/*
 * Reads camera file.
 */
void readCameraFile(char *cameraFileName);

/*
 * Reads scene file.
 */
void readSceneFile(char *sceneFileName);

/*
 * If given value is less than 0, converts value to 0.
 * If given value is more than 255, converts value to 255.
 * Otherwise returns value itself.
 */
int make_between_0_255(double value);

/*
 * Writes contents of image (Color**) into a PPM file.
 */
void writeImageToPPMFile(Camera camera);

/*
	Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
	os_type == 1 		-> Ubuntu
	os_type == 2 		-> Windows
	os_type == other	-> No conversion
*/
void convertPPMToPNG(char* ppmFileName, int os_type);

#endif //HW2_FILE_OPS_H
