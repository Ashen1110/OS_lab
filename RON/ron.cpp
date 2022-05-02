//#define _GNU_SOURCE
#include <iostream>
#include <thread>
#include <atomic>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>
#define nCS_size 10
#define Num_core 4
//millisecond
using namespace std;
//sysconf(_SC_NPROCESSORS_ONLN);//std::thread::hardware_concurrency();//get_number_of_cpu_core
int TSP_ID_ARRAY[Num_core];
thread_local int TSP_ID;
atomic_bool InUse=ATOMIC_VAR_INIT(false);
atomic_int WaitArray[Num_core];
bool cmpxchg(atomic_bool *goal, bool x, bool y){
	if(*goal==x){
		*goal=y;
		return true;
	}else{
		// if(*goal)
		// 	printf("true\n");
		// else
		// 	printf("false\n");
		return false;
	}
}
void spin_init(int i){
	// unsigned int cpu,node;
	// TSP_ID = TSP_ID_ARRAY[getcpu(&cpu,&node)];
	// for(int i=0; i<Num_core; i++){
	// 	WaitArray[i]=0;
	// }
	TSP_ID = TSP_ID_ARRAY[i%Num_core];


}

void spin_lock(){
	WaitArray[TSP_ID] = 1;
	while(1){
		if(WaitArray[TSP_ID] == 0) return;
		if(cmpxchg(&InUse, false, true)){
			WaitArray[TSP_ID] = 0;
			return;
		}//printf("I am waiting in %d.\n",TSP_ID);
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
int array2[4];
void * dothread(void *arg){
	int num=*((int*)arg);
	spin_init(num);
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(TSP_ID, &cpuset);
	pthread_t th = pthread_self();
	pthread_setaffinity_np(th, sizeof(cpuset), &cpuset);
	unsigned int cpu,node;
	printf("%d %d %d\n",TSP_ID,num,sched_getcpu());
	// sched_setaffinity(0, sizeof(cpuset), &cpuset);
	double rnd;
	int i;
	struct timespec t1, t2;
	double micro_second;
	
	for(int count=0;count<1000;count++){
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
	
	int thread_num=atoi(argv[1]);
	user_num=atoi(argv[2]);
	user=new int[user_num]{0};
	int err=0;
	TSP_ID_ARRAY[0]=2;
	TSP_ID_ARRAY[1]=0;
	TSP_ID_ARRAY[2]=3;
	TSP_ID_ARRAY[3]=1;
	array2[0]=0;
	array2[1]=1;
	array2[2]=2;
	array2[3]=3;
	for(int i=0; i<Num_core; i++){
		WaitArray[i]=0;
	}
	pthread_t thread1[thread_num];
	for(int i=0;i<thread_num;i++){
		int j=i;
		err=pthread_create(&(thread1[i]), NULL, dothread, &array2[j%Num_core]);
		if(err!=0){
			printf("error in phread create\n");
		}
	}
	for(int i=0;i<thread_num;i++){
		pthread_join(thread1[i], NULL); 
	}
	for(int i=0;i<(user_num/10);i++){
		for(int j=0;j<10;j++){
			printf("%d\t",(*(user+i*10+j)));
		}printf("\n");
	}
	for(int i=0;i<Num_core;i++){
		printf("WaitArray[%d]: %d\n",i,WaitArray[i].load());
	}
	return 0;
}