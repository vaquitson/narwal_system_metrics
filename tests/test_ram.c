#include <stdio.h>

#include "narwal_ram.h"

int test_ram_basic();

int main(void){
  printf("Ram size: %ld\n", narwal_ram_size());
  printf("Ram size: %ld\n", narwal_ram_size());
  printf("Ram size: %ld\n", narwal_ram_size());
  printf("Ram usage: %ld\n", narwal_ram_usage());
  printf("Ram usage: %f%\n", narwal_ram_usage_percentage());

  return 0;
}
