/*
 *
 *  This file is part of MUMPS 5.3.3, released
 *  on Mon Jun 15 09:57:25 UTC 2020
 *
 *
 *  Copyright 1991-2020 CERFACS, CNRS, ENS Lyon, INP Toulouse, Inria,
 *  Mumps Technologies, University of Bordeaux.
 *
 *  This version of MUMPS is provided to you free of charge. It is
 *  released under the CeCILL-C license:
 *  https://cecill.info/licences/Licence_CeCILL-C_V1-en.html
 *
 */
#ifndef MUMPS_METIS_INT_H
#define MUMPS_METIS_INT_H
#include "mumps_common.h" /* includes mumps_compat.h and mumps_c_types.h */
#define MUMPS_METIS_IDXSIZE \
  F_SYMBOL(metis_idxsize,METIS_IDXSIZE)
void MUMPS_CALL
MUMPS_METIS_IDXSIZE(MUMPS_INT *metis_idx_size);
#endif
