//-----------------------------------------------------------------------------
/** @file TrackSelector.cpp
 *
 *  Implementation file for T-station alignment tool : TrackSelector
 *
 *  CVS Log :-
 *  $Id: ATrackSelector.cpp,v 1.8 2010-03-17 16:42:01 jblouw Exp $
 *
 *  @author J. Blouw  Johan.Blouw@cern.ch
 *  @date   31/09/2006
 */
//-----------------------------------------------------------------------------
#include <string>

#include "GaudiKernel/ToolFactory.h"

#include "GaudiKernel/SystemOfUnits.h"

#include "ATrackSelector.h"
#include "Event/Node.h"
#include "Event/FitNode.h"
#include "Event/OTMeasurement.h"

using namespace LHCb;

DECLARE_TOOL_FACTORY( ATrackSelector );

//-----------------------------------------------------------------------------

ATrackSelector::ATrackSelector( const std::string& type,
                              const std::string& name,
                              const IInterface* parent ):
  GaudiTool ( type, name , parent ) {

  // interface
  declareInterface<IATrackSelectorTool>(this);

  // define track containers
  
  // cut options
  declareProperty( "MinPCut",    m_minPCut         = 0.0 ); // in GeV
  declareProperty( "MinPtCut",   m_minPtCut        = 0.0 ); // in GeV
  declareProperty( "MinChi2Cut", m_minChi2Cut      = 0.0 );
  declareProperty( "Charge",     m_charge          = 0 ); // charge of particle selection
  declareProperty( "MaxPCut",    m_maxPCut         = boost::numeric::bounds<double>::highest() ); // in GeV
  declareProperty( "MaxPtCut",   m_maxPtCut        = boost::numeric::bounds<double>::highest() ); // in GeV
  declareProperty( "MaxChi2Cut", m_maxChi2Cut      = boost::numeric::bounds<double>::highest() );
  declareProperty( "MinITHitCut",  m_minITHitCut   = 0.0 );
  declareProperty( "MinOTHitCut",  m_minOTHitCut   = 0.0 );
  declareProperty( "MinTTHitCut",  m_minTTHitCut   = 0.0 );
  declareProperty( "YCutMax_at_T1", m_yMaxCut = 9999.99 );
  declareProperty( "YCutMin_at_T1", m_yMinCut = -9999.99 );
  declareProperty( "UseWeights", m_weights = true ); // enable uniformization of track distribution
  declareProperty( "UniformCutOff", m_uniCut = 4 ); //  m_uniCut = 9 disables the uniformity.
  declareProperty( "MinEnergyCut", m_energyMinCut = 0.0 ); // in GeV

}

ATrackSelector::~ATrackSelector() {};

StatusCode ATrackSelector::initialize() {
  debug() << "Initialize track selector tool" << endreq;
  // retrieve track-calo match tool
  if( m_energyMinCut > 0. ) 
    m_trackenergy = tool<ITrackCaloMatch>( "TrackCaloMatch" );
  else 
    m_trackenergy = NULL;
  
  for ( unsigned int i = 0; i < 3; i++ )  // stations
    for ( unsigned int j = 0; j < 4; j++ ) // quadrants
      for ( unsigned int k = 0; k < 4; k++ ) // layers
	for ( unsigned int l = 0; l < 9; l++ )
	  m_uniform[i][j][k][l] = 0;
  if ( m_uniCut < 1 ) {
    warning() << "Resetting uniformity cut to m_uniCut = 1" << endreq;
    m_uniCut = 1;
  } else {
    info() << "Attempting to create a uniform track distribution from module " << m_uniCut << " till module 9" << endreq;
  }
  if ( m_yMinCut > -9999.99 || m_yMaxCut < 9999.99 ) {
    info() << "Using window " << m_yMinCut << " < y < " << m_yMaxCut << " to select tracks" << endreq; 
    m_extrapolator = tool<ITrackExtrapolator>( "TrackMasterExtrapolator" );
  } else
    m_extrapolator = NULL;
  return StatusCode::SUCCESS;
}

StatusCode ATrackSelector::finalize() {
  this->PrintUniformTD();
  debug() << "Finalize track selector tool" << endreq;
  return StatusCode::SUCCESS;
}

bool ATrackSelector::accept ( const LHCb::Track& aTrack ) {
  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "Trying Track " << aTrack.key() << " " << aTrack.type()
            << endreq;
  }

  // simple cuts first

  // select positively and/or negatively charged tracks
  if(m_charge != -1){
    const int charge = aTrack.charge();
    if ( m_charge*charge < 0 ) {
      debug() << "Removing particle with charge " << charge << endreq;
      return false;
    }
  }

  // chi-squared
  const double chi2 = aTrack.chi2PerDoF();
  //info() << "Into TrackSelector " <<chi2<<" "<<m_maxChi2Cut<< endreq;
  if ( chi2 < m_minChi2Cut || chi2 > m_maxChi2Cut ) {
    if ( msgLevel(MSG::DEBUG) )
        debug() << " -> Chi^2 " << chi2 << " failed cut" << endreq;
        //info()  << " -> Chi^2 " << chi2 << " failed cut" << endreq;
    return false;
  }

  // cut p
  if(m_maxPCut != -1) {
    const double p = aTrack.p();
    if ( p < m_minPCut || p > m_maxPCut ) {
      if ( msgLevel(MSG::DEBUG) )
	debug() << " -> P " << aTrack.p() << " failed cut" << endreq;
      return false;
    }
  }

  // cut on pt
  if(m_maxPtCut != -1) {
    const double pt = aTrack.pt();
    if ( pt < m_minPtCut || pt > m_maxPtCut ) {
      if ( msgLevel(MSG::DEBUG) )
	debug() << " -> Pt " << aTrack.pt() << " failed cut" << endreq;
      return false;
    }
  }

  int OThits = 0, IThits = 0 , TThits = 0;
  //if all ok so far, cut on #Hits
  bool test = countTHits( aTrack, OThits, IThits ,TThits);
  if ( OThits >= m_minOTHitCut )      
    debug() << "--> " << OThits <<  "  hits in  OT !" << endmsg;
  else {
    debug() << "--> not enough  hits in  OT: "<< OThits << endmsg;
      return false;
  }
  // Generate uniform track distribution in x direction
  if ( ! Unify( aTrack ) ) {
    return false;
  }
  // cut on energy deposited in calorimeters:
  if( m_energyMinCut > 0.0 ) {
    double energy = m_trackenergy->energy( aTrack );
    debug() << "Energy on track = " << energy << endreq;
    if ( energy < m_energyMinCut ) {
      if ( msgLevel(MSG::DEBUG) )
	debug() << " -> energy " << energy << " failed cut" << endreq;
      return false;
    }
  }

  if ( IThits >= m_minITHitCut )      debug() << "--> " << IThits <<  "  hits in  IT !" << endmsg;
  else {
      debug() << "--> not enough  hits in  IT ! (# hits="<< IThits<<")" << endmsg;
      return false;
  }
  if ( TThits >= m_minTTHitCut )      debug() << "--> " << TThits <<  "  hits in  TT !" << endmsg;
  else {
      debug() << "--> not enough  hits in  TT !" << endmsg;
      return false;
  }
  if ( msgLevel(MSG::DEBUG) ) debug() << " -> Track selected" << endreq;
    
  if ( m_weights ) 
    return test;
  return true;
}


bool ATrackSelector::Unify( const LHCb::Track& aTrack ) {
  LHCb::Track::ConstNodeRange::const_iterator node;
  if ( m_extrapolator && ! yCut( aTrack ) ) {
    return false;
  }
  for ( node = aTrack.nodes().begin(); aTrack.nodes().end() != node; node++ ) {
    const LHCb::FitNode *aNode = dynamic_cast<const LHCb::FitNode*>(*node);
    if ( ( aNode->type() != LHCb::Node::HitOnTrack 
	   && aNode->type() 
	   && aNode->type() != LHCb::Node::Outlier ) 
	 || ! aNode->measurement().detectorElement() )
      continue;
    LHCb::Measurement *measurement = &(aNode->measurement());
    if ( measurement == 0 ) {
      debug() << "No measurement available..." << endreq;
      continue;
    }
    LHCb::LHCbID id = measurement->lhcbID();
    if ( id.isOT() && aTrack.isOnTrack( id ) ) {
      const LHCb::OTMeasurement* otm = 
	dynamic_cast<const LHCb::OTMeasurement*>(&aNode->measurement());
      const LHCb::OTChannelID channel = otm->channel();
      bool res = uniformTD( channel );
      if ( ! res ) 
	return false;
    }
  }
  return true;
}

bool ATrackSelector::uniformTD( const LHCb::OTChannelID &ch ) {
  // Note: Stations count from 1-3...
  //       Modules count from 1-9
  //       Layers count from 0-3
  //       Quadrants from 0-3
  // Module 1 is furthest from the beampipe,
  // Module 9 is closest to the beampipe.
  int l = ch.layer();
  int q = ch.quarter();
  int m = ch.module() - 1;
  int s = ch.station() - 1;
  m_uniform[s][q][l][m]++;
  if ( s < 1 ) {
    if ( ! uniformCut( m ) ) {
      m_uniform[s][q][l][m]--;
      return false;
    }
  }
  return true;
}

bool ATrackSelector::uniformCut( int &mod ) {
  unsigned int prev[9];
  for (unsigned int t = 0; t<9;t++)
    prev[t] = 0;
  unsigned int tot, s = 0;
  // always return true when an outside module was hit
  if ( mod < m_uniCut )
    return true;
  // Sum over quarters & layers
  for( unsigned int q = 0; q < 4; q++ )
    for ( unsigned int l = 0; l < 4; l++ )
      for( unsigned int m = 0; m < 9; m++ )
	prev[m] += m_uniform[s][q][l][m];
  // check if the number of hits in this module
  // is more than what we want:
  if ( prev[mod-1] >= (int) (0.9*prev[mod]) ){
    // debug() << "Keeping track..." << endreq;
    return true;
  }
  //  debug() << "Removing track because it breaks uniformity!" << endreq;
  return false;
}


bool ATrackSelector::countTHits(const LHCb::Track& aTrack, int& nOThits, int& nIThits, int& nTThits ) {
  int itSum = 0, otSum = 0, ttSum = 0;
  bool test = true;
  LHCb::Track::ConstNodeRange::const_iterator node;
  if ( m_extrapolator && ! yCut( aTrack ) ) {
    return false;
  }
  for ( node = aTrack.nodes().begin(); aTrack.nodes().end() != node; node++ ) {
    const LHCb::FitNode *aNode = dynamic_cast<const LHCb::FitNode*>(*node);
    if ( ( aNode->type() != LHCb::Node::HitOnTrack 
	   && aNode->type() 
	   && aNode->type() != LHCb::Node::Outlier ) 
	 || ! aNode->measurement().detectorElement() )
      continue;
    LHCb::Measurement *measurement = &(aNode->measurement());
    if ( measurement == 0 ) {
      debug() << "No measurement available..." << endreq;
      continue;
    }
    LHCb::LHCbID id = measurement->lhcbID();
    if ( aTrack.isOnTrack( id ) ) {
      if ( id.stID().isIT() ) {
        itSum++;
      }
      else if ( id.isOT() ) {
	const LHCb::OTMeasurement* otm = 
	  dynamic_cast<const LHCb::OTMeasurement*>(&aNode->measurement());
	const LHCb::OTChannelID channel = otm->channel();
        otSum++;
      }
      else if ( id.stID().isTT() ) {
        ttSum++;
      }
    }
  }
  nIThits = itSum;
  nOThits = otSum;
  nTThits = ttSum;
  return test;
}


bool ATrackSelector::yCut( const LHCb::Track &t ) {
  Gaudi::XYZPoint pRec;
  double Z_position = 8630.0;
  m_extrapolator->position( t, Z_position, pRec );
  info() << "Entry point = " << pRec << endreq;
  if ( pRec.y() < m_yMaxCut && pRec.y() > m_yMinCut )
    return true;
  return false;
}



int ATrackSelector::traversesIT(LHCb::Track& aTrack, int& nOThits, int& nIThits ) const {
  int itSum = 0, otSum = 0;
  const std::vector<LHCbID>& ids = aTrack.lhcbIDs();
  for ( std::vector<LHCbID>::const_iterator iter = ids.begin();
        iter != ids.end(); ++iter ) {
    if ( aTrack.isOnTrack( *iter ) ) {
      if ( iter->isST() ) {
	itSum++;
      } else if ( iter->isOT() ) {
	otSum--;
      }
    }
  }
  if ( itSum > 0 && otSum < 0 ) {
    nIThits = itSum;
    nOThits = -otSum;
    return 0;
  } else if ( itSum > 0 && otSum == 0 ) {
    nIThits = itSum;
    return 1;
  } else if ( itSum == 0 && otSum < 0 ) {
    nOThits = -otSum;
    return -1;
  } else if ( itSum == 0 && otSum == 0 ) {
    nOThits = -otSum;
    nIThits = itSum;
    return 10;
  }
  return 11;
}

void ATrackSelector::PrintUniformTD() {
  unsigned int prev[4][9];
  for ( unsigned int i = 0; i < 3; i++ )  // stations
    for ( unsigned int l = 0; l < 9; l++ )
      prev[i][l] = 0;
  
  for ( unsigned int i = 0; i < 3; i++ )  {// stations
    info() << "Station nr. " << i << " : ";
    for ( unsigned int j = 0; j < 4; j++ ) {// quadrants
      info() << "Quadrant nr: " << j << endreq;
      for ( unsigned int k = 0; k < 4; k++ ) {// layers
	info() << "Layer nr " << k << " : ";
	for ( unsigned int l = 0; l < 9; l++ )
	  info() << m_uniform[i][j][k][l] << " ";
	info() << endreq;
      }
    }
  }
  for ( unsigned int i = 0; i < 3; i++ )  // stations
    for ( unsigned int j = 0; j < 4; j++ ) // quadrants
      for ( unsigned int k = 0; k < 4; k++ ) // layers
	for ( unsigned int l = 0; l < 9; l++ )
	  prev[i][l] += m_uniform[i][j][k][l];
  for ( unsigned int i = 0; i < 3; i++ )  {// stations
    info() << "Station " << i << endreq;
    info() << "Hits : ";
    for ( unsigned int l = 0; l < 9; l++ )
      info() << prev[i][l] << " ";
    info() << endreq;
  }
}
