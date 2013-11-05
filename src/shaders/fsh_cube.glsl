#version 420

/* --------------- SAMPLER UNIFORMS ------------- */
/* The binding layout parameter needs to be the   */
/* texture attachment point (TAP) the texture     */
/* is attached to in the host code                */
/* ---------------------------------------------- */

layout (binding=2) uniform sampler3D tex;

/* --------------- INPUT VARIABLES -------------- */
/* In a fragment shader, attributes sent out with */
/* processed vertices in the vertex shader        */
/* and interpolated on the rasterization stage    */
/* ---------------------------------------------- */

flat in uint face_id;
in vec3 tcoord;

/* ----------- OUTPUT VARIABLES ----------------- */
/* For `simple' rendering we do here, there is    */ 
/* just one: RGB value for the fragment           */
/* ---------------------------------------------- */

out vec3 fragcolor;

/* ---------------------------------------------- */
/* ----------- MAIN FUNCTION -------------------- */
/* goal: compute the color of the fragment        */
/*  [put it into the only output variable]        */
/* ---------------------------------------------- */

void main()
{
  // look up color from texture using tcoord as the texture coordinate
  //  note that if tex is a 3D sampler, the texture coordinate has to be of 
  //  vec3 type

  fragcolor = texture(tex,tcoord).rgb;
}
