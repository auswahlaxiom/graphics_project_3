#version 420

/* --------------- SAMPLER UNIFORMS ------------- */
/* The binding layout parameter needs to be the   */
/* texture attachment point (TAP) the texture     */
/* is attached to in the host code                */
/* ---------------------------------------------- */

layout (binding=1) uniform sampler2D tex;

/* --------------- INPUT VARIABLES -------------- */
/* In a fragment shader, attributes sent out with */
/* processed vertices in the vertex shader        */
/* and interpolated on the rasterization stage    */
/* ---------------------------------------------- */

in vec2 param;

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
  // look up color from texture
  //  in this case, we want to produce a border around our image
  //  this is why the second argument is not just param
  // param is of vec2 type and its values vary from (0,0) to (1,1) at two
  //  opposite corners of the square
  // The transformation 1.1*... -0.05 maps the coordinates from range 0...1
  //  to range -0.05 ... 1.05; since clamping to border is used for this
  //  texture (see host code), whenever one of the coordinates of the second
  //  argument is not in the 0...1 range, the border color is used 

  fragcolor = texture(tex,1.1*param-0.05).rgb;
}
