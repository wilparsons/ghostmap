# GhostMap
![GhostMap Logo](https://repository-images.githubusercontent.com/751117710/bfd255a1-2bd8-4c55-92ce-fb247accf5a8)

## Description
GhostMap is an open-addressed, unordered hash table.

Read [this article](https://medium.com/@wilparsons/ghostmap-is-a-new-unordered-hash-table-with-a-high-average-load-factor-and-low-overhead-c360ee0acb4b) for an in-depth explanation.

## License
GhostMap is subject to the software licensing terms from the [LICENSE file](https://github.com/williamstaffordparsons/ghostmap/blob/master/LICENSE).

## Reference
#### `ghostmap_initialize()`
This is the initialization function.

The return value data type is `struct ghostmap_s *`.

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

``` c
ghostmap->keys[ghostmap->position[0]][ghostmap->position[1]]
ghostmap->values[ghostmap->position[0]][ghostmap->position[1]]
```

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

## Usage
``` c
#include "ghostmap.h"

int main(void) {
  struct ghostmap_s *ghostmap = ghostmap_initialize();

  ghostmap_insert("key", "value", ghostmap);
  ghostmap_find("key", ghostmap);
  ghostmap_remove("key", ghostmap);
  ghostmap_destroy(ghostmap);
  free(ghostmap);
  return 0;
}
```
