#ifndef H_MYPTHREAD
#define H_MYPTHREAD

#include <sys/ucontext.h>
#include <stdlib.h>
#include <stdio.h>

#define UNUSED 0
#define RUNNING 1
#define READY 2
#define BLOCKED 3
#define ZOMBIE 4
#define DEAD 5



// Types
typedef struct {
  int tid;
  ucontext_t uconn;
  int status;
  char* stack;
  int waiter;
  void* retval;

  //void (*destroy)( void * );

  // Define any fields you might need inside here.
} mypthread_t;

typedef struct {
  // Define any fields you might need inside here.
} mypthread_attr_t;


// Functions

void arrcleanup();

int nextSchedule();

int checkWait(mypthread_t* thread);

int exists(mypthread_t* thread);

int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr,
		     void *(*start_routine) (void *), void *arg);

void mypthread_exit(void *retval);

int mypthread_yield(void);

int mypthread_join(mypthread_t thread, void **retval);


/* Don't touch anything after this line.
 *
 * This is included just to make the mtsort.c program compatible
 * with both your ULT implementation as well as the system pthreads
 * implementation. The key idea is that mutexes are essentially
 * useless in a cooperative implementation, but are necessary in
 * a preemptive implementation.
 */

typedef int mypthread_mutex_t;
typedef int mypthread_mutexattr_t;

static inline int mypthread_mutex_init(mypthread_mutex_t *mutex,
				       const mypthread_mutexattr_t *attr) { return 0; }

static inline int mypthread_mutex_destroy(mypthread_mutex_t *mutex) { return 0; }

static inline int mypthread_mutex_lock(mypthread_mutex_t *mutex) { return 0; }

static inline int mypthread_mutex_trylock(mypthread_mutex_t *mutex) { return 0; }

static inline int mypthread_mutex_unlock(mypthread_mutex_t *mutex) { return 0; }

#endif
