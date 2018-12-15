#include <iostream>
#include "parser.h"
#include "ppm.h"
#include <cmath>

using namespace parser;
using namespace std;

typedef unsigned char RGB[3];

typedef struct Ray
{
    Vec3f origin;
    Vec3f direction;
    bool shadowRay;
} ray;


enum ObjectType
{
	SPHERE, TRIANGLE, MESH
};


typedef struct Hit
{
	bool hitHappened;
	Vec3f intersectionPoint;
	Vec3f surfaceNormal;
	int material_id;
	float t;
	ObjectType objType;
	int obj_id;
} hit;


Vec3f subtract(const Vec3f &a, const Vec3f &b)
{
	Vec3f a_minus_b;
	a_minus_b.x = a.x - b.x;
	a_minus_b.y = a.y - b.y;
	a_minus_b.z = a.z - b.z;
	return a_minus_b;
}


Vec3f add(const Vec3f &a, const Vec3f &b)
{
	Vec3f a_plus_b;
	a_plus_b.x = a.x + b.x;
	a_plus_b.y = a.y + b.y;
	a_plus_b.z = a.z + b.z;
	return a_plus_b;
}


float dotProduct(const Vec3f &a, const Vec3f &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}


Vec3f crossProduct(const Vec3f &a, const Vec3f &b)
{
    Vec3f result;
    result.x = a.y*b.z - a.z*b.y;
    result.y = a.z*b.x - a.x*b.z;
    result.z = a.x*b.y - a.y*b.x;

    return result;
}


float findLength(const Vec3f &a)
{
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}


float findDistance(const Vec3f &a, const Vec3f &b)
{
	return sqrt(pow(a.x - b.x,2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}


Vec3f normalizeVector(const Vec3f &a)
{
	Vec3f result;
	result.x = a.x / findLength(a);
	result.y = a.y / findLength(a);
	result.z = a.z / findLength(a);
	return result;
}


Vec3f findIntersectionPoint(const Ray &ray, float t)
{
	Vec3f result;
	result.x = ray.origin.x + t*ray.direction.x;
	result.y = ray.origin.y + t*ray.direction.y;
	result.z = ray.origin.z + t*ray.direction.z;

	return result;
}


Hit findHit(vector<Hit> &hitInfoVector)
{
	Hit result;
	result.hitHappened = false;

	if(hitInfoVector.size() != 0)
	{
		result = hitInfoVector[0];

		for(int i = 1; i < hitInfoVector.size(); i++)
		{
			if(hitInfoVector[i].t < result.t)
			{
				result = hitInfoVector[i];
			}
		}
    	result.hitHappened = true;
	}

	return result;

}


Ray generateRay(const Camera &camera, int i, int j)
{
    float left = camera.near_plane.x;
    float right = camera.near_plane.y;
    float bottom = camera.near_plane.z;
    float top = camera.near_plane.w;

    Vec3f gaze = normalizeVector(camera.gaze);

    float su = (right - left)*(j + 0.5)/camera.image_width;
    float sv = (top - bottom)*(i + 0.5)/camera.image_height;

    Vec3f m, q, u, v;
    m.x = camera.position.x + (gaze.x * camera.near_distance);
    m.y = camera.position.y + (gaze.y * camera.near_distance);
    m.z = camera.position.z + (gaze.z * camera.near_distance);

    u = crossProduct(gaze, camera.up);
    u = normalizeVector(u);

    v = crossProduct(u, gaze);

    q.x = m.x + (u.x*left) + (v.x*top);
    q.y = m.y + (u.y*left) + (v.y*top);
    q.z = m.z + (u.z*left) + (v.z*top);

    Vec3f s;
    s.x = q.x + (u.x*su) - (v.x * sv);
    s.y = q.y + (u.y*su) - (v.y * sv);
    s.z = q.z + (u.z*su) - (v.z * sv);

    Ray ray;
    ray.origin = camera.position;
    ray.direction = subtract(s, camera.position);
    ray.direction = normalizeVector(ray.direction);
    ray.shadowRay = false;

    return ray;
}


Hit sphereIntersection(const Ray &ray, const Vec3f &center, float radius, int material_id, int obj_id)
{
	Hit hit;

	const float A = dotProduct(ray.direction, ray.direction);
	Vec3f e_minus_c = subtract(ray.origin, center);
	const float B = 2 * dotProduct(ray.direction, e_minus_c);
	const float C = dotProduct(e_minus_c, e_minus_c) - radius * radius;

	const float discriminant = B*B - 4*A*C;

	if(discriminant < 0)			// no intersection
	{
		hit.hitHappened = false;
	}
	else							// intersection at 1 or 2 points 
	{	
		const float t1 = (-1 * B + sqrtf(discriminant))/2*A;
		const float t2 = (-1 * B - sqrtf(discriminant))/2*A;

		hit.material_id = material_id;
		hit.hitHappened = true;
		hit.objType = SPHERE;
		hit.obj_id = obj_id;

		const float t = fmin(t1, t2);
		hit.intersectionPoint = findIntersectionPoint(ray, t);		/**********FIND INTERSECTION POINT**********/
		hit.surfaceNormal = subtract(hit.intersectionPoint, center);	/**********FIND UNIT NORMAL**********/
		hit.surfaceNormal.x /= radius;
		hit.surfaceNormal.y /= radius;
		hit.surfaceNormal.z /= radius;

		hit.t = t;
	}

	return hit;
}


float determinant(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2)
{
	return v0.x * (v1.y*v2.z - v2.y*v1.z)
			+ v0.y * (v2.x*v1.z - v1.x*v2.z)
			+ v0.z * (v1.x*v2.y - v1.y*v2.x);
}


// a,b,c are the vertices of the triangle
Hit triangleIntersection(const Ray &ray, const Vec3f &a, const Vec3f &b, const Vec3f &c, int material_id, int obj_id)
{
	Hit hit;
	hit.hitHappened = false;

	Vec3f o = ray.origin;
	Vec3f d = ray.direction;

	Vec3f a_minus_b = subtract(a, b);
	Vec3f a_minus_c = subtract(a, c);
	Vec3f a_minus_o = subtract(a, o);

	float detA = determinant(a_minus_b, a_minus_c, d);
	if(detA == 0.0)
	{
		return hit;
	}

	float t = (determinant(a_minus_b, a_minus_c, a_minus_o))/detA;
	if(t <= 0.0) {
		return hit;
	}

	float gamma = (determinant(a_minus_b,a_minus_o, d))/detA;
	if(gamma < 0 || gamma > 1) {
		return hit;
	}

	float beta = (determinant(a_minus_o, a_minus_c, d))/detA;
	if(beta < 0 || beta > (1 - gamma)) {
		return hit;
	}

	hit.hitHappened = true;
	hit.objType = TRIANGLE;
	hit.obj_id = obj_id;
	hit.material_id = material_id;
	hit.t = t;
	hit.intersectionPoint = findIntersectionPoint(ray, t);
	hit.surfaceNormal = crossProduct(subtract(b, a), subtract(c, a));
	hit.surfaceNormal = normalizeVector(hit.surfaceNormal);

	return hit;
}


Hit meshIntersection(const Ray &ray, const Mesh &mesh, const Scene &scene, int material_id, int obj_id)
{
	Hit hit;
	hit.hitHappened = false;
	vector<Hit> hitInfoVector;

	/*********FOR EACH TRIANGLE(FACE) IN A MESH**********/
	for(int faceNumber = 0; faceNumber < mesh.faces.size(); faceNumber++)
	{
		Vec3f v0 = scene.vertex_data[mesh.faces[faceNumber].v0_id - 1];		// find vertices of each triangle(face)
		Vec3f v1 = scene.vertex_data[mesh.faces[faceNumber].v1_id - 1];
		Vec3f v2 = scene.vertex_data[mesh.faces[faceNumber].v2_id - 1];

		hit = triangleIntersection(ray, v0, v1, v2, material_id, obj_id);
		if(hit.hitHappened && hit.t >= 0)
		{
			hit.material_id = material_id;
			hit.objType = MESH;
			hit.obj_id = obj_id;
			hit.intersectionPoint = findIntersectionPoint(ray, hit.t);
			hit.surfaceNormal = crossProduct(subtract(v1, v0), subtract(v2, v0));
            hit.surfaceNormal = normalizeVector(hit.surfaceNormal);

            hitInfoVector.push_back(hit);
		}
	}

	hit = findHit(hitInfoVector);
	return hit;
}


Vec3f findIrradiance(const PointLight &pointLight, const Vec3f &intersectionPoint)
{
    Vec3f irradiance;
    Vec3f d = subtract(pointLight.position, intersectionPoint);
    float d_square = dotProduct(d, d);

    if(d_square != 0.0)
    {
	    irradiance.x = pointLight.intensity.x/d_square;
	    irradiance.y = pointLight.intensity.y/d_square;
	    irradiance.z = pointLight.intensity.z/d_square;
    }
    return irradiance;
}


const Vec3f findDiffuse(const PointLight &currentLight, const Scene &scene, int material_id, const Vec3f &normal, const Vec3f &intersectionPoint)
{
	Vec3f diffuse;

	Vec3f irradiance = findIrradiance(currentLight, intersectionPoint);

	Vec3f l = subtract(currentLight.position, intersectionPoint);
	l = normalizeVector(l);


	float dotPro = dotProduct(l, normal);
	if(dotPro < 0)
	{
		dotPro = 0;
	}

	diffuse.x = scene.materials[material_id - 1].diffuse.x * dotPro * irradiance.x;
	diffuse.y = scene.materials[material_id - 1].diffuse.y * dotPro * irradiance.y;
	diffuse.z = scene.materials[material_id - 1].diffuse.z * dotPro * irradiance.z;

	return diffuse;
}


Vec3f findSpecular(const PointLight &currentLight, const Scene &scene, const Ray &ray, int material_id, const Vec3f &normal, const Vec3f &intersectionPoint)
{
	Vec3f specular;

	Material material = scene.materials[material_id - 1];

	Vec3f irradiance = findIrradiance(currentLight, intersectionPoint);

	Vec3f wi = subtract(currentLight.position, intersectionPoint);
	wi = normalizeVector(wi);

	Vec3f h = subtract(wi, ray.direction);
	h = normalizeVector(h);

	float dotPro = dotProduct(normal, h);
	if(dotPro < 0)
	{
		dotPro = 0;
	}

	specular.x = material.specular.x * pow(dotPro, material.phong_exponent) * irradiance.x;
	specular.y = material.specular.y * pow(dotPro, material.phong_exponent) * irradiance.y;
	specular.z = material.specular.z * pow(dotPro, material.phong_exponent) * irradiance.z;

	return specular;
}


bool isMirror(const Scene &scene, int material_id)
{
	if(scene.materials[material_id - 1].mirror.x > 0 || scene.materials[material_id - 1].mirror.y > 0 || scene.materials[material_id - 1].mirror.z > 0)
		return true;
	return false;
}


Hit findHitResult(const Scene &scene, const Ray &ray)
{
    Sphere currentSphere;
    Triangle currentTriangle;
    Mesh currentMesh;
    Vec3f normal, intersectionPoint;

    int numberOfSpheres = scene.spheres.size();
    int numberOfTriangles = scene.triangles.size();
    int numberOfMeshes = scene.meshes.size();

    vector<Hit> hitInfoVector;

    /***********INTERSECT WITH SPHERES***********/
    for(int sphereNumber = 0; sphereNumber < numberOfSpheres; sphereNumber++)
    {
        currentSphere = scene.spheres[sphereNumber];
        Vec3f center = scene.vertex_data[currentSphere.center_vertex_id - 1];
        float radius = currentSphere.radius;

        Hit hit = sphereIntersection(ray, center, radius, currentSphere.material_id, sphereNumber);

        if(hit.hitHappened && hit.t >= 0)
        {
            hitInfoVector.push_back(hit);
        }
    }


    /***********INTERSECT WITH TRIANGLES***********/
    for(int triangleNumber = 0; triangleNumber < numberOfTriangles; triangleNumber++)
    {
        currentTriangle = scene.triangles[triangleNumber];
        Vec3f v0 = scene.vertex_data[currentTriangle.indices.v0_id - 1];
        Vec3f v1 = scene.vertex_data[currentTriangle.indices.v1_id - 1];
        Vec3f v2 = scene.vertex_data[currentTriangle.indices.v2_id - 1];

        Hit hit = triangleIntersection(ray, v0, v1, v2, currentTriangle.material_id, triangleNumber);

        if(hit.hitHappened && hit.t >= 0)
        {
            hitInfoVector.push_back(hit);
        }
    }


    /**********INTERSECT WITH MESHES***********/
    for(int meshNumber = 0; meshNumber < numberOfMeshes; meshNumber++)
    {
        currentMesh = scene.meshes[meshNumber];

        Hit hit = meshIntersection(ray, currentMesh, scene, currentMesh.material_id, meshNumber);

        if(hit.hitHappened && hit.t >= 0)
        {
            hitInfoVector.push_back(hit);
        }
    }


    Hit hitResult = findHit(hitInfoVector);

    return hitResult;
}


Vec3f findPixelColor(const Scene &scene, const Hit &hitResult, const Camera &currentCamera, const Ray &ray, int maxDepth)
{
    int numberOfLights = scene.point_lights.size();
    int numberOfSpheres = scene.spheres.size();
    int numberOfTriangles = scene.triangles.size();
    int numberOfMeshes = scene.meshes.size();

    float pixel1 = 0;
    float pixel2 = 0;
    float pixel3 = 0;

    Vec3f pixelColor;

    if(hitResult.hitHappened)
    {
    	int material_id = hitResult.material_id;

		pixel1 = scene.materials[material_id - 1].ambient.x * scene.ambient_light.x;
		pixel2 = scene.materials[material_id - 1].ambient.y * scene.ambient_light.y;
		pixel3 = scene.materials[material_id - 1].ambient.z * scene.ambient_light.z;
      	
		for(int lightNumber = 0; lightNumber < numberOfLights; lightNumber++)
		{
		    bool shadowFlag = false;

		    PointLight currentLight = scene.point_lights[lightNumber];
		    float lightToCam = findDistance(currentLight.position, currentCamera.position);

			Vec3f wi = subtract(currentLight.position, hitResult.intersectionPoint);
			wi = normalizeVector(wi);

			// move the intersection point a bit further in shadowRay's direction:
			// direction * shadow_ray_epsilon

			Vec3f wi_epsilon;
			wi_epsilon.x = wi.x * scene.shadow_ray_epsilon;
			wi_epsilon.y = wi.y * scene.shadow_ray_epsilon;
			wi_epsilon.z = wi.z * scene.shadow_ray_epsilon;

			Ray shadowRay = { add(hitResult.intersectionPoint, wi_epsilon), wi, true };

			Hit shadowHit;
			vector<Hit> shadowHitVector;

			float tLight = subtract(currentLight.position, shadowRay.origin).x / shadowRay.direction.x;

			/***********INTERSECT WITH SPHERES***********/
        	for(int sphereNumber = 0; sphereNumber < numberOfSpheres; sphereNumber++)
        	{
          		Sphere currentSphere = scene.spheres[sphereNumber];
          		Vec3f center = scene.vertex_data[currentSphere.center_vertex_id - 1];
          		float radius = currentSphere.radius;

          		shadowHit = sphereIntersection(shadowRay, center, radius, currentSphere.material_id, sphereNumber);

          		if(shadowHit.hitHappened)
          		{
            		if(tLight > shadowHit.t && shadowHit.t >= 0)
            		{
              			shadowFlag = true;
            		}
          		}
        	}


	         /***********INTERSECT WITH TRIANGLES***********/
	        for(int triangleNumber = 0; triangleNumber < numberOfTriangles; triangleNumber++)
	        {
	          	Triangle currentTriangle = scene.triangles[triangleNumber];
	          	Vec3f v0 = scene.vertex_data[currentTriangle.indices.v0_id - 1];
	          	Vec3f v1 = scene.vertex_data[currentTriangle.indices.v1_id - 1];
	          	Vec3f v2 = scene.vertex_data[currentTriangle.indices.v2_id - 1];

	          	shadowHit = triangleIntersection(shadowRay, v0, v1, v2, currentTriangle.material_id, triangleNumber);

	          	if(shadowHit.hitHappened)
	          	{
	            	if(tLight > shadowHit.t && shadowHit.t >= 0)
	            	{
	              		shadowFlag = true;
	            	}
	          	}
	        }



	        if(!shadowFlag)
	        {
	          	/**********INTERSECT WITH MESHES***********/
	          	for(int meshNumber = 0; meshNumber < numberOfMeshes; meshNumber++)
	          	{
	            	Mesh currentMesh = scene.meshes[meshNumber];

	          		shadowHit = meshIntersection(shadowRay, currentMesh, scene, currentMesh.material_id, meshNumber);

	          		if(shadowHit.hitHappened)
	            	{
	              		if(tLight > shadowHit.t && shadowHit.t >= 0)
	              		{
	                		shadowFlag = true;
	              		}
	            	}
	        	}

        	}


        	// if no shadowRay intersection but there is object-ray intersection
	        if(!shadowFlag || (shadowFlag && lightToCam == 0))
	        {
		        int material_id = hitResult.material_id;

		        Vec3f diffuse = findDiffuse(currentLight, scene, material_id, hitResult.surfaceNormal, hitResult.intersectionPoint);

		        Vec3f specular = findSpecular(currentLight, scene, ray, material_id, hitResult.surfaceNormal, hitResult.intersectionPoint);
		                    
              	pixel1 += diffuse.x + specular.x;
  		        pixel2 += diffuse.y + specular.y;
  		        pixel3 += diffuse.z + specular.z;
            
	        }
	        
    	}

    	/************MIRRORNESS**********/
        bool mirrorness = isMirror(scene, material_id);
        Vec3f reflection;
        reflection.x = 0;
        reflection.y = 0;
        reflection.z = 0;

        if(maxDepth > 0 && mirrorness)
        {
        	float wi = -2 * dotProduct(ray.direction, hitResult.surfaceNormal);
        	Vec3f normal_wi;
        	normal_wi.x = hitResult.surfaceNormal.x * wi + ray.direction.x;
        	normal_wi.y = hitResult.surfaceNormal.y * wi + ray.direction.y;
        	normal_wi.z = hitResult.surfaceNormal.z * wi + ray.direction.z;

      		normal_wi = normalizeVector(normal_wi);

      		Vec3f wi_epsilon;
        	wi_epsilon.x = normal_wi.x * scene.shadow_ray_epsilon;
        	wi_epsilon.y = normal_wi.y * scene.shadow_ray_epsilon;
        	wi_epsilon.z = normal_wi.z * scene.shadow_ray_epsilon;

        	
            Ray reflectionRay = { add(hitResult.intersectionPoint, wi_epsilon), normal_wi, false};

            Hit hitRes = findHitResult(scene, reflectionRay);


            if(!(hitRes.obj_id == hitResult.obj_id && hitRes.objType == hitResult.objType))
            {
                reflection = findPixelColor(scene, hitRes, currentCamera, reflectionRay, (maxDepth - 1));
            }

      		pixel1 += reflection.x * scene.materials[material_id - 1].mirror.x;
      		pixel2 += reflection.y * scene.materials[material_id - 1].mirror.y;
      		pixel3 += reflection.z * scene.materials[material_id - 1].mirror.z;

   		}
  	}
  	else         // if hitHappened == 0
 	{
      	pixel1 = scene.background_color.x;
      	pixel2 = scene.background_color.y;
      	pixel3 = scene.background_color.z;
  	}

  	pixelColor.x = pixel1;
  	pixelColor.y = pixel2;
  	pixelColor.z = pixel3;

  	return pixelColor;
}



int main(int argc, char* argv[])
{
    // Sample usage for reading an XML scene file
    Scene scene;

    scene.loadFromXml(argv[1]);

    int numberOfCameras = scene.cameras.size();

    /**********FOR EACH PIXEL IN EACH CAMERA*************/
    for(int cameraNumber = 0; cameraNumber < numberOfCameras; cameraNumber++)
    {
        Camera currentCamera = scene.cameras[cameraNumber];
        int width = currentCamera.image_width;
        int height = currentCamera.image_height;
        int numberOfLights = scene.point_lights.size();

        unsigned char* image = new unsigned char [width * height * 3];
        int pixelNumber = 0;

        for(int i = 0; i < height; i++)
        {
            for(int j = 0; j < width; j++)
            {
            	/*********GENERATE RAYS*********/
                Ray ray = generateRay(currentCamera, i, j);

                Hit hitResult = findHitResult(scene, ray);


             	Vec3f pixelColor = findPixelColor(scene, hitResult, currentCamera, ray, (scene.max_recursion_depth));

            	if(pixelColor.x > 255)
            		image[pixelNumber] = 255;
              	else
                	image[pixelNumber] = round(pixelColor.x);

            	if(pixelColor.y > 255)
            		image[pixelNumber + 1] = 255;
             	else
                	image[pixelNumber + 1] = round(pixelColor.y);

            	if(pixelColor.z > 255)
            		image[pixelNumber + 2] = 255;
              	else
                	image[pixelNumber + 2] = round(pixelColor.z);


              	pixelNumber += 3;

            }
        }

        write_ppm(currentCamera.image_name.c_str(), image, width, height);
    }




}
