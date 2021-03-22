# Advanced Data Structures

## Description

Implementations for data structures and corresponding algorithms towards different problems like querying for the closest point or resolving collisions in hasing tables. This collection originated through the course of a university lecture.

## Data Structures

### Bisector Tree
The bisector tree is a data structure towards the closest point problem proposed in [1]. An implementation is available here `src/BisectorTree.c`.

### Linear Probing
Linear probing is a scheme to resolve collsions in hashing tables. The source code is available here `src/LinearProbing.c` and holds an implementation of both Robin Hood [2] and LCFS [3] linear probing.

### Van Emde Boas Tree
The Van Emde Boas tree [4] is a data structure towards the implementation of priority queues. An implemnetation can be found at `src/VanEmdeBoas.c`.

## Run program

In order to run experiments with the provided data structures, one first need to compile the C code using a compiler (e.g. `gcc`).

For example:

```sh
cd src

# compile code
gcc -o BisectorTree BisectorTree.c

# run compiled code
./BisectorTree
```

## References

>[1] Iraj Kalantari and Gerard McDonald. "A data structure and an algorithm for the nearest point problem." *IEEE Transactions on Software Engineering* p. 631–634, 1983.

>[2] Pedro Celis, Per-Ake Larson, and J. Ian Munro. "Robin hood hashing." *26th Annual Symposium on Foundations of Computer Science* p. 281-288, 1985.

>[3] Patricio V Poblete and J Ian Munro. "Last-come-first-served hashing." *Journal of Algorithms* p.228 - 248, 1989.

>[4] Peter van Emde Boas. "Preserving order in a forest in less than logarithmic time." *16th Annual Symposium on Foundations of Computer Science* p. 75-84, 1975.