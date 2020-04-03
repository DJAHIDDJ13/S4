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
	namedWindow("Color to BW", 0);
	for(;;) {
		cap >> frame;
		image = frame;
		for(int y = 0; y < image.rows; y++) {
			for(int x = 0; x < image.cols; x++) {
				color = image.at<Vec3b>(Point(x, y));
				
				int avg = (color.val[0] + color.val[1] + color.val[3]) / 3;
				color.val[0] = avg; color.val[1] = avg; color.val[2] = avg;

				image.at<Vec3b>(Point(x, y)) = color;
			}
		}
		imshow("Color to BW", image);
		if(waitKey(33) == 27) break;
	}

	return 0;
}
