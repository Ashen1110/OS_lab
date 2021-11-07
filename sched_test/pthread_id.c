#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef NUM_THREADS
#define NUM_THREADS 4
#endif

void *printHello(void *threadid) {
    /*
    pthread_id_np_t   tid;
	pthread_t         self;
	self = pthread_self();
	pthread_getunique_np(&self, &tid);
    printf("Hello There! thread %ld, pthread ID - %lu\n", tid, self);
    */
    
    //pid_t tid = gettid();
     printf("before calling pthread_create getpid: %d getpthread_self: %lu; gettid: %d\n",getpid(), pthread_self(), gettid());
    pthread_exit(NULL);
}


int main(int argc, char const *argv[]) {
    pthread_t threads[NUM_THREADS];
    int rc;
    long t;

    for (t = 0; t < NUM_THREADS; t++) {
        rc = pthread_create(&threads[t], NULL, printHello, (void *)t);
        if (rc) {
            printf("ERORR; return code from pthread_create() is %d\n", rc);
            exit(EXIT_FAILURE);
        }
    }
    pthread_exit(NULL);
}
