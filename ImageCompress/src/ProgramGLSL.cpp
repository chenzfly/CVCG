////////////////////////////////////////////////////////////////////////////
//	File:		ProgramGLSL.cpp
//	Author:		Zhuo.Chen
//	Description : GLSL related classes
//		class ProgramGLSL		A simple wrapper of GLSL programs
////////////////////////////////////////////////////////////////////////////

  
#include "GL/glew.h"
#include <cstring>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;


#include "ProgramGLSL.h"

ProgramGLSL::ShaderObject::ShaderObject(int shadertype, const char * source, int filesource)
{

	_type = shadertype; 
	_compiled = 0;

	_shaderID = glCreateShader(shadertype);
	if(_shaderID == 0) return;
	
	if(source)
	{
		GLint				code_length;
		if(filesource ==0)
		{
			const char* code  = source;
			code_length = (GLint) strlen(code);
			glShaderSource(_shaderID, 1, (const char **) &code, &code_length);
		}else
		{
			char * code;
			if((code_length= ReadShaderFile(source, code)) ==0) return;
			glShaderSource(_shaderID, 1, (const char **) &code, &code_length);
			delete code;
		}

		glCompileShader(_shaderID);

		CheckCompileLog();

		if(!_compiled) 		std::cout << source;
	}
}

int ProgramGLSL::ShaderObject::ReadShaderFile(const char *sourcefile,  char*& code )
{
	code = NULL;
	FILE * file;
	int    len=0;

	if(sourcefile == NULL) return 0;

	file = fopen(sourcefile,"rt");
	if(file == NULL) return 0;
	
	fseek(file, 0, SEEK_END);
	len = ftell(file);
	rewind(file);
	if(len >1)
	{
		code = new  char[len+1];
		fread(code, sizeof( char), len, file);
		code[len] = 0;
	}else
	{
		len = 0;
	}

	fclose(file);

	return len;
	
}

void ProgramGLSL::ShaderObject::CheckCompileLog()
{
	GLint status;
	glGetShaderiv(_shaderID, GL_COMPILE_STATUS, &status);
	_compiled = (status ==GL_TRUE);

	if(_compiled == 0)	PrintCompileLog(std::cout);

}

ProgramGLSL::ShaderObject::~ShaderObject()
{
	if(_shaderID)	glDeleteShader(_shaderID);
}

int ProgramGLSL::ShaderObject::IsValidFragmentShader()
{
	return _type == GL_FRAGMENT_SHADER && _shaderID && _compiled;
}

int  ProgramGLSL::ShaderObject::IsValidVertexShader()
{
	return _type == GL_VERTEX_SHADER && _shaderID && _compiled;
}


void ProgramGLSL::ShaderObject::PrintCompileLog(ostream&os)
{
	GLint len = 0;	

	glGetShaderiv(_shaderID, GL_INFO_LOG_LENGTH , &len);
	if(len <=1) return;
	
	char * compileLog = new char[len+1];
	if(compileLog == NULL) return;

	glGetShaderInfoLog(_shaderID, len, &len, compileLog);
	

	os<<"Compile Log\n"<<compileLog<<"\n";

	delete[] compileLog;
}


ProgramGLSL::ProgramGLSL()
{
	_linked = 0;
	_TextureParam0 = -1;
	_programID = glCreateProgram();
}
ProgramGLSL::~ProgramGLSL()
{
	if(_programID)glDeleteProgram(_programID);
}
void ProgramGLSL::AttachShaderObject(ShaderObject &shader)
{
	if(_programID  && shader.IsValidShaderObject()) 
		glAttachShader(_programID, shader.GetShaderID());
}
void ProgramGLSL::DetachShaderObject(ShaderObject &shader)
{
	if(_programID  && shader.IsValidShaderObject()) 
		glDetachShader(_programID, shader.GetShaderID());
}
int ProgramGLSL::LinkProgram()
{
	_linked = 0;

	if(_programID==0) return 0;

	glLinkProgram(_programID);

	CheckLinkLog();

//	GlobalUtil::StartTimer("100 link test");
//	for(int i = 0; i<100; i++) glLinkProgram(_programID);
//	GlobalUtil::StopTimer();

	return _linked;
}

void ProgramGLSL::CheckLinkLog()
{
	GLint status;
	glGetProgramiv(_programID, GL_LINK_STATUS, &status);

	_linked = (status == GL_TRUE);

}

int ProgramGLSL::ValidateProgram()
{
	if(_programID && _linked)
	{
///		GLint status;
//		glValidateProgram(_programID);
//		glGetProgramiv(_programID, GL_VALIDATE_STATUS, &status);
//		return status == GL_TRUE;
		return 1;
	}
	else
		return 0;
}

void ProgramGLSL::PrintLinkLog(std::ostream &os)
{
	GLint len = 0;	

	glGetProgramiv(_programID, GL_INFO_LOG_LENGTH , &len);
	if(len <=1) return;
	
	char* linkLog = new char[len+1];
	if(linkLog == NULL) return;

	glGetProgramInfoLog(_programID, len, &len, linkLog);
	
	linkLog[len] = 0;

	if(strstr(linkLog, "failed"))
	{
		os<<linkLog + (linkLog[0] == ' '? 1:0)<<"\n";
		_linked = 0;
	}

	delete[] linkLog;
}

int ProgramGLSL::UseProgram()
{
	if(ValidateProgram())
	{
		glUseProgram(_programID);
		if (_TextureParam0 >= 0) glUniform1i(_TextureParam0, 0);
		return true;
	}
	else
	{
		return false;
	}
}


ProgramGLSL::ProgramGLSL(const char* frag_source)//, const char* vert_source)
{
	_linked = 0;
	_programID = glCreateProgram();
	_TextureParam0 = -1;
	ShaderObject f_shader(GL_FRAGMENT_SHADER, frag_source);
	//ShaderObject v_shader(GL_VERTEX_SHADER, vert_source);
	if(f_shader.IsValidFragmentShader())// && v_shader.IsValidVertexShader())
	{
		AttachShaderObject(f_shader);
		//AttachShaderObject(v_shader);
		LinkProgram();

		if(!_linked)
		{
			//shader.PrintCompileLog(std::cout);
			PrintLinkLog(std::cout);
		} else
		{
			_TextureParam0 = glGetUniformLocation(_programID, "tex");
		}
	}else
	{
		_linked = 0;
	}
	
}

/*
ProgramGLSL::ProgramGLSL(char*frag_source, char * vert_source)
{
	_used = 0;
	_linked = 0;
	_programID = glCreateProgram();
	ShaderObject shader(GL_FRAGMENT_SHADER, frag_source);
	ShaderObject vertex_shader(GL_VERTEX_SHADER, vert_source);
	AttachShaderObject(shader);
	AttachShaderObject(vertex_shader);
	LinkProgram();
	if(!_linked)
	{
		shader.PrintCompileLog(std::cout);
		vertex_shader.PrintCompileLog(std::cout);
		PrintLinkLog(std::cout);
		std::cout<<vert_source;
		std::cout<<frag_source;
	}

}
*/



void ProgramGLSL::ReLink()
{
	glLinkProgram(_programID);
}

int ProgramGLSL::IsNative()
{
	return _linked;
}
