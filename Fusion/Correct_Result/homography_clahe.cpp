#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/opencv.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

Mat ch1_orig,ch2_orig,fused;
Mat ch1,ch2;
Mat ch1_gray, ch2_gray;
Mat ch2_aligned;
Mat H;
Mat O_HIST;


/*weighted fusion*/
const int weights_slider_max=10;
int weights_slider;
double weight;

/*ref_image and unreg_image pts*/
Point2f ref_pts[10];
Point2f unreg_pts[10];
int ref_index,unreg_index;
bool ok = false;
bool reg=true;
int mode=0;



static void onMouse( int event, int x, int y, int, void* ){

	if(event == CV_EVENT_LBUTTONDOWN && ok==false){

		if(x>320){
			unreg_pts[unreg_index].x=x;
			unreg_pts[unreg_index].y=y;
			cout << "x coordinate unreg:"<< x << " y coordinate unreg:"<< y << endl;
			unreg_index++;
		} 
		else{
			ref_pts[ref_index].x=x;
			ref_pts[ref_index].y=y;
			cout << "x coordinate ref:"<< x << " y coordinate ref:"<< y << endl;
			ref_index++;
		}
		
	}

	if(event == CV_EVENT_RBUTTONDOWN){

		if(ref_index == unreg_index){
			ok=true;
			reg=false;
		}
	}



}

void on_trackbar(int,void *){

	weight=(double)weights_slider/weights_slider_max;
	fused = weight*ch1 + (1-weight)*ch2_aligned;

}

/*void on_select_points_button_clicked(int,void *){

	reg=true;

}*/

int main(int argc, char* argv[])
{

	reg=true;
    	VideoCapture cap1(0); // open the video camera no. 0
	VideoCapture cap2(1);

	namedWindow("MyVideo",WINDOW_AUTOSIZE); //create a window called "MyVideo"
	namedWindow("select points",WINDOW_AUTOSIZE);

	//Create a Trackbar
	char TrackbarName[50];
 	sprintf( TrackbarName, "Weights");
	//createTrackbar( TrackbarName, "MyVideo", &weights_slider, weights_slider_max, on_trackbar );

	//Create a button
	//createButton("register",on_select_points_button_clicked,NULL,CV_PUSH_BUTTON);


	Mat canvas(480,1280,CV_8UC3);
	Mat topleft_canvas(canvas, Rect(0, 0, 320, 240));
	Mat topright_canvas(canvas, Rect(320, 0, 320, 240));
	
	Mat canvas_final(480, 640 ,CV_8UC3);
	Mat topleft_canvas_final(canvas_final, Rect(0, 0, 320, 240));
	Mat topright_canvas_final(canvas_final, Rect(320, 0, 320, 240));
	Mat bottom_middle_canvas_final(canvas_final,Rect(160,240,320,240));
    	
	Size size(320,240);

	setMouseCallback( "select points", onMouse, 0 );
	ref_index=0;
	unreg_index=0;
	Ptr<CLAHE> clahe = createCLAHE(3.0, cv::Size(8,8));
	

	while (1){
        	
		//bool bSuccess = cap1.read(ch1_orig); // read a new frame from video
		//bool bSuccess2= cap2.read(ch2_orig);
		cap1.grab();
		cap2.grab();
		cap1.retrieve(ch1);
		cap2.retrieve(ch2);

		/*if (!bSuccess){ //if not success, break loop
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}
		if (!bSuccess2){ //if not success, break loop
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}*/

		//resize(ch1_orig,ch1,size);
		//resize(ch2_orig,ch2,size);

		if((waitKey(30) & 255) == 'p')
		mode=1;
		

		if(reg && mode==1){
			
			ch1.copyTo(topleft_canvas);
			ch2.copyTo(topright_canvas);
			while(1){  
  				if(ok == true) //right button click  
   				break;  
  
  				//draw point  
				for(int i=0;i<ref_index;++i){
					circle(canvas, ref_pts[i], 2,CV_RGB(255,0,0),5); 
					circle(canvas, unreg_pts[i], 2,CV_RGB(0,0,255),5); 
				}  
  				imshow("select points", canvas);  
      
 				if (char(waitKey(30) & 255) == 'q')
				{	destroyAllWindows();
					break;  }
 			}
			
			 //prepare to get homography matrix  
 			 vector< Point2f> P1(ref_index); //clicked positions  
 			 vector< Point2f> P2(unreg_index); //user setting positions 

			 cout << ref_index << endl;
			 cout << unreg_index << endl;
		
			 for(int i=0;i<ref_index;i++){
			 	P1[i].x = ref_pts[i].x; P1[i].y = ref_pts[i].y;
			 }
		         for(int i=0;i<unreg_index;i++){
			 	P2[i].x = unreg_pts[i].x-320; P2[i].y = unreg_pts[i].y;
			 }
   
                          
			 //get homography 

			 cout << P1 << endl;
			 cout << P2 << endl; 
 			 H = findHomography(P2, P1,CV_RANSAC);
			 

			 //calculation confirm  
 			cout << "h" << endl << H << endl;  
 			cout << "size rows and cols " << H.rows << " " << H.cols << endl;  
  
 			Mat A(3,ref_index,CV_64F); //3xN, P1  
 			Mat B(3,ref_index,CV_64F); //3xN, P2  
   
  
  
 			for(int i=0; i< ref_index; ++i){  
  				A.at< double>(0,i) = P2[i].x;  
  				A.at< double>(1,i) = P2[i].y;  
  				A.at< double>(2,i) = 1;  
    
  
  				B.at< double>(0,i) = P1[i].x;  
  				B.at< double>(1,i) = P1[i].y;  
  				B.at< double>(2,i) = 1;  
 			}  
  
 			cout << "a" << endl << A << endl;  
 			cout << "b" << endl << B << endl;  
 			Mat HA = H*A;  
   
			for(int i=0; i< ref_index; ++i){  
  				HA.at< double>(0,i) /= HA.at< double>(2,i);  
  				HA.at< double>(1,i) /= HA.at< double>(2,i);  
  				HA.at< double>(2,i) /= HA.at< double>(2,i);  
 			}  
  
 			cout << "HA" << endl << HA << endl;


		}

		else if(mode==1){
			
			warpPerspective(ch2, ch2_aligned,H,size);
			//ch1.copyTo(topleft_canvas_final);
			//ch2_aligned.copyTo(topright_canvas_final);
			cvtColor(ch2_aligned,ch2_aligned,CV_BGR2GRAY);
			clahe->apply(ch2_aligned,O_HIST);
			//Sobel(ch2_aligned,ch2_aligned,-1,0,1,3);
			cvtColor(ch2_aligned,ch2_aligned,CV_GRAY2RGB);
			
			fused=0.4*ch1+0.6*O_HIST;
			//on_trackbar(weights_slider,0);
			fused.copyTo(canvas_final);
		}


		imshow("MyVideo",canvas_final);
		

        	if (char(waitKey(30) & 255) == 'q'){ //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
			cout << "esc key is pressed by user" << endl;
			destroyAllWindows();
			break; 
		}
    }
    return 0;

}
