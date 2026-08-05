#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "narwal_cpu.h"

#define EXE_TEST(n)                                                   \
    do {                                                              \
        rc = test_##n();                                              \
        if (rc != 0) {                                                \
            printf("test_" #n ": Failed, exit code %d\n", rc);        \
        } else {                                                      \
            printf("test_" #n ": Pass\n");                            \
        }                                                             \
    } while (0) \


int test_1(void)
{
  int stat_fd;

  NarwalCpuTime_t busy_time_arr[] = {
    103026,  // cpu
    12253,   // cpu0
    12973,   // cpu1
    16033,   // cpu2
    11577,   // cpu3
    12347    // cpu4
  };

  NarwalCpuTime_t idle_time_arr[] = {
    3115217, // cpu
    389896,  // cpu0
    389192,  // cpu1
    386278,  // cpu2
    390652,  // cpu3
    389891   // cpu4
  };

  NarwalCpu cpu_arr[6];
  NarwalCpu *cur_cpu_p = cpu_arr;
  NarwalCpuTime_t idle_time;
  NarwalCpuTime_t busy_time;

  stat_fd = open("/proc/stat", O_RDONLY);
  if (stat_fd < 0)
    return -1;
  close(stat_fd);

  stat_fd = open("tests/test_files/stat", O_RDONLY);
  if (stat_fd < 0){
    printf("errno: %s\n", strerror(errno));
    return -2;
  }

  *priv_narwal_cpu_get_fd() = stat_fd;

  for (int i = -1; i < 5; i++){
    cur_cpu_p = &cpu_arr[i+1];
    narwal_cpu_init(cur_cpu_p, i);

    idle_time = narwal_cpu_idle_time(cur_cpu_p);
    busy_time =narwal_cpu_busy_time(cur_cpu_p);

    if (busy_time != busy_time_arr[i+1]){
      printf("Expect: %lld But get %lld",busy_time_arr[i+1], busy_time);
      return -3;
    }

    if (idle_time != idle_time_arr[i+1]){
      printf("Expect: %lld But get %lld",idle_time_arr[i+1], idle_time);
      return -4;
    }
  }

  return 0;

}

int test_cpu_percentage(void){
  int stat_fd;
  int rc = 0;
  NarwalCpu cpu = {0};
  NarwalCpuTime_t time = 0;


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


int main(void)
{
  int rc = 0;

  EXE_TEST(1);
  return 0;
}
