// $Id: HltRZVeloEcalMatch.cpp,v 1.1 2008-11-04 08:30:28 witekma Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "HltRZVeloEcalMatch.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltRZVeloEcalMatch
//
// 2006-12-08 : original from N. Zwahlen
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltRZVeloEcalMatch );

namespace {
    template <typename T> T sqrtquadsum(const T& x, const T& y) 
    { return sqrt(x*x+y*y); }
};

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltRZVeloEcalMatch::HltRZVeloEcalMatch
(const std::string& type, const std::string& name, const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrackBiFunctionTool>(this); 
  declareProperty("PtkickConstant", m_ptkickConstant = 1.263*Gaudi::Units::GeV);
  //TODO: check that this is in 'natural' Gaudi Units...
  declareProperty("zKick", m_zKick = 525.0);

}
//=============================================================================
// Destructor
//=============================================================================
HltRZVeloEcalMatch::~HltRZVeloEcalMatch() {}

//=============================================================================
double HltRZVeloEcalMatch::function(const Track& track, 
                                     const Track& tcalo)
{
  //TODO: change this into 'natural' Gaudi Units...
  const State& state = tcalo.firstState();
  x      = state.x()/Gaudi::Units::cm;
  y      = state.y()/Gaudi::Units::cm;
  z      = state.z()/Gaudi::Units::cm;   // Shower Max
  ex     = sqrt(state.errX2())/Gaudi::Units::cm;
  ey     = sqrt(state.errY2())/Gaudi::Units::cm;
  e      = tcalo.p()/Gaudi::Units::GeV;
  
  double matchChi2 = match(track);
  return matchChi2;
}

double HltRZVeloEcalMatch::match(const Track& track) 
{
  
  debug()  << "running confirmation2D()" << endreq;
  
  // for a 2D-track, tx() gives dr/dz, y() gives phi
  double trackDrDz = track.firstState().tx();
  double trackPhi  = track.firstState().y();
  double sectorSize = 0.785;  // 2pi/8 (phi takes 8 values)  
  
  // Absolute energy uncertainty:
  //  double de     = e*(sqrt( m_eres[0]*m_eres[0] + m_eres[1]*m_eres[1]/e ));
  double de = sqrt(track.firstState().errP2())/Gaudi::Units::GeV;
  
  double deltaZ = z - m_zKick;
  double xkick  = deltaZ * (m_ptkickConstant/Gaudi::Units::GeV)/e;
  double exkick = de * fabs(xkick/e);
  
  // Calculate the slopes and their uncertainties:
  
  double edxdz  = sqrtquadsum(ex,exkick)/z;
  double dydz   = y/z;
  double edydz  = ey/z;
  
  // Loop for -1 and +1 charge
  double matchChi2 = 99999.;
  for (int q=-1; q<2; q+=2){
    double dxdz  = (x - q*xkick)/z;
    double drdz  = sqrtquadsum(dxdz,dydz);
    double edrdz = sqrtquadsum(dxdz*edxdz,dydz*edydz)/ drdz;
    double phi   = atan2(dydz,dxdz);
    double ephi  = sqrtquadsum(dydz*edxdz,dxdz*edydz)/ (drdz*drdz);
    
    // Calculate the matching chi2:
    double ephiTrack = sectorSize/sqrt(12.);
    double deltaPhi  = fabs(phi - trackPhi);
    if (deltaPhi > 2.*M_PI) deltaPhi =      deltaPhi - 2.*M_PI;
    if (deltaPhi > M_PI)    deltaPhi = fabs(deltaPhi - 2.*M_PI);
    double chi2Phi   = ( deltaPhi*deltaPhi) / 
      ( (ephi*ephi) + (ephiTrack*ephiTrack) );
    
    double deltaDrDzSig = fabs(drdz - trackDrDz)/edrdz; 
    double chi2DrDz  = deltaDrDzSig*deltaDrDzSig ;
    double chi2 = chi2Phi + chi2DrDz;
    
    if (chi2 < matchChi2) {
      matchChi2      = chi2;
      debug() << "Best so far: q = " << q << "\tchi2 = " << matchChi2
              << " (" << chi2DrDz << " + " << chi2Phi << ")" << endreq;
    } // end if chi2 < matchChi2
  } // end loop over two charge hypotheses

  debug() << " matchChi2 " << matchChi2 << endreq;
  return matchChi2;
}
