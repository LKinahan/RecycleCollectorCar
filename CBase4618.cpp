////////////////////////////////////////////////////////////////
// CBase.cpp for Lab 4 - ELEX 4618
// Member function definitions for class CBase that
//
// Created Feb 09, 2021 by Laurel Kinahan
////////////////////////////////////////////////////////////////

#include "CBase4618.h"

using namespace std;

////////////////////////////////////////////////////////////////
// Class Base Constructor
////////////////////////////////////////////////////////////////
CBase4618::CBase4618(void)
{
}

////////////////////////////////////////////////////////////////
// Class Base Destructor
////////////////////////////////////////////////////////////////
CBase4618::~CBase4618(void)
{
}

////////////////////////////////////////////////////////////////
// Update
////////////////////////////////////////////////////////////////
void CBase4618::update()
{
}

////////////////////////////////////////////////////////////////
// Draw
////////////////////////////////////////////////////////////////
void CBase4618::draw()
{
}

////////////////////////////////////////////////////////////////
// Run
////////////////////////////////////////////////////////////////
void CBase4618::run()
{
	do
	{
		update();
	} while (cv::waitKey(1) != 'q');
}