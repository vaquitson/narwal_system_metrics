#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "narwal_cpu.h"

#define FLOAT_EPSILON 0.001f

#define EXE_TEST(n)                                                   \
    do {                                                              \
        rc = test_##n();                                              \
        if (rc != 0) {                                                \
            printf("test_" #n ": Failed, exit code %d\n", rc);        \
        } else {                                                      \
            printf("test_" #n ": Pass\n");                            \
        }                                                             \
    } while (0) \


// Test the general usage of the cpu Interface
int test_1(void) {
  int stat_fd;

  NarwalCpuTime_t busy_time_arr[] = {
    57851,  // cpu
    6204,   // cpu0
    7127,   // cpu1
    9976,   // cpu2
    7600,   // cpu3
    6382,   // cpu4
  };

  NarwalCpuTime_t idle_time_arr[] = {
    2703066, // cpu
    338611,  // cpu0
    338121,  // cpu1
    335167,  // cpu2
    337432,  // cpu3
    338666,  // cpu4
  };  

  NarwalCpu cpu_arr[6];
  NarwalCpu *cur_cpu_p = cpu_arr;
  NarwalCpuTime_t idle_time;
  NarwalCpuTime_t busy_time;

  stat_fd = open("/proc/stat", O_RDONLY);
  if (stat_fd < 0)
    return -1;
  close(stat_fd);

  stat_fd = open("tests/test_files/stat_1", O_RDONLY);
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
      printf("Expect: %lld But get %lld",(long long)busy_time_arr[i+1], (long long)busy_time);
      return -3;
    }

    if (idle_time != idle_time_arr[i+1]){
      printf("Expect: %lld But get %lld", (long long)idle_time_arr[i+1], (long long)idle_time);
      return -4;
    }
  }
  return 0;
}

int test_2(void) {
  int stat_fd = 0;
  int rc = 0;
  NarwalCpu cpu = {0}; 

  NarwalCpuTime_t idle_time_1;
  NarwalCpuTime_t busy_time_1;

  NarwalCpuTime_t idle_time_2;
  NarwalCpuTime_t busy_time_2;

  float percentage = 0;

  rc = narwal_cpu_init(&cpu, NARWAL_CPU_GENERAL_CPU);
  if (rc < 0)
    return rc;

  stat_fd = open("tests/test_files/stat_1", O_RDONLY);
  if (stat_fd < 0){
    return -1;
  }

  *priv_narwal_cpu_get_fd() = stat_fd;
  
  idle_time_1 = narwal_cpu_idle_time(&cpu);
  busy_time_1 = narwal_cpu_busy_time(&cpu);

  close(stat_fd);

  stat_fd = open("tests/test_files/stat_2", O_RDONLY);
  if (stat_fd < 0){
    return -2;
  }

  *priv_narwal_cpu_get_fd() = stat_fd;
     
  // 100 ms
  usleep(100000); 

  idle_time_2 = narwal_cpu_idle_time(&cpu);
  busy_time_2 = narwal_cpu_busy_time(&cpu);

  NarwalCpuTime_t busy_delta = busy_time_2 - busy_time_1;
  NarwalCpuTime_t idle_delta = idle_time_2 - idle_time_1;

  percentage = 100.0 * busy_delta / (busy_delta + idle_delta);  

  if (fabsf(percentage - 1.158413f) > FLOAT_EPSILON)
    return -3;

  return 0;
}




int main(void) {
  int rc = 0;

  EXE_TEST(1);
  EXE_TEST(2);
  return 0;
}
