/*
 * This file is part of the EasyDataManager Library.
 *
 * Copyright (C) 2013 by Armink <armink.ztl@gmail.com>
 *
 * Function: a logger in this library
 * Created on: 2013-11-10
 */

#include "log.h"

static pthread_mutex_t printLock;
static uint8_t isOpenPrint = FALSE;
static uint8_t isInitLog = FALSE;

static void printTime(void);
static void printThreadID(void);

/**
 * This function will initialize logger.
 *
 */
void initLogger(uint8_t isOpen) {
	isOpenPrint = isOpen;
	if (isOpen) {
		pthread_mutex_init(&printLock, NULL);
	}
	isInitLog = TRUE;
}

/**
 * This function is print debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 *
 */
void debug(const char *file, const long line, const char *format, ...) {
	va_list args;
	if (!isOpenPrint || !isInitLog) {
		return;
	}
	va_start(args, format);
	/* args point to the first variable parameter */
	/* lock the print ,make sure the print data full */
	pthread_mutex_lock(&printLock);
	printTime();
	printThreadID();
	printf("(%s:%ld) ", file, line);
	/* must use vprintf to print */
	vprintf(format, args);
	printf("\n");
	pthread_mutex_unlock(&printLock);
	va_end(args);
}

/**
 * This function destroy the logger.
 *
 */
void destroyLogger(void) {
	if (isOpenPrint) {
		pthread_mutex_destroy(&printLock);
	}
	isOpenPrint = FALSE;
	isInitLog = FALSE;
}

/**
 * This function is print thread info.
 *
 */
void printThreadID(void){
#if defined(WIN32) || defined(WIN64)
	printf("tid:%04ld ",GetCurrentThreadId());
#else
	printf("tid:%#x ",pthread_self());
#endif
}

/**
 * This function is print time info.
 *
 */
void printTime(void) {
#if defined(WIN32) || defined(WIN64)
	SYSTEMTIME currTime;
	GetLocalTime(&currTime);
	printf("%02d-%02d %02d:%02d:%02d.%03d ", currTime.wMonth, currTime.wDay,
			currTime.wHour, currTime.wMinute, currTime.wSecond,
			currTime.wMilliseconds);
#else
	time_t timep;
	struct tm *p;
	time(&timep);
	p=localtime(&timep);
	if(p==NULL) {
		return;
	}
	printf("%02d-%02d %02d:%02d:%02d.%03d ",p->tm_mon+1 ,p->tm_mday ,p->tm_hour ,p->tm_min,p->tm_sec);
#endif
}

