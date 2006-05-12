#ifndef _MEPHDR_H
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

namespace LHCb
{
    struct __attribute__((__packed__)) MEPHdr {
	u_int32_t m_l0ID;
	u_int16_t m_totLen;
	u_int16_t m_nEvt;
	u_int32_t m_partitionID;
    };
    struct __attribute__((__packed__)) MEPFrgHdr {
	u_int16_t m_l0IDlow;
	u_int16_t m_len;
    };
}
#endif /* _MEPHDR_H */
