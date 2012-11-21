// $Id: TrackMatchChecker.cpp,v 1.9 2010-03-24 12:10:01 rlambert Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/Tuples.h"

// from LinkerEvent
#include "Linker/LinkedFrom.h"
#include "Linker/LinkedTo.h"

// from MCEvent
#include "Event/MCParticle.h"

// from STDet
#include "STDet/DeTTDetector.h"

// from TrackFitEvent
#include "Event/STMeasurement.h"
#include "Event/STCluster.h"
#include "Event/Node.h"

// local
#include "TrackMatchChecker.h"

DECLARE_ALGORITHM_FACTORY( TrackMatchChecker )

/** @file TrackMatchChecker.cpp 
 *
 *  Implementation of TrackMatchChecker algorithm
 *  
 *  @author Jeroen van Tilburg Jeroen.van.Tilburg@cern.ch
 *  @date   15-05-2006
 */

using namespace Gaudi;
using namespace LHCb;

TrackMatchChecker::TrackMatchChecker( const std::string& name,
                                      ISvcLocator* pSvcLocator) :
  GaudiTupleAlg(name, pSvcLocator),
  m_extrapolatorVelo(0),
  m_extrapolatorSeed(0),
  m_chi2Calculator(0),
  m_stateCreator(0)
{
  declareProperty( "MatchAtZPosition", m_matchAtZPosition = 830.0 );
  declareProperty( "VariableZ",        m_variableZ        = false );
  declareProperty( "VarZParameters",   m_varZParameters           );
  declareProperty( "ExtrapolatorVelo",
                   m_extrapolatorVeloName = "TrackLinearExtrapolator" );
  declareProperty( "ExtrapolatorSeed",
                   m_extrapolatorSeedName = "TrackHerabExtrapolator"  );
  declareProperty( "InputVeloTracks",  m_veloTracks  = TrackLocation::Velo  );
  declareProperty( "InputSeedTracks",  m_seedTracks  = TrackLocation::Tsa   );
  declareProperty( "InputMatchTracks", m_matchTracks = TrackLocation::Match );
  declareProperty( "VeloLinker", m_veloLinker = TrackLocation::Velo );
  declareProperty( "SeedLinker", m_seedLinker = TrackLocation::Tsa  );
  declareProperty( "TTClusterTool",
                   m_ttClusterToolName = "AddTTClusterTool" );
}

TrackMatchChecker::~TrackMatchChecker()
{
  // TrackMatchChecker destructor
}

StatusCode TrackMatchChecker::initialize()
{
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if( sc.isFailure() ) return sc;

  // Reset global counters
  m_countTotal   = 0;
  m_countMCTotal = 0;
  m_countGood    = 0;
  m_countTTGood  = 0;
  m_countTTBad   = 0;

  // Access the extrapolators tools
  m_extrapolatorVelo = tool<ITrackExtrapolator>( m_extrapolatorVeloName );
  m_extrapolatorSeed = tool<ITrackExtrapolator>( m_extrapolatorSeedName );

  // Access the chi2 calculator tool
  m_chi2Calculator = tool<ITrackChi2Calculator>( "TrackChi2Calculator", this );

  // Retrieve IdealCreator tool for resolutions and pulls along the track
  m_stateCreator = tool<IIdealStateCreator>( "IdealStateCreator" );

  // Tool to add TT clusters to a track
  m_addTTClusterTool = tool<IAddTTClusterTool>( m_ttClusterToolName,
                                                "TTClusterTool",  this );


  // end
  return StatusCode::SUCCESS;
}

StatusCode TrackMatchChecker::writeNTuples()
{
  // Cheated tracks part NTuple
  Tuples::Tuple nt1 = GaudiTupleAlg::nTuple( 1, "TrackMatchChecker",
                                             CLID_ColumnWiseTuple );
  nt1->column ("NMatch", m_numMatches );
  nt1->column ("NMCMatch", m_numMCMatches );
  nt1->column ("NGood", m_numGood );
  nt1->column ("NVeloClone", m_numVeloClones );
  nt1->column ("NSeedClone", m_numSeedClones );
  nt1->column ("NVeloGhost", m_numVeloGhost );
  nt1->column ("NSeedGhost", m_numSeedGhost );
  nt1->column ("NVelotracks", m_numVeloTracks );
  nt1->column ("NSeedtracks", m_numSeedTracks );
  nt1->farray( "Chi2", m_chi2Match.begin(), m_chi2Match.end(), "nMatch", 1000 );
  nt1->farray( "Good", m_goodMatch.begin(), m_goodMatch.end(), "nMatch", 1000 );
  nt1->farray( "Electron", m_electron.begin(), m_electron.end(), 
               "nMatch", 1000 );
  nt1->farray( "matchZPos", m_matchZPos.begin(), m_matchZPos.end(), 
               "nMatch", 1000 );
  nt1->farray( "HitsVelo", m_numOfHitsVelo.begin(), m_numOfHitsVelo.end(), 
               "nMatch", 1000 );
  nt1->farray( "HitsSeed", m_numOfHitsSeed.begin(), m_numOfHitsSeed.end(), 
               "nMatch", 1000 );
  nt1->farray( "ZPosVelo", m_zPosLastVeloHit.begin(), m_zPosLastVeloHit.end(), 
               "nMatch", 1000 );
  nt1->farray( "TrueMom", m_trueMomentum.begin(), m_trueMomentum.end(), 
               "nMatch", 1000 );
  nt1->farray( "VeloMCMatch", m_veloMCMatch.begin(), m_veloMCMatch.end(),
               "nMatch", 1000 );
  nt1->farray( "SeedMCMatch", m_seedMCMatch.begin(), m_seedMCMatch.end(),
               "nMatch", 1000 );
  nt1->farray( "nTT", m_nTT.begin(), m_nTT.end(), "nMatch", 1000 );
  nt1->farray( "purityTT", m_purityTT.begin(), m_purityTT.end(), 
               "nMatch", 1000 );
  nt1->farray( "XVelo", m_xVelo.begin(), m_xVelo.end(), "nMatch", 1000 );
  nt1->farray( "YVelo", m_yVelo.begin(), m_yVelo.end(), "nMatch", 1000 );
  nt1->farray( "TxVelo", m_txVelo.begin(), m_txVelo.end(), "nMatch", 1000 );
  nt1->farray( "TyVelo", m_tyVelo.begin(), m_tyVelo.end(), "nMatch", 1000 );
  nt1->farray( "PVelo", m_momentumVelo.begin(), m_momentumVelo.end(),
               "nMatch", 1000 );
  nt1->farray( "EXVelo", m_eXVelo.begin(), m_eXVelo.end(), "nMatch", 1000 );
  nt1->farray( "EYVelo", m_eYVelo.begin(), m_eYVelo.end(), "nMatch", 1000 );
  nt1->farray( "ETxVelo", m_eTxVelo.begin(), m_eTxVelo.end(), "nMatch", 1000 );
  nt1->farray( "ETyVelo", m_eTyVelo.begin(), m_eTyVelo.end(), "nMatch", 1000 );
  nt1->farray( "EPVelo", m_eMomentumVelo.begin(), m_eMomentumVelo.end(), 
               "nMatch", 1000 );

  nt1->farray( "XSeed", m_xSeed.begin(), m_xSeed.end(), "nMatch", 1000 );
  nt1->farray( "YSeed", m_ySeed.begin(), m_ySeed.end(), "nMatch", 1000 );
  nt1->farray( "TxSeed", m_txSeed.begin(), m_txSeed.end(), "nMatch", 1000 );
  nt1->farray( "TySeed", m_tySeed.begin(), m_tySeed.end(), "nMatch", 1000 );
  nt1->farray( "PSeed", m_momentumSeed.begin(), m_momentumSeed.end(), 
               "nMatch", 1000 );
  nt1->farray( "EXSeed", m_eXSeed.begin(), m_eXSeed.end(), "nMatch", 1000 );
  nt1->farray( "EYSeed", m_eYSeed.begin(), m_eYSeed.end(), "nMatch", 1000 );
  nt1->farray( "ETxSeed", m_eTxSeed.begin(), m_eTxSeed.end(), "nMatch", 1000 );
  nt1->farray( "ETySeed", m_eTySeed.begin(), m_eTySeed.end(), "nMatch", 1000 );
  nt1->farray( "EPSeed", m_eMomentumSeed.begin(), m_eMomentumSeed.end(), 
               "nMatch", 1000 );
  nt1->farray( "XTrue", m_xTrue.begin(), m_xTrue.end(), "nMatch", 1000 );
  nt1->farray( "YTrue", m_yTrue.begin(), m_yTrue.end(), "nMatch", 1000 );
  nt1->farray( "TxTrue", m_txTrue.begin(), m_txTrue.end(), "nMatch", 1000 );
  nt1->farray( "TyTrue", m_tyTrue.begin(), m_tyTrue.end(), "nMatch", 1000 );
  nt1->farray( "PTrue", m_pTrue.begin(), m_pTrue.end(), "nMatch", 1000 );

  StatusCode status = nt1->write();
  if( status.isFailure() ) return Error( "Cannot fill ntuple" );

  // MC match tracks part NTuple
  Tuples::Tuple nt2 = GaudiTupleAlg::nTuple( 2, "TrackMatchChecker",
                                             CLID_ColumnWiseTuple );
  nt2->column("NMCMatch", m_numMCMatches );
  nt2->column("NMatch", m_numMatches );
  nt2->column("NGood", m_numGood );
  nt2->column("NVeloClone", m_numVeloClones );
  nt2->column("NSeedClone", m_numSeedClones );
  nt2->farray("Electron", m_mcElectron.begin(), m_mcElectron.end(), 
              "nMCMatch", 1000 );
  nt2->farray("TrueChi2", m_mcChi2.begin(), m_mcChi2.end(), 
              "nMCMatch", 1000 );
  nt2->farray("TrueMom", m_mcTrueMomentum.begin(), m_mcTrueMomentum.end(),
              "nMCMatch", 1000 );
  nt2->farray("nTT", m_mcnTT.begin(), m_mcnTT.end(), "nMCMatch", 1000 );

  status = nt2->write();
  if( status.isFailure() ) return Error( "Cannot fill ntuple" );

  // end
  return StatusCode::SUCCESS;
}


StatusCode TrackMatchChecker::finalize()
{
  info() << "Results matching: Total=" << m_countTotal 
         << " MCTotal=" << m_countMCTotal
         << " Good=" << m_countGood
         << " Ghost=" << m_countTotal - m_countGood
         << endmsg;
  info() << "Matching efficiency= " << (float) m_countGood/m_countMCTotal
         << " ghost rate= " << 1. - (float) m_countGood/m_countTotal
         << endmsg;
  info() << "Total number of correct TT hits = " << m_countTTGood << endmsg;
  info() << "Total number of wrong TT hits = " << m_countTTBad << endmsg;

  debug() << "finalized succesfully" << endmsg;

  return GaudiTupleAlg::finalize();
}


StatusCode TrackMatchChecker::execute()
{
  // Initialize
  StatusCode sc;

  // match the tracks using MC information
  sc = matchMCTracks();
  if ( sc.isFailure() )
    return Error("Unable to match velo with T tracks using MC info.");
  
  // Monitor the performance and fill the ntuples
  sc = monitor();
  if ( sc.isFailure() ) return Error( "Unable to execute monitoring");

  // write NTuples
  sc = this->writeNTuples();
  if ( sc.isFailure() ) return Error( "Failed writing NTuples" );

  // end
  return StatusCode::SUCCESS;
}


//=============================================================================
// Match the two trackcontainers using the track associators
// If both tracks point at the same true track a mcMatch is added to
// the mcMatchContainer.
//=============================================================================
StatusCode TrackMatchChecker::matchMCTracks( )
{ 
  StatusCode sc;
  // Reset clone and ghost counters
  m_numVeloClones = 0;
  m_numSeedClones = 0;
  m_numVeloGhost = 0;
  m_numSeedGhost = 0;
  m_numMCMatches = 0; 

  // Get the tracks
  Tracks* veloTracks  = get<Tracks>( m_veloTracks );
  Tracks* seedTracks  = get<Tracks>( m_seedTracks );
  debug() << "retrieved succesfully " << veloTracks -> size()
          << " velo tracks" << endmsg;
  debug() << "retrieved succesfully " << seedTracks -> size()
          << " seed tracks" << endmsg;
  m_numVeloTracks = veloTracks->size();
  m_numSeedTracks = seedTracks->size();  

  // Retrieve the velo Linker table
  LinkedTo<MCParticle, Track> veloLink( evtSvc(), msgSvc(), m_veloLinker);
  if( veloLink.notFound() ) return Error( "Velo linker table not found." );

  // Retrieve the seed Linker table
  LinkedTo<MCParticle, Track> seedLink( evtSvc(), msgSvc(), m_seedLinker);
  if( seedLink.notFound() ) return Error( "Seed linker table not found." );

  // Retrieve the TT linker table
  LinkedFrom<STCluster,MCParticle>
    ttLinkInv( evtSvc(), msgSvc(), STClusterLocation::TTClusters );
  if ( ttLinkInv.notFound() ) {
    return Error( "Unable to retrieve TTCluster to MCParticle Linker table");
  }

  // Count number of velo ghost
  Tracks::const_iterator veloTrack = veloTracks->begin();
  for ( ; veloTrack != veloTracks->end(); ++veloTrack ) {
    // Get the MCParticle
    MCParticle* mcpart = veloLink.first( *veloTrack );
    if ( mcpart != 0 ) m_numVeloClones += veloLink.range(*veloTrack).size() - 1;
    else ++m_numVeloGhost;
  }
  
  debug() << "Loop over Seed tracks to find all MC combinations" 
          << " with velo tracks" << endmsg;
  Tracks::const_iterator seedTrack = seedTracks->begin();
  for ( ; seedTrack != seedTracks->end(); ++seedTrack ) {

    // Get the MCParticle of the T seed
    MCParticle* mcpart = seedLink.first( *seedTrack );
    if ( mcpart != 0 ) {
      m_numSeedClones += seedLink.range( *seedTrack ).size() - 1;
    } else {
      ++m_numSeedGhost;
      continue;
    }
    
    // Get the MCParticle of the velo
    bool found = false;
    veloTrack = veloTracks->begin();
    while ( veloTrack != veloTracks->end() && !found ) {
      if ( mcpart == veloLink.first( *veloTrack ) ) found = true;
      else ++veloTrack;
    }
    if ( !found ) continue;

    // Get the matching z-position
    double matchZ = (m_variableZ) ? determineZ( *seedTrack ) : 
      m_matchAtZPosition;

    // Extrapolate seedTrack to the actual matching z position
    TrackVector seedVector;
    TrackSymMatrix seedCov;
    sc = extrapolate( *seedTrack, m_extrapolatorSeed, 
                      matchZ, seedVector, seedCov);
    if ( sc.isFailure() ) continue;

    // Extrapolate veloTrack to the actual matching z position
    TrackVector veloVector;
    TrackSymMatrix veloCov;
    sc = extrapolate( *veloTrack, m_extrapolatorVelo, 
                      matchZ, veloVector, veloCov);
    if ( sc.isFailure() ) continue;

    // Calculate the chi2 distance between 2 tracks
    double chi2 = 0.0;
    sc = m_chi2Calculator->calculateChi2( seedVector, seedCov, 
                                          veloVector, veloCov, chi2);
    if ( sc.isFailure() ) continue;
    
    // fill mc ntuple
    m_mcChi2.push_back( (float) chi2 );
    m_mcTrueMomentum.push_back(  (float) mcpart->p() );
    m_mcElectron.push_back( mcpart->particleID().isLepton() );

    // Count TT clusters
    int numTTClusters = 0;
    STCluster* aCluster = ttLinkInv.first( mcpart );
    while ( aCluster != 0 ) {
      ++numTTClusters;
      aCluster = ttLinkInv.next();
    }
    m_mcnTT.push_back( numTTClusters );

    ++m_numMCMatches;
  } // Loop over Seed tracks

  debug() << "Found " << m_numMCMatches
          << " matches between velo and Seed in MC." << endmsg;

  return StatusCode::SUCCESS;
}


// monitor the matching efficiency and ghost rate
StatusCode TrackMatchChecker::monitor( )
{
  // Initialize
  StatusCode sc;
  m_numGood = 0;
  m_numMatches = 0;
  int countTTGood = 0;
  int countTTBad = 0;  

  // Retrieve matched tracks from EvDS
  Tracks* matchTracks = get<Tracks>( m_matchTracks );
  debug() << "retrieved succesfully " << matchTracks->size()
          << " matched tracks" << endmsg;

  // Retrieve the velo Linker table
  LinkedTo<MCParticle, Track> veloLink( evtSvc(), msgSvc(), m_veloLinker);
  if ( veloLink.notFound() ) return Error( "Velo linker table not found." );

  // Retrieve the seed Linker table
  LinkedTo<MCParticle, Track> seedLink( evtSvc(), msgSvc(), m_seedLinker);
  if( seedLink.notFound() ) return Error( "Seed linker table not found.");

  // Retrieve the TT linker table
  LinkedTo<MCParticle, STCluster>
    ttLink( evtSvc(), msgSvc(), STClusterLocation::TTClusters );
  if ( ttLink.notFound() ) {
    return Error( "Unable to retrieve TTCluster to MCParticle Linker table");
  }

  Tracks::const_iterator iMatch = matchTracks->begin();
  for ( ; iMatch != matchTracks->end(); ++iMatch) {
    bool goodMatch = false;

    // Get the original Velo and Seed track
    SmartRefVector<Track>::const_iterator iAncestor =
      ((*iMatch) -> ancestors()).begin();
    const Track* veloTrack = *iAncestor;
    ++iAncestor;
    const Track* seedTrack = *iAncestor;
      
    // Check whether this is a good match (also in MC)
    if ( veloLink.first( veloTrack ) == seedLink.first( seedTrack ) ) {
      goodMatch = true;
    }

    // Get the matching z-position
    double matchZ = (m_variableZ) ? determineZ( seedTrack ) : 
      m_matchAtZPosition;

    // get the velo state
    TrackVector veloVector;
    TrackSymMatrix veloCov;
    sc = extrapolate( veloTrack, m_extrapolatorVelo,
                      matchZ, veloVector, veloCov);
    if ( sc.isFailure() ) continue;

    // Get the seed state
    TrackVector seedVector;
    TrackSymMatrix seedCov;
    sc = extrapolate( seedTrack, m_extrapolatorSeed, 
                      matchZ, seedVector, seedCov);
    if ( sc.isFailure() ) continue;

    double ptkick =  1.0/fabs(seedVector[4]) ;    

    // Calculate the chi2
    double chi2 = 0.0;
    sc = m_chi2Calculator->calculateChi2( seedVector, seedCov, 
                                          veloVector, veloCov, chi2);
    if ( sc.isFailure() ) continue;

    // get last measurement from velo
    double lastVeloZPos = 0.0;
    LHCb::Track::ConstNodeRange nodes = veloTrack->nodes() ;
    for( LHCb::Track::ConstNodeRange::iterator inode = nodes.begin() ;
	 inode != nodes.end(); ++inode ) 
      if( (*inode)->hasMeasurement() && (*inode)->z() > lastVeloZPos )
	lastVeloZPos = (*inode)->z() ;
    
    // retrieve MCParticle Velo
    m_veloMCMatch.push_back( true );
    MCParticle* mcpartVelo = veloLink.first( veloTrack );
    if ( mcpartVelo == 0 ) {
      verbose() << "Failed to find the MCParticle corresponding"
                << " to the velotrack" << endmsg;
      m_veloMCMatch.push_back( false );
    }    

    // retrieve MCParticle Seed
    m_seedMCMatch.push_back( true );
    MCParticle* mcpartSeed = seedLink.first( seedTrack );
    if ( mcpartSeed == 0 ) {
      verbose() << "Failed to find the MCParticle corresponding"
                << " to the seedtrack" << endmsg;
      m_seedMCMatch.push_back( false );
      m_trueMomentum.push_back( 0.0 );

      m_xTrue.push_back( 0.0 );
      m_yTrue.push_back( 0.0 );
      m_txTrue.push_back( 0.0 );
      m_tyTrue.push_back( 0.0 );
      m_pTrue.push_back( 0.0 );
      m_electron.push_back( false );
    } else {
      m_trueMomentum.push_back( (float) mcpartSeed->p() );
      m_electron.push_back( mcpartSeed->particleID().isLepton() );

      // Get the true state
      State trueState;
      StatusCode sc = m_stateCreator->createState(mcpartSeed,matchZ,trueState);
      if ( sc.isSuccess() ) {
        TrackVector trueVec = trueState.stateVector();
        m_xTrue.push_back(  float(trueVec[0]) );
        m_yTrue.push_back(  float(trueVec[1]) );
        m_txTrue.push_back( float(trueVec[2]) );
        m_tyTrue.push_back( float(trueVec[3]) );
        m_pTrue.push_back(  float(1./fabs(trueVec[4])) );
      } else {
        m_xTrue.push_back( 0.0 );
        m_yTrue.push_back( 0.0 );
        m_txTrue.push_back( 0.0 );
        m_tyTrue.push_back( 0.0 );
        m_pTrue.push_back( 0.0 );
      }
    }
    if ( goodMatch ) m_numGood++;

    m_goodMatch.push_back( goodMatch );
    m_chi2Match.push_back( (float)  chi2 );
    m_matchZPos.push_back(  (float) matchZ );
    m_numOfHitsVelo.push_back( veloTrack->nMeasurements() );
    m_numOfHitsSeed.push_back( seedTrack->nMeasurements() );
    m_zPosLastVeloHit.push_back(  (float) lastVeloZPos );
    
    // loop over TT clusters
    int purity = 0;
    int nClus = 0;

    LHCb::Track::MeasurementContainer allMeas = (*iMatch)->measurements() ;
    LHCb::Track::MeasurementContainer::const_iterator iMeas = allMeas.begin();
    while ( iMeas != allMeas.end() ) {
      if ( (*iMeas)->checkType( Measurement::TT ) ) {
        const STMeasurement* ttMeas= dynamic_cast<const STMeasurement*>(*iMeas);
        const STCluster* ttCluster = ttMeas -> cluster();
        MCParticle* ttPart = ttLink.first( ttCluster ) ;
        double ttChi2 = m_addTTClusterTool -> distanceToStrip( *(*iMatch), 
                                                               *ttCluster );
        if ( ttPart == mcpartVelo ) {
          ++purity;
          ++countTTGood;
          STChannelID channel = ttCluster->channelID();
          debug() << "  Good TT hit " << goodMatch << " "
                  << channel.station() << " " << channel.layer() << " "
                  << channel.detRegion() << " " << channel.sector() << " "
                  << channel.strip() << ",  "
                  << ttChi2  << " p=" << ptkick << endmsg;
          plot( ttChi2, "Chi2 of good TT hits", 0.0, 20., 50);
        } else {
          ++countTTBad;
          STChannelID channel = ttCluster->channelID();
          debug() << "  Bad TT hit  " << goodMatch << " "
                  << channel.station() << " " << channel.layer() << " "
                  << channel.detRegion() << " " << channel.sector() << " "
                  << " " << channel.strip() << ",  "
                  << ttChi2 << " p=" << ptkick << endmsg;
          plot( ttChi2 , "Chi2 of wrong TT hits", 0.0, 20., 50);
        }
        ++nClus;
      }
      ++iMeas;
    }
    debug() << "-------------------------------------------" << endmsg;

    m_nTT.push_back( nClus );
    if ( nClus != 0 ) {
      m_purityTT.push_back( (float) purity / (float) nClus );
    }

    m_xVelo.push_back(  (float) veloVector[0] );
    m_yVelo.push_back(  (float) veloVector[1] );
    m_txVelo.push_back(  (float) veloVector[2] );
    m_tyVelo.push_back(  (float) veloVector[3] );
    m_eXVelo.push_back(  (float) sqrt(veloCov(0,0)) );
    m_eYVelo.push_back(  (float) sqrt(veloCov(1,1)) );
    m_eTxVelo.push_back(  (float) sqrt(veloCov(2,2)) );
    m_eTyVelo.push_back(  (float) sqrt(veloCov(3,3)) );
    if ( veloVector.Dim() < 5 ) {
      m_momentumVelo.push_back( 0.0 );
      m_eMomentumVelo.push_back( 0.0 );
    }
    else {
      m_momentumVelo.push_back( (float) (1.0/fabs(veloVector[4])) );
      m_eMomentumVelo.push_back( (float) (sqrt(veloCov(4,4)) / pow(veloVector[4],2)) );
    }

    m_xSeed.push_back( (float) seedVector[0] );
    m_xRich2.push_back( (float) (seedTrack->closestState(9450.).stateVector())[0] );
    m_ySeed.push_back( (float) seedVector[1] );
    m_yRich2.push_back( (float) (seedTrack->closestState(9450.).stateVector())[1] );
    m_txSeed.push_back( (float) seedVector[2] );
    m_txRich2.push_back( (float) (seedTrack->closestState(9450.).stateVector())[2] );
    m_tySeed.push_back( (float) seedVector[3] );
    m_tyRich2.push_back( (float) (seedTrack->closestState(9450.).stateVector())[3] );
    m_momentumSeed.push_back( (float) ptkick );
    m_eXSeed.push_back( (float) sqrt(seedCov(0,0)) );
    m_eYSeed.push_back( (float) sqrt(seedCov(1,1)) );
    m_eTxSeed.push_back( (float) sqrt(seedCov(2,2)) );
    m_eTySeed.push_back( (float) sqrt(seedCov(3,3)) );
    m_eMomentumSeed.push_back( float(sqrt(fabs(seedCov(4,4))) / pow(seedVector[4],2)) );
    ++m_numMatches;
  }

  // fill histogram  
  plot(1.0, "Efficiency and ghost rate", -2.5, 1.5, 4, (double) m_numGood );
  plot(-1.0, "Efficiency and ghost rate",
       -2.5, 1.5, 4, (double) m_numMCMatches - m_numGood );
  plot(-2.0, "Efficiency and ghost rate", 
       -2.5, 1.5, 4, (double) m_numMatches - m_numGood );

  // count global efficiency and ghost rate.
  m_countTotal += m_numMatches;
  m_countMCTotal += m_numMCMatches;
  m_countGood  += m_numGood;
  m_countTTGood += countTTGood;
  m_countTTBad += countTTBad;

  debug() << "Total number of good matches= " << m_numGood << endmsg;

  return StatusCode::SUCCESS;
} 


//=============================================================================
// Extrapolate a Track to a z-position starting with the closest State
//=============================================================================
StatusCode TrackMatchChecker::extrapolate( const LHCb::Track* track,
                                           ITrackExtrapolator* extrapolator,
                                           double zpos,
                                           Gaudi::TrackVector& trackVector,
                                           Gaudi::TrackSymMatrix& trackCov )
{
  State tmpState;
  StatusCode sc = extrapolator -> propagate( *track, zpos, tmpState );
  if ( sc.isFailure() ) return sc;
  trackVector = tmpState.stateVector();
  trackCov    = tmpState.covariance();

  return StatusCode::SUCCESS;  
}

//=============================================================================
// Calculate the new z
//=============================================================================
double TrackMatchChecker::determineZ( const Track* track )
{
  const State& seedState = track->closestState(9450.);
  double tX = seedState.tx();
  double zNew = m_matchAtZPosition ;
  if ( m_varZParameters.size() == 3 ) {
    zNew += m_varZParameters[0] + 
            m_varZParameters[1] * tX + 
            m_varZParameters[2] * pow(tX,2) ;
  }
  return zNew;
}
