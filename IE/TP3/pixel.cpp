#include <opencv2/opencv.hpp>
using namespace cv;

int main() {
	VideoCapture cap(0);
	if(!cap.isOpened()) {
		printf("Error !!");
		return -1;
	}
	
	Mat image, frame;
	Vec3b color;
	namedWindow("rose effect", 0);
	for(;;) {
		cap >> frame;
		image = frame;
		for(int y = 0; y < image.rows; y++) {
			for(int x = 0; x < image.cols; x++) {
				color = image.at<Vec3b>(Point(x, y));

				color.val[0] = 255;
				color.val[2] = 255;

				image.at<Vec3b>(Point(x, y)) = color;
			}
		}
		imshow("rose effect", image);
		if(waitKey(33) == 27) break;
	}

	return 0;
}
