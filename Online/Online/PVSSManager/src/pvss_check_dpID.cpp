// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/DpIdentList.cpp,v 1.4 2009-02-10 19:32:35 frankb Exp $
//  ====================================================================
//  DpIdentList.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DpIdentList.cpp,v 1.4 2009-02-10 19:32:35 frankb Exp $

#define protected public
#include "PVSS/DpID.h"
// PVSS include files
#include "DpIdentifier.hxx"
#undef protected
#include <cstdio>

static inline size_t offset(void* p1, void* p2)  {
  return (((char*)p2) - ((char*)p1));
}
#define _P(obj, member) offset(&obj, &(obj.member)),sizeof(obj.member)

static inline void check_member(const char* name, size_t o1, size_t s1, size_t o2, size_t s2)  {
  if ( o1 != o2  || s1 != s2 )  {
    ::printf("Member %s does not match at  all: DpID: %-2lu Size:%lu Bytes DpIdentifier:  %-2lu Size:%lu Bytes\n",
	     name,o1,s1,o2,s2);
  }
}

namespace PVSS {
  int pvss_check_dpid()    {
    DpID id('0');
    DpIdentifier dpid;
    ::printf("%16s %-16s %16s\n","","DpID","DpIdentifier");
    ::printf("%16s Offset:%2lu Size:%2lu Bytes %4s Offset:%2lu Size:%2lu Bytes\n","System",   _P(id,systemID),"",_P(dpid,system));
    ::printf("%16s Offset:%2lu Size:%2lu Bytes %4s Offset:%2lu Size:%2lu Bytes\n","DpID",     _P(id,dpID),    "",_P(dpid,dp));
    ::printf("%16s Offset:%2lu Size:%2lu Bytes %4s Offset:%2lu Size:%2lu Bytes\n","ElementID",_P(id,elmentID),"",_P(dpid,el));
    ::printf("%16s Offset:%2lu Size:%2lu Bytes %4s Offset:%2lu Size:%2lu Bytes\n","ConfigNo", _P(id,configNo),"",_P(dpid,config));
    ::printf("%16s Offset:%2lu Size:%2lu Bytes %4s Offset:%2lu Size:%2lu Bytes\n","DetailNo", _P(id,detailNo),"",_P(dpid,detail));
    ::printf("%16s Offset:%2lu Size:%2lu Bytes %4s Offset:%2lu Size:%2lu Bytes\n","AttrNo",   _P(id,attrNo),  "",_P(dpid,attr));
    ::printf("%16s Offset:%2lu Size:%2lu Bytes %4s Offset:%2lu Size:%2lu Bytes\n","dpTypeID", _P(id,dpTypeID),"",_P(dpid,dpType));
    ::printf("%16s           Size:%2lu Bytes %4s           Size:%2lu Bytes\n","sizeof",       sizeof(id),"",sizeof(dpid));

    check_member("System",   _P(id,systemID),_P(dpid,system));
    check_member("DpID",     _P(id,dpID),    _P(dpid,dp));
    check_member("ElementID",_P(id,elmentID),_P(dpid,el));
    check_member("ConfigNo", _P(id,configNo),_P(dpid,config));
    check_member("DetailNo", _P(id,detailNo),_P(dpid,detail));
    check_member("AttrNo",   _P(id,attrNo),  _P(dpid,attr));
    check_member("dpTypeID", _P(id,dpTypeID),_P(dpid,dpType));
    return 1;
  }
}
