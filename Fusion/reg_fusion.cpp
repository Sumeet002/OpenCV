#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

Mat frame,edges,fused;
Mat frame_gray, edges_gray;
Mat edges_aligned;
const int weights_slider_max=10;
int weights_slider;
double weight;

void on_trackbar(int,void *){

	weight=(double)weights_slider/weights_slider_max;
	fused = weight*frame + (1-weight)*edges_aligned;

}

int main(int argc, char* argv[])
{


    	VideoCapture cap(1); // open the video camera no. 0
	VideoCapture cap2(0);
	cap.set(CAP_PROP_FPS,1);
	cap2.set(CAP_PROP_FPS,1);

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

	/*Initialize ECC parameters*/
	// Define the motion model
	const int warp_mode = MOTION_EUCLIDEAN;

	// Set a 2x3 or 3x3 warp matrix depending on the motion model.
	Mat warp_matrix;
		
	// Initialize the matrix to identity
	if ( warp_mode == MOTION_TRANSLATION )
  		warp_matrix = Mat::eye(3, 3, CV_32F);
	else
  		warp_matrix = Mat::eye(2, 3, CV_32F);

	// Specify the number of iterations.
	int number_of_iterations = 100;
 
	// Specify the threshold of the increment
	// in the correlation coefficient between two iterations
	double termination_eps = 1e-10;

	// Define termination criteria
	TermCriteria criteria (TermCriteria::COUNT+TermCriteria::EPS, number_of_iterations, termination_eps);

		

	Mat canvas(480, 640 ,CV_8UC3);
	Mat topleft(canvas, Rect(0, 0, 320, 240));
	Mat topright(canvas, Rect(320, 0, 320, 240));
	Mat bottom_middle(canvas,Rect(160,240,320,240));
    	
	Size size(320,240);


	while (1){
        	
		bool bSuccess = cap.read(frame); // read a new frame from video
		bool bSuccess2= cap2.read(edges);

		if (!bSuccess){ //if not success, break loop
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}
		resize(frame,frame,size);
		resize(edges,edges,size);

		
		/*ECC based Registration*/
		
		// Convert images to gray scale;
		cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
		cvtColor(edges, edges_gray, COLOR_BGR2GRAY);

		// Run the ECC algorithm. The results are stored in warp_matrix.
		findTransformECC(frame_gray,edges_gray,warp_matrix,warp_mode,criteria);
 
		if (warp_mode != MOTION_HOMOGRAPHY)
    			// Use warpAffine for Translation, Euclidean and Affine
    			warpAffine(edges, edges_aligned, warp_matrix, frame.size(), INTER_LINEAR + WARP_INVERSE_MAP);
		else
    			// Use warpPerspective for Homography
    			warpPerspective (edges, edges_aligned, warp_matrix, frame.size(),INTER_LINEAR + WARP_INVERSE_MAP);
		

		on_trackbar(weights_slider,0);
		frame.copyTo(topleft);
		edges_aligned.copyTo(topright);
		fused.copyTo(bottom_middle);
		
		imshow("MyVideo",canvas);
		

        	if (waitKey(30) == 27){ //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
			cout << "esc key is pressed by user" << endl;
			break; 
		}
    }
    return 0;

}
