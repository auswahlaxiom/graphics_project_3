
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

using namespace gl_CSCI441;
using namespace std;
using namespace glm;

/* ----------------------------------------------------- */

/* --------------------------------------------- */
/* ----- GLOBAL VARIABLES  --------------------- */
/* --------------------------------------------- */

static const GLint VPD_DEFAULT = 800;
static GLint wid;               /* GLUT window id; value asigned in main() and should stay constant */
static GLint vpw = VPD_DEFAULT; /* viewport dimensions; changed when window is resized (resize callback) */
static GLint vph = VPD_DEFAULT;

/* ----------------------------------------------------- */

// Here are desired buffer contents for a square extending from -1 to 1 in x and y.
// The square will be rendered using index buffer.
// Note that we are specifying only x and y coordinates,
// but the z-coordinate will be added on the vertex processing stage.

GLfloat square_vertices[] = {
  -1, -1,
  -1,  1,
   1,  1,
   1, -1,
};

// Vertex indices.
// In this case, it would be easy to form a triangle strip 
// using only 4 vertex indices, but in the project you'll be using 
// triangle soup, so this is what we'll do here.

// IMPORTANT: you MUST use an unsigned type for index buffer data.

GLuint square_indices[] = {
  0, 1, 2,   // indices into the vertices of the first triangle
  0, 2, 3    // ... and second triangle (with consistent orientation)
};

/* ----------------------------------------------------- */

// Data for a cube extending from 0 to 1 in x y z.
// The cube will be rendered using plain vertex array.

GLfloat cube_vertices[] = {

  0,1,0,  0,0,0,  0,0,1,
  0,1,0,  0,0,1,  0,1,1,  // face 000 - 010 - 001 - 011

  0,0,1,  0,0,0,  1,0,0,
  0,0,1,  1,0,0,  1,0,1,  // face 000 - 100 - 001 - 101

  0,0,0,  0,1,0,  1,1,0,
  0,0,0,  1,1,0,  1,0,0,  // another face ....

  1,0,1,  1,0,0,  1,1,0,
  1,1,0,  1,1,1,  1,0,1,

  1,1,0,  0,1,0,  0,1,1,
  0,1,1,  1,1,1,  1,1,0,

  1,1,1,  0,1,1,  0,0,1,
  0,0,1,  1,0,1,  1,1,1
};
  
// This is an attribute that represents which face a vertex belongs to.
// In our triangulation of the cube, we render any of its
// faces as two triangles; thus first six vertices in the 
// cube_vertices array above represent the 0th face;
// the following six vertices represent 1st face etc.

// Alternatively, this could be done using the GLSL built-in
// variable gl_PrimitiveId, http://www.opengl.org/sdk/docs/manglsl/xhtml/gl_PrimitiveID.xml
// but this is a more flexible approach that you can use to pass
// other information from C code to the GPU code.

GLubyte cube_faceId[] = {
  0,0,0,0,0,0,
  1,1,1,1,1,1,
  2,2,2,2,2,2,
  3,3,3,3,3,3,
  4,4,4,4,4,4,
  5,5,5,5,5,5
};

/* ----------------------------------------------------- */

// all buffer and program objects used 

VertexArray *va_square = NULL;
Buffer *buf_square_vertices = NULL;
IndexBuffer *ix_square = NULL;

VertexArray *va_cube = NULL;
Buffer *buf_cube_vertices = NULL;
Buffer *buf_cube_faceId = NULL;

Program *square_program = NULL;
Program *cube_program = NULL;

// texture objects

RGBTexture2D *t2D = NULL;
RGBTexture3D *t3D = NULL;

/* ----------------------------------------------------- */

void setup_textures()
{
  // reads a PPM image from file and create a texture object
  //  last three values are border color (in the project, there is no need to use borders)
  t2D = createRGBTexture2D("textures/mines.ppm",0.5,0.25,0.25);
  t2D->linear();        // request bilinear interpolation
  t2D->clampToBorder(); // use border color if texture coordinates out of range
  t2D->attach(1);       // attach to TAP #1; must match the binding parameter in the shaders
                        // see `layout (binding=1) uniform sampler2D tex;' in fsh_square.glsl
  t2D->on();            // turn on the texture; has to be called after any parameter change - 
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

}

/* ----------------------------------------------------- */

void setup_buffers()
{
  // Fhe first argument to the Buffer constructor is the number of 
  // components per vertex (basically, numbers per vertex)
  // For square, vertices are 2D - they have 2 coordinates; hence 
  //  the number of components is 2, and there are 4 vertices.
  // The last argument is a pointer to the actual vertex data.
  buf_square_vertices = new Buffer(2,4,square_vertices);

  // this is the way to construct index buffers...
  // 6 is the size of the buffer.
  ix_square = new IndexBuffer(6,square_indices);

  // build the cube buffers now...
  buf_cube_vertices = new Buffer(3,36,cube_vertices);
  buf_cube_faceId = new Buffer(1,36,cube_faceId);
  
  // construct the square VA
  va_square = new VertexArray;

  // vertices are attribute #0
  va_square->attachAttribute(0,buf_square_vertices);

  // same for cube...
  va_cube = new VertexArray;
  // vertices are attribute #0
  va_cube->attachAttribute(0,buf_cube_vertices);
  // vertices are attribute #1
  va_cube->attachAttribute(1,buf_cube_faceId);
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

  cout << "Creating the cube program..." << endl;
  cube_program = createProgram("shaders/vsh_cube.glsl","shaders/fsh_cube.glsl");
  cout << "Creating the square program..." << endl;
  square_program = createProgram("shaders/vsh_square.glsl","shaders/fsh_square.glsl");
}

/* ----------------------------------------------------- */

bool animate = true;    // animate or not
float multiplier = 1.0; // controls rotation speed
int dcounter = 1;       // used to increment the frame counter (set to zero to freeze)

/* ----------------------------------------------------- */

void draw()
{
  // ensure we're drawing to the correct GLUT window 
  glutSetWindow(wid);

  // frame counter for zoom animation
  static int counter = 0;
  counter += dcounter;

  // rotation angle
  static GLfloat angle = 0.0;
  angle += multiplier;

  // clear buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // want to disable culling for square - it's not a watertight surface
  //  what would happen if you enable it?
  glDisable(GL_CULL_FACE);

  // want to use depth test to get visibility right
  glEnable(GL_DEPTH_TEST);

  // Compute projection matrix; perspective() is a glm function
  // Arguments: field of view in DEGREES(!), aspect ratio (1 if square window), distance to front and back clipping plane
  // Camera is located at the origin and points along -Z direction
  mat4 P = perspective(float(1.0+0.5*sin(counter/100.0))*40.0f,1.0f,3.0f,7.0f);

  // we'll use this as rotation component of the modelview matrix
  mat4 R = rotate(mat4(),angle,vec3(1.0f,2.0f,0.0f));

  // this is technically a part of the modelview matrix - it rotates around the axis [0,0,20]
  //  and then moves "forward", i.e. along -Z, by 20 units
  mat4 MV = translate(mat4(),vec3(0.0f,0.0f,-5.0f)) * R;

  // send matrices P and MV into uniform variables of the program used to render square
  // &P[0][0] is the pointer to the entries of matrix P, same for MV
  // Note that it's a coincidence that uniform names are the same as CPU code variable names 
  //    - they don't have to be the same
  square_program->setUniform("P",&P[0][0]);
  square_program->setUniform("MV",&MV[0][0]);

  // turn on the square program...
  square_program->on();

  // Send vertices 0...5 to pipeline; use the index buffer ix_square.
  // Recall that ix_square contains 0 1 2 0 2 3, which means that 
  // vertices with data at indices 0 1 2 0 2 3 in the buffers attached to the 
  // vertex array are going to be generated.
  // The first argument instructs the pipeline how to set up triangles; GL_TRIANGLES=triangle soup
  va_square->sendToPipelineIndexed(GL_TRIANGLES,ix_square,0,6);

  // turn the program off
  square_program->off();

  // OK to enable culling now: we'll be drawing cubes
  glEnable(GL_CULL_FACE);

  // We must to send the matrices again: this time to the cube program.
  // Different programs generally maintain independent sets of uniforms.
  // A more elegant and less wasteful way to do this could be based on GLSL subroutines.
  cube_program->setUniform("MV",&MV[0][0]);
  cube_program->setUniform("P",&P[0][0]);

  // Turn on cube program
  cube_program->on();

  // send translation values - this will move the cube so that it is centered at the center of the square
  // Note that you can also send a 3D vector to a uniform vec3 type variable using the setUniform method.
  //  Just use 3 values instead of 2 to do that.
  cube_program->setUniform("T",0.8f,0.8f);

  // Send vertices 0...36 to the pipeline. In this case, we use `plain' rendering with no index
  // This means that 36 vertices are going to be formed from contents of the buffers attached 
  // to the vertex array va_cube 
  va_cube->sendToPipeline(GL_TRIANGLES,0,36);
  
  // ... now render three cubes centered at the other vertices of the square
  cube_program->setUniform("T",-0.8f,0.8f);
  va_cube->sendToPipeline(GL_TRIANGLES,0,36);

  cube_program->setUniform("T",0.8f,-0.8f);
  va_cube->sendToPipeline(GL_TRIANGLES,0,36);

  cube_program->setUniform("T",-0.8f,-0.8f);
  va_cube->sendToPipeline(GL_TRIANGLES,0,36);

  // turn off program
  cube_program->off();
 
  // make sure all the stuff is drawn
  glFlush();

  // this exchanges the invisible back buffer with the visible buffer to 
  //  avoid refresh artifacts
  glutSwapBuffers();

  // Request another call to draw() in the "animation" mode.
  // Note that in the viewer you'll be implementing you may not need this - 
  //  all you have to do is to make sure the image is redrawn when rendering parameters change,
  //  e.g. as a result of mouse events.
  if (animate) 
    glutPostRedisplay();
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

void mouse_button(GLint btn, GLint state, GLint mx, GLint my)
{
  switch( btn ) {
  case GLUT_LEFT_BUTTON:
    switch( state ) {
    case GLUT_DOWN: 
      cout << "Left mouse button pressed @ " << mx << " " << my << endl;
      mouse_button_down = true;
      break;
    case GLUT_UP: 
      if (!mouse_button_down) 
	return;
      cout << "Left mouse button went up @ " << mx << " " << my << endl;
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

  cout << "Mouse movement with some button down @ " << mx << " " << my << endl;

  // refresh the image - in your code, rendering parameters may be changed in button_motion

  glutPostRedisplay();
}

/* --------------------------------------------- */

/* menu callback */

// you'll need to change this one as well...

static const int MENU_SLOWER = 1;
static const int MENU_FASTER = 2;
static const int MENU_STOP_RUN = 3;

void menu ( int value )
{
  static GLfloat stored_multiplier = 0;
  static int stored_dcounter;

  switch(value)
    {
    case MENU_SLOWER:
      multiplier *= 0.6;
      break;
    case MENU_FASTER:
      multiplier *= 1.4;
      break;
    case MENU_STOP_RUN:
      animate = !animate;
      
      // this is to make sure that when animation is off, things don't move even if
      //  glutPostRedisplay comes up somewhere (e.g. in a mouse event)
      swap(dcounter,stored_dcounter);
      swap(multiplier,stored_multiplier);

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
    
    // clean up and exit
    // you may remove these deletes and let the OS do the work

    delete va_square;
    delete ix_square;
    delete va_cube;
    delete square_program;
    delete cube_program;
    delete buf_square_vertices;
    delete buf_cube_vertices;
    delete buf_cube_faceId;
    delete t2D;
    delete t3D;

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
  id = glutCreateWindow("MACS441 OpenGL Sample code");    

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
  glutAddMenuEntry("slower",MENU_SLOWER);
  glutAddMenuEntry("faster",MENU_FASTER);
  glutAddMenuEntry("stop/run",MENU_STOP_RUN);
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
  setup_buffers();

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
