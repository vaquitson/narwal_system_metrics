#include <stdio.h>
#include "narwal_cpu.h"

int test_cpu_percentage(void){
  int rc = 0;
  NarwalCpu cpu = {0};
  NARWAL_CPU_TIME_T time = 0;

  rc = narwal_cpu_init(&cpu, NARWAL_CPU_GENERAL_CPU);
  if (rc < 0) {
    printf("fuck 1: %d\n", rc);
    return rc;
  }

  rc = narwal_cpu_print_info(&cpu);
  if (rc < 0) {
    printf("fuck 2: %d\n", rc);
    return rc;
  }

  time = narwal_cpu_idle_time(&cpu); 
  printf("idle time: %lld\n", time);

  time = narwal_cpu_busy_time(&cpu); 
  printf("busy time: %lld\n", time);


  return 0;
}


int main(void){
  test_cpu_percentage();
  return 0;
}
