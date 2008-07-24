// $Id: L0MuonCandHistos.cpp,v 1.1 2008-07-24 09:36:53 jucogan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "L0MuonCandHistos.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonCandHistos
//
// 2008-07-23 : Julien Cogan
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0MuonCandHistos );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonCandHistos::L0MuonCandHistos( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiHistoTool ( type, name , parent )
{
  declareInterface<L0MuonCandHistos>(this);

}
//=============================================================================
// Destructor
//=============================================================================
L0MuonCandHistos::~L0MuonCandHistos() {} 

//=============================================================================

void L0MuonCandHistos::bookHistos(bool shortname)
{
}

void L0MuonCandHistos::fillHistos(LHCb::L0MuonCandidates* cands, int ts)
{
  LHCb::L0MuonCandidates::const_iterator itcand;

  for ( itcand= cands->begin(); itcand<cands->end();++itcand) {
    LHCb::MuonTileID seed = (*itcand)->muonTileIDs(2)[0];
  }
  
}
