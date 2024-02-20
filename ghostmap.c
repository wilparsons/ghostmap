#include "ghostmap.h"
#include "quakehash.h"

void _handle_error() {
  exit(EXIT_FAILURE);
}

struct ghostmap_s *ghostmap_initialize() {
  struct ghostmap_s *ghostmap = malloc(sizeof(struct ghostmap_s));
  ghostmap->_digests = malloc(sizeof(uint32_t *));
  ghostmap->keys = malloc(sizeof(char **));
  ghostmap->values = malloc(sizeof(char **));

  if (
    ghostmap->_digests != NULL &&
    ghostmap->keys != NULL &&
    ghostmap->values != NULL
  ) {
    ghostmap->_digests[0] = calloc(16, sizeof(uint32_t));
    ghostmap->keys[0] = calloc(16, sizeof(char *));
    ghostmap->values[0] = calloc(16, sizeof(char *));

    if (
      ghostmap->_digests[0] != NULL &&
      ghostmap->keys[0] != NULL &&
      ghostmap->values[0] != NULL
    ) {
      ghostmap->_end_bucket_count = 0;
      ghostmap->_end_bucket_capacity_count = 16;
      ghostmap->_buckets_count = 1;
      ghostmap->_tombstones_count = 0;
      ghostmap->count = 0;
    } else {
      _handle_error();
    }
  } else {
    _handle_error();
  }

  return ghostmap;
}

void ghostmap_insert(const char *key, const char *value, struct ghostmap_s *ghostmap) {
  uint32_t **copied_digests;
  char ***copied_keys;
  char ***copied_values;
  uint32_t digest = quakehash(key, 1111111111);
  uint32_t truncated_digest;
  unsigned char maximum_bucket_probes_count = 3;
  uint32_t bucket_capacity_mask = 15;
  unsigned long i;
  unsigned long j;
  unsigned long k;
  bool is_insertable = false;
  bool is_duplicate = false;

  while (digest < 2) {
    digest++;
  }

  i = 0;

  while (
    is_insertable == false &&
    i != ghostmap->_buckets_count
  ) {
    truncated_digest = digest & bucket_capacity_mask;
    j = 0;

    while (
      j != maximum_bucket_probes_count &&
      ghostmap->_digests[i][truncated_digest] != 0
    ) {
      if (digest == ghostmap->_digests[i][truncated_digest]) {
        k = 0;

        while (
          key[k] == ghostmap->keys[i][truncated_digest][k] &&
          key[k] != 0
        ) {
          k++;
        }

        is_duplicate = key[k] == ghostmap->keys[i][truncated_digest][k];

        if (is_duplicate == true) {
          k = 0;

          while (
            value[k] == ghostmap->values[i][truncated_digest][k] &&
            value[k] != 0 &&
            ghostmap->values[i][truncated_digest][k] != 0
          ) {
            k++;
          }

          if (
            value[k] != 0 ||
            ghostmap->values[i][truncated_digest][k] != 0
          ) {
            while (
              value[k] != 0 &&
              ghostmap->values[i][truncated_digest][k] != 0
            ) {
              k++;
            }

            if (
              value[k] == 0 &&
              ghostmap->values[i][truncated_digest][k] == 0
            ) {
              k++;

              while (k != 0) {
                k--;
                ghostmap->values[i][truncated_digest][k] = value[k];
              }
            } else {
              free(ghostmap->values[i][truncated_digest]);

              while (value[k] != 0) {
                k++;
              }

              k++;
              ghostmap->values[i][truncated_digest] = calloc(k, sizeof(char));

              if (ghostmap->values[i][truncated_digest] != NULL) {
                while (k != 0) {
                  k--;
                  ghostmap->values[i][truncated_digest][k] = value[k];
                }
              } else {
                _handle_error();
              }
            }
          }

          j = maximum_bucket_probes_count;
          is_insertable = true;
        } else {
          truncated_digest = (truncated_digest + 1) & bucket_capacity_mask;
          j++;
        }
      } else {
        truncated_digest = (truncated_digest + 1) & bucket_capacity_mask;
        j++;
      }
    }

    if (is_duplicate == false) {
      if (
        j != maximum_bucket_probes_count &&
        ghostmap->_digests[i][truncated_digest] == 0
      ) {
        is_insertable = true;
      } else {
        if (maximum_bucket_probes_count != 84) {
          maximum_bucket_probes_count += 3;
          bucket_capacity_mask = (bucket_capacity_mask << 1) + 1;
        }

        i++;
      }
    }
  }

  if (is_duplicate == false) {
    if (is_insertable == false) {
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
          copied_digests[j] = ghostmap->_digests[j];
          copied_keys[j] = ghostmap->keys[j];
          copied_values[j] = ghostmap->values[j];
        }

        free(ghostmap->_digests);
        free(ghostmap->keys);
        free(ghostmap->values);
        ghostmap->_digests = copied_digests;
        ghostmap->keys = copied_keys;
        ghostmap->values = copied_values;
        ghostmap->_digests[i] = calloc(bucket_capacity_mask + 1, sizeof(uint32_t));
        ghostmap->keys[i] = calloc(bucket_capacity_mask + 1, sizeof(char *));
        ghostmap->values[i] = calloc(bucket_capacity_mask + 1, sizeof(char *));

        if (
          ghostmap->_digests[i] != NULL &&
          ghostmap->keys[i] != NULL &&
          ghostmap->values[i] != NULL
        ) {
          j = 0;

          while (j != ghostmap->_end_bucket_capacity_count) {
            if (ghostmap->_digests[i - 1][j] == 1) {
              ghostmap->_tombstones_count++;
            }

            j++;
          }

          ghostmap->_end_bucket_count = 0;
          ghostmap->_end_bucket_capacity_count = bucket_capacity_mask + 1;
          ghostmap->_buckets_count++;
          truncated_digest = digest & bucket_capacity_mask;
        } else {
          _handle_error();
        }
      } else {
        _handle_error();
      }
    }

    ghostmap->_digests[i][truncated_digest] = digest;
    j = 0;

    while (key[j] != 0) {
      j++;
    }

    j++;
    ghostmap->keys[i][truncated_digest] = calloc(j, sizeof(char));

    if (ghostmap->keys[i][truncated_digest] != NULL) {
      while (j != 0) {
        j--;
        ghostmap->keys[i][truncated_digest][j] = key[j];
      }

      while (value[j] != 0) {
        j++;
      }

      j++;
      ghostmap->values[i][truncated_digest] = calloc(j, sizeof(char));

      if (ghostmap->values[i][truncated_digest] != NULL) {
        while (j != 0) {
          j--;
          ghostmap->values[i][truncated_digest][j] = value[j];
        }

        ghostmap->count++;

        if ((i + 1) == ghostmap->_buckets_count) {
          ghostmap->_end_bucket_count++;
        }
      } else {
        _handle_error();
      }
    } else {
      _handle_error();
    }
  }
}

bool ghostmap_find(const char *key, struct ghostmap_s *ghostmap) {
  uint32_t digest = quakehash(key, 1111111111);
  uint32_t truncated_digest;
  unsigned char maximum_bucket_probes_count = 3;
  uint32_t bucket_capacity_mask = 15;
  unsigned long i;
  unsigned char j;
  unsigned long k;
  bool is_found = false;

  while (digest < 2) {
    digest++;
  }

  i = 0;

  while (i != ghostmap->_buckets_count) {
    truncated_digest = digest & bucket_capacity_mask;
    j = 0;

    while (
      j != maximum_bucket_probes_count &&
      ghostmap->_digests[i][truncated_digest] != 0
    ) {
      if (digest == ghostmap->_digests[i][truncated_digest]) {
        k = 0;

        while (
          key[k] == ghostmap->keys[i][truncated_digest][k] &&
          key[k] != 0
        ) {
          k++;
        }

        if (key[k] == ghostmap->keys[i][truncated_digest][k]) {
          ghostmap->position[0] = i;
          ghostmap->position[1] = truncated_digest;
          i = ghostmap->_buckets_count - 1;
          j = maximum_bucket_probes_count;
          is_found = true;
        } else {
          truncated_digest = (truncated_digest + 1) & bucket_capacity_mask;
          j++;
        }
      } else {
        truncated_digest = (truncated_digest + 1) & bucket_capacity_mask;
        j++;
      }
    }

    if (maximum_bucket_probes_count != 84) {
      maximum_bucket_probes_count += 3;
      bucket_capacity_mask = (bucket_capacity_mask << 1) + 1;
    }

    i++;
  }

  return is_found;
}

void ghostmap_remove(const char *key, struct ghostmap_s *ghostmap) {
  uint32_t **copied_digests;
  char ***copied_keys;
  char ***copied_values;
  uint32_t digest = quakehash(key, 1111111111);
  uint32_t truncated_digest;
  unsigned char maximum_bucket_probes_count = 3;
  uint32_t bucket_capacity_mask = 15;
  unsigned long i;
  unsigned long j;
  unsigned long k;
  bool is_removed = false;
  bool is_shrinkable;

  while (digest < 2) {
    digest++;
  }

  i = 0;

  while (i != ghostmap->_buckets_count) {
    truncated_digest = digest & bucket_capacity_mask;
    j = 0;

    while (
      j != maximum_bucket_probes_count &&
      ghostmap->_digests[i][truncated_digest] != 0
    ) {
      if (digest == ghostmap->_digests[i][truncated_digest]) {
        k = 0;

        while (
          key[k] == ghostmap->keys[i][truncated_digest][k] &&
          key[k] != 0
        ) {
          k++;
        }

        if (key[k] == ghostmap->keys[i][truncated_digest][k]) {
          ghostmap->_digests[i][truncated_digest] = 1;
          free(ghostmap->keys[i][truncated_digest]);
          free(ghostmap->values[i][truncated_digest]);
          ghostmap->count--;

          if ((i + 1) == ghostmap->_buckets_count) {
            ghostmap->_end_bucket_count--;
          } else {
            ghostmap->_tombstones_count++;
          }

          is_removed = true;

          if (
            ghostmap->_buckets_count != 1 &&
            ((ghostmap->_end_bucket_count >> 1) + ghostmap->_end_bucket_count) < ghostmap->_tombstones_count
          ) {
            is_shrinkable = true;

            if (ghostmap->_end_bucket_count != 0) {
              i = ghostmap->_end_bucket_capacity_count;

              while (i != 0) {
                i--;
                digest = ghostmap->_digests[ghostmap->_buckets_count - 1][i];

                if (digest > 1) {
                  maximum_bucket_probes_count = 3;
                  bucket_capacity_mask = 15;
                  j = 0;

                  while (j != (ghostmap->_buckets_count - 1)) {
                    truncated_digest = digest & bucket_capacity_mask;
                    k = 0;

                    while (
                      k != maximum_bucket_probes_count &&
                      ghostmap->_digests[j][truncated_digest] > 1
                    ) {
                      truncated_digest = (truncated_digest + 1) & bucket_capacity_mask;
                      k++;
                    }

                    if (
                      k != maximum_bucket_probes_count &&
                      ghostmap->_digests[j][truncated_digest] < 2
                    ) {
                      ghostmap->_digests[ghostmap->_buckets_count - 1][i] = 1;

                      if (ghostmap->_digests[j][truncated_digest] == 1) {
                        ghostmap->_tombstones_count--;
                      }

                      ghostmap->_digests[j][truncated_digest] = digest;
                      ghostmap->keys[j][truncated_digest] = ghostmap->keys[ghostmap->_buckets_count - 1][i];
                      ghostmap->values[j][truncated_digest] = ghostmap->values[ghostmap->_buckets_count - 1][i];
                      j = ghostmap->_buckets_count - 1;
                    } else {
                      if (maximum_bucket_probes_count != 84) {
                        maximum_bucket_probes_count += 3;
                        bucket_capacity_mask = (bucket_capacity_mask << 1) + 1;
                      }

                      j++;
                    }
                  }
                }

                if (ghostmap->_digests[ghostmap->_buckets_count - 1][i] > 1) {
                  i = 0;
                  is_shrinkable = false;
                }
              }
            }

            if (is_shrinkable == true) {
              copied_digests = calloc(ghostmap->_buckets_count - 1, sizeof(uint32_t *));
              copied_keys = calloc(ghostmap->_buckets_count - 1, sizeof(char **));
              copied_values = calloc(ghostmap->_buckets_count - 1, sizeof(char **));

              if (
                copied_digests != NULL &&
                copied_keys != NULL &&
                copied_values != NULL
              ) {
                i = ghostmap->_buckets_count - 1;

                while (i != 0) {
                  i--;
                  copied_digests[i] = ghostmap->_digests[i];
                  copied_keys[i] = ghostmap->keys[i];
                  copied_values[i] = ghostmap->values[i];
                }

                free(ghostmap->_digests[ghostmap->_buckets_count - 1]);
                free(ghostmap->keys[ghostmap->_buckets_count - 1]);
                free(ghostmap->values[ghostmap->_buckets_count - 1]);
                free(ghostmap->_digests);
                free(ghostmap->keys);
                free(ghostmap->values);
                ghostmap->_digests = copied_digests;
                ghostmap->keys = copied_keys;
                ghostmap->values = copied_values;
                ghostmap->_end_bucket_count = 0;
                ghostmap->_end_bucket_capacity_count >>= 1;
                ghostmap->_buckets_count--;
                i = 0;

                while (i != ghostmap->_end_bucket_capacity_count) {
                  if (ghostmap->_digests[ghostmap->_buckets_count - 1][i] > 1) {
                    ghostmap->_end_bucket_count++;
                  } else {
                    if (ghostmap->_digests[ghostmap->_buckets_count - 1][i] == 1) {
                      ghostmap->_tombstones_count--;
                    }
                  }

                  i++;
                }
              } else {
                _handle_error();
              }
            }
          }

          i = ghostmap->_buckets_count - 1;
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

    if (maximum_bucket_probes_count != 84) {
      maximum_bucket_probes_count += 3;
      bucket_capacity_mask = (bucket_capacity_mask << 1) + 1;
    }

    i++;
  }
}

void ghostmap_destroy(struct ghostmap_s *ghostmap) {
  uint32_t bucket_capacity_count = 16;
  unsigned long i = 0;
  uint32_t j;

  while (i != ghostmap->_buckets_count) {
    j = 0;

    while (j != bucket_capacity_count) {
      if (ghostmap->_digests[i][j] > 1) {
        free(ghostmap->keys[i][j]);
        free(ghostmap->values[i][j]);
      }

      j++;
    }

    free(ghostmap->_digests[i]);
    free(ghostmap->keys[i]);
    free(ghostmap->values[i]);

    if (i < 27) {
      bucket_capacity_count <<= 1;
    }

    i++;
  }

  free(ghostmap->_digests);
  free(ghostmap->keys);
  free(ghostmap->values);
}
