#include <dirent.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "narwal_thermal_zones.h"

int print_thermal_zones_info()
{
  NarwalThermalZone tz_p = {0};
  char *type;
  float temp;

  while (narwal_thermal_zones_fill_next(&tz_p) == NARWAL_THERMAL_ZONE_SUCESS){
    type = narwal_thermal_zones_get_type(&tz_p);
    if (type == NULL){
      printf("error: %s\n", strerror(tz_p.error));
      return -1;
    }

    temp = narwal_thermal_zones_get_temp(&tz_p); 
    printf("path: %s\n", tz_p.path);
    printf("type: %s\n", type);
    printf("temp: %f\n", temp);
    printf("\n");
  }

  printf("\n\nsecond iter\n\n");
  while (narwal_thermal_zones_fill_next(&tz_p) == NARWAL_THERMAL_ZONE_SUCESS){
    type = narwal_thermal_zones_get_type(&tz_p);
    if (type == NULL){
      printf("error: %s\n", strerror(tz_p.error));
      return -1;
    }

    temp = narwal_thermal_zones_get_temp(&tz_p); 
    printf("path: %s\n", tz_p.path);
    printf("type: %s\n", type);
    printf("temp: %f\n", temp);
    printf("\n");
  }
  return 0;
}

int get_thermal_zone_by_type(){
  NarwalThermalZone tz = {0};
  if (narwal_thermal_zones_fill_by_type(&tz, "x86_pkg_temp") == NARWAL_THERMAL_ZONE_SUCESS){
    printf("path: %s\n", tz.path);
    printf("type: %s\n", narwal_thermal_zones_get_type(&tz));
    printf("temp: %f\n", narwal_thermal_zones_get_temp(&tz));   
    return 0;
  } else {
    return -1;
  }
}

int main(void){
  print_thermal_zones_info();
  get_thermal_zone_by_type();
  return 0;
}
