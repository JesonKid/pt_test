#define _CRT_SECURE_NO_WARNINGS

#include "pt-master/pt.h"
#include <time.h>
#include "pt-master/pt-sem.h"
#include "pt-master/lc.h"
#include "pt-master/lc-switch.h"
#include "stdio.h"
#include "stdint.h"

static struct pt pt1, pt2;
static struct pt_sem sem_1, sem_2;

PT_THREAD(pt1_thread(struct pt* pt));
PT_THREAD(pt2_thread(struct pt* pt));

int main()
{
	//初始化pt
	PT_INIT(&pt1);
	PT_INIT(&pt2);
	time_t nowTime;
	nowTime = time(NULL);
	struct tm* sysTime = localtime(&nowTime);
	while (1)
	{
		//消费者
		PT_SCHEDULE(pt2_thread(&pt1));

		nowTime = time(NULL);
		sysTime = localtime(&nowTime);

		static uint8_t sec_last = 0;
		if (sec_last != sysTime->tm_sec)
		{
			//tick out
			printf("hello,world\n");
			//生产者
			PT_SCHEDULE(pt1_thread(&pt2));
		}
		sec_last = sysTime->tm_sec;
	
	}
	return 0;
}

//生产者
PT_THREAD(pt1_thread(struct pt *pt))
{
	PT_BEGIN(pt);
	printf("pt1_thread running\n");
	PT_SEM_SIGNAL(pt,&sem_2);
	PT_YIELD(pt);
	PT_END(pt);
}

//消费者
PT_THREAD(pt2_thread(struct pt* pt))
{
	PT_BEGIN(pt);
	PT_SEM_WAIT(pt, &sem_2);
	printf("pt2_thread running\n");
	PT_YIELD(pt);
	PT_END(pt);
}
