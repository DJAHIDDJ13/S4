#include <ctime>
#include <iostream>
#include <raspicam/raspicam_cv.h>

using namespace std;

int main(int argc, char **argv)
{
	time_t timer_begin, timer_end;
	raspicam::RaspiCam_Cv Camera;
	cv::Mat image;
	int nCount = 100;
	// set cam params
	Camera.set(CV_CAP_PROP_FORMAT, CV_8UC1);
	// Open camera
	cout << "Opening camera..." << endl;
	if(!Camera.open()) {
		cerr << "Error opening the camera" << endl;
		return -1;
	}

	// Start capture
	cout << "Capturing " << nCount << " frames ...." << endl;
	time(&timer_begin);
	for(int i = 0; i < nCount; i++) {
		Camera.grab();
		Camera.retrieve(image);
		if(i % 5 == 0) cout << "\r captured "<<i<<" images"<<std::flush;
	}

	cout<<"stop camera..."<<endl;
	Camera.release();
	//show time statistics
	time(&timer_end);

	double secondsElapsed = difftime(timer_end, timer_begin);
	cout << secondsElapsed << " seconds for " << nCount << " frames: FPS=" << (float) ((float) (nCount) / secondsElapsed) << endl;
	cv::imwrite("raspicam_cv_image.jpg", image);
	cout << "Image saved at raspicam_cv_image.jpg"<<endl;
	cout << "Image size (" << image.rows << ", " << image.cols << ")" << endl;
}
