////////////////////////////////////////////////////////////////////////////
//	File:		FrameBufferObject.h
//	Author:		Zhuo.Chen
//	Description : interface for the FrameBufferObject class.
////////////////////////////////////////////////////////////////////////////


#if !defined(_FRAME_BUFFER_OBJECT_H)
#define _FRAME_BUFFER_OBJECT_H

class FrameBufferObject  
{
	static GLuint	GlobalFBO;   //not thread-safe
	GLuint _fboID;
public:
	static int		UseSingleFBO;
public:
	static void DeleteGlobalFBO();
	static void UnattachTex(GLenum attachment);
	static void UnbindFBO();
	static void AttachDepthTexture(GLenum textureTarget, GLuint texID);
	static void AttachTexture( GLenum textureTarget, GLenum attachment, GLuint texID);
	static void AttachRenderBuffer(GLenum attachment,  GLuint buffID  );
	static void UnattachRenderBuffer(GLenum attachment);
public:
	void BindFBO();
	FrameBufferObject(int autobind = 1);
	~FrameBufferObject();

};

#endif 
