/* -*- Mode: C ; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 *  (C) 2006 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 *
 *  Portions of this code were written by Intel Corporation.
 *  Copyright (C) 2011-2016 Intel Corporation.  Intel provides this material
 *  to Argonne National Laboratory subject to Software Grant and Corporate
 *  Contributor License Agreement dated February 8, 2012.
 */
#ifndef CH4_COLL_SELECT_H_INCLUDED
#define CH4_COLL_SELECT_H_INCLUDED

#include "ch4_impl.h"
#include "ch4r_proc.h"
#include "ch4_coll_impl.h"

#ifndef MPIDI_CH4_DIRECT_NETMOD
#include "../shm/include/shm.h"
#endif

MPL_STATIC_INLINE_PREFIX const
MPIDI_coll_algo_container_t *MPIDI_Barrier_select(MPIR_Comm * comm, MPIR_Errflag_t * errflag)
{
    if (comm->comm_kind == MPIR_COMM_KIND__INTERCOMM) {
        return &MPIDI_empty_cnt;
    }

    if (MPIR_Comm_is_parent_comm(comm)) {
        return &MPIDI_Barrier_intra_composition_alpha_cnt;
    }

    return &MPIDI_Barrier_intra_composition_beta_cnt;
}

MPL_STATIC_INLINE_PREFIX const
MPIDI_coll_algo_container_t *MPIDI_Bcast_select(void *buffer,
                                                int count,
                                                MPI_Datatype datatype,
                                                int root,
                                                MPIR_Comm * comm, MPIR_Errflag_t * errflag)
{
    int nbytes = 0;
    MPI_Aint type_size = 0;

    MPIR_Datatype_get_size_macro(datatype, type_size);

    if (comm->comm_kind == MPIR_COMM_KIND__INTERCOMM) {
        return &MPIDI_empty_cnt;
    }

    nbytes = MPIR_CVAR_MAX_SMP_BCAST_MSG_SIZE ? type_size * count : 0;
    if (nbytes <= MPIR_CVAR_MAX_SMP_BCAST_MSG_SIZE && MPIR_Comm_is_parent_comm(comm)) {
        if ((nbytes < MPIR_CVAR_BCAST_SHORT_MSG_SIZE) ||
            (comm->local_size < MPIR_CVAR_BCAST_MIN_PROCS)) {
            return &MPIDI_Bcast_intra_composition_alpha_cnt;
        } else {
            if (nbytes < MPIR_CVAR_BCAST_LONG_MSG_SIZE && MPL_is_pof2(comm->local_size, NULL)) {
                return &MPIDI_Bcast_intra_composition_beta_cnt;
            }
        }
    }
    return &MPIDI_Bcast_intra_composition_gamma_cnt;
}

MPL_STATIC_INLINE_PREFIX const
MPIDI_coll_algo_container_t *MPIDI_Allreduce_select(const void *sendbuf,
                                                    void *recvbuf,
                                                    int count,
                                                    MPI_Datatype
                                                    datatype,
                                                    MPI_Op op,
                                                    MPIR_Comm * comm, MPIR_Errflag_t * errflag)
{
    MPI_Aint type_size = 0;
    int nbytes = 0;
    int is_commutative = -1;

    /* is the op commutative? We do SMP optimizations only if it is. */
    is_commutative = MPIR_Op_is_commutative(op);
    MPIR_Datatype_get_size_macro(datatype, type_size);

    if (comm->comm_kind == MPIR_COMM_KIND__INTERCOMM) {
        return &MPIDI_empty_cnt;
    }
#ifndef MPIDI_CH4_DIRECT_NETMOD
    if (comm->node_comm != NULL && MPIR_Comm_size(comm) == MPIR_Comm_size(comm->node_comm)) {
        /* All the ranks in comm are on the same node */
        if (!((count * type_size) > MPIR_CVAR_MAX_POSIX_RELEASE_GATHER_ALLREDUCE_MSG_SIZE &&
              MPIR_CVAR_ENUM_IS(BCAST_POSIX_INTRA_ALGORITHM, release_gather) &&
              MPIR_CVAR_ENUM_IS(REDUCE_POSIX_INTRA_ALGORITHM, release_gather))) {
            /* gamma is selected if the msg size is less than threshold or release_gather based
             * shm_bcast and shm_reduce is not chosen for msg sizes larger than threshold */
            return &MPIDI_Allreduce_intra_composition_gamma_cnt;
        }
    }
#endif

    nbytes = MPIR_CVAR_MAX_SMP_ALLREDUCE_MSG_SIZE ? type_size * count : 0;
    if (MPIR_Comm_is_parent_comm(comm) && is_commutative &&
        nbytes <= MPIR_CVAR_MAX_SMP_ALLREDUCE_MSG_SIZE) {
        return &MPIDI_Allreduce_intra_composition_alpha_cnt;
    }
    return &MPIDI_Allreduce_intra_composition_beta_cnt;
}

MPL_STATIC_INLINE_PREFIX const
MPIDI_coll_algo_container_t *MPIDI_Reduce_select(const void *sendbuf,
                                                 void *recvbuf,
                                                 int count,
                                                 MPI_Datatype datatype,
                                                 MPI_Op op, int root,
                                                 MPIR_Comm * comm, MPIR_Errflag_t * errflag)
{
    int is_commutative = -1;
    MPI_Aint type_size = 0;
    int nbytes = 0;

    if (comm->comm_kind == MPIR_COMM_KIND__INTERCOMM) {
        return &MPIDI_empty_cnt;
    }

    /* is the op commutative? We do SMP optimizations only if it is. */
    is_commutative = MPIR_Op_is_commutative(op);

    MPIR_Datatype_get_size_macro(datatype, type_size);
    nbytes = MPIR_CVAR_MAX_SMP_REDUCE_MSG_SIZE ? type_size * count : 0;
    if (MPIR_Comm_is_parent_comm(comm) && is_commutative &&
        nbytes <= MPIR_CVAR_MAX_SMP_REDUCE_MSG_SIZE) {
        if (nbytes <= MPIR_CVAR_REDUCE_SHORT_MSG_SIZE) {
            return &MPIDI_Reduce_intra_composition_beta_cnt;
        } else {
            return &MPIDI_Reduce_intra_composition_alpha_cnt;
        }
    }
    return &MPIDI_Reduce_intra_composition_gamma_cnt;
}

MPL_STATIC_INLINE_PREFIX const
MPIDI_coll_algo_container_t *MPIDI_Gather_select(const void *sendbuf,
                                                 int sendcount,
                                                 MPI_Datatype sendtype,
                                                 void *recvbuf,
                                                 int recvcount,
                                                 MPI_Datatype recvtype,
                                                 int root,
                                                 MPIR_Comm * comm, MPIR_Errflag_t * errflag)
{
    if (comm->comm_kind == MPIR_COMM_KIND__INTERCOMM) {
        return &MPIDI_empty_cnt;
    }

    return &MPIDI_Gather_intra_composition_alpha_cnt;
}

MPL_STATIC_INLINE_PREFIX const
MPIDI_coll_algo_container_t *MPIDI_Gatherv_select(const void *sendbuf,
                                                  int sendcount,
                                                  MPI_Datatype sendtype,
                                                  void *recvbuf,
                                                  const int *recvcounts,
                                                  const int *displs,
                                                  MPI_Datatype recvtype,
                                                  int root,
                                                  MPIR_Comm * comm, MPIR_Errflag_t * errflag)
{
    if (comm->comm_kind == MPIR_COMM_KIND__INTERCOMM) {
        return &MPIDI_empty_cnt;
    }

    return &MPIDI_Gatherv_intra_composition_alpha_cnt;
}

MPL_STATIC_INLINE_PREFIX const
MPIDI_coll_algo_container_t *MPIDI_Scatter_select(const void *sendbuf,
                                                  int sendcount,
                                                  MPI_Datatype sendtype,
                                                  void *recvbuf,
                                                  int recvcount,
                                                  MPI_Datatype recvtype,
                                                  int root,
                                                  MPIR_Comm * comm, MPIR_Errflag_t * errflag)
{

    if (comm->comm_kind == MPIR_COMM_KIND__INTERCOMM) {
        return &MPIDI_empty_cnt;
    }

    return &MPIDI_Scatter_intra_composition_alpha_cnt;
}

MPL_STATIC_INLINE_PREFIX const
MPIDI_coll_algo_container_t *MPIDI_Scatterv_select(const void *sendbuf,
                                                   const int *sendcounts,
                                                   const int *displs,
                                                   MPI_Datatype sendtype,
                                                   void *recvbuf,
                                                   int recvcount,
                                                   MPI_Datatype recvtype,
                                                   int root,
                                                   MPIR_Comm * comm, MPIR_Errflag_t * errflag)
{

    if (comm->comm_kind == MPIR_COMM_KIND__INTERCOMM) {
        return &MPIDI_empty_cnt;
    }

    return &MPIDI_Scatterv_intra_composition_alpha_cnt;
}

MPL_STATIC_INLINE_PREFIX const
MPIDI_coll_algo_container_t *MPIDI_Alltoall_select(const void *sendbuf,
                                                   int sendcount,
                                                   MPI_Datatype sendtype,
                                                   void *recvbuf,
                                                   int recvcount,
                                                   MPI_Datatype recvtype,
                                                   MPIR_Comm * comm, MPIR_Errflag_t * errflag)
{
    if (comm->comm_kind == MPIR_COMM_KIND__INTERCOMM) {
        return &MPIDI_empty_cnt;
    }

    return &MPIDI_Alltoall_intra_composition_alpha_cnt;
}

MPL_STATIC_INLINE_PREFIX const
MPIDI_coll_algo_container_t *MPIDI_Alltoallv_select(const void *sendbuf,
                                                    const int *sendcounts,
                                                    const int *sdispls,
                                                    MPI_Datatype sendtype, void *recvbuf,
                                                    const int *recvcounts,
                                                    const int *rdispls,
                                                    MPI_Datatype recvtype,
                                                    MPIR_Comm * comm, MPIR_Errflag_t * errflag)
{
    if (comm->comm_kind == MPIR_COMM_KIND__INTERCOMM) {
        return &MPIDI_empty_cnt;
    }

    return &MPIDI_Alltoallv_intra_composition_alpha_cnt;
}

MPL_STATIC_INLINE_PREFIX const
MPIDI_coll_algo_container_t *MPIDI_Alltoallw_select(const void *sendbuf,
                                                    const int sendcounts[],
                                                    const int sdispls[],
                                                    const MPI_Datatype sendtypes[],
                                                    void *recvbuf, const int recvcounts[],
                                                    const int rdispls[],
                                                    const MPI_Datatype recvtypes[],
                                                    MPIR_Comm * comm, MPIR_Errflag_t * errflag)
{
    if (comm->comm_kind == MPIR_COMM_KIND__INTERCOMM) {
        return &MPIDI_empty_cnt;
    }

    return &MPIDI_Alltoallw_intra_composition_alpha_cnt;
}

MPL_STATIC_INLINE_PREFIX const
MPIDI_coll_algo_container_t *MPIDI_Allgather_select(const void *sendbuf, int sendcount,
                                                    MPI_Datatype sendtype, void *recvbuf,
                                                    int recvcount, MPI_Datatype recvtype,
                                                    MPIR_Comm * comm, MPIR_Errflag_t * errflag)
{
    if (comm->comm_kind == MPIR_COMM_KIND__INTERCOMM) {
        return &MPIDI_empty_cnt;
    }

    return &MPIDI_Allgather_intra_composition_alpha_cnt;
}

MPL_STATIC_INLINE_PREFIX const
MPIDI_coll_algo_container_t *MPIDI_Allgatherv_select(const void *sendbuf, int sendcount,
                                                     MPI_Datatype sendtype, void *recvbuf,
                                                     const int *recvcounts,
                                                     const int *displs,
                                                     MPI_Datatype recvtype,
                                                     MPIR_Comm * comm, MPIR_Errflag_t * errflag)
{
    if (comm->comm_kind == MPIR_COMM_KIND__INTERCOMM) {
        return &MPIDI_empty_cnt;
    }

    return &MPIDI_Allgatherv_intra_composition_alpha_cnt;
}

MPL_STATIC_INLINE_PREFIX const
MPIDI_coll_algo_container_t *MPIDI_Reduce_scatter_select(const void *sendbuf,
                                                         void *recvbuf,
                                                         const int recvcounts[],
                                                         MPI_Datatype datatype,
                                                         MPI_Op op, MPIR_Comm * comm,
                                                         MPIR_Errflag_t * errflag)
{

    if (comm->comm_kind == MPIR_COMM_KIND__INTERCOMM) {
        return &MPIDI_empty_cnt;
    }

    return &MPIDI_Reduce_scatter_intra_composition_alpha_cnt;
}

MPL_STATIC_INLINE_PREFIX const
MPIDI_coll_algo_container_t *MPIDI_Reduce_scatter_block_select(const void *sendbuf,
                                                               void *recvbuf,
                                                               int recvcount,
                                                               MPI_Datatype datatype,
                                                               MPI_Op op,
                                                               MPIR_Comm * comm,
                                                               MPIR_Errflag_t * errflag)
{

    if (comm->comm_kind == MPIR_COMM_KIND__INTERCOMM) {
        return &MPIDI_empty_cnt;
    }

    return &MPIDI_Reduce_scatter_block_intra_composition_alpha_cnt;
}

MPL_STATIC_INLINE_PREFIX const
MPIDI_coll_algo_container_t *MPIDI_Scan_select(const void *sendbuf,
                                               void *recvbuf,
                                               int count,
                                               MPI_Datatype datatype,
                                               MPI_Op op, MPIR_Comm * comm,
                                               MPIR_Errflag_t * errflag)
{
    if (MPII_Comm_is_node_consecutive(comm)) {
        return &MPIDI_Scan_intra_composition_alpha_cnt;
    }

    return &MPIDI_Scan_intra_composition_beta_cnt;
}

MPL_STATIC_INLINE_PREFIX const
MPIDI_coll_algo_container_t *MPIDI_Exscan_select(const void *sendbuf,
                                                 void *recvbuf,
                                                 int count,
                                                 MPI_Datatype datatype,
                                                 MPI_Op op, MPIR_Comm * comm,
                                                 MPIR_Errflag_t * errflag)
{
    return &MPIDI_Exscan_intra_composition_alpha_cnt;
}

#endif /* CH4_COLL_SELECT_H_INCLUDED */
