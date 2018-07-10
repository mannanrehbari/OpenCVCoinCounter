/*
  Abdul Mannan
  cse4310 US Coin Counter
  July 9, 2018

*/
#include <iostream>
#include <cstdio>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

Mat imageIn;

int main(int argc, char** argv){

    //check for image path
    // argc must be 2:program and image path
    if(argc != 2){
      printf("USAGE: %s <image path>\n", argv[0]);
      return 0;
    } else {
      imageIn = imread(argv[1], CV_LOAD_IMAGE_COLOR);
      if(!imageIn.data){
        cout << "Error loading image" << endl;
        return 0;
      } else {
        imshow("Coins", imageIn);
        waitKey(0);
      }
    }

    return 0;



}
