
#include <cmath>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <string>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>

#include "program.h"
#include "vertexarray.h"
#include "shader.h"
#include "texture.h"

//Zach additions
#include "triangle.h"

using namespace gl_CSCI441;
using namespace std;
using namespace glm;

/* --------------------------------------------- */
/* ----- GLOBAL VARIABLES  --------------------- */
/* --------------------------------------------- */

static const GLint VPD_DEFAULT = 800;
static GLint wid;               /* GLUT window id; value asigned in main() and should stay constant */
static GLint vpw = VPD_DEFAULT; /* viewport dimensions; changed when window is resized (resize callback) */
static GLint vph = VPD_DEFAULT;

// User interface absolute and intermediate rotation matricies
mat4 R_trackball; //the superposition of all ‘finished’ rotations (matrix)
mat4 R_trackball_0; //the rotation that is currently being specified using the trackball interface (matrix)

// Input mesh info
int number_of_triangles, number_of_vertices, number_of_u_vertices, number_of_torrus_vertices;
float x_min, y_min, z_min;
float x_max, y_max, z_max;

//Global FOV angle for zooming
GLfloat fov_angle = 10.0f;


//Input file objects
VertexArray *va_input = NULL;
Buffer *buf_input_locations = NULL;
Buffer *buf_input_normals = NULL;

Program *input_program_3d = NULL;
Program *input_program_env = NULL;
Program *input_program_torrus = NULL;

Buffer *buf_torrus_points = NULL;
VertexArray *va_torrus = NULL;

/*--------Menu Options----------*/
static const int MENU_ENVIRONMENT_TEX = 1;
static const int MENU_3D_TEX = 2;
static const int MENU_TORRUS = 3;
static const int MENU_ZOOM_IN = 4;
static const int MENU_ZOOM_OUT = 5;

static int program = MENU_3D_TEX;

/*--------Utility----------*/
// print out matrix by rows
void printMat(glm::mat4  mat){
  int i,j;
  for (j=0; j<4; j++){
    for (i=0; i<4; i++){
    printf("%f ",mat[i][j]);
  }
  printf("\n");
 }
}

RGBTexture2D *tEnvironment = NULL;
RGBTexture3D *t3D = NULL;
RGBTexture2D *tTorrus = NULL;

/* ----------------------------------------------------- */

void setup_textures()
{
  // reads a PPM image from file and create a texture object
  //  last three values are border color (in the project, there is no need to use borders)
  tEnvironment = createRGBTexture2D("textures/EnvMap/grace.ppm",0.5,0.25,0.25);
  tEnvironment->linear();        // request bilinear interpolation
  tEnvironment->clampToBorder(); // use border color if texture coordinates out of range
  tEnvironment->attach(1);       // attach to TAP #1; must match the binding parameter in the shaders
                        // see `layout (binding=1) uniform sampler2D tex;' in fsh_square.glsl
  tEnvironment->on();            // turn on the texture; has to be called after any parameter change - 
                        //  otherwise the texture will be off

  // reads a 3D texture in the raw format - first 3 parguments are the resolution
  t3D = createRGBTexture3D(128,128,128,"textures/3D/marble.rgb");
  t3D->attach(2);     // TAP #2 is used for this one
                      // see line `layout (binding=2) uniform sampler3D tex;' in fsh_cube.glsl
  t3D->linear();      
  t3D->clampToEdge(); // clamp to the edge of the texture
  t3D->on();          // it's OK to keep all textures on all the time - we just have a few
                      //   do the same in your project - initialize all textures here and 
                      //   keep them on for simplicity; use small numbers for TAPs 
  tTorrus = createRGBTexture2D("textures/Tilable/tilable-1.ppm",0.5,0.25,0.25);
  tTorrus->repeat();
  tTorrus->linear();        // request bilinear interpolation
  tTorrus->attach(3);       // attach to TAP #1; must match the binding parameter in the shaders
                        // see `layout (binding=1) uniform sampler2D tex;' in fsh_square.glsl
  tTorrus->on();            // turn on the texture; has to be called after any parameter change - 
                        //  otherwise the texture will be off
}

/* ----------------------------------------------------- */

void setup_input_buffers()
{
  ifstream ifs("input.t");

  ifs >> number_of_triangles >> number_of_u_vertices;
  number_of_vertices = number_of_triangles * 3;


  cout << "Expecting " << number_of_triangles << " triangles, " << number_of_u_vertices << " unique verticies." << endl;
  
  Triangle *tri_table = new Triangle[number_of_triangles];
  vec3 *vert_table = new vec3[number_of_u_vertices];
  
  for (int i=0; i<number_of_triangles; i++ )
    ifs >> tri_table[i].a >> tri_table[i].b >> tri_table[i].c;
  
  for (int i=0; i<number_of_u_vertices; i++ ) {
    ifs >> vert_table[i].x >> vert_table[i].y >> vert_table[i].z;
    if (i == 0) {
      x_min = x_max = vert_table[0].x;
      y_min = y_max = vert_table[0].y;
      z_min = z_max = vert_table[0].z;
    } else {
      if (x_min > vert_table[i].x) {
        x_min = vert_table[i].x;
      }
      if (y_min > vert_table[i].y) {
        y_min = vert_table[i].y;
      }
      if (z_min > vert_table[i].z) {
        z_min = vert_table[i].z;
      }
      if (x_max < vert_table[i].x) {
        x_max = vert_table[i].x;
      }
      if (y_max < vert_table[i].y) {
        y_max = vert_table[i].y;
      }
      if (z_max < vert_table[i].z) {
        z_max = vert_table[i].z;
      }
    }
  }

  cout << "x min: " << x_min << " y min: " << y_min << " z min: " << z_min << endl;
  cout << "x max: " << x_max << " y max: " << y_max << " z max: " << z_max << endl;

  cout << "Finished creating vertex and triangle table." << endl;

  //array of normal vectors
  vec3 *N = new vec3[number_of_vertices];
  for (int i = 0; i < number_of_triangles; i++)
  {
    Triangle t = tri_table[i];
    vec3 a = vert_table[t.a];
    vec3 b = vert_table[t.b];
    vec3 c = vert_table[t.c];

    vec3 ab = vec3(b.x - a.x, b.y - a.y, b.z - a.z);
    vec3 ac = vec3(c.x - a.x, c.y - a.y, c.z - a.z);
    vec3 normal = cross(ab, ac);

    N[t.a] += normal;
    N[t.b] += normal;
    N[t.c] += normal;
  }

  cout << "Created area weighted average of normals." << endl;

  //Area weighted average for each vertex
  GLfloat *normalArray = new GLfloat[3 * number_of_vertices];
  GLfloat *coordnArray = new GLfloat[3 * number_of_vertices];

  int j = 0;
  for (int i = 0; i < number_of_triangles; i++) {
    Triangle t = tri_table[i];
    vec3 a = vert_table[t.a];
    vec3 b = vert_table[t.b];
    vec3 c = vert_table[t.c];

    normalArray[j * 3 + 0] = N[t.a].x;
    normalArray[j * 3 + 1] = N[t.a].y;
    normalArray[j * 3 + 2] = N[t.a].z;

    coordnArray[j * 3 + 0] = a.x;
    coordnArray[j * 3 + 1] = a.y;
    coordnArray[j * 3 + 2] = a.z;

    j++;

    normalArray[j * 3 + 0] = N[t.b].x;
    normalArray[j * 3 + 1] = N[t.b].y;
    normalArray[j * 3 + 2] = N[t.b].z;

    coordnArray[j * 3 + 0] = b.x;
    coordnArray[j * 3 + 1] = b.y;
    coordnArray[j * 3 + 2] = b.z;

    j++;

    normalArray[j * 3 + 0] = N[t.c].x;
    normalArray[j * 3 + 1] = N[t.c].y;
    normalArray[j * 3 + 2] = N[t.c].z;

    coordnArray[j * 3 + 0] = c.x;
    coordnArray[j * 3 + 1] = c.y;
    coordnArray[j * 3 + 2] = c.z;

    j++;
  }

  cout << "Creating torrus coordinate mapping." << endl;

  //Torus creation
  float resolution = 100.f;
  number_of_torrus_vertices = 2 * 3 * (int) pow(resolution, 2); //2 triangles per square, 3 points per triangle
  GLfloat *torrusWrapArray = new GLfloat[2 * number_of_torrus_vertices]; //2 numbers per vertex
  int index = 0;
  for (float y = 0.f; y < resolution; ++y) {
    for (float x = 0.f; x < resolution; ++x) {
      float x0 = x;
      float x1 = x+1.f;//(x + 1.f) == resolution ? 0 : x + 1.f;
      float y0 = y;
      float y1 = y+1.f;//(y + 1.f) == resolution ? 0 : y + 1.f;

      //Triangle 1:
      torrusWrapArray[index * 2 + 0] = x0/resolution;
      torrusWrapArray[index * 2 + 1] = y0/resolution;
      index++;

      torrusWrapArray[index * 2 + 0] = x1/resolution;
      torrusWrapArray[index * 2 + 1] = y0/resolution;
      index++;

      torrusWrapArray[index * 2 + 0] = x0/resolution;
      torrusWrapArray[index * 2 + 1] = y1/resolution;
      index++;

      //Triangle 2:
      torrusWrapArray[index * 2 + 0] = x0/resolution;
      torrusWrapArray[index * 2 + 1] = y1/resolution;
      index++;

      torrusWrapArray[index * 2 + 0] = x1/resolution;
      torrusWrapArray[index * 2 + 1] = y0/resolution;
      index++;

      torrusWrapArray[index * 2 + 0] = x1/resolution;
      torrusWrapArray[index * 2 + 1] = y1/resolution;
      index++;
    }
  }

  buf_torrus_points = new Buffer(2, number_of_torrus_vertices, torrusWrapArray);

  cout << "Created buffer of torrus mapping coordinates." << endl;

  buf_input_locations = new Buffer(3, number_of_vertices, coordnArray);
  buf_input_normals   = new Buffer(3, number_of_vertices, normalArray);

  cout << "Created buffer arrays for loc and normal." << endl;

  va_input = new VertexArray;
  va_input->attachAttribute(0,buf_input_locations);
  va_input->attachAttribute(1,buf_input_normals);

  va_torrus = new VertexArray;
  va_torrus->attachAttribute(0,buf_torrus_points);

  cout << "Finished attatching buffers to input vertex array." << endl;
}

/* ----------------------------------------------------- */

void setup_programs()
{
  // There is a handy function createProgram implemented in program.c,
  // which takes two file names (first=vertex shader source, 
  // second=fragment shader source) and creates a program from them.
  // The return value is of type Program*.
  // Note that we print a messahe before calling it because it
  // prints out the GLSL compiler and linker messages - this is a way to know
  // which of your shaders/programs has a problem.

  cout << "Creating input programs..." << endl;
  // TODO: use logic to determine which fragment program to load: flat, gorroud, or phong
  input_program_3d = createProgram("shaders/vsh_input_3d.glsl", "shaders/fsh_input_3d.glsl");
  input_program_env = createProgram("shaders/vsh_input_env.glsl", "shaders/fsh_input_env.glsl");
  input_program_torrus = createProgram("shaders/vsh_input_torrus.glsl", "shaders/fsh_input_torrus.glsl");
}

/* ----------------------------------------------------- */


/* ----------------------------------------------------- */

void draw()
{
  // ensure we're drawing to the correct GLUT window 
  glutSetWindow(wid);

  // clear buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  // want to use depth test to get visibility right
  glEnable(GL_DEPTH_TEST);

  // Use culling on 3D, water tight inputs
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);

  // Compute projection matrix; perspective() is a glm function
  // Arguments: field of view in DEGREES(!), aspect ratio (1 if square window), distance to front and back clipping plane
  // Camera is located at the origin and points along -Z direction
  
  GLfloat view_distance = 1.f / float(tan((10.f * M_PI / 180.f) /  2.0f));
  cout << "view distance: " << view_distance << endl;
  mat4 P = perspective(fov_angle,1.0f, view_distance - 1.f, view_distance + 3.f);

  // 1) translate mesh to origin
  // 2) scale to a (-1,1) box
  // 3) apply rotations
  // 4) translate away from view point

  GLfloat x_trans = (x_min + x_max) / -2.f;
  GLfloat y_trans = (y_min + y_max) / -2.f;
  GLfloat z_trans = (z_min + z_max) / -2.f;
  mat4 normalize_translate = translate(mat4(),vec3(x_trans, y_trans, z_trans));

  GLfloat scale_factor = 2.0f / fmax(fmax(x_max - x_min, y_max - y_min), z_max - z_min);

  mat4 normalize_scale = scale(mat4(), vec3(scale_factor, scale_factor, scale_factor));
  
  mat4 view_translate = translate(mat4(), vec3(0.f, 0.f, -1 - view_distance));

  // vsh uniforms (including P from above)
  mat4 R_full = R_trackball_0 * R_trackball;
  mat4 MV = view_translate * R_full * normalize_scale * normalize_translate;

  if (program == MENU_TORRUS) {
    MV = view_translate * R_full;
  }

  mat4 NM = R_full; // Normal matrix is just the rotation, normals aren't affected by trans and are normalized

  vec3 LL = vec3(0.0f, 0.5f, 0.f); // Lightsource location above viewpoint

  //texture normalization
  vec3 texture_translate;
  GLfloat texture_scale;
  //texture normalization to [0,1]
  GLfloat xt_trans = -1.f * x_min;
  GLfloat yt_trans = -1.f * y_min;
  GLfloat zt_trans = -1.f * z_min;

  switch(program) 
  {
    case MENU_ENVIRONMENT_TEX:
      texture_translate = vec3(1.f,1.f,1.f);
      texture_scale = 0.5f;
      break;
    case MENU_3D_TEX:
      texture_translate = vec3(xt_trans, yt_trans, zt_trans);
      texture_scale = 0.5f * scale_factor;
      break;
    case MENU_TORRUS:

      break;
  }
  

  // fsh uniforms
  //Illumination total = I * ( kd*(N·L) + ks*(H·N)^n ) + kaIa
  GLfloat LightIntensity = 1.f;
  GLfloat N_Spec = 1000.f;

  vec3 K_Spec  = vec3(0.5f, 0.5f, 0.5f);
  vec3 Ambient = vec3(0.3f, 0.3f, 0.3f);
  vec3 K_Diff  = vec3(0.4f, 0.4f, 0.4f);


  //=============== Input Program ================

  Program *input_program; 

  switch(program)
  {
  case MENU_ENVIRONMENT_TEX:
    input_program = input_program_env;
    break;
  case MENU_3D_TEX:
    input_program = input_program_3d;
    break;
  case MENU_TORRUS:
    input_program = input_program_torrus;
    break;
  }

  // Vertex program uniforms
  input_program->setUniform("P",&P[0][0]);
  input_program->setUniform("NM", &NM[0][0]);
  input_program->setUniform("LL", &LL[0]);
  input_program->setUniform("MV",&MV[0][0]);

  // Texture uniforms
  input_program->setUniform("TT", &texture_translate[0]);
  input_program->setUniform("TS", &texture_scale);

  // Fragment program uniforms
  input_program->setUniform("LightIntensity", &LightIntensity);
  input_program->setUniform("N_Spec", &N_Spec);

  input_program->setUniform("K_Diff", &K_Diff[0]);
  input_program->setUniform("K_Spec", &K_Spec[0]);
  input_program->setUniform("Ambient", &Ambient[0]);

  input_program->on();

  if (program == MENU_TORRUS) {
    // Torrus uniforms
    GLfloat TOR_R = 0.5f;
    GLfloat TOR_r = 0.4f;
    input_program->setUniform("TOR_R", &TOR_R);
    input_program->setUniform("TOR_r", &TOR_r);

    va_torrus->sendToPipeline(GL_TRIANGLES, 0, number_of_torrus_vertices);
  } else {
    va_input->sendToPipeline(GL_TRIANGLES, 0, number_of_vertices);
  }
  input_program->off();

  //=============== Input Program ================


  // make sure all the stuff is drawn
  glFlush();

  // this exchanges the invisible back buffer with the visible buffer to 
  //  avoid refresh artifacts
  glutSwapBuffers();
}


/* ----------------------------------------------------- */

/* --------------------------------------------- */
/* --- INTERACTION ----------------------------- */
/* --------------------------------------------- */

/* handle mouse events */

// you'll need to replace code that prints out messages with trackball code 
// in the two functions below

// some versions of glut like to generate a mouse button up event
//  after the user selects a menu item
// this variable will allow us to distinguish these spurious events
//  and the actual mouse button up events...
bool mouse_button_down = false;

// Additional global variabes for rotations

//i_0 , j_0 : coordinates of the last mouse button down event
GLint i_0 = -1;
GLint j_0 = -1;

void sphere_coordinates(GLint i, GLint j, GLfloat& x, GLfloat& y, GLfloat& z)
{
  GLfloat d = (GLfloat) VPD_DEFAULT; //screen resolution
  x = 2.0f * (GLfloat)i / (d - 1.0f) - 1.0f; //translate i to world coordinates
  y = -1.0f * (2.0f * (GLfloat)j / (d - 1.0f) - 1.0f); //translate j to world coordinates
  if (sqrt(pow(x,2) + pow(y,2)) >= 1.0f) {
    //we are outside circle
    GLfloat x_normal = x / sqrt(pow(x,2) + pow(y,2));
    GLfloat y_normal = y / sqrt(pow(x,2) + pow(y,2));
    x = x_normal;
    y = y_normal;
    z = 0.0f;
  } else {
    //we are inside circle
    z = sqrt(1.0f - pow(x, 2) - pow(y,2));
  }
  //cout << "x: " << x << endl << "y: " << y << endl << "z: " << z << endl;
}

mat4 rotation_matrix_for_point(GLint i, GLint j)
{
  //no rotation if point not specified
  if (i_0 < 0 || j_0 < 0) {
    return mat4();
  }

  GLfloat x, y, z;
  sphere_coordinates(i, j, x, y, z); //world coordinates of q
  vec3 q = vec3(x, y, z);


  GLfloat x_0, y_0, z_0;
  sphere_coordinates(i_0, j_0, x_0, y_0, z_0); //world coordinates of p
  vec3 p = vec3(x_0, y_0, z_0);

  GLfloat angle = acos(dot(p, q)) * 180.0 / M_PI;

  //cout << "dot: " << dot(p,q) << endl;

  //rotate from p (at i_0,j_0) to q ( at i,j)
  if (i == i_0 && j == j_0) {
    return mat4(); //identity
  } else {
    vec3 axis = cross(p, q);
    //cout << "rotation matrix: " << endl;
    //cout << "axis: " << axis[0] << " " << axis[1] << " " << axis[2] << endl << "angle: " << angle << endl;
    //printMat(rotate(mat4(), angle, axis));
    return rotate(mat4(), angle, axis);
  }
}

void mouse_button(GLint btn, GLint state, GLint mx, GLint my)
{
  switch( btn ) {
  case GLUT_LEFT_BUTTON:
    switch( state ) {
    case GLUT_DOWN: 
      cout << "Left mouse button pressed @ " << mx << " " << my << endl;
      mouse_button_down = true;
      i_0 = mx;
      j_0 = my;
      break;
    case GLUT_UP: 
      if (!mouse_button_down) 
        return;
      cout << "Left mouse button went up @ " << mx << " " << my << endl;

      //reset R_0 to identity, as the rotation has finished
      R_trackball_0 = mat4();

      //update R to include the latest rotation
      mat4 R_prime = rotation_matrix_for_point(mx, my);
      R_trackball = R_prime * R_trackball;

      glutPostRedisplay();

      mouse_button_down = false;
      break;
    }
    break;
  case GLUT_MIDDLE_BUTTON:
    switch( state ) {
    case GLUT_DOWN: 
      cout << "Middle mouse button pressed @ " << mx << " " << my << endl;
      mouse_button_down = true;
      break;
    case GLUT_UP:   
      if (!mouse_button_down) 
        return;
      cout << "Middle mouse button went up@ " << mx << " " << my << endl;
      mouse_button_down = false;
      break;
    }
    break;
  default:
    return;  // ignore anything else, e.g. right button pressed
  }

  // refresh the image - in your code, rendering parameters may be changed in mouse_button
  glutPostRedisplay();
}

GLvoid button_motion(GLint mx, GLint my)
{
  if (!mouse_button_down)
    return;   // mouse button not down - ignore!
  
  R_trackball_0 = rotation_matrix_for_point(mx, my);

  cout << "Mouse movement with some button down @ " << mx << " " << my << endl;
  // refresh the image - in your code, rendering parameters may be changed in button_motion

  glutPostRedisplay();
}

/* --------------------------------------------- */


/* menu callback */

void menu ( int value )
{
  static GLfloat stored_multiplier = 0;
  static int stored_dcounter;

  switch(value)
    {
    case MENU_ENVIRONMENT_TEX:
      program = MENU_ENVIRONMENT_TEX;
      break;
    case MENU_3D_TEX:
      program = MENU_3D_TEX;
      break;
    case MENU_TORRUS:
      program = MENU_TORRUS;
      break;
    case MENU_ZOOM_IN:
      fov_angle *= 0.8f;
      cout << "Viewing angle is " << fov_angle << endl;
      break;
    case MENU_ZOOM_OUT:
      fov_angle *= 1.2f;
      if (fov_angle > 89.f) {
        fov_angle = 89.f;
      }
      cout << "Viewing angle is " << fov_angle << endl;
      break;
    }

  // and again, in case any rendering paramters changed, redraw
  glutPostRedisplay();
}


/* --------------------------------------------- */

/* handle keyboard events; here, just exit if ESC is hit */

void keyboard(GLubyte key, GLint x, GLint y)
{
  switch(key) {
  case 27:  /* ESC */

    exit(0);

  default:  break;
  }
}

/* --------------------------------------------- */

/* handle resizing the glut window */

GLvoid reshape(GLint sizex, GLint sizey)
{
  glutSetWindow(wid);

  // keep the viewport square...
  vpw = sizex < sizey ? sizex : sizey;
  vph = vpw;

  glViewport(0, 0, vpw, vph);
  glutReshapeWindow(vpw, vph);

  glutPostRedisplay();
}

/* --------------------------------------------- */
/* -------- SET UP GLUT  ----------------------- */
/* --------------------------------------------- */

// initialize glut, callbacks etc.

GLint init_glut(GLint *argc, char **argv)
{
  GLint id;

  glutInit(argc,argv);

  /* size and placement hints to the window system */
  glutInitWindowSize(vpw, vph);
  glutInitWindowPosition(10,10);

  /* double buffered, RGB color mode */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

  /* create a GLUT window (not drawn until glutMainLoop() is entered) */
  id = glutCreateWindow("Zach Fleischman CSCI441 Project 3");    

  /* register callbacks */

  /* window size changes */
  glutReshapeFunc(reshape);

  /* keypress handling when the current window has input focus */
  glutKeyboardFunc(keyboard);

  /* mouse event handling */
  glutMouseFunc(mouse_button);           /* button press/release        */
  glutMotionFunc(button_motion);         /* mouse motion w/ button down */

  /* window obscured/revealed event handler */
  glutVisibilityFunc(NULL);

  /* handling of keyboard SHIFT, ALT, CTRL keys */
  glutSpecialFunc(NULL);

  /* what to do when mouse cursor enters/exits the current window */
  glutEntryFunc(NULL);

  /* what to do on each display loop iteration */
  glutDisplayFunc(draw);

  /* create menu */
  // you'll need to change this to build your menu
  GLint menuID = glutCreateMenu(menu);

  glutAddMenuEntry("Environment Mapping",MENU_ENVIRONMENT_TEX);
  glutAddMenuEntry("3D Texture",MENU_3D_TEX);
  glutAddMenuEntry("Torrus",MENU_TORRUS);
  glutAddMenuEntry("Zoom In",MENU_ZOOM_IN);
  glutAddMenuEntry("Zoom Out",MENU_ZOOM_OUT);

  glutSetMenu(menuID);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  return id;
}

/* --------------------------------------------- */
/* --------------------------------------------- */
/* --------------------------------------------- */

GLint main(GLint argc, char **argv)
{
  /* initialize GLUT: register callbacks, etc */
  wid = init_glut(&argc, argv);

  // initialize glew and check for OpenGL 4.2 support
  glewInit();
  if (glewIsSupported("GL_VERSION_4_0"))
    cout << "Ready for OpenGL 4.0" << endl;
  else 
    {
      cout << "OpenGL 4.0 not supported" << endl;;
      return 1;
    }

  // initialize programs and buffers
  setup_programs();
  setup_input_buffers();

  // setup textures
  setup_textures();

  // Main loop: keep processing events.
  // This is actually an indefinite loop - you can only exit it using 
  // brutal means like the exit() function in one of the event handlers.
  // In this code, the exit point is in the keyboard event handler.
  glutMainLoop();

  return 0;
}


/* --------------------------------------------- */
