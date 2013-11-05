
#include <vector>
#include "buffer.h"

#pragma once


/* ------------------------------------------- */

namespace gl_CSCI441 {

  class VertexArray {

  private:
    GLuint handle;
    std::vector<int> ix;
    std::vector<const Buffer*> bf;
    std::vector<bool> active;

    int find ( int aix ) const;

  public:

    VertexArray();
    ~VertexArray();

    VertexArray ( const VertexArray & );
    VertexArray & operator= ( const VertexArray & );

    void attachAttribute ( const GLuint aix, const Buffer * b );

    void on();
    void off();

    void sendToPipeline ( GLenum ptype, int first, int num );
    void sendToPipelineIndexed ( GLenum ptype, IndexBuffer *b, int first, int num );
  };
}
  

/* ------------------------------------------- */
