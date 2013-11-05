
#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>
#include <cassert>
#include <iostream>
#include <cstdlib>

#include "vertexarray.h"

using namespace std;

/* ------------------------------------------- */

namespace gl_CSCI441 {

  /* ------------------------------------------- */

  VertexArray::VertexArray ( const VertexArray & rhs )
  {
    cerr << "Attempting to copy VertexArray object" << endl;
    exit(1);     
  }

  /* ------------------------------------------- */

  VertexArray & VertexArray::operator= ( const VertexArray & rhs )
  {
    cerr << "Attempting assignment of VertexArray object" << endl;
    exit(1);
  }

  /* ------------------------------------------- */

  int VertexArray::find ( int aix ) const 
  {
    for ( int i=0; i<ix.size(); i++ )
      if (ix[i]==aix)
	return i;
    return -1;
  }

  /* ------------------------------------------- */

  VertexArray::VertexArray()
    {
      glGenVertexArrays(1,&handle);
    }

  /* ------------------------------------------- */

  VertexArray::~VertexArray()
    {
      assert(glIsVertexArray(handle));
      glDeleteVertexArrays(1,&handle);
    }

  /* ------------------------------------------- */

  void VertexArray::attachAttribute ( const GLuint aix, const Buffer * b )
    {
      int where = find(aix);
      if (where==-1)
	{
	  ix.push_back(aix);
	  bf.push_back(b);
	  active.push_back(true);
	  where = active.size()-1;
	}
      else
	{
	  assert(ix[where]==aix);
	  bf[where] = b;
	  active[where] = true;
	}
      glBindVertexArray(handle);
      b->setIndex(aix);
      glEnableVertexAttribArray(aix);
      glBindVertexArray(0);
    }

  /* ------------------------------------------- */

  void VertexArray::on()
  {
    glBindVertexArray(handle);
  }

  /* ------------------------------------------- */

  void VertexArray::off()
  {
    glBindVertexArray(0);
  }

  /* ------------------------------------------- */

  void VertexArray::sendToPipeline ( GLenum ptype, int first, int num )
  {
    on();
    glDrawArrays(ptype,first,num);
    off();
  }

  /* ------------------------------------------- */

  void VertexArray::sendToPipelineIndexed ( GLenum ptype, IndexBuffer *b, int first, int num )
  {
    on();
    b->on();
    glDrawRangeElements(ptype,first,first+num,num,b->getType(),0);
    b->off();
    off();
  }

  /* ------------------------------------------- */
}
