//-----------------------------------------------------------------------------
/** @file TrackSelector.cpp
 *
 *  Implementation file for T-station alignment tool : TrackSelector
 *
 *  CVS Log :-
 *  $Id: ATrackSelector.cpp,v 1.7 2010-02-05 16:47:54 jblouw Exp $
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
  declareProperty( "UseWeights", m_weights = true ); // enable uniformization of track distribution
  declareProperty( "UniformCutOff", m_uniCut = 4 ); //  m_uniCut = 9 disables the uniformity.
  declareProperty( "MinEnergyCut", m_energyMinCut = 0.0 ); // in GeV

}

ATrackSelector::~ATrackSelector() {};

StatusCode ATrackSelector::initialize() {
  debug() << "Initialize track selector tool" << endreq;
  // retrieve track-calo match tool
  if( m_energyMinCut != 0. ) 
    m_trackenergy = tool<ITrackCaloMatch>( "TrackCaloMatch" );
  
  for ( unsigned int i = 0; i < 3; i++ )  // stations
    for ( unsigned int j = 0; j < 4; j++ ) // quadrants
      for ( unsigned int k = 0; k < 4; k++ ) // layers
	for ( unsigned int l = 0; l < 9; l++ )
	  m_uniform[i][j][k][l] = 0;
  if ( m_uniCut < 1 ) {
    warning() << "Resetting uniformity cut to m_uniCut = 1" << endreq;
    m_uniCut = 1;
  }
  return StatusCode::SUCCESS;
}

StatusCode ATrackSelector::finalize() {
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
  // cut on energy deposited in calorimeters:
  if( m_energyMinCut > 0.0 ) {
    double energy = m_trackenergy->energy( aTrack );
    if ( energy < m_energyMinCut ) {
      if ( msgLevel(MSG::DEBUG) )
	debug() << " -> energy " << energy << " failed cut" << endreq;
      return false;
    }
  }
  //if all ok so far, cut on #Hits
  int OThits = 0, IThits = 0 , TThits = 0;
  bool test = countTHits( aTrack, OThits, IThits ,TThits);

  if ( OThits >= m_minOTHitCut )      
    debug() << "--> " << OThits <<  "  hits in  OT !" << endmsg;
  else {
    debug() << "--> not enough  hits in  OT: "<< OThits << endmsg;
      //plot(OThits,"tooFewOThits","tooFewOThits",0.,40.,40);
      return false;
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

bool ATrackSelector::countTHits(const LHCb::Track& aTrack, int& nOThits, int& nIThits, int& nTThits ) {
  int itSum = 0, otSum = 0, ttSum = 0;
  bool test = true;
  LHCb::Track::ConstNodeRange::const_iterator node;
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
	bool res = uniformTD( channel );
	if ( test )
	  test = res;
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
  debug() << "Station = " << s 
	 << " quarter = " << q 
	 << " layer = " << l
	 << " module = " << m << endreq;
  //  m_uniform[us][uq][ul][um]++;
  if ( m < m_uniCut ) {
    m_uniform[s][q][l][m]++;
    return true;
  } else if ( m_uniform[s][q][l][m - 1] >= m_uniform[s][q][l][m] ) {
    m_uniform[s][q][l][m]++;
    return true;
  }
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
