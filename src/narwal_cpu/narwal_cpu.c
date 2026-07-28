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

int stat_fd = -1;


#define NARWAL_PRIV_OPT_RESET_SEEK 1

// Initialize the global fd if not priviously intialize
ssize_t priv_narwal_string_read_fd(int *fd, char *buf, 
                                   size_t size, const char *path,
                                   unsigned int opt, int *err){

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

float narwal_cpu_usage_percentage(void){
  char buf[NARWAL_CPU_READ_SIZE] = {0};
  ssize_t read_size;
  int err;
  
  read_size = priv_narwal_string_read_fd(&stat_fd, buf, 
                                         NARWAL_CPU_READ_SIZE, NARWAL_CPU_STAT_PATH, 
                                         NARWAL_PRIV_OPT_RESET_SEEK, &err);
  if (read_size < 0){
    printf("xd: %d\n", err);
  } else {
    printf("%s\n", buf);
  }

  return 0.0;
}
