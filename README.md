# Advanced Data Structures Repository

## Description

This repository holds source code and documentation for the advanced data structures lecture given at UPC. All sources in `src` folder provided in C language.

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

## Data Structures

### Bisector Tree
The bisector tree is a data structre towards the closest point problem proposed in [1]. The source code is available here `src/BisectorTree.c` and a documentation can be found here `src/BisectorTree.pdf`.

## References

>[1] Iraj Kalantari and Gerard McDonald. "A data structure and an algorithm for the nearest point problem." *IEEE Transactions on Software Engineering* p. 631–634, 1983.