
#include <GL/gl.h>

#pragma once


namespace gl_CSCI441 {

  /* ------------------------------------------- */

  typedef enum { Array, ElemArray }  BufType;

  /* ------------------------------------------- */

  class Buffer {
  
  private:
    GLuint handle; 
    BufType tp;
    GLenum type;
    int components;
    void Bind ( GLuint index );

  public:
    Buffer ( const Buffer &a );
    Buffer ( const int cs, const int size, const GLubyte * const data = NULL, const BufType t = Array );
    Buffer ( const int cs, const int size, const GLbyte * const data = NULL, const BufType t = Array );
    Buffer ( const int cs, const int size, const GLuint * const data = NULL, const BufType t = Array );
    Buffer ( const int cs, const int size, const GLint * const data = NULL, const BufType t = Array );
    Buffer ( const int cs, const int size, const GLshort * const data = NULL, const BufType t = Array );
    Buffer ( const int cs, const int size, const GLushort * const data = NULL, const BufType t = Array );
    Buffer ( const int cs, const int size, const GLfloat * const data = NULL, const BufType t = Array );

    ~Buffer();
    Buffer & operator= ( const Buffer & rhs );

    void setIndex ( int aix ) const;
    void on();
    void off();

    GLenum getType() const { return type; }
    GLuint getHandle() const { return handle; }
  };

  /* ------------------------------------------- */

  class IndexBuffer : public Buffer {
  public:
    IndexBuffer ( const int size, const GLubyte * const data = NULL );
    IndexBuffer ( const int size, const GLuint * const data = NULL );
    IndexBuffer ( const int size, const GLushort * const data = NULL );
    IndexBuffer ( const IndexBuffer & );
    IndexBuffer & operator= ( const IndexBuffer & );
  };

  /* ------------------------------------------- */

}
