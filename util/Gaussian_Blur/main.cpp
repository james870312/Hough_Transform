#include <iostream>
#include <string.h> 
#include <math.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
	//Load image
	const char* input = argv[1];
	Mat src = imread(input, IMREAD_COLOR);

	if (src.empty())
	{
		cout << "Failed to load " << input << endl;
		return -1;
	}

    //set parameter
    float sigma = 1;
    int kernel = 3;
    float sum = 0;

	//Create gaussian kernel
	Mat G_kernel(kernel, kernel, CV_32F, Scalar::all(0));
    for(int i=0; i<kernel; i++)
    {
        for(int j=0; j<kernel; j++)
        {
            G_kernel.at<float>(i,j) = exp((-1)*(pow((i)-int(kernel/2), 2)+pow((j)-int(kernel/2), 2))/2*pow(sigma, 2));
            sum = sum + G_kernel.at<float>(i,j);
        }
    }

    //creat Gray Mat
    Mat gray(src.rows, src.cols, CV_8U);
    cout << "rows = " << src.rows << endl;
    cout << "cols = " << src.cols << endl;

    //RGB to Gray = 0.299 * Red + 0.587 * Green + 0.114 * Blue
    for(int i=0; i<src.rows; i++)
    {
        for(int j=0; j<src.cols; j++)
        {
            gray.at<uchar>(i, j) = 0.114*src.at<uchar>(i, 3*j) 
                                    + 0.587*src.at<uchar>(i, 3*j+1) 
                                    + 0.299*src.at<uchar>(i, 3*j+2);
        }
    }

    //print blur kernel
    cout << "blur kernel:" << endl;
    for(int i=0; i<kernel; i++)
    {
        for(int j=0; j<kernel; j++)
        {
        cout << G_kernel.at<float>(i, j) << "\t" ;
        }
        cout << endl;
    }

    //convolution operation with Gaussian kernel
    Mat blur(gray.rows-2, gray.cols-2, CV_8U);

    for(int i=0; i<gray.rows-2; i++)
    {
        for(int j=0; j<gray.cols-2; j++)
        {
            int cal = 0;
            for(int k=0; k<kernel; k++)
            {
                for(int n=0; n<kernel; n++)
                {
                    cal = cal + G_kernel.at<float>(k, n)*gray.at<uchar>(i+k, j+n);
                }
            }
            blur.at<uchar>(i, j) = cal/sum;
        }
    }

	//Show result
	imshow("input", src);
	imshow("Gray", gray);
    imshow("blur", blur);
	waitKey(0);

	destroyAllWindows();

	return 0;
}
