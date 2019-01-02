#version 410

layout(location = 0) in vec3 position;

// Data from CPU
uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
uniform vec4 cameraPosition;
uniform float heightFactor;

// Texture-related data
uniform sampler2D rgbTexture;
uniform int widthTexture;
uniform int heightTexture;


// Output to Fragment Shader
out vec2 textureCoordinate; // For texture-color
out vec3 vertexNormal; // For Lighting computation
out vec3 ToLightVector; // Vector from Vertex to Light;
out vec3 ToCameraVector; // Vector from Vertex to Camera;


void compute(inout vec3 neighbor, inout vec2 textureCoordinate_neighbor){
    textureCoordinate_neighbor.x = abs(neighbor.x - widthTexture) / widthTexture;
    textureCoordinate_neighbor.y = abs(neighbor.z - heightTexture) / heightTexture;

    vec4 texture_color = texture(rgbTexture, textureCoordinate_neighbor);
    vec3 rgb = texture_color.xyz;
    neighbor.y = heightFactor * (0.2126 * rgb.x + 0.7152 * rgb.y + 0.0722 * rgb.z);
}

void main()
{

    // get texture value, compute height
    vec3 pos = position;

    float textureCoordinate_x = 1 - (float(position.x) / (widthTexture + 1));
    float textureCoordinate_y = 1 - (float(position.z) / (heightTexture + 1));
    textureCoordinate = vec2(textureCoordinate_x, textureCoordinate_y);

    vec4 texture_color = texture(rgbTexture, textureCoordinate);
    vec3 rgb = texture_color.xyz;

    pos.y = heightFactor * (0.2126 * rgb.x + 0.7152*rgb.y + 0.0722 * rgb.z);


    // compute normal vector using also the heights of neighbor vertices

    vec2 textureCoordinate_neighbor;
    vec3 norm;

    if(position.z == heightTexture-1 && position.x == 0){

        vec3 neighbor0 = vec3(position.x + 1, 0, position.z);
        vec3 neighbor1 = vec3(position.x + 1, 0, position.z - 1);
        vec3 neighbor2 = vec3(position.x, 0,position.z - 1);

        compute(neighbor0, textureCoordinate_neighbor);
        compute(neighbor1, textureCoordinate_neighbor);
        compute(neighbor2, textureCoordinate_neighbor);

        vec3 vertex0 = neighbor0 - pos;
        vec3 vertex1 = neighbor1 - pos;
        vec3 vertex2 = neighbor2 - pos;

        norm = normalize(cross(vertex1, vertex0) + cross(vertex2, vertex1));

    }
    else if(position.z == heightTexture - 1 && position.x == widthTexture - 1){
        vec3 neighbor0 = vec3(position.x - 1, 0, position.z);
        vec3 neighbor1 = vec3(position.x, 0, position.z - 1);


    }
    else if(position.x == widthTexture - 1 && position.z == 0){


    }
    else if(position.x == 0 && position.z == 0){

    }
    /*
    else if(){

    }
    else{

    }*/



    // compute toLight vector vertex coordinate in VCS

   // set gl_Position variable correctly to give the transformed vertex position

   //gl_Position = vec4(0,0,0,0); // this is a placeholder. It does not correctly set the position
   gl_Position = vec4(position.x,position.y,position.z,1.0); // this is a placeholder. It does not correctly set the position
}
