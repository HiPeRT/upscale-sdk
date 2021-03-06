/* Copyright (C) 2005-2014 Free Software Foundation, Inc.
 C ontributed by Richard Henderson <r*th@redhat.com>.

 This file is part of the GNU OpenMP Library (libgomp).

 Libgomp is free software; you can redistribute it and/or modify it
 under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3, or (at your option)
 any later version.

 Libgomp is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 more details.

 Under Section 7 of GPL version 3, you are granted additional
 permissions described in the GCC Runtime Library Exception, version
 3.1, as published by the Free Software Foundation.

 You should have received a copy of the GNU General Public License and
 a copy of the GCC Runtime Library Exception along with this program;
 see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
 <http://www.gnu.org/licenses/>.  */

/* Copyright 2014 DEI - Universita' di Bologna
   author       DEI - Universita' di Bologna
                Alessandro Capotondi - alessandro.capotondi@unibo.it
   info         Libgomp support data structures declaration */

#ifndef __LIBGOMP_GLOBALS_H__
#define __LIBGOMP_GLOBALS_H__

#include "appsupport.h"
#include "config.h"

#ifndef NULL
#define NULL ((void *) 0x0) /* Standard C */
#endif

#define GOMP_WARN_NOT_SUPPORTED(what) printf("[libGOMP] " what " is not supported yet.\n");

/* Platform Information */
//#define prv_proc_num (get_proc_id())
//#define prv_num_procs (get_proc_num())

#include "libgomp_config.h"
#include "omp-lock.h"

/* task function type */
typedef void (*task_f)(int);

/* Threads/tasks support */
typedef struct global_infos_s
{
    /* NOTE If you change ANY dimension of these fields you have also to update macros below here */
    unsigned int thread_pool; //Busy = 1, available = 0. Big endian
    unsigned int idle_cores;
    omp_lock_t   lock;
} global_infos_t;

/* Threads/tasks support */
/* This struct encapsulate a generic work-share
 * (e.g. dynamic loop, sections, etc)
 */
typedef struct gomp_work_share_s
{
    /* This is the "business" lock for the WS */
    omp_lock_t lock;
    long end;
    long next;
    long chunk_size;
    long incr;

    /* These locks are to decouple enter phase (and exit phase)
     * from the "business" phase. If only one WS is defined,
     * they are the same lock (see gomp_new_work_share()) */
    omp_lock_t enter_lock;
    //FIXME should be removed
    int checkfirst;
    omp_lock_t exit_lock;
    unsigned int completed;

    /* This ptr point to next_ws in case you use NOWAIT constructs */
    struct gomp_work_share_s *next_ws;
    struct gomp_work_share_s *prev_ws;

    #ifndef __NO_OMP_PREALLOC__
    /*This field indicates the next team descriptor on the pool of preallocated descriptor, when the current
     is not used. **/
    struct gomp_work_share_s *next_free;
    #endif
} gomp_work_share_t;

#define WS_INITED     (0xdeadbeef)
#define WS_NOT_INITED (0x0)


#define TRUE 1;
#define FALSE 0;

enum gomp_schedule_type
{
  GFS_RUNTIME,
  GFS_STATIC,
  GFS_DYNAMIC,
  GFS_GUIDED,
  GFS_AUTO
};


#ifdef TASKING_ENABLED
#ifndef TASK_STACK_SIZE
#define TASK_STACK_SIZE 256       // number of words within stack
#endif
#define NUM_REGISTERS 26          // number of registers
#ifndef MAX_TASKS_FOR_THREAD
#define MAX_TASKS_FOR_THREAD 8 
#endif
#define MAX_THREADS 16
#define NUM_TASK_PARAMETERS 8 

struct gomp_queue;
typedef struct gomp_queue gomp_queue_t;

struct gomp_task;
typedef struct gomp_task gomp_task_t;

struct gomp_thread;
typedef struct gomp_thread gomp_thread_t;



enum gomp_task_kind
{
    GOMP_TASK_IMPLICIT,
    GOMP_TASK_TIED,
    GOMP_TASK_UNTIED
};

enum gomp_task_status
{
    GOMP_TASK_WAITING,
    GOMP_TASK_RUNNING,
    GOMP_TASK_SUSPENDED,
    GOMP_TASK_FINISHED,
    GOMP_TASK_IN_TASKWAIT,
    GOMP_TASK_CUTOFF
};

struct gomp_queue
{
    struct gomp_task *first_task;
    struct gomp_task *last_task;
    volatile unsigned int task_count;
};

// This structure describes a "task" to be run by a thread.

struct gomp_task
{
    struct gomp_queue *task_q;
    struct gomp_task *parent;
    struct gomp_task *next_task;
    struct gomp_task *prev_task;
    struct gomp_queue child_waiting_q;
    struct gomp_queue child_running_q;
    struct gomp_task *next_child;
    struct gomp_task *prev_child;
    enum gomp_task_kind kind;
    enum gomp_task_status status;
    unsigned int registers[NUM_REGISTERS];
    unsigned int stack[TASK_STACK_SIZE];

    struct gomp_thread *mem_space;
    //struct gomp_task_icv icv;
#ifndef NO_PREALLOCATED_DATA
    struct gomp_task *next_free_mem;
#endif
    void (*fn) (void *);
    unsigned int fn_data[NUM_TASK_PARAMETERS] __attribute__ ((aligned(_K1_DCACHE_LINE_SIZE)));
    omp_lock_t taskwait_barrier;
    //unsigned int _taskwait_barrier;
    //unsigned int _taskwait_barrier;
};

// This structure describes a thread.

struct gomp_thread
{
    struct gomp_task *implicit_task;
    struct gomp_task *curr_task;

    struct gomp_task *deferred_task;
    struct gomp_task *undeferred_task;

    unsigned int registers[NUM_REGISTERS];

    struct gomp_task *free_task_list;

    omp_lock_t mem_lock;
    //unsigned int _mem_lock;
};
#endif //TASKING_ENABLED


/* This structure contains all of the thread-local data associated with
 * a thread team.  This is the data that must be saved when a thread
 * encounters a nested PARALLEL construct.
 */
typedef struct gomp_team_s
{
  /****************** 1) Thread Info ****************************/
  /* This is the function that any thread
  * in the team must run upon launch. */
  void (*omp_task_f) (void *data);
  void *omp_args;

  /* Nesting level.  */
  unsigned level;
  struct gomp_team_s *parent;

  /******************** 2) Team Info ****************************/
  /* This is the number of threads in the current team.  */
  unsigned nthreads;

  /* This is the team descriptor/mask */
  unsigned int team;

  /* These are the local ids assigned to processors */
  unsigned int proc_ids[MAX_PARREG_THREADS];
  unsigned int thread_ids[DEFAULT_MAXPROC];

  /******************** 3) Work Info ****************************/
  /* This is the task that the thread is currently executing.  */
  /* Team (parreg) specific locks */
  omp_lock_t critical_lock; // in GCC's libgomp, they call this default_lock
  omp_lock_t atomic_lock;
  #ifdef __OMP_SINGLE_WS__
  gomp_work_share_t *work_share;
  #else
  gomp_work_share_t *work_share[DEFAULT_MAXPROC];
  #endif

  /******************** 4) (OMP 3.0) Tasks Info *************************/
#ifdef TASKING_ENABLED
    struct gomp_queue task_q;
    volatile unsigned int pending_threads;
    gomp_thread_t thread[MAX_THREADS];
    omp_lock_t task_lock;
    //unsigned int _task_lock;
    omp_lock_t sleep_lock;
    //unsigned int _sleep_lock;
#endif // TASKING_ENABLED

} gomp_team_t;


// Library memory
unsigned int job_2_slot_id[CLUSTER_COUNT*MAX_OFFLOADS_PER_CLUSTER];
char *gomp_mem_ptr[MAX_OFFLOADS_PER_CLUSTER];

INLINE unsigned int get_slot_id()
{
  JobType job_id;
  GetJobID ( &job_id );
  return job_2_slot_id[job_id];
}

/* Statically allocated global variables
 * (to avoid declaring a "real" global variable */

#define GLOBAL_INFOS_BASE       (gomp_mem_ptr[get_slot_id()] + MS_BARRIER_SIZE + 18*sizeof(int))
#define GLOBAL_INFOS_SIZE       (sizeof(global_infos_t))

#define ID_MAP_ADDR             (gomp_mem_ptr[get_slot_id()] + MS_BARRIER_SIZE)
#define ID_MAP_PTR              ((int *) (ID_MAP_ADDR))

#define ID_MAP_COUNT_ADDR       (gomp_mem_ptr[get_slot_id()] + MS_BARRIER_SIZE + 16*sizeof(int))
#define ID_MAP_COUNT_PTR        ((int *) (ID_MAP_COUNT_ADDR))

#define GLOBAL_THREAD_POOL      (*((unsigned int*) (GLOBAL_INFOS_BASE)))
#define GLOBAL_IDLE_CORES_ADDR  (GLOBAL_INFOS_BASE + SIZEOF_UNSIGNED)
#define GLOBAL_IDLE_CORES       (*((unsigned int *) GLOBAL_IDLE_CORES_ADDR))
#define GLOBAL_LOCK_ADDR        (GLOBAL_INFOS_BASE + SIZEOF_UNSIGNED + SIZEOF_PTR)
#define GLOBAL_LOCK             ((omp_lock_t*) GLOBAL_LOCK_ADDR)

//TODO set the proper lock procedure
#define GLOBAL_INFOS_WAIT()      gomp_hal_lock(GLOBAL_LOCK)
#define GLOBAL_INFOS_SIGNAL()    gomp_hal_unlock(GLOBAL_LOCK)

#define CURR_TEAM_ADDR          (GLOBAL_INFOS_BASE + GLOBAL_INFOS_SIZE)
#define CURR_TEAM_PTR(_id)      ((gomp_team_t **) (CURR_TEAM_ADDR + (_id *SIZEOF_PTR)))
#define CURR_TEAM(_id)          (*CURR_TEAM_PTR(_id))
#define CURR_TEAM_SIZE          (SIZEOF_PTR * DEFAULT_MAXPROC)

#define MAX_WS  12

//Workshare Pool Pre-allocated Pool
#define WSMEM_LOCK_ADDR                 (CURR_TEAM_ADDR + CURR_TEAM_SIZE)
#define WSMEM_LOCK                      ((omp_lock_t *) (WSMEM_LOCK_ADDR))
#define WSMEM_LOCK_SIZE                 (sizeof(omp_lock_t))
#define WSMEM_LOCK_WAIT()               gomp_hal_lock(WSMEM_LOCK)
#define WSMEM_LOCK_SIGNAL()             gomp_hal_unlock(WSMEM_LOCK)

#define WSMEM_FREE_MEM_ADDR             (WSMEM_LOCK_ADDR + WSMEM_LOCK_SIZE)
#define WSMEM_FREE_MEM                  (*((void **) (WSMEM_FREE_MEM_ADDR)))
#define WSMEM_FREE_MEM_SIZE             (SIZEOF_PTR)

#define WSMEM_FREE_LIST_ADDR            (WSMEM_FREE_MEM_ADDR + WSMEM_FREE_MEM_SIZE )
#define WSMEM_FREE_LIST                 (*((gomp_work_share_t  **) (WSMEM_FREE_LIST_ADDR)))
#define WSMEM_FREE_LIST_SIZE            (SIZEOF_PTR)

#define WSMEM_ADDR                      (WSMEM_FREE_LIST_ADDR + WSMEM_FREE_LIST_SIZE )
#define WSMEM                           (*((char *) (WSMEM_ADDR)))
#define WSMEM_SIZE                      (sizeof(gomp_work_share_t) * MAX_WS)
#define WSMEM_LIMIT                     (WSMEM_ADDR + WSMEM_SIZE)


#ifdef TASKING_ENABLED

#define ALLOC_TASKS_ADDR        (WSMEM_ADDR + WSMEM_SIZE)
#define ALLOC_TASKS             (*((volatile unsigned int *) ALLOC_TASKS_ADDR))
#define ALLOC_TASKS_SIZE        (SIZEOF_UNSIGNED)

#define CURR_TASK_ADDR          (ALLOC_TASKS_ADDR + ALLOC_TASKS_SIZE)
#define CURR_TASK_PTR(_id)      ((volatile gomp_task_t **) (CURR_TASK_ADDR + (_id * SIZEOF_PTR)))
#define CURR_TASK(_id)          (*CURR_TASK_PTR(_id))
#define CURR_TASK_SIZE          (SIZEOF_PTR * DEFAULT_MAXPROC)

#define CURR_THREAD_ADDR        (CURR_TASK_ADDR + CURR_TASK_SIZE)
#define CURR_THREAD_PTR(_id)    ((volatile gomp_thread_t **) (CURR_THREAD_ADDR + (_id * SIZEOF_PTR)))
#define CURR_THREAD(_id)        (*CURR_THREAD_PTR(_id))
#define CURR_THREAD_SIZE        (SIZEOF_PTR * MAX_PARREG_THREADS)

#define TASK_FREE_LIST_ADDR       (CURR_THREAD_ADDR + CURR_THREAD_SIZE)
#define TASK_FREE_LIST            (*((gomp_task_t  **) TASK_FREE_LIST_ADDR))
#define TASK_FREE_LIST_SIZE       (SIZEOF_PTR)

#define TASKMEM_LOCK_ADDR         (TASK_FREE_LIST_ADDR + TASK_FREE_LIST_SIZE)
#define TASKMEM_LOCK              ((omp_lock_t*) TASKMEM_LOCK_ADDR)
#define TASKMEM_LOCK_SIZE         (sizeof(omp_lock_t))
#define TASKMEM_LOCK_WAIT()       gomp_hal_lock(TASKMEM_LOCK)
#define TASKMEM_LOCK_SIGNAL()     gomp_hal_unlock(TASKMEM_LOCK)

#define CONTEXTMEM_LOCK_ADDR       (TASKMEM_LOCK_ADDR + TASKMEM_LOCK_SIZE)
#define CONTEXTMEM_LOCK            ((omp_lock_t*) CONTEXTMEM_LOCK_ADDR)
#define CONTEXTMEM_LOCK_SIZE       (sizeof(omp_lock_t))
#define CONTEXTMEM_LOCK_WAIT()     gomp_hal_lock(CONTEXTMEM_LOCK)
#define CONTEXTMEM_LOCK_SIGNAL()   gomp_hal_unlock(CONTEXTMEM_LOCK)

#define CONTEXT_FREE_LIST_ADDR     (CONTEXTMEM_LOCK_ADDR + CONTEXTMEM_LOCK_SIZE)
#define CONTEXT_FREE_LIST          (*((gomp_context_t  **) CONTEXT_FREE_LIST_ADDR))
#define CONTEXT_FREE_LIST_SIZE     (SIZEOF_PTR)

#else   /* TASKING_ENABLED */

#define ALLOC_TASKS_SIZE 0
#define CURR_TASK_SIZE 0
#define CURR_THREAD_SIZE 0
#define TASKMEM_LOCK_SIZE 0
#define TASK_FREE_LIST_SIZE 0
#define CONTEXTMEM_LOCK_SIZE 0
#define CONTEXT_FREE_LIST_SIZE 0

#endif  /* TASKING_ENABLED */


#ifdef __OMP_SINGLE_WS__
#define CURR_WS(_id)                    (((gomp_team_t *) CURR_TEAM(_id))->work_share)
#else
#define CURR_WS(_id)                    (((gomp_team_t *) CURR_TEAM(_id))->work_share[(_id)])
#endif



/* Platform Information */

INLINE int get_virtual_proc_id(int proc_id)
{
  return ID_MAP_PTR[proc_id];
}

INLINE int get_virtual_proc_num()
{
  return *ID_MAP_COUNT_PTR;
}

//#ifdef EE_SCHEDULER_GLOBAL
#if 0

INLINE int get_logical_thread_id()
{
  TaskType TaskID;
  GetTaskID(&TaskID);
  return ((unsigned int) TaskId) % 16;
}

#define prv_proc_num (get_logical_thread_id())
#define prv_num_procs (get_virtual_proc_num())

#else

#define prv_proc_num (get_virtual_proc_id(get_proc_id()))
#define prv_num_procs (get_virtual_proc_num())

#endif /* EE_SCHEDULER_GLOBAL */

#endif // __LIBGOMP_GLOBALS_H__
