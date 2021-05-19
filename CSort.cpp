////////////////////////////////////////////////////////////////
// ELEX 4699 System Design Project
// Recycling Robot
// Created April 27 2021 by Laurel Kinahan
////////////////////////////////////////////////////////////////
#include <iostream>
#include <cstdlib>
#include <string>
#include <cmath>
#include <vector>
#include "pigpio.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <thread>
#include "CBase4618.h"
#include "CSort.h"
#include "server.h"
#include <time.h>

using namespace std;
CSort::CSort()
{
    vid.open(0);
    if (gpioInitialise() < 0)
    {
        cout << "GPIO Init BAD" << endl;
    }
    else
    {
        cout << "GPIO Init GOOD" << endl;
    }

    serverthreadexit = false;
    server();

    roi_PointOffset.x = 150;
    roi_PointOffset.y = 100;
    mode_textpoint.x = 200;
    mode_textpoint.y = 20;
    count_textpoint.x = 5;
    count_textpoint.y = 20;
    keyPress = 'z';
    OnOffFlag = 1;
    collect_count = 1;
    //char keyPress = getchar();

    //SERVO
    gpioSetMode(SERVO1, PI_OUTPUT);
    gpioServo(SERVO1, GATE_DOWN);

    //LED
    gpioSetMode(LED, PI_OUTPUT);
    gpioWrite(LED, OFF);

    //DC MOTOR Input1
    gpioSetMode(AIN1, PI_OUTPUT);
    gpioSetMode(BIN1, PI_OUTPUT);
    //DC MOTOR Input2
    gpioSetMode(AIN2, PI_OUTPUT);
    gpioSetMode(BIN2, PI_OUTPUT);
    //DC MOTOR SpeedControl
    gpioSetMode(PWMA, PI_OUTPUT);
    gpioSetMode(PWMB, PI_OUTPUT);
    gpioSetPWMrange(PWMA, 100);
    gpioSetPWMrange(PWMB, 100);
    //DC MOTOR standby
    gpioSetMode(STBY, PI_OUTPUT);
    gpioWrite(STBY, 1); // Set STBY HIGH

}

CSort::~CSort()
{
    gpioWrite(LED, OFF);
    vid.release();
    cv::destroyWindow("RecycleSort");
    gpioTerminate();
    imserv.stop();
    cmdserv.stop();
    cv::waitKey(100);
}

Server CSort::cmdserv;
Server CSort::imserv;

void CSort::update()
{
    string mode_text;

    vid >> rgb;
    cv::resize(rgb, rgb, rgb.size() / 2);

    if (kbhit())
        keyPress = getchar();

    std::vector<std::string> cmds;

    cmdserv.get_cmd(cmds);

    if (cmds.size() > 0)
    {
        for (int i = 0; i < cmds.size(); i++)
        {
            if (cmds.at(i) == "S01 \n")
            {
                std::cout << "\nInitiate Auto Mode";
                keyPress = 'm';
                //OnOffFlag == -1;
                //std::cout << "cmds";
                std::string reply = "\nSorter Automatic Mode Initiated";
                cmdserv.send_string(reply);
            }
            else if (cmds.at(i) == "S00 \n")
            {
                std::cout << "\nInitiate Manual Mode";
                keyPress = 'm';
                //OnOffFlag == 1;
                std::string reply = "\nSorter Manual Mode Initiated";
                cmdserv.send_string(reply);
            }
            else if (cmds.at(i) == "FWD \n")
            {
                std::cout << "\nManual Forwards";
                //move car if button pressed/held
                move_time_1 = cv::getTickCount();
                delta_move_time = (move_time_1 - move_time_2) / cv::getTickFrequency();

                if (cmds.at(i) == "FWD \n" && delta_move_time > 0.1)
                {
                    move_time_2 = cv::getTickCount();
                    keyPress = 'I';
                    //MoveFWD();
                    std::string reply = "\nForward";
                    cmdserv.send_string(reply);
                }
                else
                {
                    keyPress = 'S';
                }
            }
            else if (cmds.at(i) == "BWD \n")
            {
                std::cout << "\nManual Reverse";
                keyPress = 'K';
                std::string reply = "\nRevese";
                cmdserv.send_string(reply);
            }
            else if (cmds.at(i) == "LEFT \n")
            {
                std::cout << "\nManual Left Turn";
                keyPress = 'J';
                std::string reply = "\n Left Turn";
                cmdserv.send_string(reply);
            }
            else if (cmds.at(i) == "RIGHT \n")
            {
                std::cout << "\nManual Right Turn";
                keyPress = 'L';
                std::string reply = "\n Right Turn";
                cmdserv.send_string(reply);
            }
            else if (cmds.at(i) == "STOP \n")
            {
                std::cout << "\nManual Stop";
                keyPress = 'S';
                std::string reply = "\n Stop";
                cmdserv.send_string(reply);
            }
            else if (cmds.at(i) == "COLLECT \n")
            {
                std::cout << "\nManual Collect Recycling";
                keyPress = 'W';
                std::string reply = "\n Recycling Collected ";
                cmdserv.send_string(reply);
            }
            else if (cmds.at(i) == "SPEEDup \n")
            {
                //std::cout << "\nManual Speed Increase";
                std::cout << "\nManual Collect Recycling Complete";
                keyPress = 'A';
                std::string reply = "\n Recycling Collect Completed ";
                //std::string reply = "\n Speed Increased";
                cmdserv.send_string(reply);
            }
            //        else if (cmds.at(i) == "SPEEDdown\n")
            //        {
            //            std::cout << "\n Manual Speed Decrease";
            //            keyPress = 'D';
            //            std::string reply = "\n Speed Decreased";
            //            cmdserv.send_string(reply);
            //        }
            else
            {
                std::string reply = "\nGot some other message";
                cmdserv.send_string(reply);
            }
        }
    }

    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    if (keyPress == 'm')
    {
        OnOffFlag *= -1;
        keyPress = 'z';
    }
    // If the System in ON
    if (OnOffFlag == -1)
    {
        mode_text = "Auto";
        gpioWrite(LED, ON);
        {
            //            cv::waitKey(1);
            //
            //            if (kbhit())
            //                keyPress = getchar();
            //
            //            vid >> rgb;
            //
            //            // If Nothing detected
            //            int state = detectImage();
            //            if (state == 2)
            //            {
            //                NothingDetected();
            //            }
            //            // IF Ball is detected in detectImage()
            //            else
            //            {
            //                vid.release();
            //                //if ball is to the right
            //                if (state == 1)
            //                {
            //                    //SortLeft();
            //                }
            //                //if ball is to the left
            //                else
            //                {
            //                    //SortRight();
            //                }
            //                vid.open(0);
            //            }
            draw();
        }
    }
    // If the system is OFF manual mode is activated
    if (OnOffFlag == 1)
    {
        mode_text = "Manual";
        gpioWrite(LED, OFF);
        //FWD
        if (keyPress == 'I')
        {
            MoveFWD();
            keyPress = 'z';
        }
        //BWD
        if (keyPress == 'K')
        {
            MoveBWD();
            keyPress = 'z';
        }
        //LEFT
        if (keyPress == 'J')
        {
            MoveLeft();
            keyPress = 'z';
        }
        //RIGHT
        if (keyPress == 'L')
        {
            MoveRight();
            keyPress = 'z';
        }
        //STOP
        if (keyPress == 'S')
        {
            MoveSTOP();
            keyPress = 'z';
        }
        //Collect
        if (keyPress == 'W')
        {
            //            collect_count *= -1;
            //            if (collect_count == -1)
            //            {
            collect();
            //            }
            //            else
            //            {
            //            GateKeeper();
            //            }
            keyPress = 'z';
        }
        //SPEEDup
        if (keyPress == 'A')
        {
            GateKeeper();
            //SpeedUP();
            keyPress = 'z';
        }
        //SPEEDdown
        if (keyPress == 'D')
        {
            //SlowDOWN();
            keyPress = 'z';
        }
        display_mode_text = "Mode: " + mode_text;
        draw();
    }
}

void CSort::MoveBWD()
{
    // Set PWMA and PWMB
    gpioPWM(PWMA, PWMPERIODA + 12);
    gpioPWM(PWMB, PWMPERIODB);
    // Set direction of motor 1
    gpioWrite(AIN1, LOW); // Set AIN1 LOW
    gpioWrite(AIN2, HIGH); // Set AIN2 HIGH
    // Set direction of motor 2
    gpioWrite(BIN1, LOW); // Set BIN1 LOW
    gpioWrite(BIN2, HIGH); // Set BIN2 HIGH
}

void CSort::MoveFWD()
{
    // Set PWMA and PWMB
    gpioPWM(PWMA, PWMPERIODA + 12);
    gpioPWM(PWMB, PWMPERIODB);
    // Set direction of motor 1
    gpioWrite(AIN1, HIGH); // Set AIN1 HIGH
    gpioWrite(AIN2, LOW); // Set AIN2 LOW
    // Set direction of motor 2
    gpioWrite(BIN1, HIGH); // Set BIN1 HIGH
    gpioWrite(BIN2, LOW); // Set BIN2 LOW
    // cv::waitKey(1250);
}

void CSort::MoveLeft()
{
    // Set direction of motor 1
    gpioWrite(AIN1, LOW); // Set AIN1 LOW
    gpioWrite(AIN2, HIGH); // Set AIN2 HIGH
    // Set direction of motor 2
    gpioWrite(BIN1, HIGH); // Set BIN1 HIGH
    gpioWrite(BIN2, LOW); // Set BIN2 LOW
    // Set PWMA and PWMB
    gpioPWM(PWMA, PWMPERIODA);
    gpioPWM(PWMB, PWMPERIODB);
}

void CSort::MoveRight()
{
    // Set direction of motor 1
    gpioWrite(AIN1, HIGH); // Set AIN1 HIGH
    gpioWrite(AIN2, LOW); // Set AIN2 LOW
    // Set direction of motor 2
    gpioWrite(BIN1, LOW); // Set BIN1 LOW
    gpioWrite(BIN2, HIGH); // Set BIN2 HIGH
    // Set PWMA and PWMB
    gpioPWM(PWMA, PWMPERIODA);
    gpioPWM(PWMB, PWMPERIODB);
}

void CSort::MoveSTOP()
{
    // Set direction of motor 1
    gpioWrite(AIN1, LOW); // Set AIN1 LOW
    gpioWrite(AIN2, LOW); // Set AIN2 LOW
    // Set direction of motor 2
    gpioWrite(BIN1, LOW); // Set BIN1 LOW
    gpioWrite(BIN2, LOW); // Set BIN2 LOW
    // Set PWMA and PWMB
    gpioPWM(PWMA, LOW);
    gpioPWM(PWMB, LOW);
}

//void CSort::SpeedUP()
//{
//    // Set direction of motor 1
//	gpioWrite(AIN1, HIGH); // Set AIN1 HIGH
//	gpioWrite(AIN2, LOW); // Set AIN2 LOW
//	// Set direction of motor 2
//	gpioWrite(BIN1, HIGH); // Set BIN1 HIGH
//	gpioWrite(BIN2, LOW); // Set BIN2 LOW
//    // Set PWMA and PWMB
//    gpioPWM(PWMA,PWMPERIODA*2);
//    gpioPWM(PWMB,PWMPERIODB*2);
//}
//
//void CSort::SlowDOWN()
//{
//    // Set direction of motor 1
//	gpioWrite(AIN1, HIGH); // Set AIN1 HIGH
//	gpioWrite(AIN2, LOW); // Set AIN2 LOW
//	// Set direction of motor 2
//	gpioWrite(BIN1, HIGH); // Set BIN1 HIGH
//	gpioWrite(BIN2, LOW); // Set BIN2 LOW
//    // Set PWMA and PWMB
//    gpioPWM(PWMA,PWMPERIODA/2);
//    gpioPWM(PWMB,PWMPERIODB/2);
//}

void CSort::collect()
{
    //    if (collect_count == -1)
    gpioServo(SERVO1, GATE_UP);
}

void CSort::NothingDetected()
{

}

void CSort::GateKeeper()
{
    gpioServo(SERVO1, GATE_DOWN);
}

void CSort::draw()
{
    cv::putText(rgb, display_mode_text, mode_textpoint, 1, 1, cv::Scalar(209, 206, 0), 1, 1, false);
    imserv.set_txim(rgb);
}

void CSort::delay(double milliseconds)
{
    double start_time = cv::getTickCount();
    double freq = cv::getTickFrequency();
    double elapsed_time;
    double current_time;

    do
    {
        current_time = cv::getTickCount();
        elapsed_time = (current_time - start_time) / freq * 1000;
    } while ((int)elapsed_time < milliseconds);
}

int CSort::detectImage()
{
    //vid.open(0);
    //if(vid.isOpened() == true)
    {
        //vid.set(cv::CAP_PROP_BRIGHTNESS, 200);
        //vid.set(cv::CAP_PROP_EXPOSURE, 0.25);
        {
            vid >> rgb;
            cv::resize(rgb, rgb, rgb.size() / 2);

            cv::cvtColor(rgb, hsvlarge, CV_BGR2HSV);
            cv::Rect roi = cv::Rect(200, 85, 40, 40);//Rect(X, Y, Width, Height)
            cv::Mat hsv = hsvlarge(roi);
            //cv::imshow("RGB2HSV", hsv);

            cv::inRange(hsv, cv::Scalar(18, 50, 50), cv::Scalar(30, 150, 255), yellow);
            cv::inRange(hsv, cv::Scalar(61, 0, 80), cv::Scalar(180, 150, 255), balls);
            cv::findContours(yellow, contours_yellow, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            cv::findContours(balls, contours_balls, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            // If GRN Ball Detected
            if ((contours_yellow.size() || contours_balls.size()) == 0)
            {
                return 2;
            }
            if ((contours_yellow.size() != 0))
            {
                for (unsigned int i = 0; i < contours_yellow.size(); i++)
                {
                    cv::Rect rect_yellow = cv::boundingRect(contours_yellow.at(i));
                    if (rect_yellow.area() > 1500)
                    {
                        cv::drawContours(rgb, contours_yellow, i, cv::Scalar(127, 127, 127), 1, 8, false, false, roi_PointOffset);
                        return 1;
                    }
                }
            }
            // If OTHER BAllz Detected
            if ((contours_balls.size() != 0))
            {
                for (unsigned int i = 0; i < contours_balls.size(); i++)
                {
                    cv::Rect rect_balls = cv::boundingRect(contours_balls.at(i));
                    if (rect_balls.area() > 1500)
                    {
                        cv::drawContours(rgb, contours_balls, i, cv::Scalar(127, 127, 127), 1, 8, false, false, roi_PointOffset);
                        return 0;
                    }
                }
            }
            // cv::erode(hsvsel,hsvsel,cv::Mat());
            // cv::imshow("erode",hsvsel);

            // cv::dilate(hsvsel,hsvsel, cv::Mat());
            // cv::imshow("Dilate",hsvsel)
        }
    }
    //vid.release();
}

void CSort::imserverthread()
{
    // Start server
    imserv.start(4618);
}

void CSort::cmdserverthread()
{
    // Start server
    cmdserv.start(4619);
}

void CSort::server()
{
    // Start server thread
    std::thread t2(&imserverthread);
    t2.detach();

    std::thread t3(&cmdserverthread);
    t3.detach();
}

/*////////////////////////////////////////////////////////////////////////////////////////////////
/
/This function was taken from https://cboard.cprogramming.com/c-programming/63166-kbhit-linux.html
/It is used to implement kbhit on linux
/
////////////////////////////////////////////////////////////////////////////////////////////////*/
int CSort::kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
