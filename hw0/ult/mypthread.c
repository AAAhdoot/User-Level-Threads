#include "mypthread.h"
#define ARRAYSIZE 600
int current=0;
int next=0;
int numthreads=1;
int numcreated=0;
int cleanup=0; //delete later
mypthread_t * arrthreads [600];
mypthread_t* tmain;

void arrcleanup(){
  int i;
  for(i=1;i<ARRAYSIZE;i++){
    if(arrthreads[i]!=NULL){
      free(arrthreads[i]->stack);
    }
  }
  free(arrthreads[0]);
  //free(arrthreads);
  next = 0;
  numcreated=0;
}

int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr, void *(*start_routine) (void *), void *arg){
  int i;
  if(!thread  || !start_routine){
    return -1;
  }
  if(next==0){
    //arrthreads = malloc(ARRAYSIZE*sizeof(mypthread_t*));
    //printf("LET'S E-GO\n");
    tmain = malloc(sizeof(mypthread_t));
    tmain->tid = 0;
    tmain->status = RUNNING;
    tmain->waiter = -1;
    tmain->retval = NULL;
    arrthreads[0] = tmain;
    if (getcontext(&tmain->uconn) == -1){
      printf("get context fail\n");
      exit(EXIT_FAILURE);
    }
    for (i = 1; i < ARRAYSIZE; i++) {
      arrthreads[i] = NULL;
    }
    next++;
  }

  if (getcontext(&thread->uconn) == -1){
    printf("get context fail\n");
    exit(EXIT_FAILURE);
  }
  //printf("current is %d and next is %d\n",current,next );
  arrthreads[next] = thread;
  thread->uconn.uc_stack.ss_sp = (char*)malloc(4096);
  thread->uconn.uc_stack.ss_size = 4096;
  thread->uconn.uc_link = NULL;
  thread->retval = NULL;
  //thread->uconn.uc_link = &arrthreads[current]->uconn;
  thread->stack = thread->uconn.uc_stack.ss_sp;

  thread->tid= next;
  thread->status = READY;
  thread->waiter = -1;

  makecontext(&thread->uconn, (void*)start_routine,1,arg);

  //printf("next is %d\n",next );

  next++;
  numthreads++;
  numcreated++;

  //printf("numcreated = %d\n",numcreated );

  return 0;
}

int exists(mypthread_t* thread){
  int i;
  //printf("exists\n");
  for(i=0;i<ARRAYSIZE;i++){
    if(arrthreads[i]!=NULL){
      if(arrthreads[i]->tid == thread->tid){
	return 0;
      }
    }
  }
  //printf("exists fail\n");
  return 1;
}

int checkWait(mypthread_t* thread){
  int i;
  //printf("checkWait\n");
  //printf("thread->tid is %d\n",thread->tid );
  for(i=0;i<ARRAYSIZE;i++){
    if(arrthreads[i]!=NULL){
      if(arrthreads[i]->status == BLOCKED && arrthreads[i]->waiter == thread->tid){
	return i;
      }
    }
  }
  //printf("checkwait fail\n");
  return -1;
}

int nextSchedule(){
  int x,poswaiter;
  x=current+1;
  //printf("nextSchedule\n");
  //print_fthread_table();
  while(x!=current){
    //printf("numthreads is %d and numcreated is %d\n",numthreads,numcreated);
    //printf("numthreads is %d and ARRAYSIZE is %d\n",numthreads,ARRAYSIZE );
    if(x == ARRAYSIZE){
      x=0;
      if(x==current){
	return -1;
      }
    }
    if(arrthreads[x]!=NULL){
      poswaiter = arrthreads[x]->waiter;
      if(arrthreads[x]!=NULL && arrthreads[x]->status == READY){
	//printf("FOUND ONE AT INDEX %d\n",x);
	return x;
      }
      else if(arrthreads[x]!=NULL && poswaiter>=0){
	if(arrthreads[x]->status == BLOCKED && arrthreads[poswaiter]->status == DEAD){
	  arrthreads[x]->status = READY;
	  arrthreads[x]->waiter= -1;
	  //printf("2ND FOUND ONE AT INDEX %d\n",x);
	  return x;
	}
      }
    }
    x++;
  }
  return -1;
}

int mypthread_yield(void){
  int yonder,now;
  now = current;
  yonder = nextSchedule();
  //printf("CURRENT IS %d\n",current );
  //print_thread_table();
  if(yonder == -1){
    //printf("FUCK\n");
    return -1;
  }
  arrthreads[yonder]->status = RUNNING;
  if(current!=yonder && arrthreads[current]->status == RUNNING){
    arrthreads[current]->status = READY;
  }
  current = yonder;
  if(!&arrthreads[now]->uconn || !&arrthreads[yonder]->uconn){
  }
  if (swapcontext(&arrthreads[now]->uconn, &arrthreads[yonder]->uconn) == -1){
    printf("swapcontext error");
    return -1;
  }
  return 0;
}

int mypthread_join(mypthread_t thread, void **retval){
  int yonder,x;
  //printf("JOIN US\n");
  //printf("THERE ARE CURRENTLY %d THREADS REMAINING\n",numthreads );
  //printf("next is %d\n",next);
  if( &thread== NULL || checkWait(&thread)>=0 || exists(&thread)==1 || current == thread.waiter || current == thread.tid || thread.status == DEAD){
    //printf("IS DEAD SON\n");
    //print_fthread_table();
    if(numthreads==1 && next!=0){
      arrcleanup();
    }
    return -1;
  }//first part of this if statement could cause problems
  arrthreads[current]->status = BLOCKED;
  arrthreads[current]->waiter = thread.tid;
  //if(current==0) printf("Current is %d and status is %d and waiting on %d\n",current,arrthreads[current]->status,arrthreads[current]->waiter );
  //printf("current is %d\n",current );
  //print_thread_table();
  yonder = current;
  while(thread.status!=DEAD){
    //printf("yielding\n");
    if (mypthread_yield()==-1){
      break;
    }
  }
  arrthreads[yonder]->status = READY;
  if (retval) {
    *retval = arrthreads[yonder]->retval;
  }
  //printf("END OF JOIN\n");
  // if(numthreads==1){
  // arrcleanup();
  // }
  if(numthreads==1 && next!=0){
    arrcleanup();
  }
  return 0;
}


void mypthread_exit(void *retval){
  int x;
  arrthreads[current]->retval = retval;
  arrthreads[current]->status = DEAD;
  numthreads--;
  //free(arrthreads[current]->stack);
  //arrthreads[current] = NULL;
  //printf(" current is %d and current->waiter is %d , PRE-CHECKWAIT\n",current,arrthreads[current]->waiter);
  if((x = checkWait(arrthreads[current]))>=0){
    arrthreads[x]->waiter = -1;
    arrthreads[x]->status = READY;
  }
  //numthreads--;
  //printf("CURRENT IS %d  and NUMBER OF THREADS REMAINING: %d\n",current,numthreads );
  //print_thread_table();
  mypthread_yield();
  return;
}
