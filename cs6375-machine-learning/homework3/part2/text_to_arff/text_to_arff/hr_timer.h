/*! \file hr_timer.h
    \brief Contains functionality to get high-resolution timing on Windows.
    \note Adapted from code by David Bolton. here:
      http://cplus.about.com/od/howtodothingsi2/a/timing.htm
    \sa hr_timer.cpp
*/

#ifndef HR_TIMER_H
#define HR_TIMER_H

#include <windows.h>

/*!< The stop watch to contain the timings */
typedef struct {
    LARGE_INTEGER start;
    LARGE_INTEGER stop;
} stop_watch;

/**
 * \brief A microsecond resolution stop watch.
 */
class CStopWatch 
{
  public:
    /**
     * \brief Default constructor. 
     *
     * Sets up a timer with no time elapsed. 
     */
    CStopWatch();

    /**
     * \brief Logs the starting time of the stop watch.
     */
    void startTimer();

    /**
     * \brief Logs the stopping time of the stop watch.
     */
    void stopTimer();

    /**
     * \brief Returns the time elapsed.
     *
     * Returns the the time elapsed in seconds with a microsecond (10^-6) resolution.
     * \return double The number of seconds elapsed, accurate to microsecond resolution.
     * \note both startTimer() and stopTimer() must be called to return an actual
     * elapsed time.
     */
    double getElapsedTime();

  private:

    /**
     * \brief The timer for the stop watch. 
     */
    stop_watch m_timer;

    /**
     * \brief The frequency of the processor running the program.
     * \note Assumed to be a constant frequency that will not change during runtime.
     */
    LARGE_INTEGER m_frequency;

    /**
     * \brief Converts a large integer to seconds.
     * \return double The seconds representation of L.
     */
    double LIToSecs( LARGE_INTEGER& L);
};

#endif