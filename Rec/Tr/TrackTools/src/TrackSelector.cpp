
//-----------------------------------------------------------------------------
/** @file TrackSelector.cpp
 *
 *  Implementation file for reconstruction tool : TrackSelector
 *
 *  CVS Log :-
 *  $Id: TrackSelector.cpp,v 1.25 2009-08-06 18:19:10 smenzeme Exp $
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
                              const IInterface* parent )
  : TrackSelectorBase ( type, name, parent )
{

  // cut options

  declareProperty( "MinPCut",    m_minPCut     = 0.0 ); // in GeV
  declareProperty( "MaxPCut",    m_maxPCut     = boost::numeric::bounds<double>::highest() ); // in GeV

  declareProperty( "MinPtCut",   m_minPtCut    = 0.0 ); // in GeV
  declareProperty( "MaxPtCut",   m_maxPtCut    = boost::numeric::bounds<double>::highest() ); // in GeV

  declareProperty( "MinChi2Cut", m_minChi2Cut  = -1 );
  declareProperty( "MaxChi2Cut", m_maxChi2Cut  = -1 );

  declareProperty( "MinHitCut",  m_minHitCut   = 0.0 );
  declareProperty( "MaxHitCut",  m_maxHitCut   = boost::numeric::bounds<double>::highest() );

  declareProperty( "MinEtaCut",  m_minEtaCut   = boost::numeric::bounds<double>::lowest()  );
  declareProperty( "MaxEtaCut",  m_maxEtaCut   = boost::numeric::bounds<double>::highest() );

  declareProperty( "MinNDoF",    m_minNDoF     = 0 ) ;
  declareProperty( "MaxNDoF",    m_maxNDoF     = boost::numeric::bounds<int>::highest() ) ;

  declareProperty( "MinLikelihoodCut", m_minLikCut =  boost::numeric::bounds<double>::lowest());
  declareProperty( "MaxLikelihoodCut", m_maxLikCut =  boost::numeric::bounds<double>::highest());

  declareProperty( "AcceptClones", m_acceptClones = true );

  declareProperty( "MinCloneDistCut", m_minCloneCut    = -1e10 );
  declareProperty( "MaxCloneDistCut", m_maxCloneCut    = boost::numeric::bounds<double>::highest() );

  declareProperty( "MinGhostProbCut", m_minGhostProb   = boost::numeric::bounds<double>::lowest() );
  declareProperty( "MaxGhostProbCut", m_maxGhostProb   = boost::numeric::bounds<double>::highest() );

  // "Expert" options

  declareProperty( "vWeight", m_vWeight     = 1.0 );
  declareProperty( "oWeight", m_oWeight     = 0.5 );
  declareProperty( "iWeight", m_iWeight     = 1.0 );

  declareProperty( "MinNVeloRHits",   m_minNVeloRHits     = 0 ) ;
  declareProperty( "MinNVeloPhiHits", m_minNVeloPhiHits   = 0 ) ;
  declareProperty( "MinNOTHits",      m_minNOTHits        = 0 ) ;

}

TrackSelector::~TrackSelector() { }

bool TrackSelector::accept ( const Track& aTrack ) const
{
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Trying Track " << aTrack.key() << " " << aTrack.type()
              << endreq;
  }

  // NDOF
  const int ndof = aTrack.nDoF();
  if ( ndof < m_minNDoF || ndof > m_maxNDoF )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> NDoF " << aTrack.nDoF() << " failed cut" << endreq;
    return false;
  }

  // chi-squared
  const double chi2 = aTrack.chi2PerDoF();
  if ( (m_maxChi2Cut>=0 && (chi2 > m_maxChi2Cut || aTrack.nDoF()<=0 ) )  ||
       (m_minChi2Cut>=0 && (chi2 < m_minChi2Cut || aTrack.nDoF()<=0 ) ) )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> Chi^2 " << chi2 << " failed cut" << endreq;
    return false;
  }

  // cut p
  const double p = aTrack.p() ;
  if ( p < m_minPCut || p > m_maxPCut )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> P " << aTrack.p() << " failed cut" << endreq;
    return false;
  }

  // cut on pt
  const double pt = aTrack.pt() ;
  if ( pt < m_minPtCut || pt > m_maxPtCut )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> Pt " << aTrack.pt() << " failed cut" << endreq;
    return false;
  }

  // track types
  if ( !checkTrackType(aTrack) ) return false;

  // eta
  const double eta = aTrack.pseudoRapidity();
  if ( eta < m_minEtaCut || eta > m_maxEtaCut )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> #eta " << eta << " failed cut" << endreq;
    return false;
  }

  // Clones
  const double cloneDist = aTrack.info(LHCb::Track::CloneDist,9e99);
  if ( !m_acceptClones && ( aTrack.checkFlag(LHCb::Track::Clone) ||
                            cloneDist < m_minCloneCut || cloneDist > m_maxCloneCut ) )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> Track failed clone rejection" << endmsg;
    return false;
  }

  if ( aTrack.likelihood() < m_minLikCut || aTrack.likelihood() > m_maxLikCut )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> Track Likelihood " << aTrack.likelihood() << " failed cut"
                << endmsg;
    return false;
  }

  // GhostProbability
 if ( aTrack.ghostProbability() < m_minGhostProb || aTrack.ghostProbability() > m_maxGhostProb )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> Track GhostProbability " << aTrack.ghostProbability() << " failed cut"
                << endmsg;
    return false;
  }

  // measurements
  const double nMeas = weightedMeasurementSum(aTrack);
  if ( nMeas < m_minHitCut || nMeas > m_maxHitCut )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> #hits " << nMeas << " failed cut" << endreq;
    return false;
  }

  // count number of OT, velo phi and R hits
  if ( !checkNHits(aTrack) ) return false;

  // if get here track is selected !
  if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> Track selected" << endreq;
  return true;
}

bool TrackSelector::checkNHits( const LHCb::Track & aTrack ) const
{  
  // count number of OT, velo phi and R hits
  if ( m_minNVeloPhiHits > 0 || 
       m_minNVeloRHits   > 0 ||
       m_minNOTHits      > 0 ) 
  {
    int numVeloPhi(0), numVeloR(0), numOT(0) ;
    for( std::vector<LHCbID>::const_iterator it = aTrack.lhcbIDs().begin() ;
         it != aTrack.lhcbIDs().end(); ++it )
    {
      if     ( it->isVeloPhi() ) { ++numVeloPhi ; }
      else if( it->isVeloR()   ) { ++numVeloR ;   }
      else if( it->isOT()      ) { ++numOT ;      }
    }

    if ( numVeloPhi < m_minNVeloPhiHits ) 
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> #velo phi " << numVeloPhi << " failed cut" << endreq;
      return false;
    }
    if ( numVeloR < m_minNVeloRHits ) 
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> #velo R " << numVeloR << " failed cut" << endreq;
      return false;
    }
    if ( numOT < m_minNOTHits ) 
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> #OT " << numOT << " failed cut" << endreq;
      return false;
    }
  }
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
