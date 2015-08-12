// Include files

// local
#include "PatVeloGeneric.h"
#include "PatVeloSpaceTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatVeloGeneric
//
// 2005-09-09 : Tomas LASTOVICKA
// 2008-02-11 : Kurt Rinnert (port to Tf framework)
//-----------------------------------------------------------------------------

namespace Tf {
  DECLARE_ALGORITHM_FACTORY( PatVeloGeneric )

  //=============================================================================
  // Standard constructor, initializes variables
  //=============================================================================

  PatVeloGeneric::PatVeloGeneric( const std::string& name,
      ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
    , m_angleUtils(-Gaudi::Units::pi,Gaudi::Units::pi)
    , m_rHitManager(NULL)
    , m_phiHitManager(NULL)
    , m_PatVeloTrackTool(NULL)
    , m_velo(NULL)
    , m_outputTracks(NULL)
    {
      declareProperty( "SigmaTol",    m_sigma = 4.0 );
      declareProperty( "RAliTol",     m_rOff = 0.0 );
      declareProperty( "PAliTol",     m_pOff = 0.0 );
      declareProperty( "MaxSkip",     m_nSkip = 2  );
      declareProperty( "MinModules",  m_nMin  = 5  );
      declareProperty( "CleanSeed",   m_cleanSeeds  = true );
      declareProperty( "PrivateBest", m_privateBest = false );
      declareProperty( "KalmanState", m_kalmanState = 1);
      declareProperty( "ClusterCut",  m_clusterCut = 10000 );
      declareProperty( "FullAlignment", m_align = true );
      declareProperty( "ForwardProp", m_forward = false ); 
      declareProperty( "CheckReadOut",m_checkIfSensorIsReadOut   = false ); 
      declareProperty( "ACDC",        m_acdc = false );  
      declareProperty( "DoNotRefit",  m_doNotRefit = false );  
      declareProperty( "Output",      m_outputTracksLocation = LHCb::TrackLocation::Velo);
      declareProperty( "RHitManagerName", m_rHitManagerName="PatVeloRHitManager" );
      declareProperty( "PhiHitManagerName", m_phiHitManagerName="PatVeloPhiHitManager" );
      declareProperty( "TrackToolName",          m_trackToolName = "PatVeloTrackTool" );
      declareProperty( "ErrorX2",  m_errorX2  = 40.0 );
      declareProperty( "ErrorY2",  m_errorY2  = 40.0 );
      declareProperty( "ErrorTx2", m_errorTx2 = 6.e-5 );
      declareProperty( "ErrorTy2", m_errorTy2 = 6.e-5 );
      declareProperty( "ErrorQOP", m_momentumError = 1.e-6 );

      declareProperty( "DefaultMomentum", m_momentumDefault = 1/(10 * Gaudi::Units::GeV) );

      declareProperty( "ConsiderOverlaps", m_considerOverlaps=false );
      declareProperty( "MaxGapForOverlapSearch", m_maxGapForOverlapSearch=10.0*Gaudi::Units::micrometer );

      m_binary = 1/sqrt(12.);  // digital resolution a priori

      numberOfRSectors = 4;
      numberOfPSectors = 2;

    }

  //=============================================================================
  // Destructor
  //=============================================================================

  PatVeloGeneric::~PatVeloGeneric() {}

  //=============================================================================
  // Initialization
  //=============================================================================

  StatusCode PatVeloGeneric::initialize() {
    StatusCode sc = GaudiAlgorithm::initialize();
    if ( sc.isFailure() ) return sc;

    debug() << "==> Initialize" << endmsg;

    m_rHitManager   = tool<PatVeloRHitManager>  ( "Tf::PatVeloRHitManager", m_rHitManagerName );
    m_phiHitManager = tool<PatVeloPhiHitManager>( "Tf::PatVeloPhiHitManager", m_phiHitManagerName );
    m_PatVeloTrackTool = tool<PatVeloTrackTool>("Tf::PatVeloTrackTool",m_trackToolName);
    m_velo = getDet<DeVelo>( DeVeloLocation::Default );
    m_nEvt = 0;

    info() << "========== Algorithm parameters ======"           << endmsg
      << "Tolerance in sigma        = " << m_sigma          << endmsg
      << "R alignment tolerance     = " << m_rOff           << endmsg
      << "Phi alignment tolerance   = " << m_pOff           << endmsg
      << "Maximum skip              = " << m_nSkip          << endmsg
      << "Minimum number of modules = " << m_nMin           << endmsg
      << "Cleaning seeds            = " << m_cleanSeeds     << endmsg
      << "Private best candidate    = " << m_privateBest    << endmsg
      << "Kalman seeding state      = " << m_kalmanState    << endmsg
      << "Cluster cut               = " << m_clusterCut     << endmsg
      << "Use own cluster alignment = " << m_align          << endmsg
      << "Forward propagation       = " << m_forward        << endmsg
      << "Only sensors in readout   = " << m_checkIfSensorIsReadOut << endmsg
      << "ACDC flag (real data)     = " << m_acdc           << endmsg
      << "Output path               = " << m_outputTracksLocation << endmsg
      << "Do not re-fit tracks      = " << m_doNotRefit     << endmsg
      << "Left half x-offset        = " << m_velo->halfBoxOffset(0).x() << " mm"<< endmsg
      << "Right half x-offset       = " << m_velo->halfBoxOffset(1).x() << " mm"<< endmsg
      << "======================================"           << endmsg;

    std::vector<DeVeloPhiType*>::const_iterator phiIt = m_velo->phiSensorsBegin();  
    m_inner2 = m_binary*(*phiIt)->phiPitch( (unsigned int)1 ); 
    m_outer2 = m_binary*(*phiIt)->phiPitch( (unsigned int)1000 );
    m_inner2 *= m_inner2;
    m_outer2 *= m_outer2;  
    m_rOff *= m_rOff;
    m_pOff *= m_pOff;

    return StatusCode::SUCCESS;
  }

  //=============================================================================
  // Main execution
  //=============================================================================

  StatusCode PatVeloGeneric::execute() {

    debug() << "==> Execute" << endmsg;

    ++m_nEvt;
    debug() << "Event number " << m_nEvt << endmsg;

    // create output track container on TES, if it does not exist
    if ( exist<LHCb::Tracks>( m_outputTracksLocation ) ) {
      m_outputTracks = get<LHCb::Tracks>( m_outputTracksLocation );
    } else {
      m_outputTracks = new LHCb::Tracks();
      put(m_outputTracks, m_outputTracksLocation );
    }
    m_outputTracks->reserve(500);

    PatVeloRHitManager::Station *stationR0, *stationR1, *stationR2, *stationR;
    unsigned int zoneR0, zoneR;
    PatVeloPhiHitManager::Station *stationP0, *stationP2;
    unsigned int zoneP0, zoneP2;

    std::vector<PatVeloRHit*>::const_iterator   cR0, cR2;
    std::vector<PatVeloPhiHit*>::const_iterator cP0, cP2;  

    for (unsigned int half=0; half < 2; ++half) {

      //      PatVeloRHitManager::StationIterator        rStationsBegin        = m_rHitManager->stationsHalfBegin(half);
      PatVeloRHitManager::StationIterator        rStationsEnd          = m_rHitManager->stationsHalfEnd(half);
      PatVeloRHitManager::StationReverseIterator rStationsReverseBegin = m_rHitManager->stationsHalfReverseBegin(half);
      PatVeloRHitManager::StationReverseIterator rStationsReverseEnd   = m_rHitManager->stationsHalfReverseEnd(half);

      PatVeloRHitManager::StationReverseIterator stationRItr0;
      for (stationRItr0  = rStationsReverseBegin;
          stationRItr0 != rStationsReverseEnd; 
          ++stationRItr0 ) {

        stationR0 = *stationRItr0;     
        if(m_checkIfSensorIsReadOut && !stationR0->sensor()->isReadOut()) continue;       // only look at read-out sensors

        PatVeloRHitManager::StationReverseIterator stationRItr1 = stationRItr0;
        ++stationRItr1;
        // look for the next read-out sensor
        if(m_checkIfSensorIsReadOut){
          while(rStationsReverseEnd != stationRItr1 && !(*stationRItr1)->sensor()->isReadOut()){
            ++stationRItr1;
          }
        }
        if (rStationsReverseEnd == stationRItr1) break;
        stationR1 = *stationRItr1;
        double zR = stationR1->z();      

        PatVeloRHitManager::StationReverseIterator stationRItr2 = stationRItr1;
        ++stationRItr2;
        // look for the next read-out sensor
        if(m_checkIfSensorIsReadOut){
          while(rStationsReverseEnd != stationRItr2 && !(*stationRItr2)->sensor()->isReadOut()){
            ++stationRItr2;
          }
        }
        if (rStationsReverseEnd == stationRItr2) break; 
        stationR2 = *stationRItr2;

        // =========================================
        // Building and propagating track seeds
        // =========================================

        for (zoneR0 = 0; zoneR0 < numberOfRSectors; ++zoneR0) {

          if (stationR0->empty(zoneR0)) continue;      
          if (stationR1->empty(zoneR0)) continue;      
          if (stationR2->empty(zoneR0)) continue;      

          double sum = stationR0->size(zoneR0)+stationR1->size(zoneR0)+stationR2->size(zoneR0);      
          if (m_clusterCut < sum) continue;


          stationP0 = m_phiHitManager->station(stationR0->sensor()->associatedPhiSensor()->sensorNumber());
          stationP2 = m_phiHitManager->station(stationR2->sensor()->associatedPhiSensor()->sensorNumber());

          std::vector<PatGenericFitter> trackSeeds;
          trackSeeds.reserve(400);

          for (zoneP0 = 0; numberOfPSectors > zoneP0; ++zoneP0) {
            if (stationP0->empty(zoneP0)) continue;

            for (zoneP2 = 0; numberOfPSectors > zoneP2; ++zoneP2) {        
              if (stationP2->empty(zoneP2)) continue;

              PatVeloRHitRange hitsR0 = stationR0->hits(zoneR0);
              for(cR0 = hitsR0.begin(); hitsR0.end() != cR0; ++cR0) {
                if ( (*cR0)->hit()->isUsedByAnyOtherThan(HitBase::UsedByVeloRZ) ) continue;

                PatVeloRHitRange hitsR2 = stationR2->hits(zoneR0);
                for(cR2 = hitsR2.begin(); hitsR2.end() != cR2; ++cR2) {
                  if ( (*cR2)->hit()->isUsedByAnyOtherThan(HitBase::UsedByVeloRZ) ) continue;

                  PatVeloPhiHitRange hitsP0 = stationP0->hits(zoneP0);
                  for(cP0 = hitsP0.begin(); hitsP0.end() != cP0; ++cP0) {
                    if ( (*cP0)->hit()->isUsedByAnyOtherThan(HitBase::UsedByVeloRZ) ) continue;

                    PatVeloPhiHitRange hitsP2 = stationP2->hits(zoneP2);
                    for(cP2 = hitsP2.begin(); hitsP2.end() != cP2; ++cP2) {
                      if ( (*cP2)->hit()->isUsedByAnyOtherThan(HitBase::UsedByVeloRZ) ) continue;

                      PatGenericFitter fitter(m_rHitManager, m_phiHitManager, m_align);
                      fitter.addPair(*cR0,*cP0);
                      fitter.addPair(*cR2,*cP2);
                      fitter.reset();

                      StatusCode sc = fitter.fit();        
                      if( !(sc.isSuccess()) ) {
                        debug() << "Fit failed in seed creation." << endmsg;
                        continue;
                      }

                      if (!rSectorTest(stationR0,zoneR0,fitter.pExtrap(stationR0->z()))) continue;                  
                      if (!rSectorTest(stationR2,zoneR0,fitter.pExtrap(stationR2->z()))) continue;
                      if (!pSectorTest(stationP0,zoneP0,fitter.rExtrap(stationP0->z()))) continue;
                      if (!pSectorTest(stationP2,zoneP2,fitter.rExtrap(stationP2->z()))) continue;

                      double rEst = fitter.rExtrap(zR);

                      PatVeloRHit* rCand = rCandidate(stationR1,zoneR0,rEst);
                      if (0 == rCand) continue;
                      if ( rCand->hit()->isUsedByAnyOtherThan(HitBase::UsedByVeloRZ) ) continue;
                      
                      PatVeloPhiHit* pCand = pCandidate(stationR1,&fitter);
                      if (0 == pCand) continue;
                      if ( pCand->hit()->isUsedByAnyOtherThan(HitBase::UsedByVeloRZ) ) continue;

                      fitter.addPair(rCand,pCand);
                      trackSeeds.push_back(fitter);
                    }
                  }
                }
              }
            }
          }

          debug() << "There is " << trackSeeds.size() << " track seeds." << endmsg;    

          // =========================================
          // Cleaning and propagation of track seeds
          // =========================================

          for (std::vector<PatGenericFitter>::iterator itSeed = trackSeeds.begin(); 
              trackSeeds.end() != itSeed; itSeed++ ) {      
            PatGenericFitter* fitter = &(*itSeed);
            if ( 0 == fitter ) continue;      

            if ( m_cleanSeeds ) {        
              for (std::vector<PatGenericFitter>::iterator itSeed2 = trackSeeds.begin(); 
                  trackSeeds.end() != itSeed2; itSeed2++ ) {
                PatGenericFitter* fitter2 = &(*itSeed2);
                if ( 0 == fitter2 ) continue;
                if ( fitter == fitter2 ) continue;
                if ( fitter->shareClustersLite(fitter2) ) {
                  fitter2->setValid( false ); 
                  fitter->setValid( false );          
                }          
              }        
            }

            if ( !(fitter->isValid()) ) continue;

            if( !m_doNotRefit ) {
              StatusCode sc = fitter->fit();        
              if( !(sc.isSuccess()) ) continue;
            }

            // ========================
            // Propagate seeds backward
            // ========================

            PatVeloRHitManager::StationReverseIterator stationRItr         = stationRItr2;
            PatVeloRHitManager::StationReverseIterator reversePropagateEnd = rStationsReverseEnd;

            // switch to global iterators if overlaps are considered, only do so
            // if the VELO is closed.
            if ( m_considerOverlaps && veloIsClosed() ) {
              PatVeloRHitManager::StationIterator si = m_rHitManager->stationIterAll((*stationRItr)->sensorNumber());
              stationRItr         = PatVeloRHitManager::StationReverseIterator(++si);
              reversePropagateEnd = m_rHitManager->stationsAllReverseEnd(); 
            }
            
            int nSkip = 0;
            double rEst = 0;
            double pEst = 0;

            while ( (++stationRItr != reversePropagateEnd) && nSkip <=  m_nSkip) {        

              stationR = *stationRItr;
              ++nSkip;        

              bool consist = false;      
              for (zoneR = 0; numberOfRSectors > zoneR; ++zoneR) {

                double zR = stationR->z();
                rEst = fitter->rExtrap(zR);
                pEst = fitter->pExtrap(zR);
                consist = rSectorTest(stationR,zoneR,pEst);
                if ( consist ) break;        
              }

              if ( !consist ) continue;

              PatVeloRHit* rCand = rCandidate(stationR,zoneR,rEst);                  
              if ( rCand &&  rCand->hit()->isUsedByAnyOtherThan(HitBase::UsedByVeloRZ) ) rCand = 0;

              PatVeloPhiHit* pCand = pCandidate(stationR,fitter);
              if ( pCand &&  pCand->hit()->isUsedByAnyOtherThan(HitBase::UsedByVeloRZ) ) pCand = 0;

              if ( 0 == pCand && 0 == rCand ) continue; 

              fitter->addPair(rCand,pCand);
              nSkip = 0; 

              if( !m_doNotRefit ) {          
                StatusCode sc = fitter->fit();        
                if( !(sc.isSuccess()) ) break;
              }

            }      

            // =======================
            // Propagate seeds forward
            // =======================

            if ( m_forward ) {        

              PatVeloRHitManager::StationReverseIterator sri   = stationRItr0;
              PatVeloRHitManager::StationIterator stationRItr  = (++sri).base();
              PatVeloRHitManager::StationIterator propagateEnd = rStationsEnd;

              // switch to global iterators if overlaps are considered, only do so
              // if the VELO is closed.
              if ( m_considerOverlaps && veloIsClosed() ) {
                stationRItr  = m_rHitManager->stationIterAll((*stationRItr)->sensorNumber());
                propagateEnd = m_rHitManager->stationsAllEnd(); 
              }

              nSkip = 0;
              rEst = 0;
              pEst = 0;

              while ( ( ++stationRItr != propagateEnd) && nSkip <=  m_nSkip) {        

                stationR = *stationRItr;
                ++nSkip;        

                bool consist = false;      
                for (zoneR = 0; numberOfRSectors > zoneR; ++zoneR) {
                  double zR = stationR->z();
                  rEst = fitter->rExtrap(zR);
                  pEst = fitter->pExtrap(zR);
                  consist = rSectorTest(stationR,zoneR,pEst);
                  if ( consist ) break;        
                }

                if ( !consist ) continue;

                PatVeloRHit* rCand = rCandidate(stationR,zoneR,rEst);                  
                if ( rCand &&  rCand->hit()->isUsedByAnyOtherThan(HitBase::UsedByVeloRZ) ) rCand = 0;
                
                PatVeloPhiHit* pCand = pCandidate(stationR,fitter);
                if ( pCand &&  pCand->hit()->isUsedByAnyOtherThan(HitBase::UsedByVeloRZ) ) pCand = 0;

                if ( 0 == pCand && 0 == rCand ) continue; 

                fitter->addPair(rCand,pCand);
                nSkip = 0; 

                if( !m_doNotRefit ) {            
                  StatusCode sc = fitter->fit();        
                  if( !(sc.isSuccess()) ) break;
                }

              }        
            }      
          }  

          // =========================================
          // Store tracks
          // =========================================

          for (std::vector<PatGenericFitter>::iterator itTrack = trackSeeds.begin(); 
              trackSeeds.end() != itTrack; itTrack++ ) {
            PatGenericFitter* fitter = &(*itTrack);

            debug() << " Seed has " << fitter->entries() 
              << " entries after propagation." << endmsg;

            if ( !(fitter->isValid()) ) continue;
            if ( fitter->entries() < m_nMin ) continue;

            StatusCode sc = fitter->fit();        
            if( !(sc.isSuccess()) ) continue;

            fitter->markUsed();

            const DeVeloSensor* lastSensor = fitter->lastSensor();
            Gaudi::XYZPoint boxPoint, globalPoint;

            LHCb::Track* track = new LHCb::Track(m_outputTracks->size());
            track->setType( LHCb::Track::Velo );
            track->setHistory( LHCb::Track::PatVeloGeneric );
            track->setFlag( LHCb::Track::Backward, false);

            // Use PatVeloSpaceTrack to define State

            if ( 1 == m_kalmanState ) {        

              LHCb::State temp;
              PatVeloSpaceTrack tr(m_PatVeloTrackTool);

              std::vector<PatVeloRHit*>::iterator itR;
              for ( itR = fitter->rCoords().begin(); fitter->rCoords().end() != itR;
                  ++itR ) {
                if ( 0 == (*itR) ) continue;
                PatVeloRHit* coord = (*itR);
                double rr = coord->coordHalfBox();
                double  z = coord->z();        
                coord->setRadiusAndPhi(rr,fitter->pExtrap(z));
                tr.addRCoord(coord);        
              }

              std::vector<PatVeloPhiHit*>::iterator itP;
              for ( itP = fitter->phiCoords().begin(); fitter->phiCoords().end() != itP;
                  ++itP ) {
                if ( 0 == (*itP) ) continue;        
                PatVeloPhiHit* coord = (*itP);        
                double  z = coord->z();        
                double rr = fitter->rExtrap(z);
                double phi = coord->coordHalfBox() + coord->sensor()->halfboxPhiOffset(coord->zone(),rr);

                coord->setRadiusAndPhi(rr,phi);
                tr.addPhi(coord);        
              }

              tr.fitSpaceTrack(0.002);
	      
	      m_PatVeloTrackTool->addStateToTrack(&tr,track,
						  LHCb::State::EndVelo,
						  tr.covariance());

              track->setChi2PerDoF( tr.chi2Dof( ) );
              track->setNDoF( tr.rCoords()->size() + tr.phiCoords()->size() );

              debug() << "Saving track with Chi2 " <<  track->chi2() 
                << " and dof " << track->nDoF() << endmsg;  

            } 

            // Put State on the first/last sensor on track + large covariance matrix

            if ( 2 == m_kalmanState || 3 == m_kalmanState ) {  

              LHCb::State temp;

              double z0 = 0;        
              if ( 2 == m_kalmanState ) {  
                z0 = fitter->lastSensorZ();
              } else {
                z0 = fitter->firstSensorZ();
              }

              boxPoint.SetCoordinates( fitter->getX(z0), fitter->getY(z0), z0 );
              globalPoint = lastSensor->veloHalfBoxToGlobal(boxPoint);

              temp.setState( globalPoint.x(), globalPoint.y(), globalPoint.z(),
                  fitter->getSX(),
                  fitter->getSY(),
                  m_momentumDefault );

              Gaudi::TrackSymMatrix cov;
              cov(0,0) = m_errorX2;
              cov(1,1) = m_errorY2;
              cov(2,2) = m_errorTx2;
              cov(3,3) = m_errorTy2;
              cov(4,4) = m_momentumError;

              temp.setCovariance( cov );
              temp.setLocation( LHCb::State::EndVelo );

              track->addToStates( temp );

            }

            // State at z=0 + covariance matrix from PatGenericFitter

            if ( 4 == m_kalmanState ) {  

              LHCb::State temp;

              double z0 = 0;
              boxPoint.SetCoordinates( fitter->getX(z0), fitter->getY(z0), z0 );
              globalPoint = lastSensor->veloHalfBoxToGlobal(boxPoint);

              temp.setState( globalPoint.x(), globalPoint.y(), globalPoint.z(),
                  fitter->getSX(),
                  fitter->getSY(),
                  m_momentumDefault );

              Gaudi::TrackSymMatrix m_cov;
              m_cov.Place_at(fitter->covariance(),0,0);        
              m_cov(4,4) =  m_momentumError;

              temp.setCovariance( m_cov );
              temp.setLocation( LHCb::State::EndVelo );

              track->addToStates( temp );

              // temporary, PatGenericFitter does not calculate Chi2 and ndof

              track->setChi2PerDoF( 1. );
              track->setNDoF( 8 );

            }

            //      fitter->sortCoordinates();

            std::vector<PatVeloRHit*>::iterator itR;
            for ( itR = fitter->rCoords().begin(); fitter->rCoords().end() != itR;
                ++itR ) {
              if ( 0 == (*itR) ) continue;
              track->addToLhcbIDs( (*itR)->hit()->lhcbID() );
            }

            std::vector<PatVeloPhiHit*>::iterator itP;
            for ( itP = fitter->phiCoords().begin(); fitter->phiCoords().end() != itP;
                ++itP ) {
              if ( 0 == (*itP) ) continue;        
              track->addToLhcbIDs( (*itP)->hit()->lhcbID() );
            }

            track->setPatRecStatus( LHCb::Track::PatRecIDs );
            m_outputTracks->insert(track);
          }


        } // end of loop over R-sectors
      } // end of loop over R-sensors is here
    } // end of loop over VELO halfs is here

  debug() << "Number of stored tracks: " << m_outputTracks->size() << endmsg;  

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================

StatusCode PatVeloGeneric::finalize() {
  debug() << "==> Finalize" << endmsg;
  return GaudiAlgorithm::finalize();
}


}

