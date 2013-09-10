#include <cstdlib>
#include <cstdio>
#include <vector>
#include <iostream>
#include <dirent.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "GLTexImage.h"
#include "GL/glew.h"
#include "LiteWindow.h"
#include "FrameBufferObject.h"
#include "ProgramGLSL.h"
#define   MAX_PATH_LEN 256
//using namespace std;

void SetTextureParameter()
{
	glTexParameteri (GLTexImage::_texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GLTexImage::_texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GLTexImage::_texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GLTexImage::_texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

void InitializeContext()
{
	//first time in this function
	glewInit();

	int _MemCapGPU;
	const char * vendor = (const char * )glGetString(GL_VENDOR);
	if(vendor)
	{
		if(glewGetExtension("GL_NVX_gpu_memory_info"))
		{
			glGetIntegerv(0x9049/*GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX*/, &_MemCapGPU);
			_MemCapGPU /= (1024);
			std::cout << "[GPU VENDOR]:\t" << vendor << ' ' <<_MemCapGPU << "MB\n";
		}else if(strstr(vendor, "ATI") && glewGetExtension("GL_ATI_meminfo"))
		{
			int info[4]; 	glGetIntegerv(0x87FC/*GL_TEXTURE_FREE_MEMORY_ATI*/, info);
			_MemCapGPU = info[0] / (1024);
			std::cout << "[GPU VENDOR]:\t" << vendor << ' ' <<_MemCapGPU << "MB\n";
		}else
		{
			std::cout << "[GPU VENDOR]:\t" << vendor << "\n";
		}

	}

	if (glewGetExtension("GL_ARB_fragment_shader")    != GL_TRUE ||
		glewGetExtension("GL_ARB_shader_objects")       != GL_TRUE ||
		glewGetExtension("GL_ARB_shading_language_100") != GL_TRUE)
	{
		std::cerr << "Shader not supported by your hardware!\n";
	}

	if (glewGetExtension("GL_EXT_framebuffer_object") != GL_TRUE)
	{
		std::cerr << "Framebuffer object not supported!\n";
	}

	if(glewGetExtension("GL_ARB_texture_rectangle")==GL_TRUE)
	{
		GLint value;
		GLTexImage::_texTarget =  GL_TEXTURE_RECTANGLE_ARB;
		glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT, &value);
		int _texMaxDimGL = value;
		std::cout << "TEXTURE:\t" << _texMaxDimGL << "\n";

		if(GLTexImage::_texMaxDim == 0 || GLTexImage::_texMaxDim > _texMaxDimGL)
		{
			GLTexImage::_texMaxDim = _texMaxDimGL;
		}
		glEnable(GLTexImage::_texTarget);
	}else
	{
		std::cerr << "GL_ARB_texture_rectangle not supported!\n";
	}

	int _SupportNVFloat = glewGetExtension("GL_NV_float_buffer");
	int _SupportTextureRG = glewGetExtension("GL_ARB_texture_rg");


	glShadeModel(GL_FLAT);
	glPolygonMode(GL_FRONT, GL_FILL);

	SetTextureParameter();
}

int CreateWindowEZ(LiteWindow* window)
{
	if(window == NULL) return 0;
	int		_WindowInitX = -1;
	int		_WindowInitY = -1;
	const char*		_WindowDisplay = NULL;

    if(!window->IsValid())window->Create(_WindowInitX, _WindowInitY, _WindowDisplay);
    if(window->IsValid())
    {
        window->MakeCurrent();
        return 1;
    }
    else
    {

        std::cerr << "Unable to create OpenGL Context!\n";
		std::cerr << "For nVidia cards, you can try change to CUDA mode in this case\n";
        return 0;
    }
}

int CreateWindowEZ()
{
	static LiteWindow window;

    return CreateWindowEZ(&window);
}

int CreateContextGL()
{
	if(!CreateWindowEZ())
	{
		return 0;
	}
	return 1;
}

void TextureDownSample(GLTexImage *dst, GLTexImage *src, float scale)
{
	//output parameter

	dst->AttachToFBO(0);

	//input parameter
	src->BindTex();

	//
	dst->FitTexViewPort();

	ProgramGLSL* s_sampling = new ProgramGLSL(
		"uniform sampler2DRect tex; void main(void){gl_FragColor = texture2DRect(tex, gl_TexCoord[0].xy);}");

	s_sampling->UseProgram();

	dst->DrawQuadDS(scale);

/*	glBegin (GL_QUADS);
		glTexCoord2i ( 0,	0);
		glVertex2i   ( 0,	0);
		glTexCoord2i ( src->GetImgWidth(),	0);
		glVertex2i   ( dst->GetImgWidth(),	0);
 		glTexCoord2i ( src->GetImgWidth(),	src->GetImgHeight());
 		glVertex2i   ( dst->GetImgWidth(),	dst->GetImgHeight());
		glTexCoord2i ( 0,	src->GetImgHeight());
		glVertex2i   ( 0,	dst->GetImgHeight());
	glEnd ();
	glFlush();*/

	src->UnbindTex();
	glUseProgram(0);
	dst->DetachFBO(0); 
}


int main(int argc, char** argv)
{
	if (argc < 3)
	{
		std::cout << "args error!\n\targv[1]: image path\n\targv[2]: compress factor(int)\n";
        return -1;
	}


	if(!CreateContextGL())
	{
		return 0;
	}
	InitializeContext();

	int num = 0;
	struct dirent* ent;
	DIR* pDir;
	pDir = opendir(argv[1]);
	char curr[MAX_PATH_LEN];
	memset(curr, 0, sizeof(curr));
	double timeT = cv::getTickCount();
	int width, height;
	float factor = atof(argv[2]);
	//int resW, resH;
	bool flag = false;
	while((ent = readdir(pDir)) != NULL)
	{

		if(strcmp(ent->d_name,".")==0 ||strcmp(ent->d_name,"..")==0)
		{
			continue;
		}

		sprintf(curr, "%s/%s", argv[1], ent->d_name);
		char curr1[MAX_PATH_LEN];
		memset(curr1, 0, sizeof(curr1));
		sprintf(curr1, "%s", ent->d_name);
		std::string imageName(curr1);
		imageName = "out/" + imageName;
		glEnable(GL_BLEND);
		//glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GLTexImage::_texTarget);
		glActiveTexture(GL_TEXTURE0);
		GLTexInput* input = new GLTexInput;
		input->LoadImageFile(curr, width, height);//, flag);

		FrameBufferObject fbo;
		input->BindTex();

		GLTexImage* out = new GLTexImage;

		int resW = 473;//640;//(float)width / factor;
		int resH = 473.0f / (float)width * height;//(float)height / factor;
		factor = (float)width / 473.0f;

		out->InitTexture(resW, resH, 1);

		out->AttachToFBO(0);
		TextureDownSample(out, input, factor);

		out->AttachToFBO(0);
		unsigned char* data = new unsigned char[resW * resH * 4];
		glReadPixels(0, 0, resW, resH, GL_BGRA, GL_UNSIGNED_BYTE, data);
 		IplImage* img = cvCreateImage(cvSize(resW, resH), 8, 4);
 		int nChannels = 4;
		unsigned char flagc;
 		for( int i = 0 ; i < resW ; i++ )
 		{
 		 	for( int j = 0 ; j < resH ; j++ )
 		 	{
 		 		img->imageData[j * resW * nChannels + i * nChannels + 0] = data[j * resW * 4 + i * 4 + 0];
 				img->imageData[j * resW * nChannels + i * nChannels + 1] = data[j * resW * 4 + i * 4 + 1];
 				img->imageData[j * resW * nChannels + i * nChannels + 2] = data[j * resW * 4 + i * 4 + 2];
 				flagc = img->imageData[j * resW * nChannels + i * nChannels + 3] = data[j * resW * 4 + i * 4 + 3];
				if( flagc < 255 && flagc > 0 )
				{
					flag = true;
				}
 		 	}
 		}
		std::string res = imageName;
	/*	if( flag )
		{
			imageName += ".png";
			std::vector<int> param;
			param.push_back(CV_IMWRITE_PNG_COMPRESSION);
			param.push_back(9);
			cv::imwrite(imageName.c_str(), cv::Mat(img), param);
		}
		else*/
		{
			imageName += ".jpg";
			std::vector<int> param;
			param.push_back(CV_IMWRITE_JPEG_QUALITY);
			param.push_back(70);
			cv::imwrite(imageName.c_str(), cv::Mat(img), param);
		}
 		//cvSaveImage(imageName.c_str(), img);
		rename(imageName.c_str(), res.c_str());
 		cvReleaseImage(&img);
 		//cvReleaseImage(&tmp);
 		delete [] data;
		num++;

//		memset(curr, 0, sizeof(curr));
//			cv::Mat imgM = cv::imread(argv[1]);
//			int count = 0;
//			for( int i = 0 ; i < keys1.size() ; i++ )
//			{
//				count++;
//				CvPoint position = cvPoint(keys1[i].x, keys1[i].y);
//				cv::circle(imgM, position, 5, CV_RGB(0, 255, 0), 1, 8, 3);
//			}
//			printf("draw %d keys\n", count);
//			cv::imshow("test", imgM);
//			cv::waitKey(0);
		delete input;
		delete out;
	}
	timeT = (cv::getTickCount() - timeT) * 1000 / cv::getTickFrequency();
	printf("%d images cost %06fms average time: %06f\n", num, timeT, timeT / (float)num);

	return 1;
}

