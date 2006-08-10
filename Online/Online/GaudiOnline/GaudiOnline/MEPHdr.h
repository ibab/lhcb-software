#ifndef GAUDIONLINE_MEPHDR_H
#define GAUDIONLINE_MEPHDR_H
//	============================================================
//
//	MEPHdr.h
//	------------------------------------------------------------
//
//	Package   : GaudiOnline
//
//	Author    : Niko Neufeld 
//   //
//	===========================================================
#include <sys/types.h>
/* according to EDMS xxx */
namespace LHCb
{
    struct __attribute__((__packed__)) MEPHdr {
	u_int32_t m_l0ID;
	u_int16_t m_nEvt;
	u_int16_t m_totLen;
	u_int32_t m_partitionID;
    };
    struct __attribute__((__packed__)) MEPFrgHdr {
	u_int16_t m_l0IDlow;
	u_int16_t m_len;
    };
}
#endif /* GAUDIONLINE_MEPHDR_H */
