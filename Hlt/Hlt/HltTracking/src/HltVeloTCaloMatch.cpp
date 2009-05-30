// $Id: HltVeloTCaloMatch.cpp,v 1.1 2009-05-30 12:40:21 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "HltVeloTCaloMatch.h"
#include "HltBase/HltUtils.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltVeloTCaloMatch
//
// 2006-12-08 : original from N. Zwahlen
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltVeloTCaloMatch );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltVeloTCaloMatch::HltVeloTCaloMatch
(const std::string& type, const std::string& name, const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrackMatch>(this);
  declareInterface<ITrackBiFunctionTool>(this);
  declareInterface<ITrackL0CaloMatchTool>(this);
  declareProperty("PtkickConstant", m_ptkickConstant = 1.263*Gaudi::Units::GeV);
  declareProperty("zKick", m_zKick   = 525.0);
  declareProperty("eRes0", m_eres[0] = 0.60 );
  declareProperty("eRes1", m_eres[1] = 0.70 );

  declareProperty("Rematch2D",m_doRematch2D = true);
  declareProperty("Max3DChi2",m_max3DChi2 = 10.);
  declareProperty("Max2DChi2",m_max2DChi2 = 10.);  

  m_rematchTool = 0;

}
//=============================================================================
// Destructor
//=============================================================================
HltVeloTCaloMatch::~HltVeloTCaloMatch() {} 

StatusCode HltVeloTCaloMatch::initialize() {

  StatusCode sc = GaudiTool::initialize();
  
  if (m_doRematch2D) {
    m_rematchTool = tool<ITrackBiFunctionTool>("HltRZVeloTCaloMatch");
    Assert(m_rematchTool," Not able tocreate rematch 2D-TCalo tool");
  }
  
  return sc;
}

double HltVeloTCaloMatch::function(const Track& tvelo, const Track& tcalo) {

  const State& state = tcalo.firstState();
  x      = state.x()/Gaudi::Units::cm;
  y      = state.y()/Gaudi::Units::cm;
  z      = state.z()/Gaudi::Units::cm;   // Shower Max
  // trick!!
  ex     = state.tx()/Gaudi::Units::cm;
  ey     = state.ty()/Gaudi::Units::cm;
  e      = tcalo.pt()/Gaudi::Units::GeV;
  
  double matchChi2 = match(tvelo);
  return matchChi2;
}



//=============================================================================
StatusCode HltVeloTCaloMatch::match(const Track& tvelo, 
                                    const Track& tcalo,
                                    Track& otrack,
                                    double& matchChi2,
                                    double& chi2) 
{
  matchChi2 = function(tvelo,tcalo);

  double ok = (matchChi2 < m_max3DChi2);
  
  debug() << " match 3D Chi2 "<< matchChi2 << " accepted? " << ok << endreq;
  if (!ok) return StatusCode::FAILURE;
  
  // check rematch
  if (m_doRematch2D) {
    const LHCb::Track& rzvelo = *(tvelo.ancestors()[0]);
    chi2 = m_rematchTool->function(rzvelo,tcalo);
    bool ok2 = (chi2 < m_max2DChi2);
    debug() << " rematch 2D Chi2 " << chi2 << " accepted? " << ok2 << endreq;
    if (!ok2) return StatusCode::FAILURE;
  }
  
  // merge tracks
  Hlt::TrackMerge(tvelo,otrack);
  Hlt::TrackMerge(tcalo,otrack);
  otrack.setType(tvelo.type());
  
  debug() << " match track slopes " << otrack.slopes() 
          << " quality " << matchChi2 << ", " << chi2 << endreq;
  
  return StatusCode::SUCCESS;
}

double HltVeloTCaloMatch::match(const Track& track, 
                                const L0CaloCandidate& calo)
{  

  static const double s = double(4)/sqrt(double(12));
  // Get energy and position of L0 calo candidate:
  x      = calo.position().x()/Gaudi::Units::cm;
  y      = calo.position().y()/Gaudi::Units::cm;
  z      = calo.position().z()/Gaudi::Units::cm;   // Shower Max
  ex     = calo.posTol()*s/Gaudi::Units::cm;
  ey     = calo.posTol()*s/Gaudi::Units::cm;
  double et     = calo.et()/Gaudi::Units::GeV;
  
  e      = fabs(et) *( sqrt(x*x + y*y + z*z)/
                       sqrt(x*x + y*y));
  return match(track);
  
}

double HltVeloTCaloMatch::match(const Track& track) 
{
  double matchChi2 = 999.;
  
  debug() << "running confirmation3D()" << endreq;

  // get track slopes
  double trackDxDz = track.firstState().tx();
  double trackDyDz = track.firstState().ty();
  
  // Absolute energy uncertainty:
  double de = e*(sqrt( m_eres[0]*m_eres[0] + m_eres[1]*m_eres[1]/e ));
  
  double deltaZ = z - m_zKick;
  double xkick  = deltaZ * (m_ptkickConstant/Gaudi::Units::GeV)/e;
  double exkick = fabs( xkick/e)*de;
  
  // Calculate the slopes and their uncertainties:
  double edxdz  = sqrt(ex*ex + exkick*exkick)/z;
  double dydz   = y/z;
  double edydz  = ey/z;
  
  // loop for -1 and +1 charge
  double q[2]={-1.,1.};
  for (int i= 0; i< 2; ++i) {
    double dxdz = (x + q[i]*xkick)/z;
    
    // calculate chi2 
    double deltaX = q[i]*(dxdz - trackDxDz)/edxdz;
    double deltaY = (dydz/fabs(dydz))*(dydz - trackDyDz)/edydz;
    double chi2 = deltaX*deltaX + deltaY*deltaY;
    
    if (chi2 < matchChi2) {
      matchChi2      = chi2;
      debug() << "Best so far: q = " << q[i] << "\tchi2 = " << matchChi2
              << " (" << deltaX*deltaX << " + " << deltaY*deltaY << ")" << endreq;
    } // end if chi2 < matchChi2
  } // end loop for -1 and +1 charge
  
  debug() << " matchChi2 " << matchChi2 << endreq;
  return matchChi2;
}
