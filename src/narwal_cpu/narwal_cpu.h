#ifndef NARWAL_CPU_H
#define NARWAL_CPU_H

#include <stdint.h>
#include <stddef.h>

#define NARWAL_CPU_TIME_T int64_t

#define NARWAL_CPU_GENERAL_CPU -1

typedef struct {
  int cpu_n;
  NARWAL_CPU_TIME_T info_arr[10];
} NarwalCpu;

int narwal_cpu_print_info(NarwalCpu *cpu_p);

NARWAL_CPU_TIME_T narwal_cpu_idle_time(NarwalCpu *cpu_p);

NARWAL_CPU_TIME_T narwal_cpu_busy_time(NarwalCpu *cpu_p);

int narwal_cpu_init(NarwalCpu *cpu_p, int cpu_n);


/**
* @brief Return the stat file descriptor.
*
* This function should only be used in tests
*
* Return the stat file descriptor used for almost 
* all functions in the narwal cpu interfaze
*
* @return Return a pointer to the 
*/
#ifdef NARWAL_TEST
int *priv_narwal_cpu_get_fd();
#endif


#endif
