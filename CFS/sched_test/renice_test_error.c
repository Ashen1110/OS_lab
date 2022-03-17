#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

long long a = 0;
int n=0;

void *thread1(void *param){
	n = gettid();
    long long i;
	for(int i=0; i<10000000000; i++){
        a++;
    }

}

void *thread2(void *param){
	printf("tid: %d\n", n);
	int ret; char buffer[6];
    sprintf(buffer, "%d", n);
	char *argv[]={"renice", "10", buffer,NULL};
	for(int i=0; i<10000000000; i++){
        ret = execvp("renice", argv);
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

	int k = getpid();
	printf("pid: %d\n", k);
/*
	char buffer[6];
	sprintf(buffer, "%d", k);
	int ret;
	char *argv[]={"renice", "5", buffer,NULL};
	ret = execvp("renice", argv);
*/
	while(1){
        printf("a=%lld\n", a);
    }
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	return 0;
}
