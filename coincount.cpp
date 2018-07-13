/*
  Abdul Mannan
  Coin Counter
  CSE4310 Intro to Computer Vision
  July 12, 2018

*/




// include necessary dependencies
#include <iostream>
#include <string>
#include <vector>
#include "opencv2/opencv.hpp"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


using namespace cv;
using namespace std;

float getCirTh(vector<Point> center);
void processCoin(float ratio);

int quarters =0, nickels=0, dimes =0, pennies= 0;
float total = 0.0;

struct sort_pred{
  bool operator()(const Vec3f &left, const Vec3f &right){
    return left[2]<right[2];
  }
};

int main(int argc, char **argv)
{
    // open the input image

  Mat imageIn = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
  Mat gray;
  cvtColor(imageIn,gray, CV_BGR2GRAY);

  //imshow("Blurred", gray);

  GaussianBlur(gray, gray, Size(1,1), 0, 0);
	/* / check for file error
	if(!imageIn.data)
	{
		std::cout << "Error while opening file " << inputFileName << std::endl;
		return 0;
	}
  */

  Mat imgTh = Mat(gray.rows, gray.cols, CV_8UC1);
  double otsuThVal =  threshold(
    gray, imgTh, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU
  ) ;

  //imshow("otsu", imgTh);
  double highTh = otsuThVal, lowTh = otsuThVal * 0.5;

  Mat edges ;
  Canny(gray, edges, lowTh, highTh, 3);

  // Morph, Dilation
  int dilType = MORPH_RECT, dilSize = 1;
  Mat element = getStructuringElement(
    dilType,
    Size(2* dilSize +1, 2*dilSize +1),
    Point(dilSize, dilSize)
  );
  morphologyEx(edges, edges, MORPH_DILATE, element);

  //Canny
  Canny(edges, edges, 50, 250, 3);
  //namedWindow("Canny", 0);
  //imshow("Canny", edges);

  // removing false circles
  vector< vector<Point> > contours, contoursFill;
  vector<Vec4i> hierarchy;
  Mat contImg1 = Mat::ones(edges.rows, edges.cols, edges.type());
  Mat contImg2 = Mat::ones(edges.rows, edges.cols, edges.type());

  //find contours in edges
  findContours(edges.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);

  float circTh;

  for(int j = 0; j<contours.size(); j++){
    if((hierarchy[j][2]) >= 0 && (hierarchy[j][3]) < 0){
      circTh = getCirTh(contours[j]);
      if((circTh > 10) && (circTh <= 30)){
        contoursFill.push_back(contours[j]);
      }
    }
  }

  //

  for(int j = 0; j < contoursFill.size(); j++){
    drawContours(contImg2, contoursFill, j, CV_RGB(255, 255, 255), 1,8);
  }

  // this this point it clears out most of the background
  // and edges of coins are almost clear with some noise

  // HoughCircles implementation in following lines
  vector<Vec3f> circles;
  HoughCircles(contImg2, circles, CV_HOUGH_GRADIENT, 2, gray.rows/24 , 200, 30,15,45);


  //sorting the circles in descending order
  std::sort(circles.rbegin(), circles.rend(), sort_pred());

  float maxRadius = circles[0][2];
  float ratio;

  for(size_t i = 0; i < circles.size(); i++){
    Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
    float radius = circles[i][2];

    //first is center circle, second is outline
    //circle(imageIn, center, 3, Scalar(0, 255, 0),-1,8,0);
    circle(imageIn, center, radius, Scalar(0,0,255),2,8,0);
    // cout << "Radius: " << radius << endl;
    ratio = ((radius*radius)/(maxRadius*maxRadius));
    processCoin(ratio);
  }
  // display the input image
  namedWindow("imageIn", WINDOW_AUTOSIZE);
  //setMouseCallback("imageIn", clickCallback, &imageIn);
	imshow("imageIn", imageIn);

	//setMouseCallback("imageIn", clickCallback, &imageIn);
  cout << "Total: " << total << endl;
  cout << "Quarters: "<< quarters << endl;
  cout << "Nickels: " << nickels << endl;
  cout << "Dimes: " << dimes << endl;
  cout << "Pennies" << pennies << endl;

	waitKey(0);

  return 0;
}

float getCirTh(vector<Point> center){
  float perimeter, area;
  perimeter = arcLength(Mat(center), true);
  area = contourArea(Mat(center));
  return ((perimeter * perimeter)/area);
}

void processCoin(float ratio){
  if(ratio >= 0.90){
    //probably a quarter
    total += 0.25;
    quarters +=1;
  } else if((ratio >= 0.72) && (ratio<0.90)){
    //probably a nickel
    total += 0.05;
    nickels +=1;
  } else if((ratio>=0.60)&&(ratio<0.72)){
    //probably a penny cause its bigger than a dime
    total += 0.01;
    pennies +=1;
  } else if((ratio>=0.20) && (ratio <0.60)){
    //a dime
    total += 0.10;
    dimes += 1;
  }
}
