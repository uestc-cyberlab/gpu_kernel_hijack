#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;

int main()
{
	//1.read image
	Mat image_r = imread("tumor.jpg", IMREAD_GRAYSCALE); //read origional image
	imshow("image_r", image_r);
	int h =image_r.rows;
	int w =image_r.cols;
	int chan = image_r.channels(); //grayscale image's channel is 1
	cout<<"resolution is "<<w<<"* "<<h<<endl;
	cout<<"channel is "<<chan<<endl;
	
	
	//2.process image
	//2.1.image filtering
	Mat image_f;
	medianBlur(image_r, image_f, 5); //median filtering
	//blur(image_r, image_f, Size(5,5));
	//2.2.copy image into array
	vector<vector<unsigned char>> array(h, vector<unsigned char>(w, 0)); //create 2-D array with initialized vaules 0
	for(int i=0; i<h; i++) //copy image into array
	{
		unsigned char* list = image_f.ptr<unsigned char>(i); //one row in image
		for(int j=0; j<w; j++)
		{
			array[i][j] = list[j];
		}
	}
	//2.3.threshold segmentation
	int threshold = 230;
	for(int i=0; i<h; i++) //copy image into array
	{
		for(int j=0; j<w; j++)
		{
			if(array[i][j]>= threshold)
			{
				array[i][j] = 255;
			}
			else
			{
				array[i][j] = 0;
			}
		}
	}
	
	
	//3.save image
	Mat image_p(h, w, CV_8U, 255); //create new image, CV_8U is data type, 255 is initialized value
	for(int i=0; i<h; i++) //copy array into image
	{
		unsigned char* list = image_p.ptr<unsigned char>(i);
		for(int j=0; j<w; j++)
		{
			list[j] = array[i][j];
		}
	}
	imshow("image_p", image_p);
	imwrite("image_p.jpg", image_p); //save image

	
	waitKey(0); //pause terminal to show image
	return 0;
}


//g++ test.cpp -o test `pkg-config --cflags --libs opencv`

