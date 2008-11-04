// $Id: EcalSeedForVeloMatch.cpp,v 1.1 2008-11-04 08:30:28 witekma Exp $
// Include files 

#include <cmath>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// from Event
#include "Event/L0CaloCandidate.h"
#include "Event/Track.h"
#include "Event/State.h"

// local
#include "EcalSeedForVeloMatch.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EcalSeedForVeloMatch
//
// 2007-07-17 : Johannes Albrecht
//-----------------------------------------------------------------------------

using namespace LHCb;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( EcalSeedForVeloMatch );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EcalSeedForVeloMatch::EcalSeedForVeloMatch( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  // large energy uncertainy for electrons
  declareProperty("eRes0", m_eres[0] = 0.60 );
  declareProperty("eRes1", m_eres[1] = 0.70 );
  declareProperty("eCorect", m_eCorrect = 1.2 );
  
  declareInterface<ICaloSeedTool>(this);

}
//=============================================================================
// Destructor
//=============================================================================
EcalSeedForVeloMatch::~EcalSeedForVeloMatch() {} 

//=============================================================================

//-------------------------------------
StatusCode EcalSeedForVeloMatch::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return sc;
  }
  debug() << " Initialize EcalSeedForVeloMatch" << endmsg;
  
  return StatusCode::SUCCESS;
}


//=============================================================================
// -----------
//=============================================================================
StatusCode EcalSeedForVeloMatch::makeTrack(const LHCb::L0CaloCandidate& calo,
                                     LHCb::Track& track )
{
  if(msgLevel(MSG::DEBUG)) {

    debug()<<"EcalSeedForVeloMatch::execute!"<<endmsg;
    debug()<<"Properties of L0 candidate:  ( Et, x , y ) = ( "
           <<calo.et()<<" , "<<calo.position().X() <<" , "<<calo.position().Y() <<endmsg;
  }


  double x      = calo.position().x();
  double y      = calo.position().y();
  double z      = calo.position().z();
  double tx     = 0.;
  double ty     = 0.;
  double et     = calo.et();

  double sintheta = sqrt(x*x + y*y)/(sqrt(x*x + y*y + z*z));
  double e = fabs(et)/sintheta;

  e *= m_eCorrect;

  double xerr     = calo.posTol()*(4./sqrt(12.0));

  debug() << " position " << calo.position() << " et " << et
          << " sintheta " << sintheta << " e " << e << endreq;  

  // Relative and absolute energy uncertainty:
  double de_over_e = sqrt( m_eres[0]*m_eres[0] + m_eres[1]*m_eres[1]/(e/Gaudi::Units::GeV) );
  double de = e * de_over_e;

  State state;    
  Gaudi::TrackSymMatrix stateCov = Gaudi::TrackSymMatrix();
  state.setLocation(State::MidECal);
  state.setState(x,y,z,tx,ty,1./e); 

  stateCov(0,0) = xerr*xerr;
  stateCov(1,1) = stateCov(0,0);
  stateCov(2,2) = 0.;
  stateCov(3,3) = 0.;
  stateCov(4,4) = 1./(de*de);
  state.setCovariance(stateCov);

  track.addToStates(state);

  CaloCellID id = calo.id();
  LHCb::CaloCellID id1(id.calo(), id.area(), id.row()+1, id.col()   );
  LHCb::CaloCellID id2(id.calo(), id.area(), id.row()+1, id.col()+1 );
  LHCb::CaloCellID id3(id.calo(), id.area(), id.row()  , id.col()+1 );

  track.addToLhcbIDs( LHCbID(id) );
  track.addToLhcbIDs( LHCbID(id1) );
  track.addToLhcbIDs( LHCbID(id2) );
  track.addToLhcbIDs( LHCbID(id3) );

  return StatusCode::SUCCESS; 
}


