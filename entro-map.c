// #include <stdio.h>
#include "entro-hash.h"
#include "entro-map.h"

void _entro_map_handle_error() {
  exit(EXIT_FAILURE);
}

struct entro_map_s *entro_map_initialize() {
  struct entro_map_s *entro_map = malloc(sizeof(struct entro_map_s));
  entro_map->_digests = malloc(sizeof(uint32_t *));
  entro_map->keys = malloc(sizeof(char **));
  entro_map->values = malloc(sizeof(char **));

  if (
    entro_map->_digests != NULL &&
    entro_map->keys != NULL &&
    entro_map->values != NULL
  ) {
    entro_map->_digests[0] = calloc(16, sizeof(uint32_t));
    entro_map->keys[0] = calloc(16, sizeof(char *));
    entro_map->values[0] = calloc(16, sizeof(char *));

    if (
      entro_map->_digests[0] != NULL &&
      entro_map->keys[0] != NULL &&
      entro_map->values[0] != NULL
    ) {
      entro_map->_end_bucket_count = 0;
      entro_map->_end_bucket_capacity_count = 16;
      entro_map->_buckets_count = 1;
      entro_map->_tombstones_count = 0;
      entro_map->count = 0;
      entro_map->capacity = 16;
    } else {
      _entro_map_handle_error();
    }
  } else {
    _entro_map_handle_error();
  }

  return entro_map;
}

void entro_map_insert(const char *key, const char *value, struct entro_map_s *entro_map) {
  uint32_t **copied_digests;
  char ***copied_keys;
  char ***copied_values;
  uint32_t digest = entro_hash(key, 1111111111);
  uint32_t truncated_digest;
  unsigned long maximum_bucket_probes_count = 3;
  unsigned long bucket_capacity_mask = 15;
  unsigned long i;
  unsigned long j;
  unsigned long k;
  unsigned char is_insertable = 0;
  unsigned char is_duplicate = 0;

  while (digest < 2) {
    digest++;
  }

  i = 0;

  while (
    is_insertable == 0 &&
    i != entro_map->_buckets_count
  ) {
    truncated_digest = digest & bucket_capacity_mask;
    j = 0;

    while (
      j != maximum_bucket_probes_count &&
      entro_map->_digests[i][truncated_digest] != 0
    ) {
      if (digest == entro_map->_digests[i][truncated_digest]) {
        k = 0;

        while (
          key[k] == entro_map->keys[i][truncated_digest][k] &&
          key[k] != 0
        ) {
          k++;
        }

        is_duplicate = key[k] == entro_map->keys[i][truncated_digest][k];

        if (is_duplicate == 1) {
          k = 0;

          while (
            value[k] == entro_map->values[i][truncated_digest][k] &&
            value[k] != 0 &&
            entro_map->values[i][truncated_digest][k] != 0
          ) {
            k++;
          }

          if (
            value[k] != 0 ||
            entro_map->values[i][truncated_digest][k] != 0
          ) {
            while (
              value[k] != 0 &&
              entro_map->values[i][truncated_digest][k] != 0
            ) {
              k++;
            }

            if (
              value[k] == 0 &&
              entro_map->values[i][truncated_digest][k] == 0
            ) {
              k++;

              while (k != 0) {
                k--;
                entro_map->values[i][truncated_digest][k] = value[k];
              }
            } else {
              free(entro_map->values[i][truncated_digest]);

              while (value[k] != 0) {
                k++;
              }

              k++;
              entro_map->values[i][truncated_digest] = calloc(k, sizeof(char));

              if (entro_map->values[i][truncated_digest] != NULL) {
                while (k != 0) {
                  k--;
                  entro_map->values[i][truncated_digest][k] = value[k];
                }
              } else {
                _entro_map_handle_error();
              }
            }
          }

          j = maximum_bucket_probes_count;
          is_insertable = 1;
        } else {
          truncated_digest = (truncated_digest + 1) & bucket_capacity_mask;
          j++;
        }
      } else {
        truncated_digest = (truncated_digest + 1) & bucket_capacity_mask;
        j++;
      }
    }

    if (is_duplicate == 0) {
      if (
        j != maximum_bucket_probes_count &&
        entro_map->_digests[i][truncated_digest] == 0
      ) {
        is_insertable = 1;
      } else {
        // todo: bucket_capacity_mask should have a 32-bit limit
        maximum_bucket_probes_count += 3;
        bucket_capacity_mask = (bucket_capacity_mask << 1) + 1;
        i++;
      }
    }
  }

  if (is_duplicate == 0) {
    if (is_insertable == 0) {
      // printf("probes: %lu\n", maximum_bucket_probes_count);
      // printf("%lu of %lu (%lu load factor)\n", entro_map->count, entro_map->capacity, (entro_map->count * 100) / entro_map->capacity);

      copied_digests = calloc(i + 1, sizeof(uint32_t *));
      copied_keys = calloc(i + 1, sizeof(char **));
      copied_values = calloc(i + 1, sizeof(char **));

      if (
        copied_digests != NULL &&
        copied_keys != NULL &&
        copied_values != NULL
      ) {
        j = i;

        while (j != 0) {
          j--;
          copied_digests[j] = entro_map->_digests[j];
          copied_keys[j] = entro_map->keys[j];
          copied_values[j] = entro_map->values[j];
        }

        free(entro_map->_digests);
        free(entro_map->keys);
        free(entro_map->values);
        entro_map->_digests = copied_digests;
        entro_map->keys = copied_keys;
        entro_map->values = copied_values;
        entro_map->_digests[i] = calloc(bucket_capacity_mask + 1, sizeof(uint32_t));
        entro_map->keys[i] = calloc(bucket_capacity_mask + 1, sizeof(char *));
        entro_map->values[i] = calloc(bucket_capacity_mask + 1, sizeof(char *));

        if (
          entro_map->_digests[i] != NULL &&
          entro_map->keys[i] != NULL &&
          entro_map->values[i] != NULL
        ) {
          entro_map->_end_bucket_count = 0;
          entro_map->_end_bucket_capacity_count = bucket_capacity_mask + 1;
          entro_map->_buckets_count++;
          truncated_digest = digest & bucket_capacity_mask;
          entro_map->capacity += bucket_capacity_mask + 1;
        } else {
          _entro_map_handle_error();
        }
      } else {
        _entro_map_handle_error();
      }
    }

    entro_map->_digests[i][truncated_digest] = digest;
    j = 0;

    while (key[j] != 0) {
      j++;
    }

    j++;
    entro_map->keys[i][truncated_digest] = calloc(j, sizeof(char));

    if (entro_map->keys[i][truncated_digest] != NULL) {
      while (j != 0) {
        j--;
        entro_map->keys[i][truncated_digest][j] = key[j];
      }

      while (value[j] != 0) {
        j++;
      }

      j++;
      entro_map->values[i][truncated_digest] = calloc(j, sizeof(char));

      if (entro_map->values[i][truncated_digest] != NULL) {
        while (j != 0) {
          j--;
          entro_map->values[i][truncated_digest][j] = value[j];
        }

        entro_map->count++;

        if (i + 1 == entro_map->_buckets_count) {
          entro_map->_end_bucket_count++;
        }
      } else {
        _entro_map_handle_error();
      }
    } else {
      _entro_map_handle_error();
    }
  }
}

unsigned char entro_map_find(const char *key, struct entro_map_s *entro_map) {
  uint32_t digest = entro_hash(key, 1111111111);
  uint32_t truncated_digest;
  unsigned long maximum_bucket_probes_count = 3;
  unsigned long bucket_capacity_mask = 15;
  unsigned long i;
  unsigned long j;
  unsigned long k;
  unsigned char is_found = 0;

  while (digest < 2) {
    digest++;
  }

  i = 0;

  while (i != entro_map->_buckets_count) {
    truncated_digest = digest & bucket_capacity_mask;
    j = 0;

    while (
      j != maximum_bucket_probes_count &&
      entro_map->_digests[i][truncated_digest] != 0
    ) {
      if (digest == entro_map->_digests[i][truncated_digest]) {
        k = 0;

        while (
          key[k] == entro_map->keys[i][truncated_digest][k] &&
          key[k] != 0
        ) {
          k++;
        }

        if (key[k] == entro_map->keys[i][truncated_digest][k]) {
          entro_map->position[0] = i;
          entro_map->position[1] = truncated_digest;
          i = entro_map->_buckets_count;
          j = maximum_bucket_probes_count;
          is_found = 1;
        } else {
          truncated_digest = (truncated_digest + 1) & bucket_capacity_mask;
          j++;
        }
      } else {
        truncated_digest = (truncated_digest + 1) & bucket_capacity_mask;
        j++;
      }
    }

    if (is_found == 0) {
      maximum_bucket_probes_count += 3;
      bucket_capacity_mask = (bucket_capacity_mask << 1) + 1;
      i++;
    }
  }

  return is_found;
}

void entro_map_remove(const char *key, struct entro_map_s *entro_map) {
  uint32_t **copied_digests;
  char ***copied_keys;
  char ***copied_values;
  uint32_t digest = entro_hash(key, 1111111111);
  uint32_t truncated_digest;
  unsigned long maximum_bucket_probes_count = 3;
  unsigned long bucket_capacity_mask = 15;
  unsigned long i;
  unsigned long j;
  unsigned long k;
  unsigned char is_removed = 0;
  unsigned char is_shrinkable;

  while (digest < 2) {
    digest++;
  }

  i = 0;

  while (i != entro_map->_buckets_count) {
    truncated_digest = digest & bucket_capacity_mask;
    j = 0;

    while (
      j != maximum_bucket_probes_count &&
      entro_map->_digests[i][truncated_digest] != 0
    ) {
      if (digest == entro_map->_digests[i][truncated_digest]) {
        k = 0;

        while (
          key[k] == entro_map->keys[i][truncated_digest][k] &&
          key[k] != 0
        ) {
          k++;
        }

        if (key[k] == entro_map->keys[i][truncated_digest][k]) {
          entro_map->_digests[i][truncated_digest] = 1;
          free(entro_map->keys[i][truncated_digest]);
          free(entro_map->values[i][truncated_digest]);
          entro_map->count--;

          if (i + 1 == entro_map->_buckets_count) {
            entro_map->_end_bucket_count--;
          } else {
            entro_map->_tombstones_count++;
          }

          is_removed = 1;

          if (
            entro_map->_buckets_count != 1 &&
            (entro_map->_end_bucket_count >> 1) + entro_map->_end_bucket_count < entro_map->_tombstones_count
          ) {
            is_shrinkable = 1;

            if (entro_map->_end_bucket_count != 0) {
              i = entro_map->_end_bucket_capacity_count;

              while (i != 0) {
                i--;
                digest = entro_map->_digests[entro_map->_buckets_count - 1][i];

                if (digest > 1) {
                  maximum_bucket_probes_count = 2;
                  bucket_capacity_mask = 15;
                  j = 0;

                  while (j != entro_map->_buckets_count - 1) {
                    truncated_digest = digest & bucket_capacity_mask;
                    k = 0;

                    while (
                      k != maximum_bucket_probes_count &&
                      entro_map->_digests[j][truncated_digest] > 1
                    ) {
                      truncated_digest = (truncated_digest + 1) & bucket_capacity_mask;
                      k++;
                    }

                    if (
                      k != maximum_bucket_probes_count &&
                      entro_map->_digests[j][truncated_digest] < 2
                    ) {
                      entro_map->_digests[entro_map->_buckets_count - 1][i] = 0;

                      if (entro_map->_digests[j][truncated_digest] == 1) {
                        entro_map->_tombstones_count--;
                      }

                      entro_map->_digests[j][truncated_digest] = digest;
                      entro_map->keys[j][truncated_digest] = entro_map->keys[entro_map->_buckets_count - 1][i];
                      entro_map->values[j][truncated_digest] = entro_map->values[entro_map->_buckets_count - 1][i];
                      j = entro_map->_buckets_count - 1;
                    } else {
                      maximum_bucket_probes_count += 3;
                      bucket_capacity_mask = (bucket_capacity_mask << 1) + 1;
                      j++;
                    }
                  }
                }

                if (entro_map->_digests[entro_map->_buckets_count - 1][i] != 0) {
                  i = 0;
                  is_shrinkable = 0;
                }
              }
            }

            if (is_shrinkable == 1) {
              copied_digests = calloc(entro_map->_buckets_count - 1, sizeof(uint32_t *));
              copied_keys = calloc(entro_map->_buckets_count - 1, sizeof(char **));
              copied_values = calloc(entro_map->_buckets_count - 1, sizeof(char **));

              if (
                copied_digests != NULL &&
                copied_keys != NULL &&
                copied_values != NULL
              ) {
                i = entro_map->_buckets_count - 1;

                while (i != 0) {
                  i--;
                  copied_digests[i] = entro_map->_digests[i];
                  copied_keys[i] = entro_map->keys[i];
                  copied_values[i] = entro_map->values[i];
                }

                free(entro_map->_digests[entro_map->_buckets_count - 1]);
                free(entro_map->keys[entro_map->_buckets_count - 1]);
                free(entro_map->values[entro_map->_buckets_count - 1]);
                free(entro_map->_digests);
                free(entro_map->keys);
                free(entro_map->values);
                entro_map->_digests = copied_digests;
                entro_map->keys = copied_keys;
                entro_map->values = copied_values;
                entro_map->_end_bucket_count = 0;
                entro_map->_end_bucket_capacity_count >>= 1;
                entro_map->_buckets_count--;
                i = 0;

                while (i != entro_map->_end_bucket_capacity_count) {
                  if (entro_map->_digests[entro_map->_buckets_count - 1][i] > 1) {
                    entro_map->_end_bucket_count++;
                  } else {
                    if (entro_map->_digests[entro_map->_buckets_count - 1][i] == 1) {
                      entro_map->_tombstones_count--;
                    }
                  }

                  i++;
                }
              } else {
                _entro_map_handle_error();
              }
            }
          }

          i = entro_map->_buckets_count;
          j = maximum_bucket_probes_count;
        } else {
          truncated_digest = (truncated_digest + 1) & bucket_capacity_mask;
          j++;
        }
      } else {
        truncated_digest = (truncated_digest + 1) & bucket_capacity_mask;
        j++;
      }
    }

    if (is_removed == 0) {
      maximum_bucket_probes_count += 3;
      bucket_capacity_mask = (bucket_capacity_mask << 1) + 1;
      i++;
    }
  }
}

void entro_map_destroy(struct entro_map_s *entro_map) {
  unsigned long bucket_capacity_count = 16;
  unsigned long i = 0;
  unsigned long j;

  while (i != entro_map->_buckets_count) {
    j = 0;

    while (j != bucket_capacity_count) {
      if (entro_map->_digests[i][j] > 1) {
        free(entro_map->keys[i][j]);
        free(entro_map->values[i][j]);
      }

      j++;
    }

    free(entro_map->_digests[i]);
    free(entro_map->keys[i]);
    free(entro_map->values[i]);
    bucket_capacity_count <<= 1;
    i++;
  }

  free(entro_map->_digests);
  free(entro_map->keys);
  free(entro_map->values);
}
