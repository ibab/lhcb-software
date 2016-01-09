//-----------------------------------------------------------------------------
/** @file TrackSelector.cpp
 *
 *  Implementation file for reconstruction tool : TrackSelector
 *
 *  @author M.Needham Matt.Needham@cern.ch
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/12/2005
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/SystemOfUnits.h"

// Tsa
#include "TrackSelector.h"
#include "Kernel/HitPattern.h"
#include "Event/KalmanFitResult.h"

using namespace LHCb;

DECLARE_TOOL_FACTORY( TrackSelector )

//-----------------------------------------------------------------------------

TrackSelector::TrackSelector( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : TrackSelectorBase ( type, name, parent )
{

  // cut options

  declareProperty( "MinPCut",    m_minPCut     = 0.0 ); // in GeV
  declareProperty( "MaxPCut",    m_maxPCut     = -1 ); // in GeV

  declareProperty( "MinPtCut",   m_minPtCut    = 0.0 ); // in GeV
  declareProperty( "MaxPtCut",   m_maxPtCut    = -1); // in GeV

  declareProperty( "MinChi2Cut", m_minChi2Cut  = -1 );
  declareProperty( "MaxChi2Cut", m_maxChi2Cut  = -1 );

  declareProperty( "MaxChi2PerDoFMatch",      m_maxChi2Match  = -1 );
  declareProperty( "MaxChi2PerDoFUpstream",   m_maxChi2Upstream  = -1 );
  declareProperty( "MaxChi2PerDoFDownstream", m_maxChi2Downstream  = -1 );
  declareProperty( "MaxChi2PerDoFVelo",       m_maxChi2Velo  = -1 );

  declareProperty( "MinHitCut",  m_minHitCut   = 0.0 );
  declareProperty( "MaxHitCut",  m_maxHitCut   = boost::numeric::bounds<double>::highest() );

  declareProperty( "MinEtaCut",  m_minEtaCut   = boost::numeric::bounds<double>::lowest()  );
  declareProperty( "MaxEtaCut",  m_maxEtaCut   = boost::numeric::bounds<double>::highest() );

  declareProperty( "MinPhiCut",  m_minPhiCut   = boost::numeric::bounds<double>::lowest()  );
  declareProperty( "MaxPhiCut",  m_maxPhiCut   = boost::numeric::bounds<double>::highest() );

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
  declareProperty( "MaxNVeloHoles",   m_maxNVeloHoles     = 999 ) ;
  declareProperty( "MinNOTHits",      m_minNOTHits        = 0 ) ;
  declareProperty( "MinNTTHits",      m_minNTTHits        = 0 ) ;
  declareProperty( "MaxNTHoles",      m_maxNTHoles = 999 ) ;

}

TrackSelector::~TrackSelector() { }

bool TrackSelector::accept ( const Track& aTrack ) const
{
  
  // Use a try block to catch exceptions from Track and/or State classes
  try
  {
    
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Trying Track " << aTrack.key() << " " << aTrack.type();
      if ( !aTrack.states().empty() )
        verbose() << " P=" << aTrack.p() << " Pt=" << aTrack.pt();
      verbose()  << endmsg;
    }

    // NDOF
    const int ndof = aTrack.nDoF();
    if ( ndof < m_minNDoF || ndof > m_maxNDoF )
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> NDoF " << aTrack.nDoF() << " failed cut" << endmsg;
      return false;
    }

    // chi-squared
    double chi2 = aTrack.chi2PerDoF();
    if ( (m_maxChi2Cut>=0 && (chi2 > m_maxChi2Cut || aTrack.nDoF()<=0 ) )  ||
         (m_minChi2Cut>=0 && (chi2 < m_minChi2Cut || aTrack.nDoF()<=0 ) ) )
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> Chi^2 " << chi2 << " failed cut" << endmsg;
      return false;
    }

    const LHCb::KalmanFitResult* kalfit = dynamic_cast<const LHCb::KalmanFitResult*>(aTrack.fitResult() ) ;
    if(kalfit) {
      if( m_maxChi2Velo > 0 && (chi2=kalfit->chi2Velo().chi2PerDoF()) > m_maxChi2Velo ) {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " -> Velo Chi^2 " << chi2 << " failed cut" << endmsg;
        return false;
      }
      if( m_maxChi2Upstream > 0 && (chi2=kalfit->chi2Upstream().chi2PerDoF()) > m_maxChi2Upstream ) {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " -> Upstream Chi^2 " << chi2 << " failed cut" << endmsg;
        return false;
      }
      if( m_maxChi2Downstream > 0 && (chi2=kalfit->chi2Downstream().chi2PerDoF()) > m_maxChi2Downstream ) {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " -> Downstream Chi^2 " << chi2 << " failed cut" << endmsg;
        return false;
      }
      if( m_maxChi2Match > 0 && (chi2=kalfit->chi2Match().chi2PerDoF()) > m_maxChi2Match ) {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " -> Match Chi^2 " << chi2 << " failed cut" << endmsg;
        return false;
      }
    }

    // cut p
    const double p = aTrack.p() ;
    if ( p < m_minPCut || ( m_maxPCut > 0 && p > m_maxPCut) )
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> P " << aTrack.p() << " failed cut" << endmsg;
      return false;
    }

    // cut on pt
    const double pt = aTrack.pt() ;
    if ( pt < m_minPtCut || ( m_maxPtCut > 0 && pt > m_maxPtCut ) )
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> Pt " << aTrack.pt() << " failed cut" << endmsg;
      return false;
    }

    // track types
    if ( !checkTrackType(aTrack) ) return false;

    // eta
    const double eta = aTrack.pseudoRapidity();
    if ( eta < m_minEtaCut || eta > m_maxEtaCut )
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> #eta " << eta << " failed cut" << endmsg;
      return false;
    }

    // phi
    const double phi = aTrack.phi();
    if ( phi < m_minPhiCut || phi > m_maxPhiCut )
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> #phi " << phi << " failed cut" << endmsg;
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
        verbose() << " -> #hits " << nMeas << " failed cut" << endmsg;
      return false;
    }

    // count number of OT, velo phi and R hits
    if ( !checkNHits(aTrack) ) return false;

    // if get here track is selected !
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> Track selected" << endmsg;

  }
  catch ( const GaudiException & excpt )
  {
    // print the exception message as a warning and reject the track
    std::ostringstream mess;
    mess << "GaudiException caught " << excpt.message() << " " 
         << excpt.tag() << " -> Track rejected";
    Warning( mess.str() ).ignore();
    return false;
  }
  catch ( const std::exception & excpt )
  {
    // print the exception message as a warning and reject the track
    std::ostringstream mess;
    mess << "std::exception caught " << excpt.what() << " -> Track rejected";
    Warning( mess.str() ).ignore();
    return false;
  }

  // return OK
  return true;
}

bool TrackSelector::checkNHits( const LHCb::Track & aTrack ) const
{
  // count number of OT, velo phi and R hits
  if ( m_minNVeloPhiHits > 0 ||
       m_minNVeloRHits   > 0 ||
       m_minNOTHits      > 0 ||
       m_minNTTHits      > 0 )
  {
    int numVeloPhi(0), numVeloR(0), numTT(0), numOT(0) ;
    for( std::vector<LHCbID>::const_iterator it = aTrack.lhcbIDs().begin() ;
         it != aTrack.lhcbIDs().end(); ++it )
    {
      if     ( it->isVeloPhi() ) { ++numVeloPhi ; }
      else if( it->isVeloR()   ) { ++numVeloR ;   }
      else if( it->isOT()      ) { ++numOT ;      }
      else if( it->isTT()      ) { ++numTT ;      }
    }

    if ( numVeloPhi < m_minNVeloPhiHits )
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> #velo phi " << numVeloPhi << " failed cut" << endmsg;
      return false;
    }
    if ( numVeloR < m_minNVeloRHits )
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> #velo R " << numVeloR << " failed cut" << endmsg;
      return false;
    }
    if ( numOT < m_minNOTHits )
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> #OT " << numOT << " failed cut" << endmsg;
      return false;
    }
    if ( numTT < m_minNTTHits )
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> #TT " << numTT << " failed cut" << endmsg;
      return false;
    }
  }
  
  if( m_maxNVeloHoles < 99 || m_maxNTHoles < 99 ) {
    LHCb::HitPattern hitpattern(aTrack.lhcbIDs()) ;
    int numVeloHoles = hitpattern.numVeloHoles() ;
    if( numVeloHoles > m_maxNVeloHoles ) {
      if ( msgLevel(MSG::VERBOSE) )
	verbose() << " -> #VeloHoles " << numVeloHoles << " failed cut" << endmsg;
      return false;
    }
    int numTHoles = hitpattern.numTHoles() ;
    if( numTHoles > m_maxNTHoles ) {
      if ( msgLevel(MSG::VERBOSE) )
	verbose() << " -> #THoles " << numTHoles << " failed cut" << endmsg;
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
