#version 420

/* --------------- INPUT VARIABLES -------------- */
/* In a fragment shader, attributes sent out with */
/* processed vertices in the vertex shader        */
/* and interpolated on the rasterization stage    */
/* ---------------------------------------------- */

noperspective in vec3 interp_norm;
noperspective in vec3 interp_coord;


/* ------------- UNIFORM VARIABLES -------------- */
/* This is `global state' that every invocation   */
/* of the shader has access to.                   */
/* Note that these variables can also be declared */
/* in the fragment shader if necessary.           */
/* If the names are the same, the same value will */
/* be seen in both shaders.                       */
/* ---------------------------------------------- */

uniform vec3 LL;  // light location

uniform float LightIntensity;
uniform float N_Spec;

uniform vec3 Ambient;
uniform vec3 K_Diff;
uniform vec3 K_Spec;


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
  //Illumination total = I * ( kd*(N·L) + ks*(H·N)^n ) + kaIa
  vec3 L = normalize(LL - interp_coord);

  vec3 N = normalize(interp_norm);

  //viewpoint is (0,0,0), so V is negative world coord
  vec3 V = normalize(-vec3(interp_coord));

  vec3 H = (1.f / (length(V) + length(L))) * (V + L);

  // Set N dot L for fragment program
  float NdotL = (dot(N,L) > 0.0f ? dot(N,L) : 0.0f);
  float NdotH = (dot(N,H) > 0.0f ? dot(N,H) : 0.0f);

  fragcolor = vec3(LightIntensity * (NdotL * K_Diff + pow(NdotH, N_Spec) * K_Spec) + Ambient);
}
