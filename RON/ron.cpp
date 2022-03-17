#define _GNU_SOURCE
#include <iostream>
#include <thread>
#include <atomic>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>

using namespace std;

int TSP_ID_ARRAY[Num_core];
thread_local TSP_ID;
atomic_bool InUse=false;
atomic_int WaitArray[Num_core];

void spin_init(){
	TSP_ID = TSP_ID_ARRAY[getcpu()];
	for(int i=0; i<Num_core; i++){
		WaitArray[i]=0;
	}
}

void spin_lock(){
	WaitArray[TSP_ID] = 1;
	while(1){
		if(WaitArray[TSP_ID] == 0) return;
		if(cmpxchg(&InUse, false, true)){
			WaitArray[TSP_ID] = 0;
			return;
		}
	}
}

void spin_unlock(){
	for(int i=1; i<Num_core; i++){
		if(WaitArray[(i+TSP_ID)%Num_core] == 1){
			WaitArray[(i+TSP_ID)%Num_core] = 0;
			return;
		}
	}
	InUse=false;
}


