#include <stdio.h>
#include "ghostmap.h"

int main(void) {
  struct ghostmap_s *ghostmap = ghostmap_initialize();

  ghostmap_insert("key", "value", ghostmap);

  if (ghostmap_find("key", ghostmap) == 1) {
    printf("%s: %s\n", ghostmap->keys[ghostmap->position[0]][ghostmap->position[1]], ghostmap->values[ghostmap->position[0]][ghostmap->position[1]]);
  }

  ghostmap_remove("key", ghostmap);
  ghostmap_destroy(ghostmap);
  free(ghostmap);
  return 0;
}
