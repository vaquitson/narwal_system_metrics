/**
 * @file narwal_thermal_zones.c
 * @brief Internal implementation of the Narwal Thermal Zones interface.
 *
 * This file contains the internal functions used to enumerate and read
 * thermal zones exposed by the Linux thermal subsystem.
*/

#include <dirent.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "narwal_thermal_zones.h"

#define EVENT_PREFIX(X) NARWAL_THERMAL_ZONES_##X

DIR *d_p = NULL;

/*
 * Returns the length of a null-terminated string, up to a specified limit.
 *
 * This function examines at most `max_len` characters from `str`. If a null
 * terminator is found, its position is returned. Otherwise, `max_len` is
 * returned.
 *
 * This function is provided as a C99-compatible replacement for `strnlen()`,
 * which is not part of the C99 standard.
 *
 * @param str      Pointer to the string to examine.
 * @param max_len  Maximum number of characters to examine.
 *
 * @return The number of characters before the null terminator, excluding the
 *         null terminator itself, or `max_len` if no null terminator is found.
*/
size_t priv_narwal_thermal_zones_strnlen(const char *str, size_t max_len)
{
  size_t length = 0;

  while (length < max_len && str[length] != '\0'){
    ++length;
  }

  return length;
}

/*
 * Replaces the first newline character in a string.
 *
 * This function searches `str` for the first newline character (`'\n'`) and
 * replaces it with the character specified by `replacement`.
 *
 * @param str          Pointer to the null-terminated string to modify.
 * @param replacement  Character used to replace the first newline.
 *
 * @return The index of the replaced newline character. If no newline is found,
 *         the length of the string is returned.
*/
size_t priv_narwal_thermal_zones_strip_line_jump(char *ptr, char replacement)
{
  size_t i = 0; 
  while (ptr[i] != '\0'){
    if (ptr[i] == '\n') {
      ptr[i] = replacement;
      return i;
  }
    i++;
  }
  return i;
}


/**
* @brief Retrieves the next available thermal zone.
*
* On each successful call, this function clears the structure pointed to by
* @p tz_p and fills it with information about the next thermal zone available
* in the system.
*
* Successive calls iterate through the thermal zones exposed under
* "/sys/class/thermal/".
*
* When no more thermal zones are available, the function returns
* @ref NARWAL_THERMAL_ZONES_NO_MORE_TZ and rewinds the internal directory
* iterator so that the next call starts again from the first thermal zone.
*
* when starting a new search for a specific thermal zone.
*
* @param[out] tz_p 
*   Pointer to the structure in which the thermal-zone
*   information will be stored. The structure is cleared before
*   being filled.
*
* @param[in] opts 
* Iteration options.
*
* The @p opts parameter controls the iteration behavior:
*
* * `opts == PRIV_NARWAL_THERMAL_ZONES_OPT_NO_REWIND`: Continue from the current iterator position.
* * `opts == PRIV_NARWAL_THERMAL_ZONES_OPT_REWIND`: Rewind the directory iterator before searching. This is useful
*
* @return A Narwal Thermal Zones status code. Negative values indicate errors.
*
* @retval NARWAL_THERMAL_ZONES_SUCCESS
*      A thermal zone was retrieved successfully and @p tz_p was filled.
*
* @retval NARWAL_THERMAL_ZONES_NO_MORE_TZ 
*     No more thermal zones are available. The internal iterator was rewound.
*
* @retval NARWAL_THERMAL_ZONES_OPEN_ERR
*   The `/sys/class/thermal/` directory could not be opened.
*
*/
int priv_narwal_thermal_zones_fill_next(NarwalThermalZone *tz_p, unsigned int opts)
{
  struct dirent *dir_p = NULL;
  size_t path_len = 0;

  if (d_p == NULL){
    d_p = opendir(NARWAL_THERMAL_ZONE_THERMAL_DIR);
    if (d_p == NULL){
      tz_p->error = NARWAL_THERMAL_ZONES_OPEN_ERR;
      return  EVENT_PREFIX(OPEN_ERR);
    }
  }

  if ((opts & PRIV_NARWAL_THERMAL_ZONES_OPT_REWIND) == PRIV_NARWAL_THERMAL_ZONES_OPT_REWIND)
    rewinddir(d_p);
    
  memset(tz_p, 0, sizeof(NarwalThermalZone));

  strncpy(tz_p->path, NARWAL_THERMAL_ZONE_THERMAL_DIR, NARWAL_THERMAL_ZONE_PATH_MAX);
  path_len = strlen(NARWAL_THERMAL_ZONE_THERMAL_DIR);

  while ((dir_p = readdir(d_p)) != NULL){
    if (strstr(dir_p->d_name, "thermal_zone") != NULL){
      strncat(tz_p->path, dir_p->d_name, NARWAL_THERMAL_ZONE_PATH_MAX - path_len); 
      path_len += priv_narwal_thermal_zones_strnlen(dir_p->d_name, NARWAL_THERMAL_ZONE_PATH_MAX);

      tz_p->path_len = path_len;
      return EVENT_PREFIX(SUCCESS);
    }
  }

  rewinddir(d_p);
  return EVENT_PREFIX(NO_MORE_TZ);
}


char *narwal_thermal_zones_get_type(NarwalThermalZone *tz_p) 
{ 
  if (tz_p == NULL){
    tz_p->error = EVENT_PREFIX(NULL_PTR);
    return NULL;
  }

  if (tz_p->type_len == 0){
    int fd;
    ssize_t read_s;
    char path[NARWAL_THERMAL_ZONE_PATH_MAX] = {0};

    strncpy(path, tz_p->path, tz_p->path_len);
    strcat(path, "/type");

    fd = open(path, O_RDONLY);
    if (fd < 0){
      tz_p->error = EVENT_PREFIX(OPEN_ERR);
      return NULL;
    }

    read_s = read(fd, tz_p->type, NARWAL_THERMAL_ZONE_TYPE_MAX - 1);
    if (read_s <= 0){
      tz_p->error = EVENT_PREFIX(READ_ERR);
      return NULL;
    }

    tz_p->type[read_s] = '\0';
    priv_narwal_thermal_zones_strip_line_jump(tz_p->type, '\0');
    tz_p->type_len = read_s - 1;
    close(fd);
  }

  return tz_p->type;
}


float narwal_thermal_zones_get_temp(NarwalThermalZone *tz_p)
{
  int fd;
  ssize_t read_s;
  char path[NARWAL_THERMAL_ZONE_PATH_MAX] = {0};
  char temp_str[NARWAL_THERMAL_ZONE_TEMP_MAX];

  long long_temp;

  strncpy(path, tz_p->path, tz_p->path_len);
  strcat(path, "/temp");

  fd = open(path, O_RDONLY);
  if (fd < 0){
    tz_p->error = errno;
    return errno;
  }

  read_s = read(fd, temp_str, NARWAL_THERMAL_ZONE_TEMP_MAX - 1); 
  if (read_s < 0){
    tz_p->error = errno;
    return errno;
  }
  temp_str[read_s] = '\0';
  long_temp = strtol(temp_str, NULL, 10);

  close(fd);
  return long_temp/1000.0;
}


int narwal_thermal_zones_fill_by_type(NarwalThermalZone *tz_p, const char *type_str_p){
  int rc;
  char *type_rs;

  if (d_p != NULL)
    rewinddir(d_p);

  if (tz_p == NULL)
    return EVENT_PREFIX(NULL_PTR);
  
  while ((rc = priv_narwal_thermal_zones_fill_next(tz_p, PRIV_NARWAL_THERMAL_ZONES_OPT_NO_REWIND)) == NARWAL_THERMAL_ZONE_SUCESS){
    type_rs = narwal_thermal_zones_get_type(tz_p);

    if (type_rs ==  NULL)
      return tz_p->error;

    if (strncmp(type_rs, type_str_p, NARWAL_THERMAL_ZONE_TYPE_MAX) == 0)
      return EVENT_PREFIX(SUCCESS);
  }

  return rc;
}

