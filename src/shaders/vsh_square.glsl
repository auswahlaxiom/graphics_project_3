#version 420

/* --------------- INPUT VARIABLES -------------- */
/* In a vertex shader, they are vertex attributes */
/* location=... informs the shader of the index   */
/* of the attribute - it has to be in agreement   */
/* with the first argument of attachAttribute()   */
/* in the CPU code                                */
/* ---------------------------------------------- */

layout (location=0) in vec2 model_coord;

/* -------------- OUTPUT VARIABLES -------------- */
/* Attributes of the processed vertices           */
/* Interpolated by the rasterizer and sent with   */
/* fragments to the fragment shader               */
/* ---------------------------------------------- */

// x- and y- coordinates scaled to 0...1
//  use default interpolation = perspectively correct
//  basically, interpolated values will be linear
//  functions varying from 0...1 on the 3D quad

out vec2 param;

/* ------------- UNIFORM VARIABLES -------------- */
/* This is `global state' that every invocation   */
/* of the shader has access to.                   */
/* Note that these variables can also be declared */
/* in the fragment shader if necessary.           */
/* If the names are the same, the same value will */
/* be seen in both shaders.                       */
/* ---------------------------------------------- */

uniform mat4 MV;  // modelview matrix in homogenous coordinates
uniform mat4 P;  // projection matrix in homogenous coordinates

/* ---------------------------------------------- */
/* ----------- MAIN FUNCTION -------------------- */
/* goal: set gl_Position (location of the         */
/* projected vertex in homogenous coordinates)    */
/* and values of the output variables             */
/* ---------------------------------------------- */


void main()
{
  vec3 scaled_coords = vec3(0.8*model_coord,0);

  // vertices of the square scaled to 0...1

  param = 0.5*(-model_coord+vec2(1,1));


  // gl_Position holds the coordinates of the processed vertex
  //  in homogenous coordinates (vec4 type)

  gl_Position = P * MV * vec4(scaled_coords,1);
}
