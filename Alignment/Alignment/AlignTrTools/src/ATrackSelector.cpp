//-----------------------------------------------------------------------------
/** @file TrackSelector.cpp
 *
 *  Implementation file for T-station alignment tool : TrackSelector
 *
 *  CVS Log :-
 *  $Id: ATrackSelector.cpp,v 1.2 2007-04-25 14:07:06 jblouw Exp $
 *
 *  @author J. Blouw  Johan.Blouw@cern.ch
 *  @date   31/09/2006
 */
//-----------------------------------------------------------------------------
#include <string>

#include "GaudiKernel/ToolFactory.h"

#include "GaudiKernel/SystemOfUnits.h"

#include "ATrackSelector.h"

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
  info() << "Creating trackselector tool" << endreq;
  declareProperty( "MinPCut",    m_minPCut     = 0.0 ); // in GeV
  declareProperty( "MinPtCut",   m_minPtCut    = 0.0 ); // in GeV
  declareProperty( "MinChi2Cut", m_minChi2Cut  = 0.0 );
  declareProperty( "Charge", m_charge = 0 ); // charge of particle selection

  declareProperty( "MaxPCut",    m_maxPCut     = boost::numeric::bounds<double>::highest() ); // in GeV
  declareProperty( "MaxPtCut",   m_maxPtCut    = boost::numeric::bounds<double>::highest() ); // in GeV
  declareProperty( "MaxChi2Cut", m_maxChi2Cut  = boost::numeric::bounds<double>::highest() );
  declareProperty( "MaxHitCut",  m_maxHitCut   = boost::numeric::bounds<double>::highest() );

}

ATrackSelector::~ATrackSelector() {};

StatusCode ATrackSelector::initialize() {
 debug() << "Initialize track selector tool" << endreq;
  return StatusCode::SUCCESS;
}

bool ATrackSelector::accept ( const LHCb::Track& aTrack ) const {
  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "Trying Track " << aTrack.key() << " " << aTrack.type()
            << endreq;
  }

  // simple cuts first

  // select positively and/or negatively charged tracks

  const int charge = aTrack.charge();
  if ( m_charge*charge < 0 ) {
    debug() << "Removing particle with charge " << charge << endreq;
    return false;
  }

  // chi-squared
  const double chi2 = aTrack.chi2PerDoF();
  //  info() << "Into TrackSelector::accept (1)" << endreq;
  if ( chi2 < m_minChi2Cut || chi2 > m_maxChi2Cut ) {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Chi^2 " << chi2 << " failed cut" << endreq;
    return false;
  }

  // cut p
  const double p = aTrack.p();
  if ( p < m_minPCut || p > m_maxPCut ) {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> P " << aTrack.p() << " failed cut" << endreq;
    return false;
  }

  // cut on pt
  const double pt = aTrack.pt();
  if ( pt < m_minPtCut || pt > m_maxPtCut ) {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Pt " << aTrack.pt() << " failed cut" << endreq;
    return false;
  }

  if ( msgLevel(MSG::DEBUG) ) debug() << " -> Track selected" << endreq;
  return true;
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
