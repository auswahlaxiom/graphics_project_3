#version 420
#define PI 3.1415926535897932384626433832795
/* --------------- INPUT VARIABLES -------------- */
/* In a vertex shader, they are vertex attributes */
/* location=... informs the shader of the index   */
/* of the attribute - it has to be in agreement   */
/* with the first argument of attachAttribute()   */
/* in the CPU code                                */
/* ---------------------------------------------- */

// vertices in model coordinates - normalized from -1 to 1 in x, y, z
layout (location=0) in vec2 map_coord;



/* -------------- OUTPUT VARIABLES -------------- */
/* Attributes of the processed vertices           */
/* Interpolated by the rasterizer and sent with   */
/* fragments to the fragment shader               */
/* ---------------------------------------------- */

// interploated dot products
noperspective out vec3 interp_norm;
noperspective out vec3 interp_coord;

out vec2 tex_coord;


/* ------------- UNIFORM VARIABLES -------------- */
/* This is `global state' that every invocation   */
/* of the shader has access to.                   */
/* Note that these variables can also be declared */
/* in the fragment shader if necessary.           */
/* If the names are the same, the same value will */
/* be seen in both shaders.                       */
/* ---------------------------------------------- */


//Torrus constants
uniform float TOR_R;
uniform float TOR_r;

//Regular uniforms
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
  tex_coord = map_coord; //direct correlation between mapping and texture;

  float tri = 2.f*PI*map_coord.x;
  float phi = 2.f*PI*map_coord.y;

  float tor_x = cos(phi) * (TOR_R + TOR_r * cos(tri));
  float tor_y = sin(phi) * (TOR_R + TOR_r * cos(tri));
  float tor_z = TOR_r * sin(tri);

  vec4 coord = vec4(tor_x, tor_y, tor_z, 1.f);
  vec4 world_coord = MV * coord;

  //d_P / d_phi
  vec3 norm1 = vec3(
  	-sin(phi) * (TOR_R + TOR_r * cos(tri)),
  	cos(phi) * (TOR_R + TOR_r * cos(tri)),
  	0);
  //d_P / d_tri
  vec3 norm2 = vec3(
  	-1.f * TOR_r * sin(tri) * cos(phi),
  	-1.f * TOR_r * sin(tri) * sin(phi),
  	TOR_r * cos(tri));

  vec3 norm = normalize((NM * vec4(cross(norm1, norm2), 1.f)).xyz);



  interp_norm = normalize(norm);

  interp_coord = vec3(world_coord);

  // apply projection to location in the world coordinates
  // gl_Position is a built-in output variable of type vec4
  // note that NO DIVISION by the homogenous coordinate is done here - 
  //   this is what is supposed to happen (don't do it!)

  gl_Position = P * world_coord;
}
