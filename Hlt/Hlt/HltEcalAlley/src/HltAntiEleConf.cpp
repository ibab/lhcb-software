// $Id: HltAntiEleConf.cpp,v 1.1 2008-12-13 13:01:58 witekma Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "HltAntiEleConf.h"

//-----------------------------------------------------------------------------
// Implementation file for class :HltAntiEleConf
//
// 2008-07-05 : Mariusz Witek
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltAntiEleConf );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltAntiEleConf::HltAntiEleConf( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrackFunctionTool>(this);
}
//=============================================================================
// Destructor
//=============================================================================
HltAntiEleConf::~HltAntiEleConf() {} 

//=============================================================================
//=============================================================================
// Initialization
//=============================================================================
StatusCode HltAntiEleConf::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_tool = tool<ITracksFromTrack>("L0ConfirmWithT",this);
  return StatusCode::SUCCESS;
}

StatusCode HltAntiEleConf::finalize() {
  StatusCode sc = GaudiTool::finalize(); // must be executed first
  return sc;
}
//=============================================================================

double HltAntiEleConf::function(const Track& ctrack)
{

  std::vector<LHCb::Track*> tracks;
  StatusCode sc = m_tool->tracksFromTrack(ctrack,tracks);
  if (sc.isFailure()) return 0.; 

  // anti confirmation. If T confirmed then return 0. If not return 1.
  if(tracks.size()<1) return 1.; 
  return 0.;

} 


