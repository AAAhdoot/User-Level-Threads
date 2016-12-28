# User-Level-Threads

Emily Ng  en140
Ariel Ahdoot aa1046
CS416 HW0 Report


For our program, we created an array of pthread_t pointers of size 600. In pthread_create(), we set up a context for each thread and we allocate memory for the main thread, which is at index 0 of our array. For each subsequent thread, we allocate 4096 bytes for each thread's stack and add the thread to the next available index in the array. 

For pthread_yield(), we implemented a scheduling function that begins a loop at the index after the running thread and searches for the next READY thread.
The scheduler returns the index of the next READY thread, which pthread_yield() uses for swapcontext(). The scheduler returns -1 if it cannot find an available thread or if the running thread is the only available thread in the array.  

For pthread_join(), if the target thread has not been terminated and does not have another thread waiting on it, the running thread blocks and waits for the target thread. pthread_join() makes repeated calls to pthread_yield() while the target thread has not terminated. When the target thread terminates, the function sets the return value of the waiting thread.

For pthread_exit(), we set the status of the exiting thread to DEAD. We check to see if there is a thread waiting on the thread that is to be terminated. If there is a waiting thread, we unblock it and then call pthread_yield() to run another thread.  

Finally, our arrcleanup function frees all of the stack space used by the threads that were created, as well as the main thread that was created when the first pthread_create was called. This function is only called at the end of pthread_join() once it has been determined that the main is the only thread running and the rest of the threads are DEAD.



	
