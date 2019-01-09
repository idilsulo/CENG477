#version 410

layout(location = 0) in vec3 position;

// Data from CPU
uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
uniform vec4 cameraPosition;
uniform float heightFactor;

uniform mat4 M_norm;


// Texture-related data
uniform sampler2D rgbTexture;
uniform int widthTexture;
uniform int heightTexture;


// Output to Fragment Shader
out vec2 textureCoordinate; // For texture-color
out vec3 vertexNormal; // For Lighting computation
out vec3 ToLightVector; // Vector from Vertex to Light;
out vec3 ToCameraVector; // Vector from Vertex to Camera;


vec3 light_pos = vec3(widthTexture / 2, widthTexture + heightTexture, heightTexture / 2);


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

    vec3 neighbor0, neighbor1, neighbor2, neighbor3, neighbor4, neighbor5;
    vec3 vertex0, vertex1, vertex2, vertex3, vertex4, vertex5;

    if(position.z == heightTexture-1 && position.x == 0){

        neighbor0 = vec3(position.x + 1, 0, position.z);
        neighbor1 = vec3(position.x + 1, 0, position.z - 1);
        neighbor2 = vec3(position.x, 0,position.z - 1);

        compute(neighbor0, textureCoordinate_neighbor);
        compute(neighbor1, textureCoordinate_neighbor);
        compute(neighbor2, textureCoordinate_neighbor);

        vertex0 = neighbor0 - pos;
        vertex1 = neighbor1 - pos;
        vertex2 = neighbor2 - pos;

        norm = normalize(cross(vertex1, vertex0) + cross(vertex2, vertex1));

    }
    else if(position.z == heightTexture - 1 && position.x == widthTexture - 1){

        neighbor0 = vec3(position.x - 1, 0, position.z);
        neighbor1 = vec3(position.x, 0, position.z - 1);

        compute(neighbor0, textureCoordinate_neighbor);
        compute(neighbor1, textureCoordinate_neighbor);

        vertex0 = pos -  neighbor0;
        vertex1 =  neighbor1 - neighbor0;

        norm = normalize(cross(vertex0, vertex1));

    }
    else if(position.z == 0 && position.x == widthTexture - 1){

        neighbor0 = vec3(position.x - 1, position.y, position.z);
        neighbor1 = vec3(position.x - 1, position.y, position.z + 1);
        neighbor2 = vec3(position.x, 0, position.z + 1);

        compute(neighbor0, textureCoordinate_neighbor);
        compute(neighbor1, textureCoordinate_neighbor);
        compute(neighbor2, textureCoordinate_neighbor);

        vertex0 = neighbor0 - pos;
        vertex1 = neighbor1 - pos;
        vertex2 = neighbor2 - pos;

        norm = normalize(cross(vertex0, vertex1) + cross(vertex1, vertex2));

    }
    else if(position.z == 0 && position.x == 0){

        neighbor0 = vec3(position.x + 1, position.y, position.z);
        neighbor1 = vec3(position.x, position.y, position.z + 1);

        compute(neighbor0, textureCoordinate_neighbor);
        compute(neighbor1, textureCoordinate_neighbor);

        vertex0 = pos - neighbor0;
        vertex1 = neighbor1 - neighbor0;

        norm = normalize(cross(vertex0, vertex1));
    }
    else if((0 < position.x && (position.z == heightTexture - 1 || position.z == 0)) ||
    (0 < position.z && (position.x == widthTexture - 1 || position.x == 0))){

        if(position.z == 0){

            neighbor0 = vec3(position.x + 1, 0, position.z);
            neighbor1 = vec3(position.x, 0, position.z + 1);
            neighbor2 = vec3(position.x - 1, 0, position.z + 1);
            neighbor3 = vec3(position.x - 1, 0, position.z);

        }
        else if(position.z == heightTexture - 1){

            neighbor0 = vec3(position.x - 1, 0, position.z);
            neighbor1 = vec3(position.x, 0, position.z - 1);
            neighbor2 = vec3(position.x + 1, 0, position.z - 1);
            neighbor3 = vec3(position.x + 1, 0, position.z);

        }
        else if(position.x == 0){

            neighbor0 = vec3(position.x, 0, position.z - 1);
            neighbor1 = vec3(position.x + 1, 0, position.z - 1);
            neighbor2 = vec3(position.x + 1, 0, position.z);
            neighbor3 = vec3(position.x, 0, position.z + 1);

        }
        else if(position.x == widthTexture - 1){

            neighbor0 = vec3(position.x, 0, position.z + 1);
            neighbor1 = vec3(position.x - 1, 0, position.z + 1);
            neighbor2 = vec3(position.x - 1, 0, position.z);
            neighbor3 = vec3(position.x, 0, position.z - 1);

        }

        compute(neighbor0, textureCoordinate_neighbor);
        compute(neighbor1, textureCoordinate_neighbor);
        compute(neighbor2, textureCoordinate_neighbor);
        compute(neighbor3, textureCoordinate_neighbor);

        vertex0 = neighbor0 - position;
        vertex1 = neighbor1 - position;
        vertex2 = neighbor2 - position;
        vertex3 = neighbor3 - position;

        norm = normalize(cross(vertex1, vertex0) + cross(vertex2, vertex1) + cross(vertex3, vertex2));

    }
    else{

        neighbor0 = vec3(position.x - 1, 0, position.z);
        neighbor1 = vec3(position.x - 1, 0, position.z + 1);
        neighbor2 = vec3(position.x, 0, position.z + 1);
        neighbor3 = vec3(position.x + 1, 0, position.z);
        neighbor4 = vec3(position.x + 1, 0, position.z - 1);
        neighbor5 = vec3(position.x, 0, position.z - 1);

        compute(neighbor0, textureCoordinate_neighbor);
        compute(neighbor1, textureCoordinate_neighbor);
        compute(neighbor2, textureCoordinate_neighbor);
        compute(neighbor3, textureCoordinate_neighbor);
        compute(neighbor4, textureCoordinate_neighbor);
        compute(neighbor5, textureCoordinate_neighbor);

        vertex0 = neighbor0 - pos;
        vertex1 = neighbor1 - pos;
        vertex2 = neighbor2 - pos;
        vertex3 = neighbor3 - pos;
        vertex4 = neighbor4 - pos;
        vertex5 = neighbor5 - pos;

        norm = normalize(cross(vertex0, vertex1) + cross(vertex1, vertex2) + cross(vertex2, vertex3) + cross(vertex3, vertex4)
                            + cross(vertex4, vertex5) + cross(vertex5, vertex0));

    }



    // compute toLight vector vertex coordinate in VCS
    vertexNormal = normalize(vec3(M_norm * vec4(norm, 0)));
    ToCameraVector = normalize(vec3(MV * (vec4(vec3(cameraPosition) - pos, 0))));
    ToLightVector = normalize(vec3(MV * vec4(light_pos - pos, 0)));


    // set gl_Position variable correctly to give the transformed vertex position

    //gl_Position = vec4(0,0,0,0); // this is a placeholder. It does not correctly set the position
    gl_Position = MVP * vec4(pos, 1.0);

}
