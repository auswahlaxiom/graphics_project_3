
#include <GL/gl.h>
#include "shader.h"

#pragma once

/* ------------------------------------------- */

namespace gl_CSCI441 {

  class Program {

  private:
    static const Program * laston;

    GLuint handle;
    const Shader *c, *e, *v, *f, *g;
    const Shader *cmp;
    bool one_warning_printed;

  public:
    Program ( const Program &a );   // will generate an error
    Program & operator= ( const Program & rhs );  // will generate an error

    Program ( const Shader *aa = NULL, const Shader *bb = NULL, const Shader *cc = NULL, const Shader *dd = NULL, const Shader *ee = NULL );

    ~Program();
    void on() const;
    void off() const;

    void setUniform ( const GLchar *name, GLint, GLint = 0, GLint = 0, GLint = 1 );
    void setUniform ( const GLchar *name, GLfloat, GLfloat = 0, GLfloat = 0, GLfloat = 1.0 );
    void setUniform ( const GLchar *name, GLdouble, GLdouble = 0, GLdouble = 0, GLdouble = 1.0 );
    void setUniform ( const GLchar *name, GLuint, GLuint = 0, GLuint = 0, GLuint = 1 );

    void setUniform ( const GLchar *name, GLint *v, int count = 1 );
    void setUniform ( const GLchar *name, GLfloat *v, int count = 1 );
    void setUniform ( const GLchar *name, GLuint *v, int count = 1 );

    void printLog();

    operator bool();
  };

  

  // returns pointer to a program object with vertex shader and fragment shader code
  // read from files named vp and fp, respectively
  // prints out compiler and linker messages to stdout

  extern Program * createProgram ( const char *vp, const char *fp );

}

/* ------------------------------------------- */
