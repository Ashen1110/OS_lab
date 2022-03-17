void thread ( ){
	struct timespec t1, t2;
	double micro_second;
	while( 1 ){
		spin_lock ( ) ; //LS
		for (each element in SharedData){//CS
			element = element + 1 ; //CS
		}
		spin_unlock ( ) ; //US
		clock_gettime (CLOCK_PROCESS_CPUTIME_ID, &t1) ; //nCS
		do{
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
			micro_second = (t2.tv_sec - t1.tv_sec)*1000 + (t2.tv_nsec - t1.tv_nsec)/1000000;
		}while(micro_second > nCS_size*rand( 0.85~1.15))
}
