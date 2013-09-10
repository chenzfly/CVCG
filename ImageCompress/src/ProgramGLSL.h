////////////////////////////////////////////////////////////////////////////
//	File:		ProgramGLSL.h
//	Author:		Zhuo.Chen
//	Description : Interface for ProgramGLSL classes
//		ProgramGLSL:	Glsl Program
////////////////////////////////////////////////////////////////////////////


#ifndef _PROGRAM_GLSL_H
#define _PROGRAM_GLSL_H

class ProgramGLSL
{
	class ShaderObject
	{
		GLuint		_shaderID;
		int			_type;
		int			_compiled;
		static int ReadShaderFile(const char * source,  char *& code);	
		void CheckCompileLog();
	public:
		void PrintCompileLog(std::ostream & os  );
		int inline IsValidShaderObject(){	return _shaderID && _compiled;}
		int IsValidVertexShader();
		int IsValidFragmentShader();
		GLuint GetShaderID(){return _shaderID;}
		~ShaderObject(); 
		ShaderObject(int shadertype,  const char * source, int filesource =0);
	};

protected:
	int			_linked;
	GLint		_TextureParam0;
	GLuint		_programID; 
private:
	void AttachShaderObject(ShaderObject& shader);
	void DetachShaderObject(ShaderObject& shader);

public:
	void ReLink();
	int IsNative();
	int	UseProgram();
	void PrintLinkLog(std::ostream&os);
	int ValidateProgram();
	void CheckLinkLog();
	int LinkProgram();
	operator GLuint (){return _programID;}
    virtual int GetProgramID() { return _programID; }
public:
	ProgramGLSL();
	~ProgramGLSL();
	//ProgramGLSL(const char* frag_source, const char* vert_source);
	ProgramGLSL(const char* frag_source);//, const char* vert_source);
};

#endif

