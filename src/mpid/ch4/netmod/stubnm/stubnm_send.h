/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 *  (C) 2006 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 *
 *  Portions of this code were written by Intel Corporation.
 *  Copyright (C) 2011-2016 Intel Corporation.  Intel provides this material
 *  to Argonne National Laboratory subject to Software Grant and Corporate
 *  Contributor License Agreement dated February 8, 2012.
 */
#ifndef STUBNM_SEND_H_INCLUDED
#define STUBNM_SEND_H_INCLUDED

#include "stubnm_impl.h"

static inline int MPIDI_NM_mpi_send(const void *buf,
                                    MPI_Aint count,
                                    MPI_Datatype datatype,
                                    int rank,
                                    int tag,
                                    MPIR_Comm * comm, int context_offset,
                                    MPIDI_av_entry_t * addr, MPIR_Request ** request)
{
    return MPIDIG_mpi_send(buf, count, datatype, rank, tag, comm, context_offset, addr, request);
}

static inline int MPIDI_NM_send_coll(const void *buf, MPI_Aint count, MPI_Datatype datatype,
                                     int rank, int tag, MPIR_Comm * comm, int context_offset,
                                     MPIDI_av_entry_t * addr, MPIR_Request ** request,
                                     MPIR_Errflag_t * errflag)
{
    return MPIDIG_send_coll(buf, count, datatype, rank, tag, comm, context_offset, addr, request,
                            errflag);
}

static inline int MPIDI_NM_mpi_ssend(const void *buf,
                                     MPI_Aint count,
                                     MPI_Datatype datatype,
                                     int rank,
                                     int tag,
                                     MPIR_Comm * comm, int context_offset,
                                     MPIDI_av_entry_t * addr, MPIR_Request ** request)
{
    return MPIDIG_mpi_ssend(buf, count, datatype, rank, tag, comm, context_offset, addr, request);
}

static inline int MPIDI_NM_mpi_isend(const void *buf,
                                     MPI_Aint count,
                                     MPI_Datatype datatype,
                                     int rank,
                                     int tag,
                                     MPIR_Comm * comm, int context_offset,
                                     MPIDI_av_entry_t * addr, MPIR_Request ** request)
{
    return MPIDIG_mpi_isend(buf, count, datatype, rank, tag, comm, context_offset, addr, request);
}

static inline int MPIDI_NM_isend_coll(const void *buf,
                                      MPI_Aint count,
                                      MPI_Datatype datatype,
                                      int rank,
                                      int tag,
                                      MPIR_Comm * comm, int context_offset,
                                      MPIDI_av_entry_t * addr, MPIR_Request ** request,
                                      MPIR_Errflag_t * errflag)
{
    return MPIDIG_isend_coll(buf, count, datatype, rank, tag, comm, context_offset, addr, request,
                             errflag);
}

static inline int MPIDI_NM_mpi_issend(const void *buf,
                                      MPI_Aint count,
                                      MPI_Datatype datatype,
                                      int rank,
                                      int tag,
                                      MPIR_Comm * comm, int context_offset,
                                      MPIDI_av_entry_t * addr, MPIR_Request ** request)
{
    return MPIDIG_mpi_issend(buf, count, datatype, rank, tag, comm, context_offset, addr, request);
}

static inline int MPIDI_NM_mpi_cancel_send(MPIR_Request * sreq)
{
    return MPIDIG_mpi_cancel_send(sreq);
}

#endif /* STUBNM_SEND_H_INCLUDED */
