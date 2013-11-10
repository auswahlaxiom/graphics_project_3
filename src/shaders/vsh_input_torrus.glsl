#version 420

/* --------------- INPUT VARIABLES -------------- */
/* In a vertex shader, they are vertex attributes */
/* location=... informs the shader of the index   */
/* of the attribute - it has to be in agreement   */
/* with the first argument of attachAttribute()   */
/* in the CPU code                                */
/* ---------------------------------------------- */

// vertices in model coordinates - normalized from -1 to 1 in x, y, z
layout (location=0) in vec3 coord;

// Normals
layout (location=1) in vec3 norm;


/* -------------- OUTPUT VARIABLES -------------- */
/* Attributes of the processed vertices           */
/* Interpolated by the rasterizer and sent with   */
/* fragments to the fragment shader               */
/* ---------------------------------------------- */

// interploated dot products
noperspective out vec3 interp_norm;
noperspective out vec3 interp_coord;


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
uniform mat4 NM;  // normal matrix 
uniform vec3 LL;  // light location


/* ---------------------------------------------- */
/* ----------- MAIN FUNCTION -------------------- */
/* goal: set gl_Position (location of the         */
/* projected vertex in homogenous coordinates)    */
/* and values of the output variables             */
/* ---------------------------------------------- */

void main()
{
  vec4 worldCoord = MV * vec4(coord,1.0);

  interp_norm = normalize( -(NM * vec4(norm, 1.0)).xyz );

  interp_coord = vec3(worldCoord);

  // apply projection to location in the world coordinates
  // gl_Position is a built-in output variable of type vec4
  // note that NO DIVISION by the homogenous coordinate is done here - 
  //   this is what is supposed to happen (don't do it!)

  gl_Position = P * worldCoord;
}
