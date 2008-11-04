// $Id: HltVeloEcalMatch.cpp,v 1.1 2008-11-04 08:30:28 witekma Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "HltVeloEcalMatch.h"
#include "HltBase/HltUtils.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltVeloEcalMatch
//
// 2006-12-08 : original from N. Zwahlen
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltVeloEcalMatch );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltVeloEcalMatch::HltVeloEcalMatch
(const std::string& type, const std::string& name, const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrackBiFunctionTool>(this);
  declareProperty("PtkickConstant", m_ptkickConstant = 1.263*Gaudi::Units::GeV);
  declareProperty("zKick", m_zKick   = 525.0);
  declareProperty("Max3DChi2",m_max3DChi2 = 16.);

}
//=============================================================================
// Destructor
//=============================================================================
HltVeloEcalMatch::~HltVeloEcalMatch() {} 

StatusCode HltVeloEcalMatch::initialize() {

  StatusCode sc = GaudiTool::initialize();
  
  return sc;
}

double HltVeloEcalMatch::function(const Track& tvelo, const Track& tcalo) {

  const State& state = tcalo.firstState();
  x      = state.x()/Gaudi::Units::cm;
  y      = state.y()/Gaudi::Units::cm;
  z      = state.z()/Gaudi::Units::cm;   // Shower Max
  ex     = sqrt(state.errX2())/Gaudi::Units::cm;
  ey     = sqrt(state.errY2())/Gaudi::Units::cm;
  e      = tcalo.p()/Gaudi::Units::GeV;
  
  double matchChi2 = match(tvelo);
  return matchChi2;
}

double HltVeloEcalMatch::match(const Track& track) 
{
  double matchChi2 = 999.;
  
  debug() << "running confirmation3D()" << endreq;

  // get track slopes
  double trackDxDz = track.firstState().tx();
  double trackDyDz = track.firstState().ty();
  
  // Absolute energy uncertainty:
  //  double de = e*(sqrt( m_eres[0]*m_eres[0] + m_eres[1]*m_eres[1]/e ));
  double de = sqrt(track.firstState().errP2())/Gaudi::Units::GeV;
  
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
