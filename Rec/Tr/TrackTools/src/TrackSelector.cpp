
//-----------------------------------------------------------------------------
/** @file TrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : TrackSelector
 *
 *  CVS Log :-
 *  $Id: TrackSelector.cpp,v 1.19 2008-07-28 12:12:12 jonrob Exp $
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
  declareProperty( "MinChi2Cut", m_minChi2Cut  = -1  );
  declareProperty( "MinHitCut",  m_minHitCut   = 0.0 );

  declareProperty( "MinEtaCut",  m_minEtaCut   = boost::numeric::bounds<double>::lowest());
  declareProperty( "MinNDoF",    m_minNDoF     = 0 ) ;
  declareProperty( "MinNVeloRHits", m_minNVeloRHits     = 0 ) ;
  declareProperty( "MinNVeloPhiHits", m_minNVeloPhiHits = 0 ) ;

  declareProperty( "MaxPCut",    m_maxPCut     = boost::numeric::bounds<double>::highest() ); // in GeV
  declareProperty( "MaxPtCut",   m_maxPtCut    = boost::numeric::bounds<double>::highest() ); // in GeV
  declareProperty( "MaxChi2Cut", m_maxChi2Cut  = -1 );
  declareProperty( "MaxHitCut",  m_maxHitCut   = boost::numeric::bounds<double>::highest() );

  declareProperty( "MaxEtaCut",  m_maxEtaCut   = boost::numeric::bounds<double>::highest() );

  declareProperty( "LikCut", m_likCut =  boost::numeric::bounds<double>::lowest());

  declareProperty( "AcceptClones", m_acceptClones = true );
  declareProperty( "CloneDistCut", m_cloneCut     = 5000 );

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

  if ( !m_acceptClones ) 
    debug() << "Will reject clones. CloneDist cut = " << m_cloneCut << endreq;

  return sc;
}

bool TrackSelector::accept ( const Track& aTrack ) const
{
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Trying Track " << aTrack.key() << " " << aTrack.type()
            << endreq;
  }

  // NDOF
  if( aTrack.nDoF() < m_minNDoF ) 
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> NDoF " << aTrack.nDoF() << " failed cut" << endreq;
    return false;
  }
  
  // chi-squared
  const double chi2 = aTrack.chi2PerDoF();
  if ( (m_maxChi2Cut>=0 && (chi2 > m_maxChi2Cut || aTrack.nDoF()<=0 ) )  ||
       (m_minChi2Cut>=0 && (chi2 < m_minChi2Cut || aTrack.nDoF()<=0 ) ) )
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

  // eta
  const double eta = aTrack.pseudoRapidity();
  if ( eta < m_minEtaCut || eta > m_maxEtaCut )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> #eta " << eta << " failed cut" << endreq;
    return false;
  }

  // Clones
  info() << "CloneDist " << aTrack.info(LHCb::Track::CloneDist,9e99) << endreq;
  if ( !m_acceptClones && ( aTrack.checkFlag(LHCb::Track::Clone) || 
                            aTrack.info(LHCb::Track::CloneDist,9e99) < m_cloneCut ) )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Track failed clone rejection" << endreq;
    return false;
  }

  // remove tracks with bad likelihood
  const double lik = aTrack.info(Track::Likelihood, -9999.);
  if ( lik < m_likCut) 
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Likelihood " << lik << " failed cut" << endreq;
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

  // count number of velo phi and R hits
  if( m_minNVeloPhiHits > 0 || m_minNVeloRHits > 0 ) {
    int numVeloPhi(0), numVeloR(0) ;
    for( std::vector<LHCbID>::const_iterator it = aTrack.lhcbIDs().begin() ;
         it != aTrack.lhcbIDs().end(); ++it)
      if     ( it->isVeloPhi() ) ++numVeloPhi ;
      else if( it->isVeloR() ) ++numVeloR ;
    if ( numVeloPhi < m_minNVeloPhiHits ) {
      if ( msgLevel(MSG::DEBUG) )
        debug() << " -> #velo phi " << numVeloPhi << " failed cut" << endreq;
      return false;
    }
    if ( numVeloR < m_minNVeloRHits ) {
      if ( msgLevel(MSG::DEBUG) )
        debug() << " -> #velo R " << numVeloR << " failed cut" << endreq;
      return false;
    }
  }

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
