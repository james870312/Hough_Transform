#include <cstdio>
#include <ctime>
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#define OPENCV

double calcDeg(const std::vector<cv::Vec2f>& lines);
void drawLines(cv::Mat &input, const std::vector<cv::Vec2f> &lines);
void drawLinesP(cv::Mat &input, const std::vector<cv::Vec4i> &lines);

using namespace std;
using namespace cv;
using std::cout;
using std::endl;
using cv::createTrackbar;

struct TweakData {
    const static int rhoScale = 10;
    const static int thetaScale = 1000;
    int thres1 = 196;
    int thres2 = 159;
    int rho = 15;
    int theta = 63;
    int lineThres = 60;
};

int main(int argc, char* argv[]){

    Mat image ;
    TweakData tweakData;

    namedWindow("tweak");
    createTrackbar("thres1", "tweak", &tweakData.thres1, 1000);
    createTrackbar("thres2", "tweak", &tweakData.thres2, 1000);
    createTrackbar("rho", "tweak", &tweakData.rho, 1000);
    createTrackbar("theta", "tweak", &tweakData.theta, 1000);
    createTrackbar("lineThres", "tweak", &tweakData.lineThres, 1000);

    const char* input = argv[1];
    bool run = true;

    while (run){

        image = imread(input, IMREAD_COLOR);

        Mat gray;
        cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        Mat canny;
        Mat cannyBGR;
        vector<cv::Vec2f> lines;
        Canny(gray, canny, tweakData.thres1, tweakData.thres2);
        HoughLines(canny, lines, (double)tweakData.rho / tweakData.rhoScale, (double)tweakData.theta / tweakData.thetaScale, tweakData.lineThres);
        cvtColor(canny, cannyBGR, cv::COLOR_GRAY2BGR);
        drawLines(image, lines);
        Mat combine = cv::Mat::zeros(image.rows + 20, image.cols * 2, CV_8UC3);
        image.copyTo(combine(cv::Rect(cv::Point(), image.size())));
        cannyBGR.copyTo(combine(cv::Rect(cv::Point(canny.cols, 0), canny.size())));
        double degree = calcDeg(lines);
        putText(combine, std::to_string(degree), cv::Point(0, image.rows + 18), 0, 0.5, cv::Scalar(255, 255, 255));

        imshow("Canny + HoughLines", combine);

        char input = cv::waitKey(30);
        switch(input) {
            case 27:
                run = false;
                break;
        }
    }

    destroyAllWindows();

    return 0;
}

double calcDeg(const std::vector<cv::Vec2f>& lines)
{
    double ret = -999;
    if(lines.size() > 0)
    {
        ret = 0;
        std::vector<double> degs;
        for(const cv::Vec2f& line: lines)
        {
            degs.push_back(line[1] * 2.0);
        }
        double s = 0, c = 0;
        for(double& d: degs)
        {
            s += sin(d);
            c += cos(d);
        }
        s /= degs.size();
        c /= degs.size();

        ret = atan2(s, c);
        ret /= 2;
        ret = ret * 180.0 / CV_PI;
    }
    return ret;
}


void drawLines(cv::Mat &input, const std::vector<cv::Vec2f> &lines)
{
    for(size_t i=0; i<lines.size(); i++)
    {
        float r = lines[i][0];
        float theta = lines[i][1];
        if(theta<CV_PI/4.0 || theta>3*CV_PI/4.0)
        {
            cv::Point pt1(r/cos(theta),0);
            cv::Point pt2((r-input.rows*sin(theta))/cos(theta), input.rows);
            line(input, pt1, pt2, cv::Scalar(255,0,0), 1);
        }
        else
        {
            cv::Point pt1(0,r/sin(theta));
            cv::Point pt2(input.cols, (r-input.cols*cos(theta))/sin(theta));
            cv::line(input, pt1, pt2, cv::Scalar(255,0,0), 1);
        }
    }
}



void drawLinesP(cv::Mat &input, const std::vector<cv::Vec4i> &lines) {
    for(size_t i=0; i<lines.size(); i++){
        line(input, cv::Point(lines[i][0], lines[i][1]), cv::Point(lines[i][2], lines[i][3]), cv::Scalar(255,0,0), 1);
    }
}
