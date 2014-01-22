// $Id: HltVeloEcalMatch.cpp,v 1.2 2009-10-07 06:38:33 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "HltVeloEcalMatch.h"

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
  declareProperty("eRes0", m_eres[0] = 0.60 );
  declareProperty("eRes1", m_eres[1] = 0.70 );
  declareProperty("eCorrect", m_eCorrect = 1.2 );

  declareProperty("Max3DChi2",m_max3DChi2 = 10.);

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

  double cell_size = ecal_cell_size(tcalo);

  const State& state = tcalo.firstState();
  x      = state.x()/Gaudi::Units::cm;
  y      = state.y()/Gaudi::Units::cm;
  z      = state.z()/Gaudi::Units::cm;   // Shower Max
  ex     = 2.*cell_size/Gaudi::Units::cm;
  ey     = ex;
  e      = tcalo.pt()/Gaudi::Units::GeV;
  
  e *= m_eCorrect;

  double matchChi2 = match(tvelo);
  return matchChi2;
}

double HltVeloEcalMatch::match(const Track& track) 
{
  double matchChi2 = 999.;
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "running confirmation3D()" << endmsg;

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
      if ( msgLevel(MSG::DEBUG) ) debug() << "Best so far: q = " << q[i] 
                                          << "\tchi2 = " << matchChi2
                                          << " (" << deltaX*deltaX << " + " 
                                          << deltaY*deltaY << ")" << endmsg;
    } // end if chi2 < matchChi2
  } // end loop for -1 and +1 charge
  
  if ( msgLevel(MSG::DEBUG) ) debug() << " matchChi2 " << matchChi2 << endmsg;
  return matchChi2;
}


double HltVeloEcalMatch::ecal_cell_size(const Track& ctrack) {

  static const double cellSizeECal[3] = { 121.7, 60.85, 40.56 };
    // get LHCbID of coresponding L0CaloCandidate
  int ids_ecal = 0;
  double siz_ecal =0.;
  for (const auto& id : ctrack.lhcbIDs() ) {
    if( ! id.isCalo() ) continue;
    LHCb::CaloCellID cid = id.caloID();
    if ( cid.calo() != 2 ) continue;
    int area = cid.area();
    if (area < 0 || area > 2 ) continue;
    ids_ecal++;
    siz_ecal += cellSizeECal[area];
  }  

  double cs = cellSizeECal[0];
  if ( ids_ecal > 0 ) {
    cs = siz_ecal/ids_ecal;
  }
  return cs;
}
