#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat frame,edges,fused;
const int weights_slider_max=10;
int weights_slider;
double weight;


void on_trackbar(int,void *){

	weight=(double)weights_slider/weights_slider_max;
	fused = weight*frame + (1-weight)*edges;

}

int main(int argc, char* argv[])
{


    	VideoCapture cap(0); // open the video camera no. 0
	VideoCapture cap2(1);

    	if (!cap.isOpened()){  // if not success, exit program
    	
		cout << "Cannot open the video cam" << endl;
        	return -1;
    	}

	double dWidth = cap.get(CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = cap.get(CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

	cout << "Frame size : " << dWidth << " x " << dHeight << endl;

	namedWindow("MyVideo",WINDOW_AUTOSIZE); //create a window called "MyVideo"

	//Create a Trackbar
	char TrackbarName[50];
 	sprintf( TrackbarName, "Weights");
	createTrackbar( TrackbarName, "MyVideo", &weights_slider, weights_slider_max, on_trackbar );
	
	Mat canvas(480, 640 ,CV_8UC3);
	Mat topleft(canvas, Rect(0, 0, 320, 240));
	Mat topright(canvas, Rect(320, 0, 320, 240));
	Mat bottom_middle(canvas,Rect(160,240,320,240));
    	
	Size size(320,240);


	while (1){
        	
		bool bSuccess = cap.read(frame); // read a new frame from video
		cap2.read(edges);
		if (!bSuccess){ //if not success, break loop
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}
		resize(frame,frame,size);
		resize(edges,edges,size);

		//cvtColor(frame, edges, COLOR_BGR2RGB);
		on_trackbar(weights_slider,0);
		frame.copyTo(topleft);
    		edges.copyTo(topright);
		fused.copyTo(bottom_middle);
		
		imshow("MyVideo",canvas);
		

        	if (waitKey(30) == 27){ //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
			cout << "esc key is pressed by user" << endl;
			break; 
		}
    }
    return 0;

}
