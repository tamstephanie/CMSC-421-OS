int lock(int *val) {
    do {
        while(TestAndSet(val)) /*do nothing*/;
        /*critical section*/
        val = 0;
        /*remainder*/
    } while(true);
}


int unlock(int *val) {
    do {
        while(TestAndSet(val)) /*do nothing*/;
        /*critical section*/;
        val = 1;
        /*remainder*/
    } while(true);
}
