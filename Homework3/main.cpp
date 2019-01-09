#include "helper.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtx/rotate_vector.hpp"

//#include "glm/gtc/type_ptr.hpp"


static GLFWwindow * win = NULL;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idMVPMatrix;

int widthTexture, heightTexture;

/***********************Variables defined by me *****************************/

glm::vec3 camera_pos;
glm::vec3 camera_up = glm::vec3(0.0, 1.0, 0.0);
glm::vec3 camera_gaze = glm::vec3(0.0, 0.0, 1.0);
glm::vec3 camera_cross = cross(camera_up, camera_gaze);

GLfloat camera_speed = 0.0f;

bool fullscreen_mode;

int widthDisplay = 600;
int heightDisplay = 600;

int current_widthDisplay, current_heigthDisplay;


GLFWmonitor* primary_monitor;
const GLFWvidmode* vidmode;

/* These four variables should have the same type due to the template argument T
   that is used in the definition of the glm::perspective function */
GLfloat fovy = 45; /* Set the field of view for the projection matrix */
GLfloat aspect_ratio = 1;
GLfloat near = 0.1;
GLfloat far = 1000;

GLfloat heightFactor = 10.0f;

int vertex_cnt;
glm::vec3* vertices;

bool viewport_flag = false;
/****************************************************************************/

void cameraSpecifications(){

    glm::mat4 M_projection = glm::perspective(fovy, aspect_ratio, near, far);

    /* template <typename T, precision P>
     GLM_FUNC_DECL tvec3<T, P> operator*(tvec3<T, P> const & v, T const & scalar);
       Scalar and vec3 should use the same type float */
    glm::vec3 center = camera_pos + camera_gaze * near;

    /* lookAt function generates a transform from world
       space into the specific eye space that the projective matrix functions
       (perspective, ortho, etc) are designed to expect.

       Usage: eye = camera_pos, center, up = camera_up */
    glm::mat4 M_view = glm::lookAt(camera_pos, center, camera_up);

    glm::mat4 M_model = glm::mat4(1.0f);

    glm::mat4 M_model_view_projection = M_projection * M_view * M_model;

    glm::mat4 M_normal = glm::inverseTranspose(M_view);

    GLint loc_model_view = glGetUniformLocation(idProgramShader, "MV");
    glUniformMatrix4fv(loc_model_view, 1, GL_FALSE, &M_view[0][0]);

    GLint loc_model_view_projection = glGetUniformLocation(idProgramShader, "MVP");
    glUniformMatrix4fv(loc_model_view_projection, 1, GL_FALSE, &M_model_view_projection[0][0]);

    GLint loc_normal = glGetUniformLocation(idProgramShader, "M_norm");
    glUniformMatrix4fv(loc_normal, 1, GL_FALSE, &M_normal[0][0]);

    GLint loc_camera_pos = glGetUniformLocation(idProgramShader, "cameraPosition");
    glUniform3fv(loc_camera_pos, 1, &camera_pos[0]);
}

void createMeshes(){

    vertex_cnt = 6 * widthTexture * heightTexture;
    vertices = new glm::vec3[vertex_cnt];

    glm::vec3 vertex0, vertex1, vertex2, vertex3;
    int index = 0;

    for(int i = 0; i < widthTexture; i++){
        for(int j = 0; j < heightTexture; j++){

            /* Triangle #1: vertex0, vertex1, vertex2 */
            vertex0 = glm::vec3(i, 0, j);
            vertices[index] = vertex0;

            vertex1 = glm::vec3(i+1, 0, j+1);
            vertices[index+1] = vertex1;

            vertex2 = glm::vec3(i+1, 0, j);
            vertices[index+2] = vertex2;

            /* Triangle #2: vertex3, vertex0, vertex1 */
            vertex3 = glm::vec3(i, 0, j+1);
            vertices[index+3] = vertex3;

            vertices[index+4] = vertex0,
            vertices[index+5] = vertex1;

            index += 6;
        }
    }
}


void setLocations(){
    GLint loc_sampler = glGetUniformLocation(idProgramShader, "texture");
    glUniform1i(loc_sampler, 0);

    GLint loc_width_texture = glGetUniformLocation(idProgramShader, "widthTexture");
    glUniform1i(loc_width_texture, widthTexture);

    GLint loc_height_texture = glGetUniformLocation(idProgramShader, "heightTexture");
    glUniform1i(loc_height_texture, heightTexture);

    GLint loc_heightFactor = glGetUniformLocation(idProgramShader, "heightFactor");
    glUniform1f(loc_heightFactor, heightFactor);
}


void clearColorDepthStencil(){

    glClearStencil(0);
    glClearDepth(1.0f);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

}

void render(){

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_TRIANGLES, 0, vertex_cnt);
    glDisableClientState(GL_VERTEX_ARRAY);

}
/* As stated in the glfw documentation,
    The callback function receives the keyboard key, platform-specific scancode, key action and modifier bits. */
static void keyPressCallback(GLFWwindow *win, int key, int scode, int act, int mods){

    bool checkFullScreen = false;
    if(act == GLFW_PRESS){
        switch(key){
            case GLFW_KEY_F:
                checkFullScreen = true;
                break;

            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(win, GLFW_TRUE);
                break;

            case GLFW_KEY_U:
                camera_speed += 0.25;
                break;

            case GLFW_KEY_J:
                camera_speed -= 0.25;
                break;
        }
    }

    if(checkFullScreen){

        checkFullScreen = false;
        viewport_flag = true;

        if(fullscreen_mode){

            fullscreen_mode = false;

            current_heigthDisplay = heightDisplay;
            current_widthDisplay = widthDisplay;
            glfwSetWindowMonitor(win, nullptr, 0, 0, current_widthDisplay, current_heigthDisplay, 0);

        }
        else{

            fullscreen_mode = true;

            current_heigthDisplay = vidmode->height;
            current_widthDisplay = vidmode->width;

            glfwSetWindowMonitor(win, primary_monitor,0,0,current_widthDisplay, current_heigthDisplay, vidmode->refreshRate);

        }
    }

    if(act == GLFW_PRESS || act == GLFW_REPEAT){
        switch(key){
            case GLFW_KEY_O:
            {
                heightFactor += 0.5;
                GLint loc_heightFactor = glGetUniformLocation(idProgramShader, "heightFactor");
                glUniform1f(loc_heightFactor, heightFactor);
                break;
            }
            case GLFW_KEY_L:
            {
                heightFactor -= 0.5;
                GLint loc_heightFactor = glGetUniformLocation(idProgramShader, "heightFactor");
                glUniform1f(loc_heightFactor, heightFactor);
                break;
            }
            case GLFW_KEY_A:
            {
                camera_cross = glm::rotate(camera_cross, 0.01f, camera_up);
                camera_gaze = glm::rotate(camera_gaze, 0.01f, camera_up);
                break;
            }
            case GLFW_KEY_D:
            {
                camera_cross = glm::rotate(camera_cross, -0.01f, camera_up);
                camera_gaze = glm::rotate(camera_gaze, -0.01f, camera_up);
                break;
            }
            case GLFW_KEY_S:
            {
                camera_up = glm::rotate(camera_up, 0.01f, camera_cross);
                camera_gaze = glm::rotate(camera_gaze, 0.01f, camera_cross);
                break;
            }
            case GLFW_KEY_W:
            {
                camera_up = glm::rotate(camera_up, -0.01f, camera_cross);
                camera_gaze = glm::rotate(camera_gaze, -0.01f, camera_cross);
                break;
            }
        }
    }
}


static void errorCallback(int error,
  const char * description) {
  fprintf(stderr, "Error: %s\n", description);
}

int main(int argc, char * argv[]) {

  if (argc != 2) {
    printf("Only one texture image expected!\n");
    exit(-1);
  }

  glfwSetErrorCallback(errorCallback);

  if (!glfwInit()) {
    exit(-1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

  // Necessary commands to work on MAC OS, should be closed for submission
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  win = glfwCreateWindow(600, 600, "CENG477 - HW3", NULL, NULL);

  if (!win) {
    glfwTerminate();
    exit(-1);
  }
  glfwMakeContextCurrent(win);

  GLenum err = glewInit();
  if (err != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

    glfwTerminate();
    exit(-1);
  }

  initShaders();
  glUseProgram(idProgramShader);
  initTexture(argv[1], & widthTexture, & heightTexture);

  /***************************************************************************/

  // Set the keyPressCallback function to respond user input given by the keyboard
  glfwSetKeyCallback(win, keyPressCallback);

  // Set camera specifications
  cameraSpecifications();
  camera_pos = glm::vec3(widthTexture/2, widthTexture/10, -widthTexture/4);
  // Set the viewport
  glViewport(0,0, widthDisplay, heightDisplay);
  // Create the meshes
  createMeshes();

  glEnable(GL_DEPTH_TEST);

  setLocations();

  primary_monitor = glfwGetPrimaryMonitor();
  vidmode = glfwGetVideoMode(primary_monitor);
  glfwGetWindowSize(win, &current_widthDisplay, &current_heigthDisplay);


  while (!glfwWindowShouldClose(win)) {

      if( viewport_flag == true){

          viewport_flag = false;
          glViewport(0, 0, current_widthDisplay, current_heigthDisplay);

      }

      clearColorDepthStencil();

      camera_pos += camera_gaze * camera_speed;
      cameraSpecifications();

      render();

      glfwSwapBuffers(win);
      glfwPollEvents();
  }

  /***************************************************************************/
  glfwDestroyWindow(win);
  glfwTerminate();

  return 0;
}
