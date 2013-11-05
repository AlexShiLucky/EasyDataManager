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
	uint16_t cacheLength,valueTemp[CACHE_LENGTH_MAX];
	uint32_t cacheSize;
	initCache(&cache,"cache");
	valueTemp[0] = 0;
	valueTemp[1] = 1;
	valueTemp[2] = 2;
	valueTemp[3] = 3;
	cache.add(&cache,"�¶�",1,1,valueTemp,NULL);
	cache.add(&cache,"ѹ��",2,2,valueTemp,NULL);
	cache.add(&cache,"ʪ��",3,3,valueTemp,NULL);
	cache.add(&cache,"PM2.5",4,4,valueTemp,NULL);
	cache.getSize(&cache,&cacheLength,&cacheSize);
	cache.get(&cache,"�¶�",valueTemp);
	cache.get(&cache,"ѹ��",valueTemp);
	cache.get(&cache,"ʪ��",valueTemp);
	cache.get(&cache,"PM2.5",valueTemp);
	cache.remove(&cache,"�¶�");
	cache.remove(&cache,"ѹ��");
	cache.remove(&cache,"ʪ��");
	cache.remove(&cache,"PM2.5");
	cache.remove(&cache,"PM2.5");
	cache.get(&cache,"PM2.5",valueTemp);
	cache.add(&cache,"PM2.5",4,4,valueTemp,NULL);
	cache.get(&cache,"PM2.5",valueTemp);
	cache.getSize(&cache,&cacheLength,&cacheSize);
	valueTemp[0] = 3;
	valueTemp[1] = 2;
	valueTemp[2] = 1;
	valueTemp[3] = 0;
	cache.put(&cache,"PM2.5",valueTemp);
	cache.get(&cache,"PM2.5",valueTemp);
	cache.remove(&cache,"PM2.5");
	cache.get(&cache,"PM2.5",valueTemp);
	cache.getSize(&cache,&cacheLength,&cacheSize);
//	cache.put(&cache,"PM2.5",valueTemp);
//	cache.get(&cache,"�¶�",valueTemp);
//	cache.get(&cache,"ѹ��",valueTemp);
//	cache.get(&cache,"ʪ��",valueTemp);
//	cache.get(&cache,"PM2.5",valueTemp);
//
//	cache.remove(&cache,"PM2.5");
//	cache.get(&cache,"PM2.5",valueTemp);
//	cache.remove(&cache,"�¶�");
//	cache.findData(&cache,"�¶�");
//	cache.add(&cache,"�¶�",1,1,valueTemp,NULL);
//	cache.get(&cache,"�¶�",valueTemp);

	return 0;
}
