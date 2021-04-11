//
// Created by Shiva Shankar patel psy on 2/18/21.
//

#ifndef PHOTOFX_TIME_H
#define PHOTOFX_TIME_H


#include <linux/time.h>
#include <time.h>
#include "Commons.h"

class TimeDiff{
    timespec startTime={.tv_sec=0,.tv_nsec=0};
    timespec endTime={.tv_sec=0,.tv_nsec=0};
    timespec timeDiff={.tv_sec=0,.tv_nsec=0};
    int CLOCKTYPE=CLOCK_REALTIME;

public:
    __attribute__((always_inline)) void start()
    {
        clock_gettime(CLOCKTYPE,&startTime);//check Thread Instead of Process
    }
    __attribute__((always_inline))void end()
    {
        clock_gettime(CLOCKTYPE,&endTime);
    }
    void getTimeDiff()
    {
        if(endTime.tv_nsec < startTime.tv_nsec)
        {
            int numSecs= (startTime.tv_nsec - endTime.tv_nsec) / 1000000000 + 1;
            startTime.tv_nsec-= 1000000000 * numSecs;
            startTime.tv_sec+=numSecs;
        }
        if((endTime.tv_nsec - startTime.tv_nsec) > 1000000000)
        {
            int numSecs= (endTime.tv_nsec - startTime.tv_nsec) / 1000000000;
            startTime.tv_nsec+= 1000000000 * numSecs;
            startTime.tv_sec-=numSecs;
        }

        timeDiff.tv_sec= endTime.tv_sec - startTime.tv_sec;
        timeDiff.tv_nsec= endTime.tv_nsec - startTime.tv_nsec;//should be positive check
        Loge("Time taken:","%.10lf secs ",timeDiff.tv_sec+timeDiff.tv_nsec/1000000000.0);
        endTime={.tv_sec=0,.tv_nsec=0};
        startTime={.tv_sec=0,.tv_nsec=0};
    }


};


#endif //PHOTOFX_TIME_H
/*wall time
 * below relative times
 * time_t for seconds
 * timeval for microseconds
 * #include <sys/time.h>\
 *struct timeval {
  time_t tv_sec; /* seconds
  suseconds_t tv_usec; /* microseconds
   };
   #include <time.h>
    struct timespec {
    time_t tv_sec; /* seconds
    long tv_nsec; /* nanoseconds
    };

  /*  #include <time.h>
    struct tm {
        int tm_sec; /* seconds
        int tm_min;  minutes
        int tm_hour;  hours */
//  int tm_mday; /* the day of the month */
//  int tm_mon; /* the month */
// int tm_year; /* the year */
// int tm_wday; /* the day of the week */
// int tm_yday; /* the day in the year */
// int tm_isdst; /* daylight savings time? */
// #ifdef _BSD_SOURCE
// long tm_gmtoff; /* time zone's offset from GMT */
// const char *tm_zone; /* time zone abbreviation */
// #endif
// };*/
/*
  *clock_t represent systems timer frequency HZ
  * clockid_t clocks[] = {
CLOCK_REALTIME,
CLOCK_MONOTONIC,
CLOCK_PROCESS_CPUTIME_ID,..nanosecond precision
CLOCK_THREAD_CPUTIME_ID,..nano
CLOCK_MONOTONIC_RAW,
 for above clocks clock_getres() for obtaining the resolution
 ret = gettimeofday (&tv, NULL);//timeval millisecod precision
 int clock_gettime (clockid_t clock_id ,struct timespec *ts);//timespec for nano precion clockid_t are above five enums//return 0 on succes -1 failure  with errorsEFAULTts is an invalid pointer.EINVALclock_id is an invalid time source on this system.
 */