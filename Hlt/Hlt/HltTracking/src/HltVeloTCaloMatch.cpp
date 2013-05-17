// $Id: HltVeloTCaloMatch.cpp,v 1.5 2010-05-27 22:05:17 gligorov Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "HltVeloTCaloMatch.h"
#include "HltBase/HltTrackMerge.h"

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
  declareProperty("PtkickConstant", m_ptkickConstant = (1.263/1.2)*Gaudi::Units::GeV);
  declareProperty("zKick", m_zKick   = 525.0);
  declareProperty("eRes0", m_eres[0] = 0.10 );
  declareProperty("eRes1", m_eres[1] = 0.70 );

  declareProperty("Max3DChi2",m_maxYChi2 = 100.);
  declareProperty("Max2DChi2",m_maxXChi2 = 100.);  

}
//=============================================================================
// Destructor
//=============================================================================
HltVeloTCaloMatch::~HltVeloTCaloMatch() {} 

StatusCode HltVeloTCaloMatch::initialize() {

  StatusCode sc = GaudiTool::initialize();
  
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
  
  double chi2 = match(tvelo,1);
  return chi2;
}



//=============================================================================
StatusCode HltVeloTCaloMatch::match(const Track& tvelo, 
                                    const Track& tcalo,
                                    Track& otrack,
                                    double& Chi2Y,
                                    double& Chi2X) 
{
  Chi2Y = function(tvelo,tcalo); //Chi2 in y
  Chi2X = match(tvelo,2); //Chi2 in x

  bool ok = ((Chi2Y < m_maxYChi2)&&(Chi2X < m_maxXChi2));
  
  if ( msgLevel(MSG::DEBUG) ) debug() << " match Chi2 "<< Chi2Y << " " << Chi2X << " accepted? " << ok << endmsg;
  if (!ok) return StatusCode::FAILURE;
  
  // merge tracks
  Hlt::TrackMerge(tvelo,otrack);
  Hlt::TrackMerge(tcalo,otrack);
  otrack.setType(tvelo.type());
  
  if ( msgLevel(MSG::DEBUG) ) debug() << " match track slopes " << otrack.slopes() 
          << " quality " << Chi2Y << ", " << Chi2X << endmsg;
  
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
  return match(track,1)+match(track,2);
  
}

double HltVeloTCaloMatch::match(const Track& track, int matchtype) 
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "running confirmation3D()" << endmsg;

  //First the y Chi2X 
  const std::vector<State*> TSV = track.states();
  State* iS = *(TSV.begin());
  if (matchtype == 1 ) {
    double tracky_atcalo = (iS)->y()/Gaudi::Units::cm + (iS)->ty()*(z-(iS)->z()/Gaudi::Units::cm);
    double deltaY = (y-tracky_atcalo)/ey;
    double Chi2Y = deltaY*deltaY;
    return Chi2Y;
  } 
  if (matchtype == 2 ) {
    double de = e*(sqrt( m_eres[0]*m_eres[0] + m_eres[1]*m_eres[1]/e ));
    double deltaZ = z - m_zKick;
    double xkick  = deltaZ * (m_ptkickConstant/Gaudi::Units::GeV)/e;
    double exkick = fabs( xkick/e)*de;
    double edxdz  = sqrt(ex*ex + exkick*exkick);

    double q[2]={-1.,1.};
    double bestChi2 = 9999.;
    for (int i= 0; i< 2; ++i) {
      
      double trackx_atcalo = (iS)->x()/Gaudi::Units::cm + (iS)->tx()*(z-(iS)->z()/Gaudi::Units::cm); 
      trackx_atcalo += q[i]*xkick; 

      double deltaX = (x - trackx_atcalo)/edxdz;

      double Chi2X = deltaX*deltaX;
      if (fabs(Chi2X)<fabs(bestChi2)) bestChi2 = Chi2X;
    }
    return bestChi2; 
  }
  return -99999.;//Won't happen  
}
