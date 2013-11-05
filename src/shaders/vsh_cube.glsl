#version 420

/* --------------- INPUT VARIABLES -------------- */
/* In a vertex shader, they are vertex attributes */
/* location=... informs the shader of the index   */
/* of the attribute - it has to be in agreement   */
/* with the first argument of attachAttribute()   */
/* in the CPU code                                */
/* ---------------------------------------------- */

// vertices in model coordinates - recall the cube extends from 0 to 1
layout (location=0) in vec3 modelCoord;

// ID of the face a vertex belongs to
layout (location=1) in uint faceId;


/* -------------- OUTPUT VARIABLES -------------- */
/* Attributes of the processed vertices           */
/* Interpolated by the rasterizer and sent with   */
/* fragments to the fragment shader               */
/* ---------------------------------------------- */

// we pass the face ID to the fragment shader
// flat means flat interpolation request - this is the only one
// that makes sense for integer variables

flat out uint face_id;

// we also pass on the original model coordinates in this
//  variable - this value will be used as texture coordinate
//  in the fragment shader

out vec3 tcoord;

/* ------------- UNIFORM VARIABLES -------------- */
/* This is `global state' that every invocation   */
/* of the shader has access to.                   */
/* Note that these variables can also be declared */
/* in the fragment shader if necessary.           */
/* If the names are the same, the same value will */
/* be seen in both shaders.                       */
/* ---------------------------------------------- */

uniform mat4 MV;  // modelview matrix in homogenous coordinates
uniform mat4 P;   // projection matrix in homogenous coordinates
uniform vec2 T;   // translation along xy-plane in model coordinates

/* ---------------------------------------------- */
/* ----------- MAIN FUNCTION -------------------- */
/* goal: set gl_Position (location of the         */
/* projected vertex in homogenous coordinates)    */
/* and values of the output variables             */
/* ---------------------------------------------- */

void main()
{
  // pass face ID to fragment shader
  // note that flat interpolation is used - anything other than that
  // does not make sense for integer attributes

  face_id = faceId;

  // set the texture coordinate to model coordinates

  tcoord = modelCoord;


  // Explanation of the next line:
  //  ... -vec(0.5,0.5,0.5) ... : translate the cube to put its center into (0,0,0)
  //  ... 0.4* ... : scale to a smaller size
  //  ... +vec3(T,0)... : translate by (Tx,Ty,0) - to one of the corners of the colorful square
  //  vec4(...) : we need to add homogenous coordinate of 1.0; basically, to convert 3d->3d+homogenous
  //  MV * ... : reminder of the transform to world coordinates
  //             rotate about an axis living in xy-plane and translate `forward' (see C++ code)

  vec4 worldCoord = MV * vec4(0.4*(modelCoord-vec3(0.5,0.5,0.5))+vec3(T,0),1.0);



  // apply projection to location in the world coordinates
  // gl_Position is a built-in output variable of type vec4
  // note that NO DIVISION by the homogenous coordinate is done here - 
  //   this is what is supposed to happen (don't do it!)

  gl_Position = P * worldCoord;
}
