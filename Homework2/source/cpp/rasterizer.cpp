#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include "hw2_types.h"
#include "hw2_math_ops.h"
#include "hw2_file_ops.h"
#include <iostream>
#include <vector>

using namespace std;


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


/*********************************** OUR OWN HELPER FUNCTIONS AND STRUCTS ***********************************************/


typedef struct{
    double vertices[3][3];
    Color colors[3];
} Triangle_transformed;

typedef struct{
    vector<Triangle_transformed> transformed_triangles;
    int modelId;
    int numberOfTriangles;
} Model_transformed;


vector<Model_transformed> transformed_models;

double M_cam[4][4];
double M_per[4][4];
double M_vp[3][4];




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

void deepcopy(double M_copied[4][4], double M_original[4][4]){
    for(int i=0; i < 4; i++){
        for(int j=0; j < 4; j++){
            M_copied[i][j] = M_original[i][j];
        }
    }
}

void deepcopyVec4(double M_copied[4], double M_original[4]){
    for(int i=0; i < 4; i++){
        M_copied[i] = M_original[i];
    }
}
/*
	Transformations, culling, rasterization are done here.
	You can define helper functions inside this file (rasterizer.cpp) only.
	Using types in "hw2_types.h" and functions in "hw2_math_ops.cpp" will speed you up while working.
*/

void camera_transformations(Camera& cam, double M_cam[4][4]){

    M_cam[0][0] = cam.u.x;
    M_cam[0][1] = cam.u.y;
    M_cam[0][2] = cam.u.z;
    M_cam[0][3] = -1*(cam.u.x*cam.pos.x + cam.u.y*cam.pos.y + cam.u.z*cam.pos.z);

    M_cam[1][0] = cam.v.x;
    M_cam[1][1] = cam.v.y;
    M_cam[1][2] = cam.v.z;
    M_cam[1][3] = -1*(cam.v.x*cam.pos.x + cam.v.y*cam.pos.y + cam.v.z*cam.pos.z);

    M_cam[2][0] = cam.w.x;
    M_cam[2][1] = cam.w.y;
    M_cam[2][2] = cam.w.z;
    M_cam[2][3] = -1*(cam.w.x*cam.pos.x + cam.w.y*cam.pos.y + cam.w.z*cam.pos.z);

    M_cam[3][0] = 0.0;
    M_cam[3][1] = 0.0;
    M_cam[3][2] = 0.0;
    M_cam[3][3] = 1.0;

    //cout << "#######################################" << endl;
    //cout << cam.pos.x << "  " << cam.pos.y << "  " << cam.pos.z << endl;
    //cout << cam.gaze.x << "  " << cam.gaze.y << "  " << cam.gaze.z << endl;
    //cout << "#######################################" << endl;

}

void perspective_projection(Camera& cam, double M_per[4][4]){

    M_per[0][0] = (2.0)*cam.n/(cam.r - cam.l);
    M_per[0][1] = 0.0;
    M_per[0][2] = (cam.r + cam.l)/(cam.r - cam.l);
    M_per[0][3] = 0.0;

    M_per[1][0] = 0.0;
    M_per[1][1] = (2.0)*cam.n / (cam.t - cam.b);
    M_per[1][2] = (cam.t + cam.b)/(cam.t - cam.b);
    M_per[1][3] = 0.0;

    M_per[2][0] = 0.0;
    M_per[2][1] = 0.0;
    M_per[2][2] = (-1.0)*(cam.f + cam.n) / (cam.f - cam.n);
    M_per[2][3] = (-2.0)*cam.f*cam.n / (cam.f - cam.n);

    M_per[3][0] = 0.0;
    M_per[3][1] = 0.0;
    M_per[3][2] = -1.0;
    M_per[3][3] = 0.0;



}

void viewport_transformation(Camera& cam, double M_vp[3][4]){

    /* Camera sizeX = n_x
       Camera sizeY = n_y
       It represents the resolution */
    M_vp[0][0] = cam.sizeX/2.0;
    M_vp[0][1] = 0.0;
    M_vp[0][2] = 0.0;
    M_vp[0][3] = (cam.sizeX-1.0)/2.0;

    M_vp[1][0] = 0.0;
    M_vp[1][1] = cam.sizeY/2.0;
    M_vp[1][2] = 0.0;
    M_vp[1][3] = (cam.sizeY-1.0)/2.0;

    M_vp[2][0] = 0.0;
    M_vp[2][1] = 0.0;
    M_vp[2][2] = 0.5;
    M_vp[2][3] = 0.5;
}

void scaling(Scaling scaling, double scaling_matrix[4][4]){

    /* Homogenous coordinates used */

    scaling_matrix[0][0] = scaling.sx;
    scaling_matrix[0][1] = 0.0;
    scaling_matrix[0][2] = 0.0;
    scaling_matrix[0][3] = 0.0;

    scaling_matrix[1][0] = 0.0;
    scaling_matrix[1][1] = scaling.sy;
    scaling_matrix[1][2] = 0.0;
    scaling_matrix[1][3] = 0.0;

    scaling_matrix[2][0] = 0.0;
    scaling_matrix[2][1] = 0.0;
    scaling_matrix[2][2] = scaling.sz;
    scaling_matrix[2][3] = 0.0;

    scaling_matrix[3][0] = 0.0;
    scaling_matrix[3][1] = 0.0;
    scaling_matrix[3][2] = 0.0;
    scaling_matrix[3][3] = 1.0;

}

void translation(Translation translation, double translation_matrix[4][4]){

    /* Homogenous coordinates used */

    translation_matrix[0][0] = 1.0;
    translation_matrix[0][1] = 0.0;
    translation_matrix[0][2] = 0.0;
    translation_matrix[0][3] = translation.tx;

    translation_matrix[1][0] = 0.0;
    translation_matrix[1][1] = 1.0;
    translation_matrix[1][2] = 0.0;
    translation_matrix[1][3] = translation.ty;

    translation_matrix[2][0] = 0.0;
    translation_matrix[2][1] = 0.0;
    translation_matrix[2][2] = 1.0;
    translation_matrix[2][3] = translation.tz;

    translation_matrix[3][0] = 0.0;
    translation_matrix[3][1] = 0.0;
    translation_matrix[3][2] = 0.0;
    translation_matrix[3][3] = 1.0;

}

void rotation(Rotation rotation, double rotation_matrix[4][4], double tx, double ty, double tz){

    /* Alternative method*/

    /* Important: Rotation axis may not be given as a unit vector */
    //double rotation_axis_length = sqrt(rotation.ux*rotation.ux + rotation.uy*rotation.uy + rotation.uz*rotation.uz);

    Vec3 u;
    u.x = rotation.ux;
    u.y = rotation.uy;
    u.z = rotation.uz;

    double x = normalizeVec3(u).x;
    double y = normalizeVec3(u).y;
    double z = normalizeVec3(u).z;

    u.x = x;
    u.y = y;
    u.z = z;

    /* Step 1: To find v, set the smallest component of u (in an
       absolute sense) to zero and swap the other two while
       negating one. */

     Vec3 v;

     double min_element = u.x;
     char element_index = 'x';
     if(abs(u.y) < abs(min_element)){
         min_element = u.y;
         element_index = 'y';
     }
     if(abs(u.z) < abs(min_element)){
         min_element = u.z;
         element_index = 'z';
     }

     if(element_index == 'x'){
         v.x = 0.0;
         v.y = (-1.0)*u.z;
         v.z = u.y;
     }
     else if(element_index = 'y'){
         v.x = (-1.0)*u.z;
         v.y = 0.0;
         v.z = u.x;
     }
     else{
         v.x = (-1.0)*u.y;
         v.y = u.x;
         v.z = 0.0;
     }


     x = normalizeVec3(v).x;
     y = normalizeVec3(v).y;
     z = normalizeVec3(v).z;

     v.x = x;
     v.y = y;
     v.z = z;

     /* Note: Cross product of two unit vectors is also a unit vector.
        No need to normalize further */

     Vec3 w= crossProductVec3(u, v);

     /* Now rotate uvw such that it aligns with xyz: call this
        transform M
        Rotate around x (u is now x)
        Undo the initial rotation: call this M-1
        Finding M-1 (rotating xyz to uvw) is trivial */

    double M[4][4];
    double M_inverse[4][4];

    M_inverse[0][0] = u.x;
    M_inverse[0][1] = v.x;
    M_inverse[0][2] = w.x;
    M_inverse[0][3] = 0.0;

    M_inverse[1][0] = u.y;
    M_inverse[1][1] = v.y;
    M_inverse[1][2] = w.y;
    M_inverse[1][3] = 0.0;

    M_inverse[2][0] = u.z;
    M_inverse[2][1] = v.z;
    M_inverse[2][2] = w.z;
    M_inverse[2][3] = 0.0;

    M_inverse[3][0] = 0.0;
    M_inverse[3][1] = 0.0;
    M_inverse[3][2] = 0.0;
    M_inverse[3][3] = 1.0;



    M[0][0] = u.x;
    M[0][1] = u.y;
    M[0][2] = u.z;
    M[0][3] = 0.0;

    M[1][0] = v.x;
    M[1][1] = v.y;
    M[1][2] = v.z;
    M[1][3] = 0.0;

    M[2][0] = w.x;
    M[2][1] = w.y;
    M[2][2] = w.z;
    M[2][3] = 0.0;

    M[3][0] = 0.0;
    M[3][1] = 0.0;
    M[3][2] = 0.0;
    M[3][3] = 1.0;


    double theta = (rotation.angle*M_PI)/180.0;
    double Rx_theta[4][4];

    Rx_theta[0][0] = 1.0;
    Rx_theta[0][1] = 0.0;
    Rx_theta[0][2] = 0.0;
    Rx_theta[0][3] = 0.0;

    Rx_theta[1][0] = 0.0;
    Rx_theta[1][1] = cos(theta);
    Rx_theta[1][2] = (-1.0)*sin(theta);
    Rx_theta[1][3] = 0.0;

    Rx_theta[2][0] = 0.0;
    Rx_theta[2][1] = sin(theta);
    Rx_theta[2][2] = cos(theta);
    Rx_theta[2][3] = 0.0;

    Rx_theta[3][0] = 0.0;
    Rx_theta[3][1] = 0.0;
    Rx_theta[3][2] = 0.0;
    Rx_theta[3][3] = 1.0;


    double Rx_times_M[4][4];
    //multiplyMatrixWithMatrix(rotation_matrix, M, T);
    multiplyMatrixWithMatrix(Rx_times_M, Rx_theta, M);
    multiplyMatrixWithMatrix(rotation_matrix, M_inverse, Rx_times_M);
    //multiplyMatrixWithMatrix(rotation_matrix, T_inverse, rotation_matrix);
}


void modeling_transformations(Camera& cam, double M_model[4][4], Model& model){


        /* Initializing the M_model matrix as an identity matrix
           Therefore, we can do the first multiplication without encounteringany problems */
        makeIdentityMatrix(M_model);

        int _numberOfTransformations = model.numberOfTransformations;
        for(int j=0; j < _numberOfTransformations; j++){
            char transformation_type = model.transformationTypes[j];
            int transformation_id = model.transformationIDs[j];

            if(transformation_type == 't'){
                double translation_matrix[4][4];
                double M_temp[4][4];
                translation(translations[transformation_id], translation_matrix);
                multiplyMatrixWithMatrix(M_temp, translation_matrix, M_model);
                deepcopy(M_model, M_temp);

            }
            else if(transformation_type == 's'){
                double scaling_matrix[4][4];
                double M_temp[4][4];
                scaling(scalings[transformation_id], scaling_matrix);
                multiplyMatrixWithMatrix(M_temp, scaling_matrix, M_model);
                deepcopy(M_model, M_temp);

            }
            else if(transformation_type == 'r'){
                double rotation_matrix[4][4];
                double M_temp[4][4];
                rotation(rotations[transformation_id], rotation_matrix, cam.pos.x, cam.pos.y, cam.pos.z);
                multiplyMatrixWithMatrix(M_temp, rotation_matrix, M_model);
                deepcopy(M_model, M_temp);
            }

        }

}

void multiply_3x4_MatrixWithVec4d(double r[3], double m[3][4], double v[4]) {
    int i, j;
    double total;
    for (i = 0; i < 3; i++) {
        total = 0;
        for (j = 0; j < 4; j++)
            total += m[i][j] * v[j];
        r[i] = total;
    }
}

void transformations_stage(Camera& cam){

    for(int i=0; i < numberOfModels; i++){

        Model_transformed transformed_model;
        transformed_model.modelId = models[i].modelId;
        transformed_model.numberOfTriangles = models[i].numberOfTriangles;


        double M_model[4][4];
        modeling_transformations(cam, M_model, models[i]);
        double M_cam[4][4];
        camera_transformations(cam, M_cam);
        double M_per[4][4];
        perspective_projection(cam, M_per);
        double M_vp[3][4];
        viewport_transformation(cam, M_vp);

        double M_temp[4][4];
        double M_result[4][4];
        multiplyMatrixWithMatrix(M_temp, M_cam, M_model);
        multiplyMatrixWithMatrix(M_result, M_per, M_temp);
        /*
        cout << M_result[0][0] << "  " <<  M_result[0][1]<< "  " << M_result[0][2] << "  " << M_result[0][3] << endl;
        cout << M_result[1][0] << "  " <<  M_result[1][1]<< "  " << M_result[1][2] << "  " << M_result[1][3] << endl;
        cout << M_result[2][0] << "  " <<  M_result[2][1]<< "  " << M_result[2][2] << "  " << M_result[2][3] << endl;
        cout << M_result[3][0] << "  " <<  M_result[3][1]<< "  " << M_result[3][2] << "  " << M_result[3][3] << endl;
        */
        int _numberOfTriangles = models[i].numberOfTriangles;
        for(int j=0; j < _numberOfTriangles; j++){

            Triangle_transformed transformed_triangle;

            for(int k=0; k < 3; k++){
                double Vec4d[4];
                Vec4d[0] = vertices[models[i].triangles[j].vertexIds[k]].x;
                Vec4d[1] = vertices[models[i].triangles[j].vertexIds[k]].y;
                Vec4d[2] = vertices[models[i].triangles[j].vertexIds[k]].z;
                Vec4d[3] = 1.0;

                // DO NOT OPEN
                // GIVES NAN
                //cout << "Vec4d2: " << Vec4d[2] << endl;
                double Vec4d_temp[4];
                multiplyMatrixWithVec4d(Vec4d_temp, M_result, Vec4d);
                deepcopyVec4(Vec4d, Vec4d_temp);

                /*cout << Vec4d[0] << " " << Vec4d[1] << " " << Vec4d[2] << "  " << Vec4d[3] << endl;
                cout << Vec4d_temp[0] << " " << Vec4d_temp[1] << " " << Vec4d_temp[2] << "  " << Vec4d_temp[3] << endl;
                cout << "##############" << endl;
                */
                Vec4d[0] /= Vec4d[3];
                Vec4d[1] /= Vec4d[3];
                Vec4d[2] /= Vec4d[3];
                Vec4d[3] /= Vec4d[3];

                //cout << Vec4d[0] << " " << Vec4d[1] << " " << Vec4d[2] << "  " << Vec4d[3] << endl;

                /*
                cout <<  "Vec4d1: " << Vec4d[0] << endl;
                cout <<  "Vec4d2: " << Vec4d[1] << endl;
                cout <<  "Vec4d3: " << Vec4d[2] << endl;
                */
                double Vec3d[3];
                multiply_3x4_MatrixWithVec4d(Vec3d, M_vp, Vec4d);


                transformed_triangle.vertices[k][0] = (int)round(Vec3d[0]);
                transformed_triangle.vertices[k][1] = (int)round(Vec3d[1]);
                transformed_triangle.vertices[k][2] = (int)round(Vec3d[2]);

                cout << transformed_triangle.vertices[k][0] << " " <<  transformed_triangle.vertices[k][1] << "  " << transformed_triangle.vertices[k][2] << endl;

                transformed_triangle.colors[k] = colors[vertices[models[i].triangles[j].vertexIds[k]].colorId];

                //cout << (int)transformed_triangle.vertices[0][0] << "  " << (int)transformed_triangle.vertices[0][1] << endl;
                //image[(int)round(transformed_triangle.vertices[k][0])][(int)round(transformed_triangle.vertices[k][1])].r = transformed_triangle.colors[k].r;
                //image[(int)round(transformed_triangle.vertices[k][0])][(int)round(transformed_triangle.vertices[k][1])].g = transformed_triangle.colors[k].g;
                //image[(int)round(transformed_triangle.vertices[k][0])][(int)round(transformed_triangle.vertices[k][1])].b = transformed_triangle.colors[k].b;
            }

            transformed_model.transformed_triangles.push_back(transformed_triangle);

        }

        transformed_models.push_back(transformed_model);
    }
}


void midpoint_algorithm(){
    //cout << transformed_models[0].transformed_triangles[0].vertices[0][0] << endl;
    for(int i=0; i < numberOfModels; i++){

        for(int j=0; j < transformed_models[i].numberOfTriangles; j++){


            for(int k=0; k < 3; k++){

                int x_0 = transformed_models[i].transformed_triangles[j].vertices[k][0];
                int x_1 = transformed_models[i].transformed_triangles[j].vertices[(k+1)%3][0];

                int y_0 = transformed_models[i].transformed_triangles[j].vertices[k][1];
                int y_1 = transformed_models[i].transformed_triangles[j].vertices[(k+1)%3][1];

                Color c_0 = transformed_models[i].transformed_triangles[j].colors[k];
                Color c_1 = transformed_models[i].transformed_triangles[j].colors[(k+1)%3];

                int x, y, d;

                // Examples for other slop ranges:
                /* m = INFINITY
                x_0 = 100;
                y_0 = 200;
                x_1 = 100;
                y_1 = 400;
                */

                /* m = 0
                x_0 = 100;
                y_0 = 200;
                x_1 = 600;
                y_1 = 200;*/

                double m = (double)(y_1-y_0)/(x_1-x_0);
                double alpha = abs(x_1-x_0);

                bool is_m_infinite = false;
                if(abs(x_1-x_0) == 0){
                    alpha = abs(y_1-y_0);
                    is_m_infinite = true;
                }

                if(0.0 < m && m < 1.0){
                    x = min(x_0, x_1);
                    y = min(y_0, y_1);

                    d = 2*abs(y_1-y_0)-abs(x_1-x_0);

                    /*c.r = c_0.r;
                    c.g = c_0.g;
                    c.b = c_0.b;

                    dc.r = (c_1.r - c_0.r)/() */

                    for(int x = x_0; x < x_1; x++){
                        if(d <= 0){          //choose E
                            d += 2*(y_1-y_0);
                        }
                        else{                //choose nE
                            d += 2*(abs(y_1-y_0)-abs(x_1-x_0));
                            y++;
                        }
                        image[x][y].r = (double)(c_0.r*abs(x-x_1) + c_1.r*abs(x_0-x))/(double)alpha;
                        image[x][y].g = (double)(c_0.g*abs(x-x_1) + c_1.g*abs(x_0-x))/(double)alpha;
                        image[x][y].b = (double)(c_0.b*abs(x-x_1) + c_1.b*abs(x_0-x))/(double)alpha; // draw(x,y)
                    }
                }
                else if(1.0 < m){
                    x = min(x_0, x_1);
                    y = min(y_0, y_1);
                    d = 2*abs(x_1-x_0)-abs(y_1-y_0);
                    for(int y = y_0; y < y_1; y++){
                        if(d <= 0){             //choose N
                            d += 2*abs(x_1-x_0);
                        }
                        else{                   //choose NE
                            d += 2*(abs(x_1-x_0)-abs(y_1-y_0));
                            x++;
                        }
                        if(is_m_infinite){
                            image[x][y].r = (double)(c_0.r*abs(y-y_1) + c_1.r*abs(y_0-y))/(double)alpha;
                            image[x][y].g = (double)(c_0.g*abs(y-y_1) + c_1.g*abs(y_0-y))/(double)alpha;
                            image[x][y].b = (double)(c_0.b*abs(y-y_1) + c_1.b*abs(y_0-y))/(double)alpha; // draw(x,y)
                        }
                        else{
                            image[x][y].r = (double)(c_0.r*abs(x-x_1) + c_1.r*abs(x_0-x))/(double)alpha;
                            image[x][y].g = (double)(c_0.g*abs(x-x_1) + c_1.g*abs(x_0-x))/(double)alpha;
                            image[x][y].b = (double)(c_0.b*abs(x-x_1) + c_1.b*abs(x_0-x))/(double)alpha; // draw(x,y)
                        }

                    }
                }
                else if(m  <= 0 && m >= -1.0){
                    x = min(x_0, x_1);
                    y = max(y_0, y_1);

                    //y = y_0;    // y = y_0
                    d = 2*abs(y_1-y_0)-abs(x_1-x_0);
                    for(int x = x_0; x < x_1; x++){
                        if(d <= 0){             //choose W
                            d += 2*abs(y_1-y_0);
                        }
                        else{                   //choose NW
                            d += 2*(abs(y_1-y_0)-abs(x_1-x_0));
                            y--;
                        }
                        image[x][y].r = (double)(c_0.r*abs(x-x_1) + c_1.r*abs(x_0-x))/(double)alpha;
                        image[x][y].g = (double)(c_0.g*abs(x-x_1) + c_1.g*abs(x_0-x))/(double)alpha;
                        image[x][y].b = (double)(c_0.b*abs(x-x_1) + c_1.b*abs(x_0-x))/(double)alpha; // draw(x,y)
                    }
                }
                else if(-1.0 > m){
                    x = max(x_0, x_1);
                    y = min(y_0, y_1);
                    d = 2*abs(x_1-x_0)-abs(y_1-y_0);
                    for(int y = y_0; y < y_1; y++){
                        if(d <= 0){             //choose N
                            d += 2*abs(x_1-x_0);
                        }
                        else{                   // choose NW
                            d += 2*(abs(x_1-x_0)-abs(y_1-y_0));
                            x--;
                        }
                        if(is_m_infinite){
                            image[x][y].r = (double)(c_0.r*abs(y-y_1) + c_1.r*abs(y_0-y))/(double)alpha;
                            image[x][y].g = (double)(c_0.g*abs(y-y_1) + c_1.g*abs(y_0-y))/(double)alpha;
                            image[x][y].b = (double)(c_0.b*abs(y-y_1) + c_1.b*abs(y_0-y))/(double)alpha; // draw(x,y)
                        }
                        else{
                            image[x][y].r = (double)(c_0.r*abs(x-x_1) + c_1.r*abs(x_0-x))/(double)alpha;
                            image[x][y].g = (double)(c_0.g*abs(x-x_1) + c_1.g*abs(x_0-x))/(double)alpha;
                            image[x][y].b = (double)(c_0.b*abs(x-x_1) + c_1.b*abs(x_0-x))/(double)alpha; // draw(x,y)
                        }
                    }

                }

            }
        }
    }
}

void clear_transformed_models(){
    for(int i=0; i < numberOfModels; i++){
        transformed_models[i].transformed_triangles.clear();
    }
    transformed_models.clear();
}

void rasterization_stage(Camera& cam){

    /* Draw the boundaries for the models using the
       midpoint line drawing algorithm */
    midpoint_algorithm();

    /* Clear transformed models for the next
       iteration */
    clear_transformed_models();

}

void culling_stage(){

}

void forwardRenderingPipeline(Camera cam) {
    // TODO: IMPLEMENT HERE

    transformations_stage(cam);
    cout << "***********************************" << endl;
    rasterization_stage(cam);
}


int main(int argc, char **argv) {
    int i, j;

    if (argc < 2) {
        std::cout << "Usage: ./rasterizer <scene file> <camera file>" << std::endl;
        return 1;
    }

    // read camera and scene files
    readSceneFile(argv[1]);
    readCameraFile(argv[2]);

    image = 0;

    for (i = 0; i < numberOfCameras; i++) {

        // allocate memory for image
        if (image) {
			for (j = 0; j < cameras[i].sizeX; j++) {
		        delete image[j];
		    }

			delete[] image;
		}

        image = new Color*[cameras[i].sizeX];

        if (image == NULL) {
            std::cout << "ERROR: Cannot allocate memory for image." << std::endl;
            exit(1);
        }

        for (j = 0; j < cameras[i].sizeX; j++) {
            image[j] = new Color[cameras[i].sizeY];
            if (image[j] == NULL) {
                std::cout << "ERROR: Cannot allocate memory for image." << std::endl;
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
        convertPPMToPNG(cameras[i].outputFileName, 1);
    }

    return 0;

}
