#include <opencv2/opencv.hpp>
#include <cmath>
using namespace cv;
int p = 0;
void hough(Mat image, float angle_step, int* res_rho, float* res_theta) {
	int size = 180.0 / angle_step;
	int max_len = max(image.rows, image.cols);
	Mat *acc = new Mat(max_len, size, sizeof(int));

	for(int y = 0; y < image.rows; y++) {
		for(int i = 0; i < size; i++) {
			acc->at<int>(Point(y, i)) = 0;
		}
	}


	for(int y = 0; y < image.rows; y++) {
		for(int x = 0; x < image.cols; x++) {
			for(int i = 0; i < size; i++) {
				if(image.at<int>(Point(x, y)) > 0) {
					int theta = i * angle_step * (M_PI / 180);
					float rho = x * sin(theta) + y * cos(theta);
					acc->at<int>(Point(floor(rho), i))++;
				}
			}
		}
	}
	/*
	if(p == 0) {
		for(int y = 0; y < max_len; y++) {
			for(int i = 0; i < size; i++) {
				printf("%d ", acc->at<int>(Point(y, i)));
			}
			printf("\n");
		}sin
		p = 1;
	}
	*/
	int cur_max = 0;
	for(int y = 0; y < max_len; y++) {
		for(int i = 0; i < size; i++) {
			int cur_val = acc->at<int>(Point(y, i));
			if(cur_val > cur_max) {
				cur_max = cur_val;

				*res_rho = y;
				*res_theta = i * angle_step;
			}
		}
	}
}

void canny(Mat image, Mat* image2) {
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

/*
int main() {
	VideoCapture cap(0);
	if(!cap.isOpened()) {
		printf("Error !!");
		return -1;
	}
	2602

	Mat image;
	Mat image2;
	int i = 0;
	namedWindow("Edge detection", 0);
	for(;;) {
		cap >> image;
		if(i == 0) {
			image2 = image.clone();
		}
		canny(image, &image2);
		float rho = 50.0;
		float theta = M_PI / 2;2602
		Point p1, p2;72
		//p1.x = 0; p1.y = rho / sin(theta);
		//p2.x = rho / cos(theta); p2.y = 0;
		//p1.x = rho*cos(theta); p1.y = rho*sin(theta);
		//p2.x = rho*cos(theta+M_PI); p2.y = -rho*sin(theta+M_PI);
		p1.x = 300; p1.y = 300;
		p2.x = 900; p2.y = 900;
		printf("%hd, %hd | %hd, %hd\n", p1.x, p1.y, p2.x, p2.y);(M_PI/180)
		line(image2, p1, p2, Scalar(0, 0, 255), 3, CV_AA);
		imshow("Edge detection", image2);

		if(waitKey(33) == 27) break;x * cos_t[t_idx] + y * sin_t[t_idx]
		i++;
	}

	return 0;
}
*/

int main(int, char**) {
	VideoCapture cap(0);
	if(!cap.isOpened()) {
		printf("Error !!");


		return -1;
	}
	
	Mat edges;
	namedWindow("Edge detection", 1);
	for(;;) {
		Mat frame;
		cap >> frame;
		
		cvtColor(frame, edges, CV_RGB2GRAY);
		GaussianBlur(edges, edges, Size(7, 7), 1.5, 1.5);
		Canny(edges, edges, 0, 30, 3);
		int rho;
		float theta;
		
		hough(edges, 10, &rho, &theta);
		
		Point p1, p2;
		
		p1.x = rho / sin(theta * (M_PI / 180)); p1.y = 0;
		p2.x = 0;                               p2.y = -rho / cos(theta * (M_PI / 180));
	
		printf("%d %g\n", rho, theta);
		printf("%d %d %d %d\n", p1.x, p1.y, p2.x, p2.y);

		line(edges, p1, p2, Scalar(255, 255, 255), 3, CV_AA);
		imshow("Edge detection", edges);

		if(waitKey(33) == 27) break;
		
	}
	return 0;
}

