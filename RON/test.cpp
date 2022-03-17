void thread ( ) :
	while( 1 ) :
	spin_lock ( ) ; //LS
	for (each element in SharedData) : //CS
		element = element + 1 ; //CS
	spin_unlock ( ) ; //US
	t = clock_gettime ( ) ; //nCS
	while(clock_gettime ( ) −t > nCS_size*rand
		( 0 . 85~1 . 15 ) ) ;
