// $Id: HltDeltaPTool.cpp,v 1.3 2009-05-30 19:43:58 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

//boost
#include <boost/assign/list_of.hpp>

// local
#include "HltDeltaPTool.h"
#include "Event/MuonCoord.h"

//-----------------------------------------------------------------------------
// Implementation file for class :HltDeltaPTool
//
// 2008-09-25 : Leandro de Paula
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltDeltaPTool );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltDeltaPTool::HltDeltaPTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  // DeltaP parameters
  declareProperty( "ptrackcut", m_pCut = boost::assign::list_of (60000.)(30000.)(15000.)(7500.)(-2.));
  declareInterface<ITrackFunctionTool>(this);
}
//=============================================================================
// Destructor
//=============================================================================
HltDeltaPTool::~HltDeltaPTool() {} 

//=============================================================================
//=============================================================================
// Initialization
//=============================================================================
StatusCode HltDeltaPTool::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (msgLevel (MSG::DEBUG)) debug() << "==> Initialize" << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================

double  HltDeltaPTool::function(const Track& track)
{
  
  std::vector< LHCb::LHCbID >  lista = track.lhcbIDs ();
  MuonTileID tile;
  std::vector<LHCbID>::iterator it; 
  float ptrack, pancestor, dpop;
  
  ptrack=track.p();
  // looping over lhcbIDs
  for(it=lista.end()-1;it>=lista.begin();it--){
    if(it->isMuon()){
      MuonTileID tile=it->muonID();
      const SmartRefVector<Track>& ancestors = track.ancestors();
      if (msgLevel (MSG::DEBUG)) debug() << " # of ancestors = " << ancestors.size() << endreq;
      if (ancestors.size() == 0) return 0;
      pancestor=(*ancestors[0]).p();
      if(pancestor > 0){
        // Checking dP/P
        dpop = (ptrack-pancestor)/ptrack;
        if (msgLevel (MSG::DEBUG)) debug() << " pTt = " << ptrack << "  pancestor = " << pancestor << " dpop = " << dpop << endreq;
        if(dpop < m_pCut[4]){
          int region = tile.region();
          if(ptrack < m_pCut[region]) return 0;;
        }
        return 1;
      }
    }
  }  
  return 0;
} 
