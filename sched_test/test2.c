#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <assert.h>
#include <signal.h>

long long a = 0;
long long b = 0;



void *thread1(void *param){
    long long i;
    for(int i=0; i<10000000000; i++){
        a++;
    }
}

void *thread2(void *param){
    long long i;
    for(int i=0; i<10000000000; i++){
        b++;
    }
}


void sighandler(int signo) {
    if (signo == SIGINT) {
    	double k = (a+0.0)/(b+0.0);
        printf("%lld, %lf\n", a-b, k);
   
    }
}

int main(){

    pthread_t t1;
    pthread_t t2;  

    if (pthread_create(&t1, NULL, thread1, NULL) < 0){
        printf("create t1 failed!\n");
        return -1;
    }

    if (pthread_create(&t2, NULL, thread2, NULL) < 0){
        printf("create t2 failed!\n");
        return -1;
    }
    signal(SIGINT, sighandler);
    long long int k;
    while(1){
        //printf("a=%lld, b=%lld\n", a, b);
        k+=1000000;
        k/=1234567;
        //sleep(1);
    }
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
  
    return 0;
}
