#ifndef NARWAL_CPU_H
#define NARWAL_CPU_H

#define NARWAL_CPU_STAT_PATH "/proc/stat"
#define NARWAL_CPU_READ_SIZE 200

float narwal_cpu_usage_percentage(void);

#define NARWAL_CPU_EVENT_PREFIX(X) NARWAL_CPU_##X

#define NARWAL_CPU_SUCCESS         0
#define NARWAL_CPU_OPEN_ERR       -1
#define NARWAL_CPU_READ_ERR       -2
#define NARWAL_CPU_KEY_NOT_FOUND  -3
#define NARWAL_CPU_TO_LARGE_ENTRY -4
#define NARWAL_CPU_NULL_BUF       -5
#define NARWAL_CPU_SEEK_RESET_ERR -6

#endif
