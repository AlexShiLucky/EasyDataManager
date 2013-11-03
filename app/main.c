/*
 * main.c
 *
 *  Created on: 2013-10-30
 *      Author: Armink
 */
#include "cache.h"
#include <stdio.h>
int main()
{
	Cache cache;
	uint16_t valueTemp[CACHE_LENGTH_MAX];
	initCache(&cache);
	valueTemp[0] = 0;
	valueTemp[1] = 1;
	valueTemp[2] = 2;
	valueTemp[3] = 3;
	addData(&cache,"�¶�",1,1,valueTemp,NULL);
	addData(&cache,"ѹ��",2,2,valueTemp,NULL);
	addData(&cache,"ʪ��",3,3,valueTemp,NULL);
	addData(&cache,"PM2.5",4,4,valueTemp,NULL);
	getValue(&cache,"�¶�",valueTemp);
	getValue(&cache,"ѹ��",valueTemp);
	getValue(&cache,"ʪ��",valueTemp);
	getValue(&cache,"PM2.5",valueTemp);
	removeData(&cache,"�¶�");
	removeData(&cache,"ѹ��");
	removeData(&cache,"ʪ��");
	removeData(&cache,"PM2.5");
	removeData(&cache,"PM2.5");
	getValue(&cache,"PM2.5",valueTemp);
	addData(&cache,"PM2.5",4,4,valueTemp,NULL);
	getValue(&cache,"PM2.5",valueTemp);
	valueTemp[0] = 3;
	valueTemp[1] = 2;
	valueTemp[2] = 1;
	valueTemp[3] = 0;
	putValue(&cache,"PM2.5",valueTemp);
	getValue(&cache,"PM2.5",valueTemp);
	removeData(&cache,"PM2.5");
	getValue(&cache,"PM2.5",valueTemp);
//	putValue(&cache,"PM2.5",valueTemp);
//	getValue(&cache,"�¶�",valueTemp);
//	getValue(&cache,"ѹ��",valueTemp);
//	getValue(&cache,"ʪ��",valueTemp);
//	getValue(&cache,"PM2.5",valueTemp);
//
//	removeData(&cache,"PM2.5");
//	getValue(&cache,"PM2.5",valueTemp);
//	removeData(&cache,"�¶�");
//	findData(&cache,"�¶�");
//	addData(&cache,"�¶�",1,1,valueTemp,NULL);
//	getValue(&cache,"�¶�",valueTemp);

	return 0;
}
