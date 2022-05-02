#include <stdio.h>
#include <sched.h> // for sched_getcpu()
#include <pthread.h>

int main() {
    int cpu_id = 3; // set thread to cpu3
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);

    printf("pthread_setaffinity_np cpu%d\n", cpu_id);
    pthread_t th = pthread_self();
    printf("sched_getcpu = %d\n", sched_getcpu());
    pthread_setaffinity_np(th, sizeof(cpuset), &cpuset);

    printf("sched_getcpu = %d\n", sched_getcpu());

    return 0;
}
