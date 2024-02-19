#include <stdio.h>
#include "ghostmap.h"

int main(void) {
  struct ghostmap_s *ghostmap = ghostmap_initialize();

  ghostmap_insert("key1", "value1", ghostmap);
  ghostmap_insert("key2", "value2", ghostmap);
  ghostmap_insert("key3", "value3", ghostmap);
  ghostmap_insert("key4", "value4", ghostmap);

  if (ghostmap_find("key1", ghostmap) == 1) {
    printf("The value for %s is %s.\n", ghostmap->keys[ghostmap->position[0]][ghostmap->position[1]], ghostmap->values[ghostmap->position[0]][ghostmap->position[1]]);
  }

  printf("\n");

  while (ghostmap_iterate_next(ghostmap) <= ghostmap->count) {
    printf("Entry %lu is %s and %s.\n", ghostmap->iterator, ghostmap->keys[ghostmap->position[0]][ghostmap->position[1]], ghostmap->values[ghostmap->position[0]][ghostmap->position[1]]);
  }

  printf("\n");
  ghostmap_remove("key4", ghostmap);

  while (ghostmap_iterate_next(ghostmap) <= ghostmap->count) {
    printf("Entry %lu is %s and %s.\n", ghostmap->iterator, ghostmap->keys[ghostmap->position[0]][ghostmap->position[1]], ghostmap->values[ghostmap->position[0]][ghostmap->position[1]]);
  }

  ghostmap_destroy(ghostmap);
  free(ghostmap);
  return 0;
}
