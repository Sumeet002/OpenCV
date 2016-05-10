#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <sys/time.h>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{

	struct timeval t0,t1,t2,t3,t4;
	
	static Mat I_GRAY;
	static Mat O_GRAY;
	
	I_GRAY=imread("sample3.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	int scale = 1 ;
	Size size(I_GRAY.cols/scale,I_GRAY.rows/scale);
	int imgSize= (I_GRAY.cols/scale)*(I_GRAY.rows/scale);

	resize(I_GRAY,O_GRAY,size);
	imshow("Gray Image" , O_GRAY);
	waitKey(30);
	gettimeofday(&t0,0);
	Ptr<CLAHE> clahe = createCLAHE(4.0, cv::Size(8,8));
		
	Mat O_HIST;
	clahe->apply(O_GRAY,O_HIST);
	
	gettimeofday(&t1,0);
	long elapsed = t1.tv_usec-t0.tv_usec;
	cout << "Time :"<<  double(elapsed)/1000000 << endl;
	
	cout << "done " <<endl;

	while(true){
		imshow("CLAHE Image",O_HIST);
		waitKey(30);
	}
	
	return 0;

}
