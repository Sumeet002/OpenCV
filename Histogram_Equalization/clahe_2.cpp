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
	
	I_RGB=imread("sample2.jpg");
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
	int bin;
	
	int window_size = 6 ;
	int w = O_GRAY.cols/6;
	int h = O_GRAY.rows/6;
	float normFactor = (float)numBins/(w*h);
	int histArray[36][256]={0};
	int clipLimit =150;
	int outPixCount;
	
	for(int k=0; k < window_size ; k++){
		for(int l=0;l< window_size;l++){
			for(int i=k*h ; i < (k+1)*h ; i++){
				for(int j=l*w; j < (l+1)*w ; j++){
					bin =(int)O_GRAY.at<uchar>(i,j);  
					if(histArray[k*window_size+l][bin] > clipLimit)
           					outPixCount++;
					else
						histArray[k*window_size+l][bin]=histArray[k*window_size+l][bin]+1;   
				}
			}

			int numDistribute = (int)outPixCount/numBins;
			
			/*for(int i=0; i < 256 ; i++){
				histArray[k*window_size+l][i] = histArray[k*window_size+l][i] + numDistribute;

			}*/

			for(int i=1; i < 256 ; i++){
				histArray[k*window_size+l][i] = histArray[k*window_size+l][i-1] + histArray[k*window_size+l][i];

			}
		}
	}

	int ulvalue,uvalue,lvalue,value,curvalue,newvalue;
	
	
	for(int k=1; k < 6 ; k++){
		for(int l=1;l< 6 ;l++){
			for(int i=k*h ; i < (k+1)*h ; i++){
				for(int j=l*w; j < (l+1)*w ; j++){
					curvalue = (int)(O_GRAY.at<uchar>(i,j));
					
					value = (int)(histArray[k*window_size+l][curvalue]*normFactor);
					uvalue = (int)(histArray[(k-1)*window_size+l][curvalue]*normFactor);
					ulvalue = (int)(histArray[(k-1)*window_size+(l-1)][curvalue]*normFactor);
					lvalue = (int)(histArray[k*window_size+(l-1)][curvalue]*normFactor);

					float A = ((float)((l*w+w/2)-j)/w)*lvalue + ((float)(j-((l-1)*w+w/2))/w)*value;
					float B = ((float)((l*w+w/2)-j)/w)*ulvalue + ((float)(j-((l-1)*w+w/2))/w)*uvalue;

					O_GRAY.at<uchar>(i,j) = (int)(((float)((k*h+h/2)-i)/h)*A + ((float)(i-((k-1)*h+h/2))/h)*B);
					
				}
			}
		}
	}

	gettimeofday(&t1,0);
	long elapsed = t1.tv_usec-t0.tv_usec;
	cout << "Time :"<<  double(elapsed)/1000000 << endl;
	
	cout << "done " <<endl;

	while(true){
		imshow("Adaptive Histogram Equalized Image",O_GRAY);
		waitKey(30);
	}

	return 0;

}
