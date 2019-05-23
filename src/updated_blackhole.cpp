#include <stdio.h> //Declaring the headers
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <string>

using namespace cv;
using namespace std;

Mat hsv_img; //Global Declaration
Mat img, img_corrected, img_gamma_corrected;


void mouseCB(int event, int x, int y, int flags, void* userdata) // Mouse Callback on Hsv image. Outputs the pixel value on the pixel mouse was pressed
{
	if  ( event == EVENT_LBUTTONDOWN )
	{
		cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
		cout << (int)hsv_img.at<Vec3b>(y,x)[0] << " " << (int)hsv_img.at<Vec3b>(y,x)[1] << " " << (int)hsv_img.at<Vec3b>(y,x)[2] << endl;
	}
}

void basicLinearTransform(const double alpha_, const int beta_)
{
    img.convertTo(img_corrected, -1, alpha_, beta_);
}


int main(int argc, char** argv)
{
	string name=argv[1];//Taking image or Video name from command line
	char a;
	cout<<"image(i) or video(v)"; 
	cin>>a;

	Mat red_thr,white_thr,black_thr,p0,high_pr,required; //Declaration of Image names we need. Mat stands for Matrix of image.

	int h_r=0,s_r=210,ht_r=23,st_r=47; //Hue Red, Saturation red, Hue Threshold, Saturation threshold
	int r_w=124, g_w=133,b_w=136, rt_w=120, gt_w=86,bt_w=73 ;//Red white, Green white, Blue White
	int r_b=0,g_b=0,b_b=0,rt_b=110,gt_b=110,bt_b=20; //Red black, 

	int alpha = 360;
	int beta = 00;

	//values accepted are (h_r-ht_r, h_r+ht_r)
	namedWindow("Sliders"); //Slider Interface
	namedWindow("MainIM", WINDOW_NORMAL);
	namedWindow("Brightness and contrast adjustments", WINDOW_NORMAL);
	setMouseCallback("MainIM", mouseCB, NULL);

	createTrackbar("H_r","Sliders",&h_r,255); //Creating sliders
	createTrackbar("S_r","Sliders",&s_r,255);
	createTrackbar("HT_r","Sliders",&ht_r,255);
	createTrackbar("ST_r","Sliders",&st_r,255);

	createTrackbar("r_w","Sliders",&r_w,255);
	createTrackbar("g_w","Sliders",&g_w,255);
	createTrackbar("b_w","Sliders",&b_w,255);
	createTrackbar("rt_w","Sliders",&rt_w,255);
	createTrackbar("gt_w","Sliders",&gt_w,255);
	createTrackbar("bt_w","Sliders",&bt_w,255);

	createTrackbar("r_b","Sliders",&r_b,255);
	createTrackbar("g_b","Sliders",&g_b,255);
	createTrackbar("b_b","Sliders",&b_b,255);
	createTrackbar("rt_b","Sliders",&rt_b,255);
	createTrackbar("gt_b","Sliders",&gt_b,255);
	createTrackbar("bt_b","Sliders",&bt_b,255);

	createTrackbar("Alpha gain (contrast)", "Brightness and contrast adjustments", &alpha, 500);
    createTrackbar("Beta bias (brightness)", "Brightness and contrast adjustments", &beta, 200);


	if (a=='i') //If image
	{
		img=imread(argv[1],IMREAD_COLOR); //Reading

		// cvtColor(img, hsv_img, COLOR_BGR2HSV_FULL); //RGB_IMG->HSV_IMG //COLOR Conversion CODES
		img_corrected = Mat(img.rows, img.cols, img.type());
	    // img_gamma_corrected = Mat(img_original.rows, img_original.cols*2, img_original.type());


		imshow("MainIM",img);
		while(1)
		{

			double alpha_value = alpha / 100.0;
		    int beta_value = beta - 100;
		    basicLinearTransform(alpha_value, beta_value);
		    cvtColor(img_corrected, hsv_img, COLOR_BGR2HSV_FULL); 
		    GaussianBlur( img_corrected, img_corrected, Size(3, 3), 2, 2 );//Applying Gaussian Blur of parameter 1,1
		    GaussianBlur( img_corrected, img_corrected, Size(3, 3), 2, 2 );//Applying Gaussian Blur of parameter 1,1
			//No thresholding has been done on HSV images with V channel as it stands for brightness
			inRange(hsv_img,Scalar(h_r-ht_r,s_r-st_r,0), Scalar(h_r+ht_r,s_r+st_r,255),red_thr);//Thresholding, 1 if value lie between [(h_r-ht_r,s_r-st_r,0),(h_r+ht_r,s_r+st_r,255)] else 0.                  ,
			inRange(img_corrected,Scalar(r_w-rt_w,g_w-gt_w,b_w-bt_w), Scalar(r_w+rt_w,g_w+gt_w,b_w+bt_w),white_thr);
			inRange(img_corrected,Scalar(r_b-rt_b,g_b-gt_b,b_b-bt_b), Scalar(r_b+rt_b,g_b+gt_b,b_b+bt_b),black_thr);

			bitwise_or(red_thr,white_thr,p0);//Img_A[x][y]+Img_B[x][y]
			bitwise_not(p0, high_pr);//Img_A[x][y]'
			bitwise_and(high_pr,black_thr,required);//Img_A[x][y].Img_B[x][y]


			int erosion_type;//Erosion and Dilation initial declaration
			erosion_type = MORPH_RECT;//Erosion type
			int erosion_size=1; //size of the kernel will be erosion_size*2+1
			Mat element =  getStructuringElement( erosion_type,
			Size( 2*erosion_size + 1, 2*erosion_size+1 ),
			Point( erosion_size, erosion_size ) );

			erode( required, required , element );
			// dilate( required, required , element );//Dilate on Required to output Required with element 'element'
			dilate( required, required , element );
			dilate( required, required , element );
			dilate( required, required , element );
			dilate( required, required , element );
			// dilate( required, required , element );
			
			

			namedWindow("hsv_img", WINDOW_NORMAL ); //Window Naming
			namedWindow("red_thr", WINDOW_NORMAL );
			namedWindow("black_thr", WINDOW_NORMAL );
			namedWindow("white_thr", WINDOW_NORMAL );
			namedWindow("p0", WINDOW_NORMAL );
			namedWindow("high_pr", WINDOW_NORMAL );			
			namedWindow("required", WINDOW_NORMAL );

			imshow("hsv_img" 	,hsv_img	); //Showing of image
			imshow("red_thr"	,red_thr 	);
			imshow("black_thr"	,black_thr  );
			imshow("white_thr"	,white_thr 	);
			imshow("p0"			,p0  		);
			imshow("high_pr"	,high_pr  	);			
			imshow("required"	,required  	);
			imshow("Brightness and contrast adjustments", img_corrected);

			imwrite("../output/hsv_img"+name.substr(9,10)+".jpg",hsv_img	); //Writing of image output with custom name adjusted with input image name
			imwrite("../output/red_thr"+name.substr(9,10)+".jpg"	,red_thr 	);
			imwrite("../output/black_thr"+name.substr(9,10)+".jpg"	,black_thr  );
			imwrite("../output/white_thr"+name.substr(9,10)+".jpg"	,white_thr 	);
			imwrite("../output/p0"+name.substr(9,10)+".jpg"			,p0  		);
			imwrite("../output/high_pr"+name.substr(9,10)+".jpg"	,high_pr  	);			
			imwrite("../output/required"+name.substr(9,10)+".jpg"	,required  	);

			waitKey(33);// Pause for 33 ms
		}
	}
	if(a=='v')//If video
	{
		VideoCapture cap(argv[1]);//Object for video capture
		while(1)
		{
			static int i=0; //Counter
			cap >> img; //Store Videocapture output on img
			Mat required, p0,high_pr,black_th;
			// cvtColor(img, hsv_img, COLOR_BGR2HSV_FULL);

			img_corrected = Mat(img.rows, img.cols, img.type());
			double alpha_value = alpha / 100.0;
		    int beta_value = beta - 100;
		    basicLinearTransform(alpha_value, beta_value);
		    cvtColor(img_corrected, hsv_img, COLOR_BGR2HSV_FULL); 
		    GaussianBlur( img_corrected, img_corrected, Size(3, 3), 2, 2 );//Applying Gaussian Blur of parameter 1,1
		    GaussianBlur( img_corrected, img_corrected, Size(3, 3), 2, 2 );//Applying Gaussian Blur of parameter 1,1


			inRange(hsv_img,Scalar(h_r-ht_r,s_r-st_r,0), Scalar(h_r+ht_r,s_r+st_r,255),red_thr);//Thresholding, 1 if value lie between [(h_r-ht_r,s_r-st_r,0),(h_r+ht_r,s_r+st_r,255)] else 0.                  ,
			inRange(img_corrected,Scalar(r_w-rt_w,g_w-gt_w,b_w-bt_w), Scalar(r_w+rt_w,g_w+gt_w,b_w+bt_w),white_thr);
			inRange(img_corrected,Scalar(r_b-rt_b,g_b-gt_b,b_b-bt_b), Scalar(r_b+rt_b,g_b+gt_b,b_b+bt_b),black_thr);

			bitwise_or(red_thr,white_thr,p0);
			bitwise_not(p0, high_pr);
			bitwise_and(high_pr,black_thr,required);


			int erosion_type;
			erosion_type = MORPH_RECT;
			int erosion_size=1; //size of the kernel will be erosion_size*2+1
			Mat element =  getStructuringElement( erosion_type,
			Size( 2*erosion_size + 1, 2*erosion_size+1 ),
			Point( erosion_size, erosion_size ) );

			erode( required, required , element );
			// dilate( required, required , element );//Dilate on Required to output Required with element 'element'
			dilate( required, required , element );
			dilate( required, required , element );
			dilate( required, required , element );
			dilate( required, required , element );

			namedWindow("hsv_img", WINDOW_NORMAL );
			namedWindow("red_thr", WINDOW_NORMAL );
			namedWindow("black_thr", WINDOW_NORMAL );
			namedWindow("white_thr", WINDOW_NORMAL );
			namedWindow("p0", WINDOW_NORMAL );
			namedWindow("high_pr", WINDOW_NORMAL );			
			namedWindow("required", WINDOW_NORMAL );

			imshow("hsv_img" 	,hsv_img	);
			imshow("red_thr"	,red_thr 	);
			imshow("black_thr"	,black_thr  );
			imshow("white_thr"	,white_thr 	);
			imshow("p0"			,p0  		);
			imshow("high_pr"	,high_pr  	);			
			imshow("required"	,required  	);
			imshow("MAINIM"		,img);
			// imwrite("../vid_output/hsv_img"+name.substr(9,10)+".jpg",hsv_img	);
			// imwrite("../vid_output/red_thr"+name.substr(9,10)+".jpg"	,red_thr 	);
			// imwrite("../vid_output/black_thr"+name.substr(9,10)+".jpg"	,black_thr  );
			// imwrite("../vid_output/white_thr"+name.substr(9,10)+".jpg"	,white_thr 	);
			// imwrite("../vid_output/p0"+name.substr(9,10)+".jpg"			,p0  		);
			// imwrite("../vid_output/high_pr"+name.substr(9,10)+".jpg"	,high_pr  	);			
			imwrite("../vid_output/required"+ to_string(i)+".jpg"	,required  	);
			i++;
			waitKey(33);
		}
		cap.release();//Releasing all allocated memories to object cap
	    destroyAllWindows();// Destroying all windows
	}
	return 0;
}
