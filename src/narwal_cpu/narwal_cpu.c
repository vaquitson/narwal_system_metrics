#include <stddef.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
 
#include "narwal_cpu.h"

#define PRIV_NARWAL_CPU_USER_TIME         1  /* Time spent executing user processes. */
#define PRIV_NARWAL_CPU_NICE_TIME         2  /* Time spent executing niced user processes. */
#define PRIV_NARWAL_CPU_SYSTEM_TIME       3  /* Time spent executing kernel processes. */
#define PRIV_NARWAL_CPU_IDLE_TIME         4  /* Time spent idle. */
#define PRIV_NARWAL_CPU_IOWAIT_TIME       5  /* Time spent waiting for I/O completion. */
#define PRIV_NARWAL_CPU_IRQ_TIME          6  /* Time spent servicing hardware interrupts. */
#define PRIV_NARWAL_CPU_SOFTIRQ_TIME      7  /* Time spent servicing software interrupts. */
#define PRIV_NARWAL_CPU_STEAL_TIME        8  /* Time involuntarily waiting for other virtual CPUs. */
#define PRIV_NARWAL_CPU_GUEST_TIME        9  /* Time spent running a guest VM. */
#define PRIV_NARWAL_CPU_GUEST_NICE_TIME  10  /* Time spent running a niced guest VM. */

#define PRIV_NARWAL_CPU_TIME_S int64_t

int stat_fd = -1;

//###### UTILITIS  ######//
#define NARWAL_PRIV_OPT_RESET_SEEK 1

// Initialize the global fd if not priviously intialize
ssize_t priv_narwal_string_read_fd(
        int *fd, 
        char *buf, size_t size, 
  const char *path,
  unsigned int opt, int *err)
{

  if (buf == NULL) {
    *err = NARWAL_CPU_EVENT_PREFIX(NULL_BUF);
    return -1;
  }

  size_t data_read;
  if (*fd < 0) {
    *fd = open(path , O_RDONLY);
    if (*fd < 0){
      *err = NARWAL_CPU_EVENT_PREFIX(OPEN_ERR);
      return -1;
    }
  }

  data_read = read(*fd, buf, size-1);
  if (data_read < 0){
    *err = NARWAL_CPU_EVENT_PREFIX(READ_ERR);
    return -1;
  }

  buf[data_read] = '\0';

  if ((opt & NARWAL_PRIV_OPT_RESET_SEEK) == NARWAL_PRIV_OPT_RESET_SEEK)
    if (lseek(*fd, 0, SEEK_SET) == -1){
      *err = NARWAL_CPU_EVENT_PREFIX(SEEK_RESET_ERR);
      return -1; 
    }
   
  return data_read;
}


// Form https://raw.githubusercontent.com/vaquitson/narwal_algorithms/refs/heads/main/string_parsing/src/narwal_column_parsing.c
int narwal_str_get_col(
  const char *str_p,   size_t str_l, 
  const char *delim_p, size_t delim_l,
  char *buf_p,   size_t buf_s,
  int    index) 
{
  size_t start = 0;
  size_t end = 0;
  size_t add = 0; 
  size_t buf_l = 0;
  
  int data_counter = -1;
  
  if (str_l == 0 || delim_l == 0)
    return -2;
  
  end = delim_l;

  while(start < str_l)
  {
    if (strncmp(str_p+start, delim_p, delim_l) == 0)
    {
      if (buf_l > 0)
      {
        buf_p[buf_l] = '\0';
        buf_l = 0;
        data_counter++;

        if (data_counter == index)
          return 0;
      }

      add = delim_l;
    } else {

      if (str_p[start] == '\n')
        break;

      if (buf_l < buf_s)
        buf_p[buf_l] = str_p[start];

      add = 1;
      buf_l++;
    }

    start += add;
    end   += add;

    if (end > str_l)
      end = start;
  }

  if (buf_l > 0)
  {
    if (data_counter + 1 == index)
    {
      buf_p[buf_l] = '\0';
    }
    return 0;
  }

  return -1; 
}

//###### END UTILITIS  ######//


int priv_narwal_cpu_parse_cpu_stat_line(
  PRIV_NARWAL_CPU_TIME_S *val_arr_p, size_t val_arr_l)
{
  char number_str_p[30] = {0};
  char stat_str_p[NARWAL_CPU_READ_SIZE] = {0};

  char c;
  int rc, i, err = 0;
  size_t read_size;

  if (val_arr_l > 10)
    return -1;

  read_size = priv_narwal_string_read_fd(&stat_fd, stat_str_p, 
                                         NARWAL_CPU_READ_SIZE, NARWAL_CPU_STAT_PATH, 
                                         NARWAL_PRIV_OPT_RESET_SEEK, &err);

  if (read_size < 0)
    return -1.0;

  for (i = 0; i < val_arr_l; i++) {
    rc = narwal_str_get_col(stat_str_p, read_size, 
                                  " ", 1, 
                                  number_str_p, 30,
                                  i+1);

    val_arr_p[i] = strtol(number_str_p, NULL, 10);
    if (rc < 0)
      return -1;
  }

  lseek(stat_fd, 0, SEEK_SET);
  return i;
}

float narwal_cpu_usage_percentage(void)
{
  PRIV_NARWAL_CPU_TIME_S stat_val_arr_1_p[10] = {0};
  PRIV_NARWAL_CPU_TIME_S stat_val_arr_2_p[10] = {0};

  ssize_t read_size;
  int err, rc = 0;

  rc = priv_narwal_cpu_parse_cpu_stat_line(stat_val_arr_1_p, 10);
  if (rc != 10){
    printf("rc: %d\n", rc);
    return -1.0;
  }
  

  for (int i = 0; i < 10; i++){
    printf("arr val: %ld\n", stat_val_arr_1_p[i]);
  }
  
  // no muy contento con esto la verdad
  usleep(100000);
  printf("\n\n\n");



  rc = priv_narwal_cpu_parse_cpu_stat_line(stat_val_arr_2_p, 10);
  if (rc != 10){
    printf("rc: %d\n", rc);
    return -1.0;
  }

  for (int i = 0; i < 10; i++){
    printf("arr val: %ld\n", stat_val_arr_2_p[i]);
  }

  return 0.0;
}

// cpu  19096 0 6533 1749929 3778 3651 866 0 0 0
