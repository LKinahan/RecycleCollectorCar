#pragma once

#include <opencv2/opencv.hpp>
#include "CBase4618.h"
#include "server.h"
#include <pigpio.h>
#include <vector>
#include <string>

using namespace std;

#define SERVO1 13
#define SERVO2 21
#define LED 26
#define ON 1
#define OFF 0
#define HIGH 1
#define LOW 0
#define AIN1 4
#define BIN1 27
#define AIN2 3
#define BIN2 22
#define PWMA 2
#define PWMB 6
#define STBY 17
#define PWMS
#define PWMPERIODB 35
#define PWMPERIODA 35

const int GATE_DOWN = 1540;
const int GATE_UP = 1300;


class CSort :public CBase4618 {
private:
    cv::VideoCapture vid;
    cv::Mat rgb, hsv, mask, temp, hsvlarge, yellow, balls, check;
    std::vector<cv::Vec4i> hierarchy;
    std::vector<std::vector<cv::Point>> contours_yellow;
    std::vector<std::vector<cv::Point>> contours_balls;
    cv::Point roi_PointOffset;
    cv::Point mode_textpoint;
    cv::Point count_textpoint;
    std::string Count_text;
    std::string other_count_text;
    std::string display_mode_text;
    std::string server_count_str;

public:
    CSort();
    ~CSort();
    int detectImage();
    void draw();
    void update();
    int kbhit(void);
    void NothingDetected();
    void GateKeeper();
    void MoveRight();
    void MoveLeft();
    void MoveFWD();
    void MoveBWD();
    void SpeedUP();
    void SlowDOWN();
    void MoveSTOP();
    void collect();

    char input;
    char keyPress;
    int OnOffFlag;
    int collect_count;
    double move_time_1;
    double move_time_2;
    double delta_move_time;

    void delay(double milliseconds);

    static Server imserv;
    static Server cmdserv;
    static void imserverthread();
    static void cmdserverthread();
    void server();

    bool serverthreadexit;
};