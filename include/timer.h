/**
 * timer.h
 * Created on Aug, 23th 2023
 * Author: Tiago Barros
 * Based on "From C to C++ course - 2002"
*/
#ifndef __TIMER_H__
#define __TIMER_H__

#include <sys/time.h>
#include <stdio.h>

void timerInit(int valueMilliSec);

void timerDestroy();

void timerUpdateTimer(int valueMilliSec);

//missing header ??
int getTimeDiff();

int timerTimeOver();

void timerPrint();

#endif