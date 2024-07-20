#include "log.h"

FILE *logpointer = NULL;
int level = 0;
char *msgtype[] = {
    "INFO",
    "MESSAGE",
    "WARNING",
    "ERROR",
    "FATAL",
    "NULL"
};

char *color[] = {
    "\e[0;34m",
    "\e[1;32m",
    "\e[1;33m",
    "\e[1;31m",
    "\e[5;41m",
    "\e[0;0m"
};

char **colors = color;
char **msgtypes = msgtype;
time_t t;
struct tm* tm;
void logFile(int level, const char *msg){
    
    t = time(NULL);
    tm = localtime(&t);
    char timestring[34];
    strftime(timestring, sizeof(timestring), "%d-%m-%y::%H:%M:%S", tm);
    fprintf(logpointer, "[%s]:[%s]:%s\n", timestring, msgtypes[level], msg);
}

void LOG_INIT(int priority){
    level = priority;
#ifdef LOGFILE
    printf("logfile enabled. with priority %d\n", level);
    logpointer = fopen(LOGFILE, "r");
    if(!logpointer) logpointer = fopen(LOGFILE, "w");
    else{
        fclose(logpointer);
        rename(LOGFILE, "shell-old.log");
        logpointer = fopen(LOGFILE, "w");

    }
    fprintf(logpointer, "\n\n\nLOGGING STARTED.\n=================================================\n");
    logFile(MESSAGE, "logging started.\n");
#endif //LOGFILE
}


void LOG(int priority, const char * message, enum loglevel level){
#ifdef LOGENABLE
    if(priority >= level) printf("%s%s %s\n\e[0;0m",colors[level], msgtypes[level], message);
#endif //LOGENABLE

#ifdef LOGFILE
    logFile(level, message);
#endif

}

void LOG_STOP(){
#ifdef LOGFILE
    logFile(MESSAGE, "Log stopped..");
    fclose(logpointer);
#endif //LOGFILE
}
