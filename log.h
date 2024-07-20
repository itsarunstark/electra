#ifndef _H_LOG
#define _H_LOG
#include<stdio.h>
#include<time.h>

enum loglevel{
    INFO, MESSAGE, WARNING, ERROR, FATAL, RESET
};

extern char **colors, **msgtypes;
extern FILE *logpointer;
#define LOGENABLE
#define LOGFILE "shell3.runtime.log"


void LOG_INIT(int pr);
void LOG_STOP();
void LOG(int level, const char *msg, enum loglevel);

#endif