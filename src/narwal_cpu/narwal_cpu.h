#ifndef NARWAL_CPU_H
#define NARWAL_CPU_H

#include <stdint.h>
#include <stddef.h>

#define NARWAL_CPU_TIME_T int64_t


typedef struct {
  int cpu_n;
  NARWAL_CPU_TIME_T info_arr[10];
} NarwalCpu;

int narwal_cpu_print_info(NarwalCpu *cpu_p);

NARWAL_CPU_TIME_T narwal_cpu_idle_time(NarwalCpu *cpu_p);

NARWAL_CPU_TIME_T narwal_cpu_busy_time(NarwalCpu *cpu_p);

int narwal_cpu_init(NarwalCpu *cpu_p, int cpu_n);

#define NARWAL_CPU_GENERAL_CPU -1

#endif
