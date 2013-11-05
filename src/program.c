
#include <GL/glew.h>
#include <GL/gl.h>
#include <assert.h>
#include <iostream>
#include <cstdlib>
#include <cstring>

#include "program.h"

using namespace std;

/* ------------------------------------------- */

namespace  gl_CSCI441 {

  const Program * Program::laston = NULL;

/* ------------------------------------------- */

  Program::Program ( const Program &a )
    {
      cerr << "Attempting to copy Program object" << endl;
      exit(1);
    }

/* ------------------------------------------- */

  Program::Program ( const Shader *aa, const Shader *bb, const Shader *cc, const Shader *dd, const Shader *ee ) :
    c(NULL), e(NULL), v(NULL), f(NULL), g(NULL), one_warning_printed(false)
    {
      const Shader *p[5] = {aa,bb,cc,dd,ee};
      handle = glCreateProgram();
      for ( int i=0; i<5; i++ )
	if (p[i])
	  {
	    switch(p[i]->getType())
	      {
	      case Vert : 
		v = p[i];
		break;
	      case Frag:
		f = p[i];
		break;
	      case Geom:
		g = p[i];
		break;
	      case Eval:
		e = p[i];
		break;
	      case TessCtr:
		c = p[i];
		break;
	      default:
		exit(1);
	      }
	    glAttachShader(handle,p[i]->getHandle());
	  }
      glLinkProgram(handle);
    }

/* ------------------------------------------- */

  Program & Program::operator= ( const Program & rhs )
    {
      cerr << "Attempting assignment for a Program object" << endl;
      exit(1);
    }

/* ------------------------------------------- */

  Program::~Program()
    {
      if (!handle)
	return;
      assert(glIsProgram(handle));
      glDeleteProgram(handle);
    }

/* ------------------------------------------- */

  void Program::on() const
  {
    laston = this;
    glUseProgram(handle);
  }

/* ------------------------------------------- */

  void Program::off() const
  {
    laston = NULL;
    glUseProgram(0);
  }

/* ------------------------------------------- */

  void Program::setUniform ( const GLchar *name, GLint x, GLint y, GLint z, GLint h )
  {
    GLchar uname[256];
    GLenum type;
    GLint size;
    GLsizei length;

    GLint loc = glGetUniformLocation(handle,name);
    if (loc==-1)
      {
	if (one_warning_printed==false)
	   cout << "No uniform variable " << name << " found" << endl;
        one_warning_printed = true;
	return;
      }

    glGetActiveUniform(handle,loc,255,&length,&size,&type,uname);

    glUseProgram(handle);
    switch(type)
      {
      case GL_INT:
	glUniform1i(loc,x);
	break;
      case GL_INT_VEC2:
	glUniform2i(loc,x,y);
	break;
      case GL_INT_VEC3:
	glUniform3i(loc,x,y,z);
	break;
      case GL_INT_VEC4:
	glUniform4i(loc,x,y,z,h);
	break;
      default:
	cout << "Type mismatch: trying to put values of type GLint into uniform variable " << name << endl;
	exit(1);
      }
    if (laston)
      laston->on();
  }

/* ------------------------------------------- */

  void Program::setUniform ( const GLchar *name, GLuint x, GLuint y, GLuint z, GLuint h )
  {
    GLchar uname[256];
    GLenum type;
    GLint size;
    GLsizei length;

    GLint loc = glGetUniformLocation(handle,name);
    if (loc==-1)
      {
	if (one_warning_printed==false)
     	  cout << "No uniform variable " << name << " found" << endl;
        one_warning_printed = true;
	return;
      }

    glGetActiveUniform(handle,loc,255,&length,&size,&type,uname);

    glUseProgram(handle);
    switch(type)
      {
      case GL_UNSIGNED_INT:
	glUniform1ui(loc,x);
	break;
      case GL_UNSIGNED_INT_VEC2:
	glUniform2ui(loc,x,y);
	break;
      case GL_UNSIGNED_INT_VEC3:
	glUniform3ui(loc,x,y,z);
	break;
      case GL_UNSIGNED_INT_VEC4:
	glUniform4ui(loc,x,y,z,h);
	break;
      default:
	cout << "Type mismatch: trying to put values of type GLuint into uniform variable " << name << endl;
	exit(1);	
      }
    if (laston)
      laston->on();
  }

/* ------------------------------------------- */

  void Program::setUniform ( const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat h )
  {
    GLchar uname[256];
    GLenum type;
    GLint size;
    GLsizei length;

    GLint loc = glGetUniformLocation(handle,name);
    if (loc==-1)
      {
	if (one_warning_printed==false)
	  cout << "No uniform variable " << name << " found" << endl;
        one_warning_printed = true;
	return;
      }

    glGetActiveUniform(handle,loc,255,&length,&size,&type,uname);

    glUseProgram(handle);
    switch(type)
      {
      case GL_FLOAT:
	glUniform1f(loc,x);
	break;
      case GL_FLOAT_VEC2:
	glUniform2f(loc,x,y);
	break;
      case GL_FLOAT_VEC3:
	glUniform3f(loc,x,y,z);
	break;
      case GL_FLOAT_VEC4:
	glUniform4f(loc,x,y,z,h);
	break;
      default:
	cout << "Type mismatch: trying to put values of type GLfloat into uniform variable " << name << endl;
	exit(1);		
      }
    if (laston)
      laston->on();
  }

/* ------------------------------------------- */

  void Program::setUniform ( const GLchar *name, GLdouble x, GLdouble y, GLdouble z, GLdouble h )
  {
    GLchar uname[256];
    GLenum type;
    GLint size;
    GLsizei length;

    GLint loc = glGetUniformLocation(handle,name);
    if (loc==-1)
      {
	if (one_warning_printed==false)
	  cout << "No uniform variable " << name << " found" << endl;
        one_warning_printed = true;
	return;
      }

    glGetActiveUniform(handle,loc,255,&length,&size,&type,uname);

    glUseProgram(handle);
    switch(type)
      {
      case GL_FLOAT:
	glUniform1f(loc,x);
	break;
      case GL_FLOAT_VEC2:
	glUniform2f(loc,x,y);
	break;
      case GL_FLOAT_VEC3:
	glUniform3f(loc,x,y,z);
	break;
      case GL_FLOAT_VEC4:
	glUniform4f(loc,x,y,z,h);
	break;
      default:
	cout << "Type mismatch: trying to put values of type GLfloat into uniform variable " << name << endl;
	exit(1);			
      }
    if (laston)
      laston->on();
  }

/* ------------------------------------------- */

  void Program::setUniform ( const GLchar *name, GLint *v, int count )
  {
    GLchar uname[256];
    GLenum type;
    GLint size;
    GLsizei length;

    GLint loc = glGetUniformLocation(handle,name);
    if (loc==-1)
      {
	if (one_warning_printed==false)
	  cout << "No uniform variable " << name << " found" << endl;
        one_warning_printed = true;
	return;
      }

    glGetActiveUniform(handle,loc,255,&length,&size,&type,uname);

    glUseProgram(handle);
    switch(type)
      {
      case GL_INT:
	glUniform1iv(loc,count,v);
	break;
      case GL_INT_VEC2:
 	glUniform2iv(loc,count,v);
	break;
      case GL_INT_VEC3:
	glUniform3iv(loc,count,v);
	break;
      case GL_INT_VEC4:
	glUniform4iv(loc,count,v);
	break;
      default:
	cout << "Type mismatch: trying to put values of type GLint into uniform variable " << name << endl;
	exit(1);			
      }
    if (laston)
      laston->on();
  }

/* ------------------------------------------- */

  void Program::setUniform ( const GLchar *name, GLuint *v, int count )
  {
    GLchar uname[256];
    GLenum type;\
    GLint size;
    GLsizei length;

    GLint loc = glGetUniformLocation(handle,name);
    if (loc==-1)
      {
	if (one_warning_printed==false)
	  cout << "No uniform variable " << name << " found" << endl;
        one_warning_printed = true;
	return;
      }

    glGetActiveUniform(handle,loc,255,&length,&size,&type,uname);

    glUseProgram(handle);
    switch(type)
      {
      case GL_UNSIGNED_INT:
	glUniform1uiv(loc,count,v);
	break;
      case GL_UNSIGNED_INT_VEC2:
	glUniform2uiv(loc,count,v);
	break;
      case GL_UNSIGNED_INT_VEC3:
	glUniform3uiv(loc,count,v);
	break;
      case GL_UNSIGNED_INT_VEC4:
	glUniform4uiv(loc,count,v);
	break;
      default:
	cout << "Type mismatch: trying to put values of type GLuint into uniform variable " << name << endl;
	exit(1);			
      }
    if (laston)
      laston->on();
  }

/* ------------------------------------------- */

  void Program::setUniform ( const GLchar *name, GLfloat *v, int count )
  {
    GLchar uname[256];
    GLenum type;
    GLint size;
    GLsizei length;

    GLint loc = glGetUniformLocation(handle,name);
    if (loc==-1)
      {
	if (one_warning_printed==false)
	  cout << "No uniform variable " << name << " found" << endl;
        one_warning_printed = true;
	return;
      }

    glGetActiveUniform(handle,loc,255,&length,&size,&type,uname);

    glUseProgram(handle);
    switch(type)
      {
      case GL_FLOAT:
	glUniform1fv(loc,count,v);
	break;
      case GL_FLOAT_VEC2:
	glUniform2fv(loc,count,v);
	break;
      case GL_FLOAT_VEC3:
	glUniform3fv(loc,count,v);
	break;
      case GL_FLOAT_VEC4:
	glUniform4fv(loc,count,v);
	break;
      case GL_FLOAT_MAT2:
	glUniformMatrix2fv(loc,count,false,v);
	break;
      case GL_FLOAT_MAT3:
	glUniformMatrix3fv(loc,count,false,v);
	break;
      case GL_FLOAT_MAT4:
	glUniformMatrix4fv(loc,count,false,v);
	break;
      case GL_FLOAT_MAT2x3:
	glUniformMatrix2x3fv(loc,count,false,v);
	break;
      case GL_FLOAT_MAT2x4:
	glUniformMatrix2x4fv(loc,count,false,v);
	break;
      case GL_FLOAT_MAT3x2:
	glUniformMatrix3x2fv(loc,count,false,v);
	break;
      case GL_FLOAT_MAT3x4:
	glUniformMatrix3x4fv(loc,count,false,v);
	break;
      case GL_FLOAT_MAT4x2:
	glUniformMatrix4x2fv(loc,count,false,v);
	break;
      case GL_FLOAT_MAT4x3:
	glUniformMatrix4x3fv(loc,count,false,v);
	break;
      default:
	cout << "Type mismatch: trying to put values of type GLfloat into uniform variable " << name << endl;
	exit(1);			
      }
    if (laston)
      laston->on();
  }

/* ------------------------------------------- */

  void Program::printLog ( )
  {
    int infologLength = 0;
    int maxLength;
    
    glGetProgramiv(handle,GL_INFO_LOG_LENGTH,&maxLength);
    char infoLog[maxLength];
    glGetProgramInfoLog(handle, maxLength, &infologLength, infoLog);
  
    if (infologLength > 0)
      cout << infoLog << endl;
  }

/* --------------------------------------------- */

  Program::operator bool()
    {
      GLint status;
      glGetProgramiv(handle,GL_LINK_STATUS,&status);
      return (status==GL_TRUE);
    }

/* --------------------------------------------- */

  Program *createProgram ( const char *vp, const char *fp )
  {
    // read shaders from files
    string vps = ReadFromFile(vp);
    string fps = ReadFromFile(fp);
    
    // create shader objects
    Shader *vs = new Shader(Vert,vps);
    Shader *fs = new Shader(Frag,fps);
    
    // print compiler output
    cout << "Vertex shader info log: " << endl;
    vs->printLog();
    cout << "Fragment shader info log: " << endl;
    fs->printLog();
    if (!*vs || !*fs)
      {
	cout << "Compilation failed!" << endl;
	exit(1);
      }

    // create program object and link the two shaders...
    Program *res = new Program(vs,fs);

    // print linker output
    cout << "Program info log: " << endl;
    res->printLog();
    if (!*res)
      {
	cout << "Linking failed!" << endl;
	exit(1);
      }
    // Mark vertex and fragment shaders for deletion.
    // They will be deleted when all programs they belong to are.
    delete vs;
    delete fs;

    return res;
  }


};
