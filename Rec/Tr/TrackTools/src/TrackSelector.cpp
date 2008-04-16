
//-----------------------------------------------------------------------------
/** @file TrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : TrackSelector
 *
 *  CVS Log :-
 *  $Id: TrackSelector.cpp,v 1.14 2008-04-16 15:34:31 pkoppenb Exp $
 *
 *  @author M.Needham Matt.Needham@cern.ch
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/12/2005
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// Tsa
#include "TrackSelector.h"

using namespace LHCb;

DECLARE_TOOL_FACTORY( TrackSelector );

//-----------------------------------------------------------------------------

TrackSelector::TrackSelector( const std::string& type,
                              const std::string& name,
                              const IInterface* parent ):
  GaudiTool ( type, name, parent )
{

  // interface
  declareInterface<ITrackSelector>(this);

  // cut options

  declareProperty( "MinPCut",    m_minPCut     = 0.0 ); // in GeV
  declareProperty( "MinPtCut",   m_minPtCut    = 0.0 ); // in GeV
  declareProperty( "MinChi2Cut", m_minChi2Cut  = 0.0 );
  declareProperty( "MinHitCut",  m_minHitCut   = 0.0 );
  declareProperty("MinEtaCut", m_minEtaCut  = boost::numeric::bounds<double>::lowest());


  declareProperty( "MaxPCut",    m_maxPCut     = boost::numeric::bounds<double>::highest() ); // in GeV
  declareProperty( "MaxPtCut",   m_maxPtCut    = boost::numeric::bounds<double>::highest() ); // in GeV
  declareProperty( "MaxChi2Cut", m_maxChi2Cut  = boost::numeric::bounds<double>::highest() );
  declareProperty( "MaxHitCut",  m_maxHitCut   = boost::numeric::bounds<double>::highest() );

  declareProperty( "MaxEtaCut",  m_maxEtaCut   = boost::numeric::bounds<double>::highest() );

  declareProperty("LikCut", m_likCut =  boost::numeric::bounds<double>::lowest());
 
  m_trTypes =
    boost::assign::list_of("Velo")("VeloR")("Long")("Upstream")("Downstream")("Ttrack");
  declareProperty( "TrackTypes", m_trTypes );

  // "Expert" options

  declareProperty( "vWeight", m_vWeight     = 1.0 );
  declareProperty( "oWeight", m_oWeight     = 0.5 );
  declareProperty( "iWeight", m_iWeight     = 1.0 );

}

TrackSelector::~TrackSelector() { }

StatusCode TrackSelector::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  debug() << "Track Selection Cuts :-" << endreq;
  debug() << " P       = " << m_minPCut  << " -> " << m_maxPCut  << " GeV" << endreq;
  debug() << " Pt      = " << m_minPtCut << " -> " << m_maxPtCut << " GeV" << endreq;
  debug() << " Chi^2   = " << m_minChi2Cut << " -> " << m_maxChi2Cut << endreq;
  debug() << " # hits  = " << m_minHitCut << " -> " << m_maxHitCut << endreq;

  // initialise track type and alg selections
  m_selTypes.clear();
  if ( !m_trTypes.empty() )
  {
    debug() << "Selecting track types = " << m_trTypes << endreq;
    for ( TrackTypes::const_iterator iT = m_trTypes.begin();
          iT != m_trTypes.end(); ++iT )
    {
      if      ( *iT == "Velo"       ) { m_selTypes[Track::Velo] = true; }
      else if ( *iT == "VeloR"      ) { m_selTypes[Track::VeloR] = true; }
      else if ( *iT == "Long"       ) { m_selTypes[Track::Long] = true; }
      else if ( *iT == "Upstream"   ) { m_selTypes[Track::Upstream] = true; }
      else if ( *iT == "Downstream" ) { m_selTypes[Track::Downstream] = true; }
      else if ( *iT == "Ttrack"     ) { m_selTypes[Track::Ttrack] = true; }
      else
      {
        return Error( "Unknown track type '"+*iT+"'" );
      }
    }
    // Note, track types not selected above, will automatically NOT be selected
  }

  return sc;
}

bool TrackSelector::accept ( const Track& aTrack ) const
{
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Trying Track " << aTrack.key() << " " << aTrack.type()
            << endreq;
  }

  // simple cuts first

  // chi-squared
  const double chi2 = aTrack.chi2PerDoF();
  if ( chi2 < m_minChi2Cut || chi2 > m_maxChi2Cut )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Chi^2 " << chi2 << " failed cut" << endreq;
    return false;
  }

  // cut p
  const double p = aTrack.p();
  if ( p < m_minPCut || p > m_maxPCut )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> P " << aTrack.p() << " failed cut" << endreq;
    return false;
  }

  // cut on pt
  const double pt = aTrack.pt();
  if ( pt < m_minPtCut || pt > m_maxPtCut )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Pt " << aTrack.pt() << " failed cut" << endreq;
    return false;
  }

  // track types
  if ( !m_selTypes[aTrack.type()] )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Track type " << aTrack.type() << " is rejected" << endreq;
    return false;
  }

  // measurements
  const double nMeas = weightedMeasurementSum(aTrack);
  if ( nMeas < m_minHitCut || nMeas > m_maxHitCut )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> #hits " << nMeas << " failed cut" << endreq;
    return false;
  }
 
  // eta
  double eta = aTrack.pseudoRapidity();
  if ( eta < m_minEtaCut || eta > m_maxEtaCut )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> #eta " << eta << " failed cut" << endreq;
    return false;
  }
 
  // remove seeds with bad likelihood
  if (aTrack.info(Track::Likelihood, -9999.) < m_likCut ) return false;
 
  if ( msgLevel(MSG::DEBUG) ) debug() << " -> Track selected" << endreq;
  return true;
}

double TrackSelector::weightedMeasurementSum(const Track& aTrack) const
{
  double wSum = 0;
  const std::vector<LHCbID>& ids = aTrack.lhcbIDs();
  for ( std::vector<LHCbID>::const_iterator iter = ids.begin();
        iter != ids.end(); ++iter )
  {
    if      ( iter->isVelo() )
    {
      wSum += m_vWeight;
    }
    else if ( iter->isST() )
    {
      wSum += m_iWeight;
    }
    else if ( iter->isOT() )
    {
      wSum += m_oWeight;
    }
  }
  return wSum;
}
