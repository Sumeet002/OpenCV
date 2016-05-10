#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <sys/time.h>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{

	struct timeval t0,t1,t2,t3,t4;


	static Mat I_RGB;
	static Mat O_RGB;
	
	I_RGB=imread("sample.png");
	int scale = 2 ;
	Size size(I_RGB.cols/scale,I_RGB.rows/scale);
	int imgSize= (I_RGB.cols/scale)*(I_RGB.rows/scale);

	gettimeofday(&t0,0);
	
	resize(I_RGB,O_RGB,size);
	static Mat O_GRAY(I_RGB.rows,I_RGB.cols,CV_8UC1,Scalar(0));
	cvtColor(O_RGB,O_GRAY,COLOR_RGB2GRAY);
	

	imshow("Gray Image" , O_GRAY);
	waitKey(30);

	
	static Mat O_HIST(O_GRAY.rows,O_GRAY.cols,CV_8UC1,Scalar(0));

	int numBins=256;
	int histArray[256]={0};
	int bin;

	for(int i=0 ; i < O_GRAY.rows ; i++){
		for(int j=0; j < O_GRAY.cols ; j++){
			bin = O_GRAY.at<uchar>(i,j);             //get grayscale value of pixel
			histArray[bin]=histArray[bin]+1;  //update count of pixels associated with the value
		}
	}

	for(int i=1; i < 256 ; i++){
		histArray[i] = histArray[i-1] + histArray[i];

	}

	float normFactor = (float)numBins/imgSize;

	for(int i=0; i< O_GRAY.rows ; i++){
		for(int j=0; j < O_GRAY.cols ; j++){
			O_HIST.at<uchar>(i,j) = (int)(histArray[(int)(O_GRAY.at<uchar>(i,j))]*normFactor);
		}
	}

	gettimeofday(&t1,0);
	long elapsed = t1.tv_usec-t0.tv_usec;
	cout << "Time :"<<  double(elapsed)/1000000 << endl;
	
	cout << "done " <<endl;

	while(true){
		imshow("Histogram Equalized Image",O_HIST);
		waitKey(30);
	}

	return 0;

}
