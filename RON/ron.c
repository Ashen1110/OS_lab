//#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <atomic>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>
#define nCS_size 10
#define Num_core 4
//millisecond
//sysconf(_SC_NPROCESSORS_ONLN);//std::thread::hardware_concurrency();//get_number_of_cpu_core
int TSP_ID_ARRAY[Num_core];
thread_local int TSP_ID;
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


int* user;
int thread_num;
int user_num;
void dothread(){
	cpu_set_t cpuset;
	CPU_SERO(&cpuset);
	CPU_SET(TSP_ID, &cpuset);
	sched_setaffinity(0, sizeof(cpuset), &cpuset);
	double rnd;
	int i;
	struct timespec t1, t2;
	double micro_second;
	
	while(1){
		spin_lock() ; //LS
		for(i=0;i<user_num;i++){
			(*(user+i))+=1;
		}
		spin_unlock() ; //US
		clock_gettime (CLOCK_PROCESS_CPUTIME_ID, &t1) ; //nCS
		rnd=(double)(rand()%31)*0.01+0.85;
		do{
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
			micro_second = (t2.tv_sec - t1.tv_sec)*1000 + (t2.tv_nsec - t1.tv_nsec)/1000000;
		}while(micro_second > nCS_size*rnd);
	}
}
int main(int argc,char* argv[]){
	if(argc<3){
		printf("usage: <number of threads> <number of users>\n");
		return 0;
	}
	spin_init()
	int thread_num=atoi(argv[1]);
	int user_num=atoi(argv[2]);
	user=new int(user_num);
	int err=0;
	for(int i=0;i<thread_num;i++){
		pthread_t thread1;
		err=pthread_create(&thread1, NULL, dothread, NULL);
		if(err!=0){
			printf("error in phread create\n");
		}
	}

	//pthread_join(thread1, NULL);  
	//pthread_join(thread2, NULL);  
	return 0;
}
