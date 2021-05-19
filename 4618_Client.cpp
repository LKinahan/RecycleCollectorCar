////////////////////////////////////////////////////////////////
// ELEX 4618 Client Template project for BCIT
// Created Oct 5, 2016 by Craig Hennessey
// Last updated April 2021 by Laurel Kinahan
////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include <string>
#include <iostream>
#include <thread>

#include "Client.h"

Client imclient(4618, "192.168.137.99"); //image client port
Client cmdclient(4619, "192.168.137.99"); //cmd client port

int cmd = -1;

void print_menu()
{
	std::cout << "\n\n***********************************";
	std::cout << "\n  ELEX4699 System Design Project";
	std::cout << "\n          RC Recycle Car";
	std::cout << "\n        By: Laurel Kinahan";
	std::cout << "\n***********************************";
	std::cout << "\n(1) Auto Mode";
	std::cout << "\n(2) Manual Mode";
	std::cout << "\n(0) Exit";
	std::cout << "\n\n Manual Mode Controls: ";
	std::cout << "\nPress (I) for FWD";
	std::cout << "\nPress (K) for BWD";
	std::cout << "\nPress (J) for LEFT";
	std::cout << "\nPress (L) for RIGHT";
	std::cout << "\nPress (S) for STOP";
	std::cout << "\nPress (W) for COLLECT";
	std::cout << "\nPress (A) to increase speed";
	std::cout << "\nPress (D) to decrease speed";
	std::cout << "\nCMD> ";
}

void send_command(std::string cmd)
{
	std::string str;
	cmdclient.tx_str(cmd);

	do
	{
		cmdclient.rx_str(str);
		if (str.length() > 0)
		{
			std::cout << "\nClient Rx: " << str;
		}
	} while (str.length() == 0);
}

void get_image()
{
	cv::Mat im;
	int count = 0;
	do
	{
		imclient.tx_str("im");

		if (imclient.rx_im(im) == true)
		{
			if (im.empty() == false)
			{
				//count++;
				//std::cout << "\nImage received: " << count;
				cv::imshow("rx", im);
				cv::waitKey(100);
			}
		}
	} while (cmd != 0);
}


int main(int argc, char* argv[])
{
	std::thread t(&get_image);
	t.detach();

	do
	{
		print_menu();
		std::cin >> cmd;

		switch (cmd)
		{
			case 1: send_command("S01 \n"); break; //auto 
			case 3: send_command("S00 \n"); break; // manual
			case 8:
			case 'i':
			case 'I': send_command("FWD \n"); break;
			case 2:
			case 'k':
			case 'K': send_command("BWD \n"); break;
			case 4:
			case 'j':
			case 'J': send_command("LEFT \n"); break;
			case 6:
			case 'l':
			case 'L': send_command("RIGHT \n"); break;
			case 5:
			case 's':
			case 'S': send_command("STOP \n"); break;
			case 9:
			case 'w':
			case 'W': send_command("COLLECT \n"); break;
			case 7:
			case 'a':
			case 'A': send_command("SPEEDup \n"); break;
			
			case 'd':
			case 'D': send_command("SPEEDdown \n"); break;
		}
	} while (cmd != 0);
}
