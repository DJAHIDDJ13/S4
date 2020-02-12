#include <opencv2/opencv.hpp>
#include <cmath>
using namespace cv;

void edge_detect(Mat image, Mat* image2) {
	float cannyX[1][3] = {{-1, 0, 1}};
	float cannyY[3][1] = {{-1},
			      {0},
			      {1}};

	Vec3b color;
	
	// X convolution
	for(int y = 0; y < image.rows; y++) {
		for(int x = 0; x < image.cols; x++) {
			double accX = 0.0;
			int n = 0;
			for(int my = 0; my < 3; my++) {
				int nx = x, ny = y + my - 1;
				if(ny >= 0 && ny < image.rows) {
					// find grayscale value	
					color = image.at<Vec3b>(Point(nx, ny));
					accX += cannyX[0][my] * (color.val[0] + color.val[1] + color.val[3]) / 3;
					n++;
				}
			}

			accX /= n;

			double accY = 0.0;
			n = 0;
			for(int mx = 0; mx < 3; mx++) {
				int nx = x + mx - 1, ny = y;
				if(nx >= 0 && nx < image.cols) {
					// find grayscale value	
					color = image.at<Vec3b>(Point(nx, ny));
					accY += cannyY[mx][0] * (color.val[0] + color.val[1] + color.val[3]) / 3;
					n++;
				}
			}

			accY /= n;
			
			Vec3b curColor = image.at<Vec3b>(Point(x, y));
			curColor.val[0] = curColor.val[1] = curColor.val[2] = (sqrt(accX * accX + accY * accY) > 8)? 255: 0 ;
			image2->at<Vec3b>(Point(x, y)) = curColor;
		}
	}

}

int main() {
	VideoCapture cap(0);
	if(!cap.isOpened()) {
		printf("Error !!");
		return -1;
	}
	

	Mat image;
	Mat image2;
	int i = 0;
	namedWindow("Edge detection", 0);
	for(;;) {
		cap >> image;
		if(i == 0) {
			image2 = image.clone();
		}
		edge_detect(image, &image2);
		imshow("Edge detection", image2);

		if(waitKey(33) == 27) break;
		i++;
	}

	return 0;
}
