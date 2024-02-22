# GhostMap
![GhostMap Logo](https://repository-images.githubusercontent.com/751117710/42df0a45-392f-4739-ac95-8caee5d96418)

## Description
GhostMap is an open-addressed, unordered hash table.

Read [this article](https://medium.com/@wilparsons/ghostmap-is-a-new-unordered-hash-table-with-a-high-average-load-factor-and-low-overhead-c360ee0acb4b) for an in-depth explanation.

## Usage
``` c
#include <stdio.h>
#include "ghostmap.h"

int main(void) {
  struct ghostmap_s *ghostmap = ghostmap_initialize();

  ghostmap_insert("key", "value", ghostmap);

  if (ghostmap_find("key", ghostmap) == true) {
    printf("%s: %s\n", ghostmap->keys[ghostmap->position[0]][ghostmap->position[1]], ghostmap->values[ghostmap->position[0]][ghostmap->position[1]]);
  }

  ghostmap_remove("key", ghostmap);
  ghostmap_destroy(ghostmap);
  free(ghostmap);
  return 0;
}
```

## Reference
#### `ghostmap_initialize()`
This is the initialization function.

The return value data type is `struct ghostmap_s`.

It returns a pointer to a `struct` instance containing counts and entries with initialized heap memory.

#### `ghostmap_insert()`
This is the insertion function that accepts the 3 following arguments.

`key` is the null-terminated string to hash and insert into the map.

`value` is null-terminated string to insert as the value for `key`.

`ghostmap` is the pointer to the `struct` instance containing the inserted map data.

The return value data type is `void`.

Duplicate `key` entries are updated with the new `value`.

#### `ghostmap_find()`
This is the searching function that accepts the 2 following arguments.

`key` is the null-terminated string to find in the map.

`ghostmap` is the pointer to the `struct` instance containing the inserted map data.

The return value data type is `bool`.

It returns either `true` when an entry is found or `false` when an entry isn't found.

The first 2 indices in `ghostmap->position` are updated with the new position of the found entry.

Pairs in `ghostmap->keys` and `ghostmap->values` have corresponding indices.

#### `ghostmap_remove()`
This is the removal function that accepts the 2 following arguments.

`key` is the null-terminated string to remove in the map.

`ghostmap` is the pointer to the `struct` instance containing the inserted map data.

The return value data type is `void`.

#### `ghostmap_destroy()`
This is the deconstructor function that accepts the following argument.

`ghostmap` is the pointer to the `struct` instance containing the inserted map data.

The return value data type is `void`.

It frees dynamically-allocated heap memory for all entries recursively.

## Support
GhostMap was designed and developed by [Wil Parsons](https://wilparsons.github.io).

I'm available for freelance work to extend, implement or modify GhostMap and other algorithms.
