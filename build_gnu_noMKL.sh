#!/bin/bash

# builds libraries for Gfortran

MKLROOT=

export FC=/usr/bin/mpif90
export CC=/usr/bin/mpicc

set -e

[[ $1 == -k ]] && CLEAN=0 || CLEAN=1

BUILDSCALAPACK=1


## Scalapack

(

[[ $BUILDSCALAPACK != 1 ]] && exit

cd scalapack/build

[[ $CLEAN == 1 ]] && make clean

cmake -Wno-dev ..

cmake --build -j . -- -l 4
)

## MUMPS
(cd MUMPS

[[ $CLEAN == 1 ]] && make clean

# no -j due to Makefile...
make s d FC=$FC FL=$FC CC=$CC \
     LSCOTCHDIR= ISCOTCH= \
     LMETISDIR= IMETIS= \
     SCALAPDIR=../../scalapack \
     SCALAP='-L$(SCALAPDIR) -lscalapack' \
     ORDERINGSF=-Dpord
)
