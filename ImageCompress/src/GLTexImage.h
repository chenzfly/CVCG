/* 2012-09-25 16:43:46 Zhuo.Chen	GLTexImage.h */
#ifndef  GLTEXIMAGE_H
#define  GLTEXIMAGE_H
#include <GL/glew.h>
class GLTexImage
{	
protected:
	GLuint	_texID;
	int		_imgWidth;
	int		_imgHeight;
	int		_texWidth;
	int		_texHeight;
	int		_drawWidth;
	int		_drawHeight;
//	int		_WindowInitX;
//	int		_WindowInitY;
//	int		_DeviceIndex;
//	const char*		_WindowDisplay;
public:
	static int _texTarget;
	static int _iTexFormat;
	static int _texMaxDim;

	static void DetachFBO(int i);
	static void UnbindTex();
	static void UnbindMultiTex(int n);
	static void DrawQuad(float x1, float x2, float y1, float y2);
	void CheckErrorsGL(const char* location);
	void FitViewPort(int width, int height);
	//int GLTexImage::CreateWindowEZ(LiteWindow* window);
	//int GLTexImage::CreateWindowEZ();
public:
	virtual void DrawQuadUS(int scale);
	virtual void DrawQuadDS(float scale);
	virtual void DrawImage();
	virtual void TexConvertRGB();
	virtual void SetImageSize(int width, int height);
	virtual void InitTexture(int width, int height, int clamp_to_edge =1 );
	void InitTexture(int width, int height, int clamp_to_edge, GLuint format);
public:
	void DrawScaledQuad(float scale);
	int  CopyToPBO(GLuint pbo, int width, int height, GLenum format = GL_RGBA);
	void CopyFromPBO(GLuint pbo, int width, int height, GLenum format = GL_RGBA);
	void FitRealTexViewPort();
	void DrawQuadMT8();
	void DrawQuadMT4();
	void DrawQuadReduction();
	void DrawQuadReduction(int w, int h);
	void DrawMargin(int right, int bottom);
	void DrawQuad();
	void FitTexViewPort();
	int  CheckTexture();
public:
	void AttachToFBO(int i );
	void BindTex();
	operator GLuint (){return _texID;}	
	GLuint GetTexID(){return _texID;}
	int	GetImgPixelCount(){return _imgWidth*_imgHeight;}
	int GetTexPixelCount(){return _texWidth*_texHeight;}
	int	GetImgWidth(){return _imgWidth;}
	int GetImgHeight(){return _imgHeight;}
	int	GetTexWidth(){return _texWidth;}
	int GetTexHeight(){return _texHeight;}
	int	GetDrawWidth(){return _drawWidth;}
	int GetDrawHeight(){return _drawHeight;}
	//int	IsTexTight(){return _texWidth == _drawWidth && _texHeight == _drawHeight;}
	int	IsTexPacked(){return _drawWidth != _imgWidth;}
	GLTexImage();
	virtual ~GLTexImage();
};

//class for handle data input, to support all openGL-supported data format, 
//data are first uploaded to an openGL texture then converted, and optionally
//when the data type is simple, we down sample/convert on cpu
class GLTexInput:public GLTexImage
{
public:
	int      _down_sampled;
	int      _rgb_converted;
    int      _data_modified;

    //////////////////////////
	float *        _converted_data;
    const void*    _pixel_data;
public:
	static int  IsSimpleGlFormat(unsigned int gl_format, unsigned int gl_type)
	{
		//the formats there is a cpu code to convert rgb and down sample
		 return (gl_format ==GL_LUMINANCE ||gl_format == GL_LUMINANCE_ALPHA||
				gl_format == GL_RGB||	gl_format == GL_RGBA||
				gl_format == GL_BGR || gl_format == GL_BGRA) && 
				(gl_type == GL_UNSIGNED_BYTE || gl_type == GL_FLOAT || gl_type == GL_UNSIGNED_SHORT); 
	}
//in vc6, template member function doesn't work
#if !defined(_MSC_VER) || _MSC_VER > 1200
	template <class Uint> 
	static int DownSamplePixelDataI(unsigned int gl_format, int width, int height, 
		int ds, const Uint * pin, Uint * pout);
	template <class Uint> 
	static int DownSamplePixelDataI2F(unsigned int gl_format, int width, int height, 
		int ds, const Uint * pin, float * pout, int skip  = 0);
#endif
	static int DownSamplePixelDataF(unsigned int gl_format, int width, int height, 
		int ds, const float * pin, float * pout, int skip = 0);
    static int TruncateWidthCU(int w) {return  w & 0xfffffffc; }
public:
	GLTexInput() : _down_sampled(0), _rgb_converted(0), _data_modified(0), 
                    _converted_data(0), _pixel_data(0){}
	int SetImageData(int width, int height, const void * data, 
					unsigned int gl_format, unsigned int gl_type);
	int LoadImageFile(char * imagepath, int & w, int &h);//, bool& flag);
    void VerifyTexture();
    virtual ~GLTexInput();
};

#endif  /*GLTEXIMAGE_H*/

