
#include <GL/glew.h>
#include <GL/gl.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <cstdlib>

#include <texture.h>

using namespace std;

namespace gl_CSCI441 {

  Texture::Texture ( const Texture & )
    {
      cerr << "Attempting to use copy constructor for a Texture object" << endl;
      exit(0);
    }

  Texture & Texture::operator= ( const Texture & )
    {
      cerr << "Attempting to use assignment operator for a Texture object" << endl;
      exit(0);
    }

  Texture::Texture ( TexType t, int x )
    {
      attachedto = GL_TEXTURE0;
      glGenTextures(1,&handle);
      dim = 1;
      size[0] = x;
      size[1] = size[2] = 0;
      finalized = false;
      tgt = GL_TEXTURE_1D;
      switch(t)
	{
	case RGB:
	  internalfmt = GL_RGB;
	  type = GL_UNSIGNED_BYTE;
	  fmt = GL_RGB;
	  break;
	case RGBA:
	  internalfmt = GL_RGBA;
	  fmt = GL_RGBA;
	  type = GL_UNSIGNED_BYTE;
	  break;
	case Depth:
	  internalfmt = GL_DEPTH_COMPONENT;
	  fmt = GL_DEPTH_COMPONENT;
	  type = GL_FLOAT;
	  break;
	case RED:
	  internalfmt = GL_R32F;
	  type = GL_FLOAT;
	  fmt = GL_RED;
	  break;
	default:
	  assert(0);
	}
    }

  Texture::Texture ( TexType t, int x, int y )
    {
      attachedto = GL_TEXTURE0;
      glGenTextures(1,&handle);
      dim = 2;
      size[0] = x;
      size[1] = y;
      size[2] = 0;
      finalized = false;
      tgt = GL_TEXTURE_2D;
      switch(t)
	{
	case RGB:
	  internalfmt = GL_RGB;
	  type = GL_UNSIGNED_BYTE;
	  fmt = GL_RGB;
	  break;
	case RGBA:
	  internalfmt = GL_RGBA;
	  fmt = GL_RGBA;
	  type = GL_UNSIGNED_BYTE;
	  break;
	case Depth:
	  internalfmt = GL_DEPTH_COMPONENT32;
	  fmt = GL_DEPTH_COMPONENT;
	  type = GL_UNSIGNED_INT;
	  break;
	case RED:
	  internalfmt = GL_R32F;
	  type = GL_FLOAT;
	  fmt = GL_RED;
	  break;
	default:
	  assert(0);
	}
    }

  Texture::Texture ( TexType t, int x, int y, int z )
    {
      attachedto = GL_TEXTURE0;
      glGenTextures(1,&handle);
      dim = 3;
      size[0] = x;
      size[1] = y;
      size[2] = z;
      finalized = false;
      tgt = GL_TEXTURE_3D;
      switch(t)
	{
	case RGB:
	  internalfmt = GL_RGB;
	  type = GL_UNSIGNED_BYTE;
	  fmt = GL_RGB;
	  break;
	case RGBA:
	  internalfmt = GL_RGBA;
	  fmt = GL_RGBA;
	  type = GL_UNSIGNED_BYTE;
	  break;
	case Depth:
	  internalfmt = GL_DEPTH_COMPONENT;
	  fmt = GL_DEPTH_COMPONENT;
	  type = GL_FLOAT;
	  break;
	case RED:
	  internalfmt = GL_R32F;
	  type = GL_FLOAT;
	  fmt = GL_RED;
	  break;
	default:
	  assert(0);
	}      
    }

  Texture::~Texture()
    {
      glDeleteTextures(1,&handle);
    }

  void Texture::sendData ( GLvoid *data )
  {
    finalized = true;
    glActiveTexture(attachedto);
    glBindTexture(tgt,handle);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    switch(dim)
      {
      case 1:
	glTexImage1D(GL_TEXTURE_1D,0,internalfmt,size[0],0,fmt,type,data);
	break;
      case 2:
	glTexImage2D(GL_TEXTURE_2D,0,internalfmt,size[0],size[1],0,fmt,type,data);
	break;
      case 3:
	glTexImage3D(GL_TEXTURE_3D,0,internalfmt,size[0],size[1],size[2],0,fmt,type,data);
	break;
      default:
	assert(0);
      }
  }

  void Texture::linear()
  {
    glActiveTexture(attachedto);
    glBindTexture(tgt,handle);
    glTexParameteri(tgt,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(tgt,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindTexture(tgt,0);
  }

  void Texture::nearest()
  {
    glActiveTexture(attachedto);
    glBindTexture(tgt,handle);
    glTexParameteri(tgt,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(tgt,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glBindTexture(tgt,0);
  }

  void Texture::repeat()
  {
    glActiveTexture(attachedto);
    glBindTexture(tgt,handle);
    glTexParameteri(tgt,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(tgt,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(tgt,GL_TEXTURE_WRAP_R,GL_REPEAT);
    glBindTexture(tgt,0);
  }

  void Texture::clampToEdge()
  {
    glActiveTexture(attachedto);
    glBindTexture(tgt,handle);
    glTexParameteri(tgt,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(tgt,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(tgt,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
    glBindTexture(tgt,0);
  }

  void Texture::clampToBorder()
  {
    glActiveTexture(attachedto);
    glBindTexture(tgt,handle);
    glTexParameteri(tgt,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
    glTexParameteri(tgt,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
    glTexParameteri(tgt,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_BORDER);
    glBindTexture(tgt,0);
  }

  void Texture::setBorderColor ( GLfloat r, GLfloat g, GLfloat b )
  {
    GLfloat rgb[3] = { r, g, b };
    glActiveTexture(attachedto);
    glBindTexture(tgt,handle);
    glTexParameterfv(tgt,GL_TEXTURE_BORDER_COLOR,rgb);
    glBindTexture(tgt,0);
  }

  void Texture::on()
  {
    glActiveTexture(attachedto);
    glBindTexture(tgt,handle);
    if (!finalized)
      {
	switch(dim)
	  {
	  case 1:
	    glTexImage1D(GL_TEXTURE_1D,0,internalfmt,size[0],0,fmt,type,NULL);
	    break;
	  case 2:
	    glTexImage2D(GL_TEXTURE_2D,0,internalfmt,size[0],size[1],0,fmt,type,NULL);
	    break;
	  case 3:
	    glTexImage3D(GL_TEXTURE_3D,0,internalfmt,size[0],size[1],size[2],0,fmt,type,NULL);
	    break;
	  default:
	    assert(0);
	  }
	finalized = true;
      }
  }

  void Texture::off()
  {
    glActiveTexture(attachedto);
    glBindTexture(tgt,0);
  }

  void Texture::attach ( int i )
  {
    glActiveTexture(attachedto = GL_TEXTURE0+i);
    glBindTexture(tgt,handle);
  }

  RGBATexture1D::RGBATexture1D ( int x, unsigned char *p ) : Texture(RGBA,x)
    {
      linear();
      sendData(p);
    }
  RGBTexture1D::RGBTexture1D ( int x, unsigned char *p ) : Texture(RGB,x)
    {
      linear();
      sendData(p);
    }
  RTexture1D::RTexture1D ( int x, unsigned char *p ) : Texture(RED,x)
    {
      linear();
      sendData(p);
    }

  RGBATexture2D::RGBATexture2D ( int x, int y, unsigned char *p ) : Texture(RGBA,x)
    {
      linear();
      sendData(p);
    }
  RGBTexture2D::RGBTexture2D ( int x, int y, unsigned char *p ) : Texture(RGB,x,y)
    {
      linear();
      sendData(p);
    }
  RTexture2D::RTexture2D ( int x, int y, unsigned char *p ) : Texture(RED,x,y)
    {
      linear();
      sendData(p);
    }

  RGBATexture3D::RGBATexture3D ( int x, int y, int z, unsigned char *p ) : Texture(RGBA,x,y,z)
    {
      linear();
      sendData(p);
    }
  RGBTexture3D::RGBTexture3D ( int x, int y, int z, unsigned char *p ) : Texture(RGB,x,y,z)
    {
      linear();
      sendData(p);
    }
  RTexture3D::RTexture3D ( int x, int y, int z, unsigned char *p ) : Texture(RED,x,y,z)
    {
      linear();
      sendData(p);
    }

  /* ----------------------------------------------------- */

  // reads 3D texture from a raw binary file,
  // creates and returns a pointer to the resulting texture object
  // arguments: resolution and file name

  RGBTexture3D *createRGBTexture3D ( int x, int y, int z, const char *name, GLfloat r, GLfloat g, GLfloat b )
  {
    ifstream ifs(name,ios::binary);
    GLubyte *vox = new GLubyte[3*x*y*z];
    ifs.read((char*)vox,3*x*y*z*sizeof(GLubyte));
    RGBTexture3D *res = new RGBTexture3D(x,y,z,vox);
    res->setBorderColor(r,g,b);
    delete[] vox;
    return res;
  }

  /* ----------------------------------------------------- */

  // reads a 2D texture from a binary PPM file
  // creates and returns a pointer to the resulting texture object
  // The argument: file name (has to be PPM with no comment lines)
  
  RGBTexture2D *createRGBTexture2D ( const char *name, GLfloat r, GLfloat g, GLfloat b )
  {
    ifstream ifs(name,ios::binary);
    char c;
    ifs >> c;
    assert(c=='P');
    ifs >> c;
    assert(c=='6');
    int x,y;
    ifs >> x >> y;
    int comps;
    ifs >> comps;
    assert(comps==255);
    GLubyte *pix = new GLubyte[3*x*y];
    ifs.get();
    ifs.read((char*)pix,3*x*y*sizeof(GLubyte));
    RGBTexture2D *res = new RGBTexture2D(x,y,pix);
    res->setBorderColor(r,g,b);
    delete[] pix;
    return res;
  }

};
