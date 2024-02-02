#ifndef ENTRO_MAP_H
#define ENTRO_MAP_H

#include <stdint.h>
#include <stdlib.h>

struct entro_map_s {
  uint32_t **_digests;
  unsigned long _end_bucket_count;
  unsigned long _end_bucket_capacity_count;
  unsigned long _buckets_count;
  unsigned long _tombstones_count;
  char ***keys;
  char ***values;
  unsigned long position[2];
  unsigned long count;

  unsigned long capacity;
};

struct entro_map_s *entro_map_initialize();
void entro_map_insert(const char *key, const char *value, struct entro_map_s *entro_map);
unsigned char entro_map_find(const char *key, struct entro_map_s *entro_map);
void entro_map_remove(const char *key, struct entro_map_s *entro_map);
void entro_map_destroy(struct entro_map_s *entro_map);
#endif
