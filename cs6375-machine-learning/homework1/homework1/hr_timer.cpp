/*! \file hr_timer.cpp
    \brief Contains the functionality for having a high resolution timer on Windows.
    \sa hr_timer.h
*/

#include <windows.h>
#include "hr_timer.h"

double CStopWatch::LIToSecs( LARGE_INTEGER& L ) 
{
  return ((double)L.QuadPart / (double)m_frequency.QuadPart);
}

CStopWatch::CStopWatch()
{
  m_timer.start.QuadPart = 0;
  m_timer.stop.QuadPart = 0;  
  QueryPerformanceFrequency( &m_frequency );
}

void CStopWatch::startTimer() 
{
  QueryPerformanceCounter( &m_timer.start );
}

void CStopWatch::stopTimer() 
{
  QueryPerformanceCounter( &m_timer.stop );
}

double CStopWatch::getElapsedTime() 
{
  LARGE_INTEGER time;
  time.QuadPart = m_timer.stop.QuadPart - m_timer.start.QuadPart;
  return LIToSecs( time );  
}