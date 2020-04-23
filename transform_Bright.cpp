/*
references: 
https://stackoverflow.com/questions/10344246/how-can-i-convert-a-cvmat-to-a-gray-scale-in-opencv
https://docs.opencv.org/3.4/d4/da8/group__imgcodecs.html#ga288b8b3da0892bd651fce07b3bbd3a56
*/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <time.h>
#include <sys/time.h>
#include <semaphore.h>
#include <stdint.h>
#include <unistd.h>
#include "cstdlib"
#include "iostream"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    struct timeval initial_time_val;
    struct timeval final_time_val;
    printf( "Locate ROI\n" );

    double minVal;
    double maxVal;
    Point minIdx;
    Point maxIdx;
    int thickness=1;
    int lineType=8;
    int shift=0;

    if (argc != 2) {
    cout << "Usage: display_Image ImageToLoadandDisplay" << endl;
    return -1;
    }
    else{
    Mat image;
    Mat grayImage;

    image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    if (!image.data) {
        cout << "Could not open the image file" << endl;
        return -1;
    }
    else {

        gettimeofday(&initial_time_val, (struct timezone *)0);

        int height = image.rows;
        int width = image.cols;

        cvtColor(image, grayImage, CV_BGR2GRAY);

	cv::minMaxLoc( grayImage, &minVal, &maxVal, &minIdx, &maxIdx );	

	circle( grayImage, maxIdx, 5, (255, 0, 0), thickness, lineType, shift );

        gettimeofday(&final_time_val, (struct timezone *)0);
        printf( "ROI found in: %ldus\n", (long)(final_time_val.tv_usec- initial_time_val.tv_usec ));

        namedWindow("Bright Image", WINDOW_AUTOSIZE);
        imshow("Bright Image", grayImage);

        cvWaitKey(0);
        image.release();
        grayImage.release();
        return 0;
    }
  }
}
