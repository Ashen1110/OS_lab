#include <stdio.h>
#include <stdlib.h>
#include <sched.h>

int main(int argc, char*argv[]){
    struct sched_param param; 
    int maxpri; 
    maxpri = sched_get_priority_max(SCHED_FIFO); 
    if(maxpri == -1) { 
          perror("sched_get_priority_max() failed"); 
          exit(1); 
    } 
    param.sched_priority = maxpri;
    int pid = atoi(argv[1]);
    if (sched_setscheduler(pid, SCHED_FIFO, &param) == -1){ 
          perror("sched_setscheduler() failed"); 
          exit(1); 
    } 
}

