// $Id: VertexDiElectronMassTool.cpp,v 1.3 2009-08-18 12:48:47 witekma Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
//
#include "Event/L0CaloCandidate.h"

// local
#include "VertexDiElectronMassTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class :VertexDiElectronMassTool
//
// 2008-07-05 : Mariusz Witek
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( VertexDiElectronMassTool );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VertexDiElectronMassTool::VertexDiElectronMassTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IRecVertexFunctionTool>(this);
}
//=============================================================================
// Destructor
//=============================================================================
VertexDiElectronMassTool::~VertexDiElectronMassTool() {} 

//=============================================================================
//=============================================================================
// Initialization
//=============================================================================
StatusCode VertexDiElectronMassTool::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================

double VertexDiElectronMassTool::function(const RecVertex& vertex)
{
  const LHCb::Track& t1 = *(vertex.tracks()[0]);
  const LHCb::Track& t2 = *(vertex.tracks()[1]);

  double enerad1 =0.;
  double enerad2 =0.;

  double raddef = 0;

  int radinfo = 401;
  if ( t1.hasInfo(radinfo) ) {
    enerad1 = t1.info(radinfo, raddef);
  } else {
    warning() << " radiation corrected invariant mass called for track not corrected for radiation, no extraInfo" << endreq;
  } 
  if ( t2.hasInfo(radinfo) ) {
    enerad2 = t2.info(radinfo, raddef);
  } else {
    warning() << " radiation corrected invariant mass called for track not corrected for radiation, no extraInfo" << endreq;
  } 

  Gaudi::XYZVector pcor1 = t1.momentum() + enerad1*t1.momentum().Unit();
  Gaudi::XYZVector pcor2 = t2.momentum() + enerad2*t2.momentum().Unit();
  Gaudi::XYZVector p = pcor1+pcor2;
  double em2 = 0.00051*0.00051;
  double e = sqrt(em2+pcor1.Mag2()) + sqrt(em2+pcor2.Mag2());    
  double m2 = e*e-p.Mag2();
  double m = 0;
  if(m2>0.) m = sqrt(m2); 
  return m;
} 

