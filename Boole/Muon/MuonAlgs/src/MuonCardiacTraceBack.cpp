//local
#include "MuonCardiacTraceBack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonHitTraceBack
//                                 derived from class ContainedObject
//
// 2003-01-21 : Alessia Satta
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonCardiacTraceBack::MuonCardiacTraceBack(  ) {
int i=0;
}

MuonCardiacTraceBack::MuonCardiacTraceBack(MuonHitTraceBack*
value ){
    m_trackeBack.first=value;
    m_trackeBack.second=true;
}; 


