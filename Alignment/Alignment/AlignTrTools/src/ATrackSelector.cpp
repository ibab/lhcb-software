//-----------------------------------------------------------------------------
/** @file TrackSelector.cpp
 *
 *  Implementation file for T-station alignment tool : TrackSelector
 *
 *  CVS Log :-
 *  $Id: ATrackSelector.cpp,v 1.1 2006-09-29 10:48:31 jblouw Exp $
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
  declareProperty( "MinHitCut",  m_minHitCut   = 0.0 );

  declareProperty( "MaxPCut",    m_maxPCut     = boost::numeric::bounds<double>::highest() ); // in GeV
  declareProperty( "MaxPtCut",   m_maxPtCut    = boost::numeric::bounds<double>::highest() ); // in GeV
  declareProperty( "MaxChi2Cut", m_maxChi2Cut  = boost::numeric::bounds<double>::highest() );
  declareProperty( "MaxHitCut",  m_maxHitCut   = boost::numeric::bounds<double>::highest() );

  //  m_trTypes =
  //    boost::assign::list_of("Velo")("VeloR")("Long")("Upstream")("Downstream");
  //  declareProperty( "TrackTypes", m_trTypes );

  // "Expert" options

  //  declareProperty( "vWeight", m_vWeight     = 1.0 );
  //  declareProperty( "oWeight", m_oWeight     = 0.5 );
  //  declareProperty(" iWeight", m_iWeight     = 1.0 );

}

ATrackSelector::~ATrackSelector() {};

bool ATrackSelector::accept ( const Track& aTrack ) const {
  //  info() << "Into TrackSelector::accept!" << endreq;
  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "Trying Track " << aTrack.key() << " " << aTrack.type()
            << endreq;
  }

  // simple cuts first

  // chi-squared
  const double chi2 = aTrack.chi2PerDoF();
  //  info() << "Into TrackSelector::accept (1)" << endreq;
  if ( chi2 < m_minChi2Cut || chi2 > m_maxChi2Cut ) {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Chi^2 " << chi2 << " failed cut" << endreq;
    return false;
  }

  //  info() << "Into TrackSelector::accept (2)" << endreq;
  // cut p
  const double p = aTrack.p();
  if ( p < m_minPCut || p > m_maxPCut ) {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> P " << aTrack.p() << " failed cut" << endreq;
    return false;
  }
  //  info() << "Into TrackSelector::accept (3)" << endreq;

  // cut on pt
  const double pt = aTrack.pt();
  if ( pt < m_minPtCut || pt > m_maxPtCut ) {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Pt " << aTrack.pt() << " failed cut" << endreq;
    return false;
  }
  //  info() << "Into TrackSelector::accept (4)" << endreq;
  // track types
//  if ( !m_selTypes[aTrack.type()] )  {
//    if ( msgLevel(MSG::DEBUG) )
//      debug() << " -> Track type " << aTrack.type() << " is rejected" << endreq;
//    return false;
//  }
//  info() << "Into TrackSelector::accept (5)" << endreq;

  if ( msgLevel(MSG::DEBUG) ) debug() << " -> Track selected" << endreq;
  return true;
}

//int ATrackSelector::traversesIT(int& nOThits, int& nIThits ) const {
int ATrackSelector::traversesIT(const Track& aTrack, int& nOThits, int& nIThits ) const {
  //  info() << "Into TrackSelector::traversesIT" << endreq;
  int itSum = 0, otSum = 0;
  //  info() << "TrackSelector::traverseIT (1)" <<endreq;
  const std::vector<LHCbID>& ids = aTrack.lhcbIDs();
  //  info() << "TrackSelector::traverseIT (2)" <<endreq;
  for ( std::vector<LHCbID>::const_iterator iter = ids.begin();
        iter != ids.end(); ++iter ) {
    if ( iter->isST() ) {
	itSum++;
      } else if ( iter->isOT() ) {
	otSum--;
      }
  }
  //  info() << "TrackSelector::traverseIT (3)" <<endreq;
  if ( itSum > 0 && otSum < 0 ) {
    nIThits = itSum;
    nOThits = -otSum;
    return 0;
  }
  //  info() << "TrackSelector::traverseIT (4)" <<endreq;
  if ( itSum > 0 && otSum == 0 ) {
    //    info() << "TrackSelector::traverseIT (4a)" <<endreq;
    nIThits = itSum;
    //    info() << "TrackSelector::traverseIT (4b)" <<endreq;
    return 1;
  }
  //  info() << "TrackSelector::traverseIT (5)" <<endreq;
  if ( itSum == 0 && otSum < 0 ) {
    nOThits = -otSum;
    return -1;
  }
  //  info() << "TrackSelector::traverseIT (6)" <<endreq;
//  nOThits = 1;
//  nIThits = 2;

  return 1;
}
