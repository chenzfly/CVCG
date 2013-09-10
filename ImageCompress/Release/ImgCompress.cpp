#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
	if( argc < 3 )
	{
		cout << "args:\n\targ1:image name.\n\targ2:\tCompress factor.\n\targ3:\tresult image name.\n";
		return -1;
	}
	IplImage* img = cvLoadImage(argv[1]);		
	cv::Mat imgM(img);
	vector<int> factor;
	//factor[0] = CV_IMWRITE_JPEG_QUALITY;
	//factor[1] = atoi(argv[2]);
	factor.push_back(CV_IMWRITE_JPEG_QUALITY);
	factor.push_back(atoi(argv[2]) );
	cv::imwrite(argv[3], imgM, factor);
	cvReleaseImage(&img);
	return 0;

}
