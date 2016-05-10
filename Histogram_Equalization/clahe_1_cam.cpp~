#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <sys/time.h>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{

	struct timeval t0,t1,t2,t3,t4;
	VideoCapture cap;	
	cap.open(0);
	
	static Mat I_BGR;
	static Mat O_GRAY;
	cap.read(I_BGR);
	cvtColor(I_BGR,O_GRAY,COLOR_BGR2GRAY);
	imshow("RGB Image", I_BGR);
	imshow("Gray Image" , O_GRAY);
	waitKey(30);
	gettimeofday(&t0,0);
	Ptr<CLAHE> clahe = createCLAHE(5.0, cv::Size(8,8));
		
	Mat O_HIST;
	clahe->apply(O_GRAY,O_HIST);
	
	gettimeofday(&t1,0);
	long elapsed = t1.tv_usec-t0.tv_usec;
	cout << "Time :"<<  double(elapsed)/1000000 << endl;
	
	cout << "done " <<endl;

	//cvtColor(O_HIST,O_HIST,COLOR_GRAY2BGR);
	//O_HIST = 0.8*O_HIST +0.2*I_BGR;
	//O_HIST = 0.8*O_HIST + 0.2*O_GRAY;
		
	while(true){
		imshow("CLAHE Image",O_HIST);
		waitKey(30);
	}
	
	return 0;

}
