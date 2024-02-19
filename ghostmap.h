#ifndef GHOSTMAP_H
#define GHOSTMAP_H

#include <stdint.h>
#include <stdlib.h>

struct ghostmap_s {
  uint32_t **_digests;
  unsigned long _end_bucket_count;
  unsigned long _end_bucket_capacity_count;
  unsigned long _buckets_count;
  unsigned long _tombstones_count;
  char ***keys;
  char ***values;
  unsigned long position[2];
  unsigned long count;
};

struct ghostmap_s *ghostmap_initialize();
void ghostmap_insert(const char *key, const char *value, struct ghostmap_s *ghostmap);
char ghostmap_find(const char *key, struct ghostmap_s *ghostmap);
void ghostmap_remove(const char *key, struct ghostmap_s *ghostmap);
void ghostmap_destroy(struct ghostmap_s *ghostmap);
#endif
