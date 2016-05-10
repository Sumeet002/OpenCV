#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <sys/time.h>

using namespace cv;
using namespace std;


void calcLutBody(const cv::Mat& src, const cv::Mat& lut, const cv::Size& tileSize, const int& tilesX, const int& clipLimit, const float& lutScale){
	
	unsigned char *tileLut = lut.ptr<uchar>(0);
        const size_t lut_step = lut.step / sizeof(uchar);


        for(int k = 0 ; k < tileSize.width*tileSize.height ; ++k,tileLut+=lut_step){
	    const int ty = k / tilesX;
            const int tx = k % tilesX;	

	    // retrieve tile submatrix
            Rect tileROI;
            tileROI.x = tx * tileSize.width;
            tileROI.y = ty * tileSize.height;
            tileROI.width = tileSize.width;
            tileROI.height = tileSize.height;

	    const Mat tile = src(tileROI);
	    
	    // calc histogram
	    int tileHist[histSize] = {0,};
	    int height = tileROI.height;
            const size_t sstep = src.step / sizeof(uchar);
            for (const uchar* ptr = tile.ptr<uchar>(0); height--; ptr += sstep){
	    	int x = 0;
                for (; x <= tileROI.width - 4; x += 4){
                    int t0 = ptr[x], t1 = ptr[x+1];
                    tileHist[t0]++; tileHist[t1]++;
                    t0 = ptr[x+2]; t1 = ptr[x+3];
                    tileHist[t0]++; tileHist[t1]++;
                }

                for (; x < tileROI.width; ++x)
                    tileHist[ptr[x]]++;
		
	    }

	    // clip histogram
	    if(clipLimit > 0){

		// how many pixels were clipped
                int clipped = 0;
		for (int i = 0; i < histSize; ++i){
                    if (tileHist[i] > clipLimit)
                    {
                        clipped += tileHist[i] - clipLimit;
                        tileHist[i] = clipLimit;
                    }
                }

	       // redistribute clipped pixels
                int redistBatch = clipped / histSize;
                int residual = clipped - redistBatch * histSize;

                for (int i = 0; i < histSize; ++i)
                    tileHist[i] += redistBatch;

                for (int i = 0; i < residual; ++i)
                    tileHist[i]++;
            }

	    // calc Lut
	    int sum = 0;
            for (int i = 0; i < histSize; ++i)
            {
                sum += tileHist[i];
                tileLut[i] = cv::saturate_cast<uchar>(sum * lutScale);
            }
       }

}

void Interpolate(const cv::Mat& src, const cv::Mat& dst, const cv::Mat& lut, const cv::Size& tileSize, const int& tilesX, const int& tilesY){

	AutoBuffer<int> buf;
	int * ind1_p, * ind2_p;
        float * xa_p, * xa1_p;
	


}


int main(int argc, char* argv[])
{

	
        static Mat src;
	
	src =imread("sample2.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	int scale = 2 ;
	Size size(src.cols/scale,src.rows/scale);
	int imgSize= (src.cols/scale)*(src.rows/scale);
        resize(src,src,size);


	imshow("Gray Image" , src);
	waitKey(30);

        //clipLimit
        double clipLimit = 4.0;

        //Tile size
        Size tileGridSize(8,8);
        tilesX = tileGridSize.width;
        tilesY = tileGridSize.height;
        Size tileSize;
       
        Mat src_Ext,lut,dst;
        int histSize = 256;
        cv::InputArray srcForLut;

        if(src.size().width % tilesX == 0 && src.size().height % tilesY == 0){
        	srcForLut = src ;
        }
        else{
		cv::copyMakeBorder(src,src_Ext,0,tilesY - (src.size().height % tilesY),0,tilesX - (src.size().width % tilesX),cv::BORDER_REFLECT_101);
                tileSize = Size(src_Ext.size().width/tilesX,src_Ext.size().height/tilesY);
                srcForLut = src_Ext; 
        }

        const int tileSizeTotal = tileSize.area();
        const float lutScale = static_cast<float>(histSize - 1) / tileSizeTotal;
        
       
        if(clipLimit > 0.0){
        	clipLimit = static_cast<int>(clipLimit * tileSizeTotal / histSize);
		clipLimit = std::max(clipLimit, 1);
        }
   
        
        dst.create(src.size(),src.type());	
        lut.create(tilesX*tilesY,histSize,src.type());

        calcLutBody(srcForLut, lut, tileSize, tilesX, clipLimit, lutScale);
        Interpolate(src, dst, lut, tileSize, tilesX, tilesY);

        
       



	static Mat O_HIST(src.rows,src.cols,CV_8UC1,Scalar(0));


	while(true){
		imshow("Adaptive Histogram Equalized Image",O_HIST);
		waitKey(30);
	}

	return 0;

}
