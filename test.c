#include <stdio.h>
#include "entro-map.h"

int main(void) {
  struct entro_map_s *entro_map = entro_map_initialize();
  const char *key = "key";
  const char *value = "value";

  entro_map_insert(key, value, entro_map);

  if (entro_map_find(key, entro_map) == 1) {
    printf("%s: %s\n", entro_map->keys[entro_map->position[0]][entro_map->position[1]], entro_map->values[entro_map->position[0]][entro_map->position[1]]);
  }

  entro_map_remove(key, entro_map);
  entro_map_destroy(entro_map);
  free(entro_map);
  return 0;
}
