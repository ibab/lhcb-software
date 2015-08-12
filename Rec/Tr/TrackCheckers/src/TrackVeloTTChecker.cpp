// Include files 

// from Gaudi
#include "GaudiAlg/Tuples.h"

// From STDet
#include "STDet/DeSTLayer.h"

// from Event/LinkerEvent
#include "Linker/LinkedTo.h"

// from Event
#include "Event/ProcStatus.h"
#include "Event/MCParticle.h"
#include "Event/Track.h"
#include "Event/STMeasurement.h"
#include "Event/STCluster.h"
#include "Event/VeloRMeasurement.h"
#include "Event/VeloPhiMeasurement.h"
#include "Event/VeloCluster.h"

// local
#include "TrackVeloTTChecker.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TrackVeloTTChecker )

/// Standard constructor, initializes variables
TrackVeloTTChecker::TrackVeloTTChecker( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_nEvents(0)
{

}

/// Default destructor
TrackVeloTTChecker::~TrackVeloTTChecker() {}

/// Initialization
StatusCode TrackVeloTTChecker::initialize() {

  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if( sc.isFailure() ) return sc;
  
  debug() << "==> Initialize" << endmsg;

  // Get the TT Detector Element
  m_TTDet = getDet<DeSTDetector>( DeSTDetLocation::location("TT") );

  return StatusCode::SUCCESS;
}

/// Main execution
StatusCode TrackVeloTTChecker::execute() {

  debug() << "==> Execute" << endmsg;

  // Counter of events processed
  ++m_nEvents;
  debug() << "Processing event number " << m_nEvents << endmsg;

  ProcStatus* procStat = get<ProcStatus>( ProcStatusLocation::Default );
  if( ( procStat != 0 ) && procStat->aborted() ) {
    info() << "Processing aborted -> no VELO-TT checking." << endmsg;
    return StatusCode::SUCCESS;
  }

  Tuples::Tuple ntuple = GaudiTupleAlg::nTuple( 1, "TrackVeloTTChecker",
                                                CLID_ColumnWiseTuple );

  // Retrieve informations about event
  //  EventHeader* evt = get<EventHeader>( EventHeaderLocation::Default );
  //  debug() << "    retrieved EVENT: " << evt->evtNum()
  //          << " RUN: " << evt->runNum() << endmsg;
  
  //  ntuple->column( "Run",   evt->runNum() );
  //  ntuple->column( "Event", evt->evtNumber() );

  // Retrieve TT clusters
  STClusters* ttClusters = get<STClusters>( STClusterLocation::TTClusters );
  if( 0 == ttClusters->size() )
    info() << "No TT clusters retrieved" << endmsg;

  // ntuple variables
  int                NTnVeloTT = 0;
  std::vector<long>  NTkeyVelo;
  std::vector<long>  NTidmcVelo;
  std::vector<float> NTpmcVelo;
  std::vector<float> NTqmcVelo;
  std::vector<float> NTptmcVelo;
  std::vector<float> NTcsthmcVelo;
  std::vector<float> NTtxVelo;
  std::vector<float> NTtyVelo;
  std::vector<float> NTxVelo;
  std::vector<float> NTyVelo;
  std::vector<float> NTzVelo;
  std::vector<long>  NTnmchit1;
  std::vector<long>  NTnmchit2;
  std::vector<long>  NTrecons;
  std::vector<long>  NTnmchit1b;
  std::vector<long>  NTnmchit2b;
  std::vector<float> NTpdet;
  std::vector<float> NTptdet;
  std::vector<float> NTqdet;
  std::vector<float> NTchi2tt;
  std::vector<float> NTqfit;
  std::vector<long>  NTndf;
  std::vector<long>  NTnAllDigi;
  std::vector<long>  NTnTrueDigi;
  std::vector<long>  NTisFwd;
  std::vector<float> NTzVeloCenter;

  // Retrieve "Best" tracks
  Tracks* allTracks = get<Tracks>( TrackLocation::Default );

  debug() << "Found " << allTracks->size()
          << " tracks at " << TrackLocation::Default << endmsg;

  Tracks::iterator aTr;
  for( aTr = allTracks->begin(); allTracks->end() != aTr; ++aTr ) {
    Track* track = *aTr ;
    if( track->checkFlag( Track::Clone ) ) continue;
    if( !track->checkType( Track::Velo )
        && !track->checkType( Track::Upstream ) ) continue;

    double zcenter = CenterVeloTr( track );
    int isFwd = 0;
    if( zcenter > 0 ) isFwd = 1;

    MCParticle* mcPartVelo = VeloTrackMCTruth( track );

    double zMCDecayVtx = 99999.;
    if( mcPartVelo ) {
      SmartRefVector<MCVertex>::const_iterator vi;
      for( vi = mcPartVelo->endVertices().begin(); 
           vi != mcPartVelo->endVertices().end(); ++vi ) {
        double zvi = (*vi)->position().z();
        if( zvi < zMCDecayVtx ) zMCDecayVtx = zvi;
      }
    }

    bool matched = false;
    Track* matchedTr = 0;

    //check if the Velo track is matched with TT or not
    if( track->checkType( Track::Upstream ) ) {
      matched   = true;
      matchedTr = track;
      ++NTnVeloTT;
    }

    int nTTall  = 0;
    int ngoodTT = 0;
    int nMeas   = 0;
    LinkedTo<MCParticle,STCluster> ttLink( evtSvc(), msgSvc(),
                                           STClusterLocation::TTClusters );
    
      if( matched ) {
	LHCb::Track::MeasurementContainer measurements = matchedTr->measurements() ;
	for( LHCb::Track::MeasurementContainer::const_iterator itm = measurements.begin();
             itm != measurements.end(); ++itm ) {
          ++nMeas;
          const Measurement* tmp= (*itm);
          const STMeasurement* ttClus = dynamic_cast<const STMeasurement*>( tmp );
          if( !ttClus ) continue;
          if( ttClus->type() == Measurement::TT ) {
            ++nTTall;
            const STCluster* cluster = ttClus->cluster();
            MCParticle* ttPart = ttLink.first( cluster );
            if( mcPartVelo == ttPart ) ++ngoodTT;
          }
        }
      }

    // number of TT1 clusters from same MC particle
    int nmchalf1  = 0;
    int nmchalf2  = 0;
    int nmchalf1b = 0;
    int nmchalf2b = 0;

    for( STClusters::const_iterator iClusIt = ttClusters->begin() ;
         iClusIt != ttClusters->end(); ++iClusIt )  { 
      STChannelID chID = (*iClusIt)->channelID();
      int iSta=chID.station();
      if( iSta != 1 && iSta != 2 ) continue;
      
      MCParticle* tempPart= ttLink.first( *iClusIt );
      if( !tempPart ) continue;
      
      DeSTLayer* myITLa = m_TTDet->findLayer( chID );     
      if( 0 != myITLa ) {
        double zhit = myITLa->globalCentre().Z();

        if( tempPart == mcPartVelo ) {
          if( iSta == 1 ) {
            ++nmchalf1;
            if( zMCDecayVtx > zhit ) ++nmchalf1b;
          }
          if( iSta == 2 ) {
            ++nmchalf2;
            if( zMCDecayVtx > zhit ) ++nmchalf2b;
          }
        } 
      }
    }

    NTisFwd.push_back( isFwd );
    NTzVeloCenter.push_back( (float) zcenter );

    if( mcPartVelo ) {
      NTkeyVelo.push_back( mcPartVelo->key() );
      NTpmcVelo.push_back( float(mcPartVelo->momentum().rho()/1000.) );
      NTptmcVelo.push_back( float(mcPartVelo->pt()/1000.) );
      double result = 0.;
      if( mcPartVelo->momentum().mag() != 0. ) {
        result = mcPartVelo->momentum().Z() / mcPartVelo->momentum().mag();
      }
      NTcsthmcVelo.push_back( (float) result );
      NTqmcVelo.push_back( (float) mcPartVelo->particleID().threeCharge() );
      NTidmcVelo.push_back( mcPartVelo->particleID().pid() );
      NTnmchit1.push_back( nmchalf1 );
      NTnmchit2.push_back( nmchalf2 );
      NTnmchit1b.push_back( nmchalf1b );
      NTnmchit2b.push_back( nmchalf2b );
    } else {
      NTkeyVelo.push_back( 0 );
      NTpmcVelo.push_back( -10. );
      NTptmcVelo.push_back( -10. );
      NTcsthmcVelo.push_back( 0 );
      NTqmcVelo.push_back( 0 );
      NTidmcVelo.push_back( 0 );
      NTnmchit1.push_back( 0 );
      NTnmchit2.push_back( 0 );
      NTnmchit1b.push_back( 0 );
      NTnmchit2b.push_back( 0 );
    }   
    if( matched ) {
      NTrecons.push_back( 1 );
      // was calculated from the closest state to z=0 in the past!
      NTpdet.push_back( float(matchedTr->p() / Gaudi::Units::GeV) );
      NTptdet.push_back( float(matchedTr->pt() / Gaudi::Units::GeV) );
      NTqdet.push_back( (float) matchedTr->charge() );
      NTqfit.push_back( (float) matchedTr->charge() ); // seems redundant with qdet!
      NTchi2tt.push_back( (float) matchedTr->chi2() );
      NTndf.push_back( nMeas-5 );
      NTnTrueDigi.push_back( ngoodTT );
      NTnAllDigi.push_back( nTTall );
    } else {
      NTrecons.push_back( 0 );
      NTpdet.push_back( -1 );
      NTptdet.push_back( -1 );
      NTqdet.push_back( 0 );
      NTchi2tt.push_back( 1e6 );
      NTndf.push_back( -1 );
      NTnTrueDigi.push_back( 0 );
      NTnAllDigi.push_back( 0 );
    }  
  
  } //end loop over offline tracks

  // fill Ntuple
  ntuple->column( "nVeloTT",     NTnVeloTT );
  ntuple->farray( "keyVelo",     NTkeyVelo.begin(),      NTkeyVelo.end(),
                  "nVelo",       1000 );
  ntuple->farray( "pmcVelo",     NTpmcVelo.begin(),      NTpmcVelo.end(),
                  "nVelo",       1000 );
  ntuple->farray( "ptmcVelo",    NTptmcVelo.begin(),     NTptmcVelo.end(),
                  "nVelo",       1000 );
  ntuple->farray( "csthmcVelo",  NTcsthmcVelo.begin(),   NTcsthmcVelo.end(),
                  "nVelo",       1000 );
  ntuple->farray( "qmcVelo",     NTqmcVelo.begin(),      NTqmcVelo.end(),
                  "nVelo",       1000 );
  ntuple->farray( "idmcVelo",    NTidmcVelo.begin(),     NTidmcVelo.end(),
                  "nVelo",       1000 );
  ntuple->farray( "txVelo",      NTtxVelo.begin(),       NTtxVelo.end(),
                  "nVelo",       1000 );
  ntuple->farray( "tyVelo",      NTtyVelo.begin(),       NTtyVelo.end(),
                  "nVelo",       1000 );
  ntuple->farray( "xVelo",       NTxVelo.begin(),        NTxVelo.end(),
                  "nVelo",       1000 );
  ntuple->farray( "yVelo",       NTyVelo.begin(),        NTyVelo.end(),
                  "nVelo",       1000 );
  ntuple->farray( "zVelo",       NTzVelo.begin(),        NTzVelo.end(),
                  "nVelo",       1000 );
  ntuple->farray( "nmchit1",     NTnmchit1.begin(),      NTnmchit1.end(),
                  "nVelo",       1000 );
  ntuple->farray( "nmchit2",     NTnmchit2.begin(),      NTnmchit2.end(),
                  "nVelo",       1000 );
  ntuple->farray( "recons",      NTrecons.begin(),       NTrecons.end(),
                  "nVelo",       1000 );
  ntuple->farray( "nmchit1b",    NTnmchit1b.begin(),     NTnmchit1b.end(),
                  "nVelo",       1000 );
  ntuple->farray( "nmchit2b",    NTnmchit2b.begin(),     NTnmchit2b.end(),
                  "nVelo",       1000 );
  ntuple->farray( "pdet",        NTpdet.begin(),         NTpdet.end(),
                  "nVelo",       1000 );
  ntuple->farray( "ptdet",       NTptdet.begin(),        NTptdet.end(),
                  "nVelo",       1000 );
  ntuple->farray( "qdet",        NTqdet.begin(),         NTqdet.end(),
                  "nVelo",       1000 );
  ntuple->farray( "qfit",        NTqfit.begin(),         NTqfit.end(),
                  "nVelo",       1000 );
  ntuple->farray( "chi2tt",      NTchi2tt.begin(),       NTchi2tt.end(),
                  "nVelo",       1000 );
  ntuple->farray( "ndf",         NTndf.begin(),          NTndf.end(),
                  "nVelo",       1000 );
  ntuple->farray( "nAllDigi",    NTnAllDigi.begin(),     NTnAllDigi.end(),
                  "nVelo",       1000 );
  ntuple->farray( "nTrueDigi",   NTnTrueDigi.begin(),    NTnTrueDigi.end(),
                  "nVelo",       1000 );
  ntuple->farray( "isFwd",       NTisFwd.begin(),        NTisFwd.end(),
                  "nVelo",       1000 );
  ntuple->farray( "zVeloCenter", NTzVeloCenter.begin(),  NTzVeloCenter.end(),
                  "nVelo",       1000 );

  StatusCode status = ntuple->write();
  if( status.isFailure() )
    return Error( "Cannot fill ntuple" );

  return StatusCode::SUCCESS;
}

///  Finalize
StatusCode TrackVeloTTChecker::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize();
}

MCParticle* TrackVeloTTChecker::VeloTrackMCTruth( Track* track )
{
  int nVeloCluster = 0;
  int nParticle    = 0;
  MCParticle* vecParticle[1000];
  int timesParticle[1000];
  for( int i = 0; i < 1000; ++i ) {
    timesParticle[i] = 0;
    vecParticle[i]   = 0;
  }

  LinkedTo<MCParticle,VeloCluster> veloLink( evtSvc(), msgSvc(),
                                             VeloClusterLocation::Default );
  
  LHCb::Track::MeasurementContainer measurements = track->measurements() ;
  for( LHCb::Track::MeasurementContainer::const_iterator iterMeas = measurements.begin();
       iterMeas != measurements.end(); ++iterMeas ) {
    const Measurement* tempMeas = (*iterMeas);
    if( tempMeas->z() > 1000. ) continue;

    const VeloCluster* cluster;
    if( tempMeas->checkType( Measurement::VeloR ) ) {
      const VeloRMeasurement* myRV = dynamic_cast<const VeloRMeasurement*>(tempMeas);
      if( myRV == 0 ) continue;
      cluster = myRV->cluster();
    }
    else if( tempMeas->checkType( Measurement::VeloPhi ) ) {
      const VeloPhiMeasurement* myPV = dynamic_cast<const VeloPhiMeasurement*>(tempMeas);
      if( myPV == 0 ) continue;
      cluster = myPV->cluster();
    }
    else { continue; }
    if( cluster == 0 ) continue;
    ++nVeloCluster;
    MCParticle* mcPartTmp = veloLink.first( cluster );
    if( mcPartTmp == 0 ) continue;
    bool found = false;
    for( int i = 0; i < nParticle; ++i ) {
      if( mcPartTmp == vecParticle[i] ) {
        ++timesParticle[i];
        found = true;
      }
    }
    if( !found ) {
      vecParticle[nParticle] = mcPartTmp;
      timesParticle[nParticle] = 1;
      ++nParticle;
    }
  }
  for( int i = 0; i < nParticle; ++i ) {
    double ratio = double(timesParticle[i]) / double(nVeloCluster);
    if( ratio > 0.7 ) return vecParticle[i];
  }

  return 0;
}

double TrackVeloTTChecker::CenterVeloTr( Track* track )
{
  double zmin = 1e8;
  double zmax = -1e8;   

  LHCb::Track::MeasurementContainer measurements = track->measurements() ;
  for( LHCb::Track::MeasurementContainer::const_iterator itm = measurements.begin();
       itm != measurements.end(); ++itm ) {
    const Measurement* tmp = (*itm);
    if( ! ( tmp->checkType( Measurement::VeloR )
            || tmp->checkType( Measurement::VeloPhi ) ) ) continue;
    double z = tmp->z();
    if( z < zmin ) zmin = z;
    if( z > zmax ) zmax = z;
  }

  double zcen = ( zmin + zmax ) / 2.;
  return zcen;
}
