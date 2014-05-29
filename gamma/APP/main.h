#ifndef __MAIN_H_
#define __MAIN_H_

//=====INCLUDE=====
#include "stm32f10x.h"
#include <board.h>
#include <pt.h>
#include <task.h>

//=====CONSTANT=====

//=====FUNCTIONS=====
void fnScheduler(void);
void fnInitializeBoard(void);
int main(void);

#endif
