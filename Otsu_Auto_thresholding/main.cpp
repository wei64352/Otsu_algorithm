#include<cv.h>
#include<highgui.h>

using namespace std;
using namespace cv;

#define color_level 256

void Count_gray_histogram(vector<int> &gray_his, Mat &image);
int Otsu_auto_thresholding(vector<int> &gray_his, Mat &image);
void Image_Histogram(vector<int> &vec, int &thre);

int main(int argc, char* argv[]){
	if (argc == 1) return 0;
	Mat image = imread(argv[1], 0);
	if (!image.empty()){
		vector<int> gray_his(color_level, 0);
		Count_gray_histogram(gray_his, image);
		int threshold = Otsu_auto_thresholding(gray_his, image);
		
		Mat binary_image = Mat(image.size(), CV_8UC1, Scalar(255));
		for (int i = 0; i < binary_image.rows; i++){
			for (int j = 0; j < binary_image.cols; j++){
				if (image.at<uchar>(i, j) < threshold)
					binary_image.at<uchar>(i, j) = 0; }}

		imwrite("Binary Image.jpg", binary_image);
		Image_Histogram(gray_his, threshold);
		gray_his.~vector();
	}else{
		printf("Error: load image fail.");
	}
}
// Count the gray pixel count of an image
void Count_gray_histogram(vector<int> &gray_his, Mat &image){
// (gray histogram, gray image)
	for (int i = 0; i < image.rows; i++){
		for (int j = 0; j < image.cols; j++){
			gray_his[image.at<uchar>(i, j)]++; }}
}
// Otsu's algorithm
int Otsu_auto_thresholding(vector<int> &gray_his, Mat &image){
// (gray histogram, gray image)
	int N = image.rows * image.cols;		// total pixel
	//float mu = 0, sigma = 0;		// mean & standard
	vector<double> prob(color_level, .0);		// total level probability
	for (int i = 0; i < color_level; i++){
		prob[i] = (double)gray_his[i] / N;
		//mu += prob[i] * (double)gray_his[i];
	}
	/*for (int i = 0; i < color_level; i++)
		sigma += prob[i] * pow((double)gray_his[i] - mu, 2);*/

	double omega1, omega2, mu1, mu2, F, maxF = 0;
	int threshold;
	for (int T = 1; T < color_level - 1; T++){
		omega1 = omega2 = mu1 = mu2 = .0;
		for (int i = 0; i <= T; i++){ omega1 += prob[i]; }
		omega2 = 1 - omega1;

		for (int i = 0; i <= T; i++){ mu1 += (i * prob[i]); }
		mu1 /= omega1;
		
		for (int i = T + 1; i < color_level; i++){ mu2 += (i * prob[i]); }
		mu2 /= omega2;
		
		F = omega1 * omega2 * pow((mu1 - mu2), 2);
		if (F > maxF){
			maxF = F;
			threshold = T;
		}
	}
	printf("MaxBC= %f, MaxLevel= %d", maxF, threshold);
	prob.~vector();

	return threshold;
}
void Image_Histogram(vector<int> &vec, int &thre){
	Mat his_image = Mat(512, color_level, CV_8UC3, Scalar(255, 255, 255));
	Vec3b red = (0, 0, 255);
	Vec3b black = (0, 0, 0);
	for (int i = his_image.rows - 1; i >= 0; i--){
		for (int j = 0; j < his_image.cols; j++){
			if (i >= his_image.rows - vec[j] / 5){
				his_image.at<Vec3b>(i, j) = black;
			}
			his_image.at<Vec3b>(i, thre) = red;
		}
	}
	imwrite("Histogram.jpg", his_image);
}