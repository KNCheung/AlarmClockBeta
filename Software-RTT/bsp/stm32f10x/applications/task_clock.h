#ifndef _TASK_CLOCK_H_
#define _TASK_CLOCK_H_

void rt_thread_clock_entry(void* parameter);
void update_timeout(void* parameter);
void update_second(void* parameter);

#endif 
