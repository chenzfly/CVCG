/* 2012-09-25 16:43:02 Zhuo.Chen	GLTexImage.cpp */
#include "GL/glew.h"
#include "GL/glu.h"
#include <cstdio>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <opencv/cv.h>
#include <opencv/highgui.h>
using namespace std;



#include "GLTexImage.h" 
//#include "LiteWindow.h"
#include "FrameBufferObject.h"



//#define SIFTGPU_NO_DEVIL

#ifndef SIFTGPU_NO_DEVIL
    #include "IL/il.h"
    #if  defined(_WIN64)
	    #pragma comment(lib, "DevIL64.lib")
    #elif  defined(_WIN32) 
	    #pragma comment(lib, "DevIL.lib")
    #endif
#endif

int GLTexImage::_texTarget = GL_TEXTURE_RECTANGLE_ARB;
int GLTexImage::_iTexFormat = GL_RGBA8;
int GLTexImage::_texMaxDim = 4096;

GLTexImage::GLTexImage()
{
	_imgWidth = _imgHeight = 0;
	_texWidth = _texHeight = 0;
	_drawWidth = _drawHeight = 0;
	_texID = 0;
//	_WindowInitX = -1;
//	_WindowInitY = -1;
//	_DeviceIndex = 0;
//	_WindowDisplay = NULL;

}

GLTexImage::~GLTexImage()
{
	if(_texID) glDeleteTextures(1, &_texID);
}

int GLTexImage::CheckTexture()
{
	if(_texID)
	{
		GLint tw, th;
		BindTex();
		glGetTexLevelParameteriv(_texTarget, 0, GL_TEXTURE_WIDTH , &tw);
		glGetTexLevelParameteriv(_texTarget, 0, GL_TEXTURE_HEIGHT , &th);
		UnbindTex();
		return tw == _texWidth && th == _texHeight;
	}else
	{
		return _texWidth == 0 && _texHeight ==0;

	}
}
//set a dimension that is smaller than the actuall size
//for drawQuad
void GLTexImage::SetImageSize( int width,  int height)
{
	_drawWidth  = _imgWidth =  width;
	_drawHeight = _imgHeight =  height;
}

void GLTexImage::CheckErrorsGL(const char* location)
{

	GLuint errnum;
	const char *errstr;
	while (errnum = glGetError())
	{
		errstr = (const char *)(gluErrorString(errnum));
		if(errstr)
		{
			std::cerr << errstr;
		}
		else
		{
			std::cerr  << "Error " << errnum;
		}

		if(location)
		{
			std::cerr  << " at " << location;
		}
		std::cerr  << "\n";
	}
	return;
}

void GLTexImage::InitTexture( int width,  int height, int clamp_to_edge)
{

	if(_texID && width == _texWidth && height == _texHeight ) return;
	if(_texID==0)	glGenTextures(1, &_texID); 

	_texWidth = _imgWidth = _drawWidth = width;
	_texHeight = _imgHeight = _drawHeight = height;

	BindTex();

	if(clamp_to_edge)
	{
		glTexParameteri (_texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri (_texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}else
	{
		//out of bound tex read returns 0??
		glTexParameteri (_texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri (_texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	}
	glTexParameteri(_texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(_texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTexImage2D(_texTarget, 0, GL_RGBA8,
		_texWidth, _texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	CheckErrorsGL("glTexImage2D");

	UnbindTex();

}

//int GLTexImage::CreateWindowEZ(LiteWindow* window)
//{
//	if(window == NULL) return 0;
//
//    if(!window->IsValid())window->Create(_WindowInitX, _WindowInitY, _WindowDisplay);
//    if(window->IsValid())
//    {
//        window->MakeCurrent();
//        return 1;
//    }
//    else
//    {
//
//        std::cerr << "Unable to create OpenGL Context!\n";
//		std::cerr << "For nVidia cards, you can try change to CUDA mode in this case\n";
//        return 0;
//    }
//}
//
//int GLTexImage::CreateWindowEZ()
//{
//	static LiteWindow window;
//
//    return CreateWindowEZ(&window);
//}

void GLTexImage::InitTexture( int width,  int height, int clamp_to_edge, GLuint format)
{

	if(_texID && width == _texWidth && height == _texHeight ) return;
	if(_texID==0)	glGenTextures(1, &_texID); 

	_texWidth = _imgWidth = _drawWidth = width;
	_texHeight = _imgHeight = _drawHeight = height;

	BindTex();

	if(clamp_to_edge)
	{
		glTexParameteri (_texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
		glTexParameteri (_texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
	}else
	{
		//out of bound tex read returns 0??
		glTexParameteri (_texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
		glTexParameteri (_texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); 
	}
	glTexParameteri(_texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(_texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTexImage2D(_texTarget, 0, format, _texWidth, _texHeight, 0, GL_RGBA, GL_FLOAT, NULL);

	UnbindTex();

}
void  GLTexImage::BindTex()
{
	glBindTexture(_texTarget, _texID);
}

void  GLTexImage::UnbindTex()
{
	glBindTexture(_texTarget, 0);
}


void  GLTexImage::DrawQuad()
{
	glBegin (GL_QUADS);
		glTexCoord2i ( 0			,   0   ); 				glVertex2i   ( 0			,		0   ); 
		glTexCoord2i ( 0			,   _drawHeight  );		glVertex2i   ( 0			,		_drawHeight   ); 
 		glTexCoord2i ( _drawWidth   ,   _drawHeight  ); 	glVertex2i   ( _drawWidth	,		_drawHeight   ); 
		glTexCoord2i ( _drawWidth	,   0   ); 				glVertex2i   ( _drawWidth	,		0   ); 
	glEnd ();
	glFlush();
}

void GLTexImage::DrawMargin(int right, int bottom)
{
	glBegin(GL_QUADS);
	if(right > _drawWidth)
	{
		glTexCoord2i ( _drawWidth	,   0   ); 				glVertex2i   ( _drawWidth	,		0   ); 
		glTexCoord2i ( _drawWidth	,   bottom  );			glVertex2i   ( _drawWidth	,		bottom   ); 
		glTexCoord2i ( right		,   bottom  ); 			glVertex2i   ( right		,		bottom   ); 
		glTexCoord2i ( right		,   0   ); 				glVertex2i   ( right		,		0   ); 	
	}
	if(bottom>_drawHeight)
	{
		glTexCoord2i ( 0			,   _drawHeight ); 		glVertex2i   ( 0			,		_drawHeight   ); 
		glTexCoord2i ( 0			,   bottom		);		glVertex2i   ( 0			,		bottom		 ); 
		glTexCoord2i ( _drawWidth	,   bottom		); 		glVertex2i   ( _drawWidth	,		bottom		 ); 
		glTexCoord2i ( _drawWidth	,   _drawHeight	); 		glVertex2i   ( _drawWidth	,		_drawHeight	 ); 
	}
	glEnd();
	glFlush();


}


void GLTexImage::DrawQuadMT4()
{
	int w = _drawWidth, h = _drawHeight;
	glBegin (GL_QUADS);
		glMultiTexCoord2i( GL_TEXTURE0, 0		,   0  ); 	
		glMultiTexCoord2i( GL_TEXTURE1, -1		,   0  ); 
		glMultiTexCoord2i( GL_TEXTURE2, 1		,   0  ); 
		glMultiTexCoord2i( GL_TEXTURE3, 0		,   -1  ); 
		glMultiTexCoord2i( GL_TEXTURE4, 0		,   1  ); 
		glVertex2i   ( 0			,		0   ); 

		glMultiTexCoord2i( GL_TEXTURE0, 0		,   h  ); 	
		glMultiTexCoord2i( GL_TEXTURE1, -1		,   h  ); 
		glMultiTexCoord2i( GL_TEXTURE2, 1		,   h ); 
		glMultiTexCoord2i( GL_TEXTURE3, 0		,   h -1 ); 
		glMultiTexCoord2i( GL_TEXTURE4, 0		,   h +1 ); 
		glVertex2i   ( 0			,		h   ); 


		glMultiTexCoord2i( GL_TEXTURE0, w		,   h  ); 	
		glMultiTexCoord2i( GL_TEXTURE1, w-1		,   h  ); 
		glMultiTexCoord2i( GL_TEXTURE2, w+1		,   h  ); 
		glMultiTexCoord2i( GL_TEXTURE3, w		,   h-1  ); 
		glMultiTexCoord2i( GL_TEXTURE4, w		,   h+1  ); 
		glVertex2i   ( w	,		h   ); 

		glMultiTexCoord2i( GL_TEXTURE0, w		,   0  ); 	
		glMultiTexCoord2i( GL_TEXTURE1, w-1		,   0  ); 
		glMultiTexCoord2i( GL_TEXTURE2, w+1		,   0  ); 
		glMultiTexCoord2i( GL_TEXTURE3, w		,   -1  ); 
		glMultiTexCoord2i( GL_TEXTURE4, w		,   1  ); 
		glVertex2i   ( w	,		0   ); 
	glEnd ();
	glFlush();
}


void GLTexImage::DrawQuadMT8()
{
	int w = _drawWidth;
	int h = _drawHeight;
	glBegin (GL_QUADS);
		glMultiTexCoord2i( GL_TEXTURE0, 0		,   0  ); 	
		glMultiTexCoord2i( GL_TEXTURE1, -1		,   0  ); 
		glMultiTexCoord2i( GL_TEXTURE2, 1		,   0  ); 
		glMultiTexCoord2i( GL_TEXTURE3, 0		,   -1  ); 
		glMultiTexCoord2i( GL_TEXTURE4, 0		,   1  ); 
		glMultiTexCoord2i( GL_TEXTURE5, -1		,   -1  ); 
		glMultiTexCoord2i( GL_TEXTURE6, -1		,   1  ); 
		glMultiTexCoord2i( GL_TEXTURE7, 1		,   -1  ); 
		glVertex2i   ( 0			,		0   ); 

		glMultiTexCoord2i( GL_TEXTURE0, 0		,   h    ); 	
		glMultiTexCoord2i( GL_TEXTURE1, -1		,   h    ); 
		glMultiTexCoord2i( GL_TEXTURE2, 1		,   h    ); 
		glMultiTexCoord2i( GL_TEXTURE3, 0		,   h  -1  ); 
		glMultiTexCoord2i( GL_TEXTURE4, 0		,   h  +1  ); 
		glMultiTexCoord2i( GL_TEXTURE5, -1		,   h  -1  ); 
		glMultiTexCoord2i( GL_TEXTURE6, -1		,   h  +1  ); 
		glMultiTexCoord2i( GL_TEXTURE7, 1		,   h  -1  ); 
		glVertex2i   ( 0			,		h   ); 


		glMultiTexCoord2i( GL_TEXTURE0, w		,   h    ); 	
		glMultiTexCoord2i( GL_TEXTURE1, w-1		,   h    ); 
		glMultiTexCoord2i( GL_TEXTURE2, w+1		,   h    ); 
		glMultiTexCoord2i( GL_TEXTURE3, w		,   h  -1  ); 
		glMultiTexCoord2i( GL_TEXTURE4, w		,   h  +1  ); 
		glMultiTexCoord2i( GL_TEXTURE5, w-1		,   h  -1  ); 
		glMultiTexCoord2i( GL_TEXTURE6, w-1		,   h  +1  ); 
		glMultiTexCoord2i( GL_TEXTURE7, w+1		,   h  -1  ); 
		glVertex2i   ( w	,		h   ); 

		glMultiTexCoord2i( GL_TEXTURE0, w		,   0  ); 	
		glMultiTexCoord2i( GL_TEXTURE1, w-1		,   0  ); 
		glMultiTexCoord2i( GL_TEXTURE2, w+1		,   0  ); 
		glMultiTexCoord2i( GL_TEXTURE3, w		,   -1  ); 
		glMultiTexCoord2i( GL_TEXTURE4, w		,   1  ); 
		glMultiTexCoord2i( GL_TEXTURE5, w-1		,   -1  ); 
		glMultiTexCoord2i( GL_TEXTURE6, w-1		,   1  ); 
		glMultiTexCoord2i( GL_TEXTURE7, w+1		,   -1  ); 
		glVertex2i   ( w	,		0   ); 
	glEnd ();
	glFlush();
}




void GLTexImage::DrawImage()
{
	DrawQuad();
}

void GLTexImage::FitViewPort(int width, int height)
{
	GLint port[4];
	glGetIntegerv(GL_VIEWPORT, port);
	if(port[2] !=width || port[3] !=height)
	{

		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, width, 0, height,  0, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

}

void GLTexImage::FitTexViewPort()
{
	FitViewPort(_drawWidth, _drawHeight);
}

void GLTexImage::FitRealTexViewPort()
{
	FitViewPort(_texWidth, _texHeight);
}

void  GLTexImage::AttachToFBO(int i)
{
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, i+GL_COLOR_ATTACHMENT0_EXT, _texTarget, _texID, 0 );
}

void  GLTexImage::DetachFBO(int i)
{
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, i+GL_COLOR_ATTACHMENT0_EXT, _texTarget, 0, 0 );
}


void GLTexImage::DrawQuad(float x1, float x2, float y1, float y2)
{

	glBegin (GL_QUADS);
		glTexCoord2f ( x1	,   y1   ); 	glVertex2f   ( x1	,		y1   ); 
		glTexCoord2f ( x1	,   y2  );		glVertex2f   ( x1	,		y2   ); 
 		glTexCoord2f ( x2   ,   y2  ); 		glVertex2f   ( x2	,		y2   ); 
		glTexCoord2f ( x2	,   y1   ); 	glVertex2f   ( x2	,		y1   ); 
	glEnd ();
	glFlush();
}

void GLTexImage::TexConvertRGB()
{
//	//change 3/22/09
//	FrameBufferObject fbo;
//	//GlobalUtil::FitViewPort(1, 1);
//	FitTexViewPort();
//
//	AttachToFBO(0);
//	ShaderMan::UseShaderRGB2Gray();
//	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
//	DrawQuad();
//	ShaderMan::UnloadProgram();
//	DetachFBO(0);
}

void GLTexImage::DrawQuadDS(float scale)
{
	DrawScaledQuad(float(scale));
}

void GLTexImage::DrawQuadUS(int scale)
{
	DrawScaledQuad(1.0f/scale);
}

void GLTexImage::DrawScaledQuad(float texscale)
{

	////the texture coordinate for 0.5 is to + 0.5*texscale
	float to = 0.5f -0.5f * texscale;
	float tx =  (float)_imgWidth*texscale +to;
	float ty = (float)_imgHeight*texscale +to;
	glBegin (GL_QUADS);
		glTexCoord2f ( to	,   to   ); 	glVertex2i   ( 0			,		0   );
		glTexCoord2f ( to	,   ty  );		glVertex2i   ( 0			,		_imgHeight   );
 		glTexCoord2f ( tx	,	ty ); 		glVertex2i   ( _imgWidth	,		_imgHeight   );
		glTexCoord2f ( tx	,   to   ); 	glVertex2i   ( _imgWidth	,		0   );
	glEnd ();
	glFlush();
}


void GLTexImage::DrawQuadReduction(int w , int h)
{
	float to = -0.5f;
	float tx = w*2 +to;
	float ty = h*2 +to;
	glBegin (GL_QUADS);
		glMultiTexCoord2f ( GL_TEXTURE0, to	,	to   ); 
		glMultiTexCoord2f ( GL_TEXTURE1, to	+1,	to   ); 
		glMultiTexCoord2f ( GL_TEXTURE2, to	,	to+1  ); 
		glMultiTexCoord2f ( GL_TEXTURE3, to	+1,	to+1  ); 
		glVertex2i   ( 0			,		0   ); 

		glMultiTexCoord2f ( GL_TEXTURE0, to	,   ty  );	
		glMultiTexCoord2f ( GL_TEXTURE1, to	+1, ty  );	
		glMultiTexCoord2f ( GL_TEXTURE2, to	,   ty +1 );	
		glMultiTexCoord2f ( GL_TEXTURE3, to	+1, ty +1 );	
		glVertex2i   ( 0			,		h   ); 

 		glMultiTexCoord2f ( GL_TEXTURE0, tx	,	ty ); 	
 		glMultiTexCoord2f ( GL_TEXTURE1, tx	+1,	ty ); 	
 		glMultiTexCoord2f ( GL_TEXTURE2, tx	,	ty +1); 	
 		glMultiTexCoord2f ( GL_TEXTURE3, tx	+1,	ty +1); 	

		glVertex2i   ( w	,		h   ); 

		glMultiTexCoord2f ( GL_TEXTURE0, tx	,   to   ); 
		glMultiTexCoord2f ( GL_TEXTURE1, tx	+1, to   ); 
		glMultiTexCoord2f ( GL_TEXTURE2, tx	,   to +1  ); 
		glMultiTexCoord2f ( GL_TEXTURE3, tx	+1, to +1  ); 
		glVertex2i   ( w	,		0   ); 
	glEnd ();

	glFlush();
}


void GLTexImage::DrawQuadReduction()
{
	float to = -0.5f;
	float tx = _drawWidth*2 +to;
	float ty = _drawHeight*2 +to;
	glBegin (GL_QUADS);
		glMultiTexCoord2f ( GL_TEXTURE0, to	,	to   ); 
		glMultiTexCoord2f ( GL_TEXTURE1, to	+1,	to   ); 
		glMultiTexCoord2f ( GL_TEXTURE2, to	,	to+1  ); 
		glMultiTexCoord2f ( GL_TEXTURE3, to	+1,	to+1  ); 
		glVertex2i   ( 0			,		0   ); 

		glMultiTexCoord2f ( GL_TEXTURE0, to	,   ty  );	
		glMultiTexCoord2f ( GL_TEXTURE1, to	+1, ty  );	
		glMultiTexCoord2f ( GL_TEXTURE2, to	,   ty +1 );	
		glMultiTexCoord2f ( GL_TEXTURE3, to	+1, ty +1 );	
		glVertex2i   ( 0			,		_drawHeight   ); 

 		glMultiTexCoord2f ( GL_TEXTURE0, tx	,	ty ); 	
 		glMultiTexCoord2f ( GL_TEXTURE1, tx	+1,	ty ); 	
 		glMultiTexCoord2f ( GL_TEXTURE2, tx	,	ty +1); 	
 		glMultiTexCoord2f ( GL_TEXTURE3, tx	+1,	ty +1); 	

		glVertex2i   ( _drawWidth	,		_drawHeight   ); 

		glMultiTexCoord2f ( GL_TEXTURE0, tx	,   to   ); 
		glMultiTexCoord2f ( GL_TEXTURE1, tx	+1, to   ); 
		glMultiTexCoord2f ( GL_TEXTURE2, tx	,   to +1  ); 
		glMultiTexCoord2f ( GL_TEXTURE3, tx	+1, to +1  ); 
		glVertex2i   ( _drawWidth	,		0   ); 
	glEnd ();

	glFlush();
}

void GLTexImage::UnbindMultiTex(int n)
{
	for(int i = n-1; i>=0; i--)
	{
		glActiveTexture(GL_TEXTURE0+i);
		glBindTexture(_texTarget, 0);
	}
}

template <class Uint> int 

#if !defined(_MSC_VER) || _MSC_VER > 1200
GLTexInput::
#endif

DownSamplePixelDataI(unsigned int gl_format, int width, int height, int ds, 
									const Uint * pin, Uint * pout)	
{
	int step, linestep;
	int i, j; 
	int ws = width/ds;
	int hs = height/ds;
	const Uint * line = pin, * p;
	Uint *po = pout;
	switch(gl_format)
	{
	case GL_LUMINANCE:
	case GL_LUMINANCE_ALPHA:
		step = ds * (gl_format == GL_LUMINANCE? 1: 2); 
		linestep = width * step; 
		for(i = 0 ; i < hs; i++, line+=linestep)
		{
			for(j = 0, p = line; j < ws; j++, p+=step)
			{
				*po++ = *p;
			}
		}
		break;
	case GL_RGB:
	case GL_RGBA:
		step = ds * (gl_format == GL_RGB? 3: 4); 
		linestep = width * step; 

		for(i = 0 ; i < hs; i++, line+=linestep)
		{
			for(j = 0, p = line; j < ws; j++, p+=step)
			{
				//*po++ = int(p[0]*0.299 + p[1] * 0.587 + p[2]* 0.114 + 0.5);
				*po++ = ((19595*p[0] + 38470*p[1] + 7471*p[2]+ 32768)>>16);
			}
		}
		break;
	case GL_BGR:
	case GL_BGRA:
		step = ds * (gl_format == GL_BGR? 3: 4); 
		linestep = width * step; 
		for(i = 0 ; i < hs; i++, line+=linestep)
		{
			for(j = 0, p = line; j < ws; j++, p+=step)
			{
				*po++ = ((7471*p[0] + 38470*p[1] + 19595*p[2]+ 32768)>>16);
			}
		}
		break;
	default:
		return 0;
	}

	return 1;

}


template <class Uint> int 

#if !defined(_MSC_VER) || _MSC_VER > 1200
GLTexInput::
#endif

DownSamplePixelDataI2F(unsigned int gl_format, int width, int height, int ds, 
									const Uint * pin, float * pout, int skip)	
{
	int step, linestep;
	int i, j; 
	int ws = width/ds - skip;
	int hs = height/ds;
	const Uint * line = pin, * p;
	float *po = pout;
    const float factor = (sizeof(Uint) == 1? 255.0f : 65535.0f); 
	switch(gl_format)
	{
	case GL_LUMINANCE:
	case GL_LUMINANCE_ALPHA:
		step = ds * (gl_format == GL_LUMINANCE? 1: 2); 
		linestep = width * step; 
		for(i = 0 ; i < hs; i++, line+=linestep)
		{
			for(j = 0, p = line; j < ws; j++, p+=step)
			{
				*po++ = (*p) / factor; 
			}
		}
		break;
	case GL_RGB:
	case GL_RGBA:
		step = ds * (gl_format == GL_RGB? 3: 4); 
		linestep = width * step; 

		for(i = 0 ; i < hs; i++, line+=linestep)
		{
			for(j = 0, p = line; j < ws; j++, p+=step)
			{
				//*po++ = int(p[0]*0.299 + p[1] * 0.587 + p[2]* 0.114 + 0.5);
				*po++ = ((19595*p[0] + 38470*p[1] + 7471*p[2]) / (65535.0f * factor));
			}
		}
		break;
	case GL_BGR:
	case GL_BGRA:
		step = ds * (gl_format == GL_BGR? 3: 4); 
		linestep = width * step; 
		for(i = 0 ; i < hs; i++, line+=linestep)
		{
			for(j = 0, p = line; j < ws; j++, p+=step)
			{
				*po++ = ((7471*p[0] + 38470*p[1] + 19595*p[2]) / (65535.0f * factor));
			}
		}
		break;
	default:
		return 0;
	}
	return 1;
}

int GLTexInput::DownSamplePixelDataF(unsigned int gl_format, int width, int height, int ds, const float * pin, float * pout, int skip)	
{
	int step, linestep;
	int i, j; 
	int ws = width/ds - skip;
	int hs = height/ds;
	const float * line = pin, * p;
	float *po = pout;
	switch(gl_format)
	{
	case GL_LUMINANCE:
	case GL_LUMINANCE_ALPHA:
		step = ds * (gl_format == GL_LUMINANCE? 1: 2); 
		linestep = width * step; 
		for(i = 0 ; i < hs; i++, line+=linestep)
		{
			for(j = 0, p = line; j < ws; j++, p+=step)
			{
				*po++ = *p;
			}
		}
		break;
	case GL_RGB:
	case GL_RGBA:
		step = ds * (gl_format == GL_RGB? 3: 4); 
		linestep = width * step; 
		for(i = 0 ; i < hs; i++, line+=linestep)
		{
			for(j = 0, p = line; j < ws; j++, p+=step)
			{
				*po++ = (0.299f*p[0] + 0.587f*p[1] + 0.114f*p[2]);
			}
		}
		break;
	case GL_BGR:
	case GL_BGRA:
		step = ds * (gl_format == GL_BGR? 3: 4); 
		linestep = width * step; 
		for(i = 0 ; i < hs; i++, line+=linestep)
		{
			for(j = 0, p = line; j < ws; j++, p+=step)
			{
				*po++ = (0.114f*p[0] + 0.587f*p[1] + 0.299f * p[2]); 
			}
		}
		break;
	default:
		return 0;
	}

	return 1;

}

int GLTexInput::SetImageData( int width,  int height, const void * data, 
							 unsigned int gl_format, unsigned int gl_type )
{
	int simple_format = IsSimpleGlFormat(gl_format, gl_type);//no cpu code to handle other formats
	int ws, hs, done = 1;
	
	//if(_converted_data != 0) {delete [] _converted_data; _converted_data  = NULL; }

    _data_modified = 0; 

	_down_sampled = 0;
	ws = width;
	hs = height;
	
	if ( ws > _texMaxDim || hs > _texMaxDim)
	{

		std::cerr<<"Input images is too big to fit into a texture\n";
		return 0;
	}

	//printf("test: %d %d\n", ws, hs);
	_texWidth = _imgWidth = _drawWidth = ws;	
	_texHeight = _imgHeight = _drawHeight = hs;


	std::cout<<"Image size :\t"<<width<<"x"<<height<<"\n";


	if(_texID ==0)		glGenTextures(1, &_texID);
	glBindTexture(_texTarget, _texID);
	CheckErrorsGL("glBindTexture");
	glTexParameteri (_texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (_texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glPixelStorei(GL_UNPACK_ALIGNMENT , 1);

	//ds must be 0 here if not simpleformat
	if(gl_format == GL_LUMINANCE || gl_format == GL_LUMINANCE_ALPHA)
	{
		//use one channel internal format if data is intensity image
		glTexImage2D(_texTarget, 0, GL_LUMINANCE32F_ARB,
				_imgWidth, _imgHeight, 0, gl_format,	gl_type, data);
		FitViewPort(1, 1); //this used to be necessary
	}
	else
	{
		//convert RGB 2 GRAY if needed
		glTexImage2D(_texTarget, 0,  _iTexFormat, _imgWidth, _imgHeight, 0, gl_format, gl_type, data);
//		if(ShaderMan::HaveShaderMan())
//		{
//			TexConvertRGB();
//		}
//		else
//		{
//			_rgb_converted = 0;  //In CUDA mode, the conversion will be done by CUDA kernel
//		}
	}

	UnbindTex();

	return done;
}


GLTexInput::~GLTexInput()
{
    if(_converted_data) delete [] _converted_data; 
}


int GLTexInput::LoadImageFile(char *imagepath, int &w, int &h)// , bool& flag)
{
#ifndef SIFTGPU_NO_DEVIL
    static int devil_loaded = 0; 
	unsigned int imID;
	int done = 1;

    if(devil_loaded == 0)
    {
	    ilInit();
	    ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	    ilEnable(IL_ORIGIN_SET);
        devil_loaded = 1; 
    }

	ilGenImages(1, &imID);
	ilBindImage(imID); 

	if(ilLoadImage(imagepath))
	{
		w = ilGetInteger(IL_IMAGE_WIDTH);
		h = ilGetInteger(IL_IMAGE_HEIGHT);
		int ilformat = ilGetInteger(IL_IMAGE_FORMAT);
		/*if( ilformat == GL_RGBA )
		{
			flag = true;
		}*/
		if(SetImageData(w, h, ilGetData(), ilformat, GL_UNSIGNED_BYTE)==0)
		{
			done =0;
		}else
		{
			std::cout<<"Image loaded :\t"<<imagepath<<"\n";
		}

	}else
	{
		std::cerr<<"Unable to open image [code = "<<ilGetError()<<"]\n";
		done = 0;
	}

	ilDeleteImages(1, &imID); 

	return done;
#else
	IplImage* img = cvLoadImage(imagepath, -1);
	w = img->width;
	h = img->height;

	int nChannels = img->nChannels;
	unsigned int gl_format = ((nChannels == 1) ? GL_LUMINANCE : (nChannels == 3 ? GL_BGR : GL_BGRA));
	unsigned int gl_type = GL_UNSIGNED_BYTE;
	_texWidth = _imgWidth = _drawWidth = w;
	_texHeight = _imgHeight = _drawHeight = h;

	std::cout<< "Image Loaded :\t" << std::string(imagepath) << "\nImage size :\t"<<w<<"x"<<h<<"\n";

	if(_texID ==0)
	{
		glGenTextures(1, &_texID);
	}
	glBindTexture(_texTarget, _texID);
	CheckErrorsGL("glBindTexture");
	glTexParameteri (_texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (_texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glPixelStorei(GL_UNPACK_ALIGNMENT , 1);

	if(gl_format == GL_LUMINANCE || gl_format == GL_LUMINANCE_ALPHA)
	{
		glTexImage2D(_texTarget, 0, GL_RGBA8,
				_imgWidth, _imgHeight, 0, gl_format, gl_type, img->imageData);
	}
	else
	{
		std::cout << "format: " << gl_format << "\tnChannels: " << img->nChannels << "\n";
		glTexImage2D(_texTarget, 0,  GL_RGBA,
				_imgWidth, _imgHeight, 0, gl_format, gl_type,  img->imageData);
		}

	UnbindTex();
	cvReleaseImage(&img);
	return 1;
#endif
}

int GLTexImage::CopyToPBO(GLuint pbo, int width, int height, GLenum format)
{
    /////////
    if(format != GL_RGBA && format != GL_LUMINANCE) return 0; 

	FrameBufferObject fbo;
    GLint bsize, esize = width * height * sizeof(float) * (format == GL_RGBA ? 4 : 1);
	AttachToFBO(0);
	glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, pbo);
	glGetBufferParameteriv(GL_PIXEL_PACK_BUFFER_ARB, GL_BUFFER_SIZE, &bsize);
	if(bsize < esize) 
	{
		glBufferData(GL_PIXEL_PACK_BUFFER_ARB, esize,	NULL, GL_STATIC_DRAW_ARB);
		glGetBufferParameteriv(GL_PIXEL_PACK_BUFFER_ARB, GL_BUFFER_SIZE, &bsize);
	}
	if(bsize >= esize)
	{
		glReadPixels(0, 0, width, height, format, GL_FLOAT, 0);
	}
	glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);
	DetachFBO(0);

	return bsize >= esize;
}

void GLTexInput::VerifyTexture()
{
    //for CUDA or OpenCL the texture is not generated by default
    if(!_data_modified) return;
    if(_pixel_data== NULL) return;
    InitTexture(_imgWidth, _imgHeight);
    BindTex();
	glTexImage2D(   _texTarget, 0, GL_LUMINANCE32F_ARB, //internal format changed
                    _imgWidth, _imgHeight, 0,
					GL_LUMINANCE, GL_FLOAT, _pixel_data);
    UnbindTex();
    _data_modified = 0; 
}

void GLTexImage::CopyFromPBO(GLuint pbo, int width, int height, GLenum format)
{
	InitTexture(max(width, _texWidth), max(height, _texHeight));
	SetImageSize(width, height);
	if(width > 0 && height > 0)
	{
		BindTex();
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
		glTexSubImage2D(_texTarget, 0, 0, 0, width, height, format, GL_FLOAT, 0);
        CheckErrorsGL("GLTexImage::CopyFromPBO->glTexSubImage2D");
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
		UnbindTex();
	}
}
/* 2012-09-25 16:43:26 Zhuo.Chen	End of GLTexImage.cpp */
