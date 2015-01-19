// $Header: $
// ====================================================================
//
// --------------------------------------------------------------------
//
// Author    : Markus Frank
//
// ====================================================================
#ifndef  NET_IPHEADER_H
#define  NET_IPHEADER_H

#include "RTL/types.h"

#ifdef _WIN32
#pragma pack(push,IPHDR,1)
#define IPHEADER_ALIGNED(x) x
#else
#define IPHEADER_ALIGNED(x) x __attribute__((__packed__))
#endif



namespace RTL {

  /// This structure encapsulates the various data fields of a IP packet header
  /**
    */
  IPHEADER_ALIGNED(struct) IPHeader {
    u_int8_t  hlv;
    u_int8_t  tos;
    u_int16_t tot_len;
    u_int16_t id;
    u_int16_t frag_off;
    u_int8_t  ttl;
    u_int8_t  protocol;
    u_int16_t check;
    u_int32_t saddr;
    u_int32_t daddr;
    /*The options start here. */
  };
}

#undef IPHEADER_ALIGNED
#ifdef _WIN32
#pragma pack(pop, IPHDR)
#endif

#endif // NET_IPHEADER_H
