
#pragma once

namespace gl_CSCI441 {

  typedef enum { RGB, RGBA, Depth, RED }  TexType;


  class Texture {

    GLuint handle;
    int dim;
    int size[3];
    GLint internalfmt;
    GLenum fmt;
    GLenum type;
    bool finalized;
    GLenum tgt;
    GLenum attachedto;

  protected:
    void sendData( GLvoid *data = NULL );

    Texture ( TexType t, int x, int y, int z );  // 3D
    Texture ( TexType t, int x, int y );         // 2D
    Texture ( TexType t, int x );                // 1D
    ~Texture();

  public:

    // don't use - will generate an error
    Texture ( const Texture & );
    Texture & operator= ( const Texture & );
    
    void linear();
    void nearest();
    void clampToEdge();
    void clampToBorder();
    void setBorderColor ( GLfloat r, GLfloat g, GLfloat b );
    void repeat();

    void on();
    void off();
    void attach ( int TAP );  // attaches to a texture attachment point #i
  };

  class RGBTexture1D : public Texture
  {
  public:
    RGBTexture1D ( int x, unsigned char *ptr );
  };

  class RGBTexture2D : public Texture
  {
  public:
    RGBTexture2D ( int x, int y, unsigned char *ptr );
  };

  class RGBTexture3D : public Texture 
  {
  public:
    RGBTexture3D ( int x, int y, int z, unsigned char *ptr );
  };

  class RGBATexture1D : public Texture
  {
  public:
    RGBATexture1D ( int x, unsigned char *ptr );
  };

  class RGBATexture2D : public Texture
  {
  public:
    RGBATexture2D ( int x, int y, unsigned char *ptr );
  };

  class RGBATexture3D : public Texture 
  {
  public:
    RGBATexture3D ( int x, int y, int z, unsigned char *ptr );
  };

  class RTexture1D : public Texture
  {
  public:
    RTexture1D ( int x, unsigned char *ptr );
  };

  class RTexture2D : public Texture
  {
  public:
    RTexture2D ( int x, int y, unsigned char *ptr );
  };

  class RTexture3D : public Texture 
  {
  public:
    RTexture3D ( int x, int y, int z, unsigned char *ptr );
  };


  /* ----------------------------------------------------- */

  // reads 3D texture from a raw binary file,
  // creates and returns a pointer to the resulting texture object
  // arguments: resolution and file name
  //  optional arguments: border color

  RGBTexture3D *createRGBTexture3D ( int x, int y, int z, const char *name, GLfloat r = 0.0, GLfloat g = 0.0, GLfloat b = 0.0 );

  /* ----------------------------------------------------- */
  
  // reads a 2D texture from a binary PPM file
  // creates and returns a pointer to the resulting texture object
  // The argument: file name (has to be PPM with no comment lines)
  //  optional arguments: border color

  extern RGBTexture2D *createRGBTexture2D ( const char *name, GLfloat r = 0.0, GLfloat g = 0.0, GLfloat b = 0.0 );
  
  /* ----------------------------------------------------- */

};
