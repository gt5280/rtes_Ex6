/*
 * 
 * Michael Fruge, Bryan Cisneros and Nelson Rodriguez
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
#include <sstream>


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

using namespace cv;
using namespace std;


int ii=0;
volatile uint8_t cnt_Img= 0;

bool gate_RoI= true;
bool gate_CalComplete= false;
bool gate_OffImage= false;
bool gate_OnImage= true;


double x_min= 0.0, y_min= 0.0, x_max= 0.0, y_max= 0.0; 
Point minIdx(x_min, y_min);
Point maxIdx(x_max, y_max);
double minVal, min_Next;
double maxVal, max_Next;
//Point minIdx, min_NextIdx;
//Point maxIdx, max_NextIdx;//<---------------<<RoI
char off_, next_;
#define PAD_LOW 30
uint32_t cnt= 0;


// display window
char timg_window_name[] = "Morse test";

Mat image, grayImage, grayCal, cal_image, off_image, on_image, on_gray;
IplImage* frame;

uint32_t idx= 0, jdx= 1;
volatile uint32_t fib= 0, fib0= 0, fib1= 1;

#define FIB_TEST(seqCnt, iterCnt)	\
	for(idx=0; idx < iterCnt; idx++)\
	{				\
	fib = fib0 + fib1;		\
	while(jdx < seqCnt)		\
	{				\
	fib0 = fib1;			\
	fib1 = fib;			\
	fib = fib0 + fib1;		\
	jdx++;				\
	}				\
	jdx=0;				\
	}

int abortTest = 0;

#define ONE_MS (1000000L)
#define PERIOD_MS (60)//(80) 48 works for f_01
#define PERIOD_SLEEP (PERIOD_MS * ONE_MS)

// main thread
int rt_max_prio, rt_min_prio;
pthread_attr_t main_attr;
pid_t mainpid;
struct sched_param rt_param_main;

// calibrate thread
pthread_attr_t rt_sched_attr_cal;
struct sched_param rt_param_cal;
pthread_t thread_cal;

// acquire thread
pthread_attr_t rt_sched_attr_acq;
struct sched_param rt_param_acq;
pthread_t thread_acq;

// xform thread (gray transform)
pthread_attr_t rt_sched_attr_xform;
struct sched_param rt_param_xform;
pthread_t thread_xform;

// RoI thread
pthread_attr_t rt_sched_attr_RoI;
struct sched_param rt_param_RoI;
pthread_t thread_RoI;

// process thread
pthread_attr_t rt_sched_attr_proc;
struct sched_param rt_param_proc;
pthread_t thread_proc;

// logging thread
pthread_attr_t rt_sched_attr_logging;
struct sched_param rt_param_logging;
pthread_t thread_logging;

sem_t service_sem, logging_sem, xform_sem, RoI_sem, proc_sem, cal_sem;

struct timeval start, stop;

#define HRES 320//640
#define VRES 240//480

bool thread_running = false;

#define SCHEDULING (SCHED_FIFO)//(PTHREAD_EXPLICIT_SCHED)//(SCHED_OTHER)


/*------------------------functions/threads---------------------------*/


double timeElapsed(struct timeval start, struct timeval stop)
{
  long seconds = (stop.tv_sec - start.tv_sec);
  long micros = (seconds * 1000000) + stop.tv_usec - start.tv_usec;

  return (double)micros / 1000000;
}

void f_01(void)
{
if(ii> 9) ii=0;//<<----------------------------------------<<
Mat mat_frame = cvarrToMat(frame);//conver to mat
//circle( mat_frame, maxIdx, 5, ( 255, 0, 0 ), 1, 8, 0 );
ii= ii+ 1;
cnt_Img= cnt_Img+ 1;
std::ostringstream name;
name << "img_" << ii << ".ppm";
//cvSaveImage("test_00.ppm",frame);
cv::imwrite(name.str(), mat_frame);

}

/*-----------------------------------------acquire------------------------------------------*/
void* acq(void* param){

cvNamedWindow( timg_window_name, CV_WINDOW_AUTOSIZE );
CvCapture* capture;
capture = (CvCapture *)cvCreateCameraCapture(0);
cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, HRES);
cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, VRES);

while(!abortTest){

	sem_wait(&service_sem);     
	thread_running = true;

	// acquire
	frame=cvQueryFrame(capture);
	if(!frame) break;

	f_01();
	//cout << "acq" << endl;

	gettimeofday(&stop, NULL);

	sem_post(&xform_sem);
      
	thread_running = false;
	}
pthread_exit(0);
}

/*-----------------------------------------transform------------------------------------------*/
void* xform(void* param) 
{

	while(!abortTest){
		sem_wait(&xform_sem);
		thread_running = true;
		std::ostringstream name;
		name << "img_" << ii << ".ppm";
		image = imread( name.str(), CV_LOAD_IMAGE_COLOR );
    		if ( !image.data ) {
        		cout << "Could not open the image file" << endl;
        		//return -1;
		}

		cvtColor( image, grayImage, CV_BGR2GRAY );

		if(!gate_OnImage){
			if(ii> 9){
				on_image = imread( "img_9.ppm", CV_LOAD_IMAGE_COLOR );
    				if ( !on_image.data ) 
        				cout << "Could not open On image" << endl;
				else{
					cvtColor( on_image, on_gray, CV_BGR2GRAY );
					cv::imwrite( "on_.ppm", on_gray );
					
					gate_OnImage= true;
					gate_CalComplete= true;
					}
				}
			}

		if(!gate_OffImage){
			cv::imwrite( "off_.ppm", grayImage );
			sem_post(&cal_sem);
		}			
		
		//cout << "xfrm " << ii << endl;
		if(gate_CalComplete){
			sem_post(&RoI_sem);
			}

		thread_running = false;
	}
	pthread_exit(0);
}

/*----------------------------------------calibrate-------------------------------------------*/
void* cal(void* param)//This only prints to screen
{

while(!abortTest){
	sem_wait(&cal_sem);
	thread_running = true;
	if(!gate_CalComplete){		
		cout << "cal" << endl;
	}
	sem_post(&logging_sem);
	thread_running = false;
	}
pthread_exit(0);
}

/*--------------------------------------------RoI--------------------------------------------*/
void* RoI(void* param){

	while(!abortTest){

		sem_wait(&RoI_sem);

		if(gate_RoI){
			grayCal = imread("on_.ppm", IMREAD_GRAYSCALE);//<-----<<read on
    			if ( !grayCal.data )
        			cout << "Could not open RoI image" << endl;
 
			cv::minMaxLoc( grayCal, &minVal, &maxVal, &minIdx, &maxIdx );//get RoI
			circle( grayCal, maxIdx, 5, ( 255, 0, 0 ), 1, 8, 0 );

			cal_image = imread("off_.ppm", IMREAD_GRAYSCALE);//<-------<<read off
    			if ( !cal_image.data )
        			cout << "Could not open Cal image" << endl;

			off_= cal_image.at<char>( maxIdx );//<----------------------<<train off
			off_= off_+ PAD_LOW;
			cout << "Calibration complete!" << endl;
			cv::imwrite( "RoI_.ppm", grayCal );
			gate_RoI= false;
		}

		sem_post(&proc_sem);
      
		thread_running = false;
		}
	pthread_exit(0);
}

/*------------------------------------------process------------------------------------------*/
void* proc(void* param)
{

while(!abortTest){

	sem_wait(&proc_sem);
	thread_running = true;

	if ( !grayImage.data )
        cout << "Could not open gray image" << endl;

	next_= grayImage.at<char>( maxIdx ); //sample
		
	if( off_ < next_ ){
		cout << "On" << endl;
	}
	else{
		cout << "Off" << endl;
	}

	cv::imwrite( "next_.ppm", grayImage );

	//cout << "proc" << endl;

	if(ii> 9)//<<-------------------------------------------<<
		sem_post(&logging_sem);
      
	thread_running = false;
	}
pthread_exit(0);
}

/*------------------------------------------logging------------------------------------------*/
void* logging(void* param)
{

while(!abortTest){

	sem_wait(&logging_sem);
	thread_running = true;
	//cout << "logging" << endl;
	thread_running = false;
	}
pthread_exit(0);
}

void shutdown(void)
{
	abortTest=1;
}

/*-------------------------------------------main-------------------------------------------*/
int main(void){

  abortTest=0;

  mainpid=getpid();

  sem_init(&service_sem, 0, 0);

  rt_max_prio = sched_get_priority_max(SCHED_FIFO);

/*-----------------------------main---------------------------------*/
  // Set this thread (main thread) to use FIFO scheduling with the highest possible priority
  int rc=sched_getparam(mainpid, &rt_param_main);
  rt_param_main.sched_priority=rt_max_prio;
  rc=sched_setscheduler(0, SCHEDULING, &rt_param_main);
  if(rc < 0) perror("main_param");

/*----------------------------acquire--------------------------------*/
  // Create the acquire thread, give it priority 2
  rc=pthread_attr_init(&rt_sched_attr_acq);
  rc=pthread_attr_setinheritsched(&rt_sched_attr_acq, PTHREAD_EXPLICIT_SCHED);
  rc=pthread_attr_setschedpolicy(&rt_sched_attr_acq, SCHEDULING);

  rt_param_acq.sched_priority=rt_max_prio-1;
  pthread_attr_setschedparam(&rt_sched_attr_acq, &rt_param_acq);

  pthread_create(&thread_acq,   // pointer to thread descriptor
                &rt_sched_attr_acq, // attributes
                acq, // thread function entry point
                NULL // parameters to pass in
                );

/*------------------------------transform------------------------------*/
  // Create the xform thread, give it priority 3
  rc=pthread_attr_init(&rt_sched_attr_xform);
  rc=pthread_attr_setinheritsched(&rt_sched_attr_xform, PTHREAD_EXPLICIT_SCHED);
  rc=pthread_attr_setschedpolicy(&rt_sched_attr_xform, SCHEDULING);

  rt_param_xform.sched_priority=rt_max_prio-2;
  pthread_attr_setschedparam(&rt_sched_attr_xform, &rt_param_xform);

  pthread_create(&thread_xform,   // pointer to thread descriptor
                &rt_sched_attr_xform, // attributes
                xform, // thread function entry point
                NULL // parameters to pass in
                );

/*----------------------------calibrate--------------------------------*/
  // Create the calibrate thread, give it priority 4
  rc=pthread_attr_init(&rt_sched_attr_cal);
  rc=pthread_attr_setinheritsched(&rt_sched_attr_cal, PTHREAD_EXPLICIT_SCHED);
  rc=pthread_attr_setschedpolicy(&rt_sched_attr_cal, SCHEDULING);

  rt_param_cal.sched_priority=rt_max_prio-3;
  pthread_attr_setschedparam(&rt_sched_attr_cal, &rt_param_cal);

  pthread_create(&thread_cal,   // pointer to thread descriptor
                &rt_sched_attr_cal, // attributes
                cal, // thread function entry point
                NULL // parameters to pass in
                );

/*--------------------------------RoI-------------------------------*/
  // Create the RoI thread, give it priority 5
  rc=pthread_attr_init(&rt_sched_attr_RoI);
  rc=pthread_attr_setinheritsched(&rt_sched_attr_RoI, PTHREAD_EXPLICIT_SCHED);
  rc=pthread_attr_setschedpolicy(&rt_sched_attr_RoI, SCHEDULING);

  rt_param_RoI.sched_priority=rt_max_prio-4;
  pthread_attr_setschedparam(&rt_sched_attr_RoI, &rt_param_RoI);

  pthread_create(&thread_RoI,   // pointer to thread descriptor
                &rt_sched_attr_RoI, // attributes
                RoI, // thread function entry point
                NULL // parameters to pass in
                );

/*--------------------------------proc------------------------------*/
  // Create the process thread, give it priority 6
  rc=pthread_attr_init(&rt_sched_attr_proc);
  rc=pthread_attr_setinheritsched(&rt_sched_attr_proc, PTHREAD_EXPLICIT_SCHED);
  rc=pthread_attr_setschedpolicy(&rt_sched_attr_proc, SCHEDULING);

  rt_param_proc.sched_priority=rt_max_prio-5;
  pthread_attr_setschedparam(&rt_sched_attr_proc, &rt_param_proc);

  pthread_create(&thread_proc,   // pointer to thread descriptor
                &rt_sched_attr_proc, // attributes
                proc, // thread function entry point
                NULL // parameters to pass in
                );
/*---------------------------------logging------------------------------*/
  // Create the logging thread, give it priority 7
  rc=pthread_attr_init(&rt_sched_attr_logging);
  rc=pthread_attr_setinheritsched(&rt_sched_attr_logging, PTHREAD_EXPLICIT_SCHED);
  rc=pthread_attr_setschedpolicy(&rt_sched_attr_logging, SCHEDULING);

  rt_param_logging.sched_priority=rt_max_prio-6;
  pthread_attr_setschedparam(&rt_sched_attr_logging, &rt_param_logging);

  pthread_create(&thread_logging,   // pointer to thread descriptor
                &rt_sched_attr_logging, // attributes
                logging, // thread function entry point
                NULL // parameters to pass in
                );

  printf("Starting Sequencer\n");

  // Set up a struct to sleep
  struct timespec periodic_sleep, out;
  periodic_sleep.tv_sec = 0;
  periodic_sleep.tv_nsec = PERIOD_SLEEP;
  

  // Enable the task
  sem_post(&service_sem);
  
  // sleep for a few seconds to allow the thread to initialize
  sleep(5);

  // Set start to the current time
  gettimeofday(&start, NULL);

  while(1)
  {
    nanosleep(&periodic_sleep, &out);
    if (thread_running)
      printf("xxxx!\n");

/*--------------------------single time events for calibration------------------------------*/
	if( !gate_OffImage ){
	cout << "Please turn camera on to complete calibration " << endl;	
	cin.get();
	gate_OnImage= false;
	gate_OffImage= true;
	}

    sem_post(&service_sem);
  }

  abortTest = 1;

  return 0;
}
                                                                  
