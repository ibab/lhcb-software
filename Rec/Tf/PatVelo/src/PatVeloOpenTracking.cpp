// Include files

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// from TrackEvent
#include "Event/Track.h"

// local
#include "PatVeloOpenTrack.h"
#include "CircularRangeUtils.h"
// local
#include "PatVeloOpenTracking.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatVeloOpenTracking
//
// 2007-02-27 : Olivier Callot from Muriel Pivk work
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PatVeloOpenTracking )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatVeloOpenTracking::PatVeloOpenTracking( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_rHitManager(NULL)
  , m_phiHitManager(NULL)
  , m_trackTool(NULL)
  , m_debugTool(NULL)
  , m_velo(NULL)
{
  declareProperty( "rMatchTol"     , m_rMatchTol       = 0.90  );
  declareProperty( "rExtrapTol"    , m_rExtrapTol      = 1.5   );
  declareProperty( "phiMatchTol"   , m_phiMatchTol     = 0.10  );
  declareProperty( "phiExtrapTol"  , m_phiExtrapTol    = 0.15  );
  declareProperty( "PhiAngularTol" , m_phiAngularTol   = 0.005 );
  declareProperty( "MaxSlope"      , m_maxSlope        = 0.400 );
  declareProperty( "MaxChi2"       , m_maxChi2         = 5.    );
  declareProperty( "MaxMissedR"    , m_maxMissedR      = 3     );

  declareProperty( "DebugToolName" , m_debugToolName   = ""   );
  declareProperty( "WantedKey"     , m_wantedKey       = -1   );

  declareProperty( "RHitManagerName" , m_rHitManagerName="PatVeloRHitManager");
  declareProperty( "PhiHitManagerName" , m_phiHitManagerName="PatVeloPhiHitManager");
  declareProperty( "TrackToolName",          m_trackToolName = "PatVeloTrackTool" );
}
//=============================================================================
// Destructor
//=============================================================================
PatVeloOpenTracking::~PatVeloOpenTracking() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatVeloOpenTracking::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_rHitManager   = tool<Tf::PatVeloRHitManager>( "Tf::PatVeloRHitManager", m_rHitManagerName );
  m_phiHitManager = tool<Tf::PatVeloPhiHitManager>( "Tf::PatVeloPhiHitManager", m_phiHitManagerName );
  m_trackTool     = tool<Tf::PatVeloTrackTool>("Tf::PatVeloTrackTool",m_trackToolName);
  m_debugTool     = 0;
  if ( "" != m_debugToolName ) m_debugTool = tool<IPatDebugTool>( m_debugToolName );

  //== Get detector element
  m_velo = getDet<DeVelo>( DeVeloLocation::Default );
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatVeloOpenTracking::execute() {

  bool isDebug   = msgLevel( MSG::DEBUG   );
  debug() << "==> Execute" << endmsg;

  //std::vector<PatVeloSpaceTrack> myTracks;
  std::vector<PatVeloOpenTrack> myTracks;
  myTracks.reserve(400);

  LHCb::Tracks* outputTracks;
  if ( exist<LHCb::Tracks>( LHCb::TrackLocation::Velo ) ) {
    outputTracks = get<LHCb::Tracks>(  LHCb::TrackLocation::Velo );
  } else {
    outputTracks = new LHCb::Tracks();
    put( outputTracks, LHCb::TrackLocation::Velo );
    outputTracks->reserve(500);
  }

  int numberOfRSectors=4;

  // 'zone' is the space zone of the R sensor, 0-7, 4-7 for right sensors
  // 'sector' is the internal division of a sensor, 0-3 R and 0-1 phi

  const DeVeloRType *sensorR0, *sensorR1, *sensorR2, *sensorR;
  Tf::PatVeloRHitManager::Station *stationR0, *stationR1, *stationR2, *stationR;
  Tf::PatVeloPhiHitManager::Station *stationPhi0, *stationPhi1, *stationPhi2, *stationPhi;
  double zR0, zR1, zR2, zPhi1;
  int sensPhi0, sensPhi1, sensPhi2;

  std::vector<Tf::PatVeloRHit*>::const_iterator   cR0, cR1, cR2;
  std::vector<Tf::PatVeloPhiHit*>::const_iterator cPhi0, cPhi1, cPhi2;
  double r0, r1, r2, coordPhi0, coordPhi2;
  double rPred, rPredPhi, phiPred;

  std::vector<DeVeloRType*>::const_reverse_iterator sensorRItR;
  double zR, zPhi;


  Tf::CircularRangeUtils<double> circularRange( -Gaudi::Units::pi, Gaudi::Units::pi );

  // Set the wanted parameters for phi clusters: direction of strip and offest at origin
  for ( int sensPhi = 64; 106 > sensPhi ; ++sensPhi ) {
    stationPhi = m_phiHitManager->station( sensPhi );
    for ( int sectorPhi = 0; 2 > sectorPhi; ++sectorPhi  ) {
      Tf::PatVeloPhiHitRange hits = stationPhi->hits( sectorPhi );
      const DeVeloPhiType* pSens = stationPhi->sensor();

      for ( cPhi0 = hits.begin() ; hits.end() != cPhi0; ++cPhi0 ) {
        int strip = (*cPhi0)->hit()->strip();
        double origin = pSens->distToOrigin( strip );
        double phi    = pSens->trgPhiDirectionOfStrip( strip );
        if ( !pSens->isDownstream() ) origin = - origin;
        (*cPhi0)->setRadiusAndPhi( origin, phi );

        if ( 0 <= m_wantedKey ) {
          if ( matchKey( *cPhi0 ) ) printCoord( *cPhi0, "** Phi " );
        }
      }
    }
  }


  // loop on R sensors starting from the last one. Stops when no longer 3 sensors
  // Backwards and forwards tracks to gether as there is no constraints of
  // pointing towards r=0.

  double firstRSensor = -1;
  for ( sensorRItR= m_velo->rSensorsReverseBegin();
        sensorRItR != m_velo->rSensorsReverseEnd(); sensorRItR++) {
    sensorR0 = (*sensorRItR);
    int iSens0  = sensorR0->sensorNumber();
    if ( 4 > iSens0 ) break;
    if ( 0 > firstRSensor ) firstRSensor = iSens0;

    stationR0   = m_rHitManager->station( iSens0 );
    sensPhi0    = sensorR0->associatedPhiSensor()->sensorNumber();
    stationPhi0 = m_phiHitManager->station( sensPhi0 );

    // loop on the 4 sectors of each R sensor
    for ( int sectorR0 = 0; numberOfRSectors > sectorR0; sectorR0++) {
      if ( stationR0->empty( sectorR0 ) ) continue;
      zR0 = stationR0->z();

      //== Get next two R sensor on the same side
      stationR1 = m_rHitManager->station( iSens0-2 );
      if (0 == stationR1) break;
      zR1         = stationR1->z();
      sensorR1    = stationR1->sensor();
      sensPhi1    = sensorR1->associatedPhiSensor()->sensorNumber();
      stationPhi1 = m_phiHitManager->station( sensPhi1 );
      zPhi1       = stationPhi1->z();

      stationR2 = m_rHitManager->station( iSens0-4 );
      if (0 == stationR2) break;
      zR2         = stationR2->z();
      sensorR2    = stationR2->sensor();
      sensPhi2    = sensorR2->associatedPhiSensor()->sensorNumber();
      stationPhi2 = m_phiHitManager->station( sensPhi2 );

      int zone0 = sectorR0;
      if ( sensorR0->isRight() ) zone0 += 4;

      Tf::PatVeloRHitRange hitsR0 = stationR0->hits( sectorR0 );

      // Loop on possible R sectors in second sensor. Start by same sector,
      // as there are more chance to find the good solution there!
      for ( int kk = 0; 3 > kk; ++kk ) {
        int sectorR2 = sectorR0;
        if ( 1 == kk ) {
          sectorR2 = sectorR0 - 1;
          if ( 0 > sectorR2 ) continue;
          if ( 0. > zR0 && 2 != sectorR2 ) continue;    // No fancy backwards tracks
        } else if ( 2 == kk ) {
          sectorR2 = sectorR0 + 1;
          if ( numberOfRSectors <= sectorR2 ) continue;
          if ( 0. > zR0 && 1 != sectorR2 ) continue;    // No fancy backwards tracks
        }
        int zone2 = sectorR2;
        if ( sensorR2->isRight() ) zone2 += 4;

        if ( stationR2->empty( sectorR2 ) ) continue;
        Tf::PatVeloRHitRange hitsR2 = stationR2->hits( sectorR2 );

        if ( isDebug ) {
          info() << format( "=== Sensor R0=%2d sect %2d nCoord%3d    R2=%2d sect %2d nCoord %3d",
                            sensorR0->sensorNumber(), zone0, stationR0->size(sectorR0),
                            sensorR2->sensorNumber(), zone2, stationR2->size(sectorR2) ) << endmsg;
          printRCoords( stationR0, sectorR0, "R0" );
          printRCoords( stationR2, sectorR2, "R2" );
        }

        //== Loop on pair of unused R coordinates, estimate the R trajectory (r = a + bz)
        //== to select the raisonable phi sector...

        for( cR0 = hitsR0.begin(); hitsR0.end() > cR0; cR0++) {
          if ((*cR0)->hit()->isUsed()) continue;
          r0 = (*cR0)->coordHalfBox();
          for( cR2 = hitsR2.begin(); hitsR2.end() > cR2; cR2++) {
            if ((*cR2)->hit()->isUsed()) continue;
            r2 = (*cR2)->coordHalfBox();
            double drdz = (r2-r0)/(zR2-zR0);

            if ( m_maxSlope < fabs(drdz) ) continue;  //== Maximum slope in x
            if ( 0 > zR0   &&   drdz > 0 ) continue;  //== Backwards : should point towards the beam line...

            if ( 0 <= m_wantedKey ) {
              isDebug = msgLevel( MSG::DEBUG );
              if ( matchKey( *cR0 ) && matchKey( *cR2 ) ) isDebug = true;
              if ( isDebug ) {
                info() << "==== Found good R pair: matching key " << m_wantedKey << " ===" << endmsg;
                printCoord( *cR0, "R0" );
                printCoord( *cR2, "R2" );
              }
            }

            // loop on phi sensors, get the proper sector each time.
            if ( isDebug ) info() << "... try  r0 " << r0 << " r2 " << r2 << endmsg;

            PatVeloOpenTrack bestTrack;

            // Simplification: Use R extrapolation to get the sector (in, out) in the phi sensor.
            // This is not correct if the track is way off axis AND phi sensor far from R ...
            double rPhi0 = r0 + ( stationPhi0->z() - zR0 ) * drdz;
            int sectorPhi0 = 0;
            if ( stationPhi0->sensor()->rMax( 0 ) < rPhi0 ) sectorPhi0 = 1;
            if ( stationPhi0->empty(sectorPhi0) ) continue;
            std::pair<double,double> range0;
            bool hasRange = m_trackTool->phiRange( rPhi0, zone0, m_phiAngularTol,
                                                   stationPhi0, sectorPhi0, range0 );
            if ( !hasRange ) continue;

            //== Same simplification: R in phi sensor from R = az + b
            double rPhi2 = r0 + ( stationPhi2->z() - zR0 ) * drdz;
            int sectorPhi2 = 0;
            if ( stationPhi2->sensor()->rMax( 0 ) < rPhi2 )  sectorPhi2 = 1;
            if ( stationPhi2->empty(sectorPhi2) ) continue;

            std::pair<double,double> range2;
            hasRange = m_trackTool->phiRange( rPhi2, zone2, m_phiAngularTol,
                                              stationPhi2, sectorPhi2, range2 );
            if ( !hasRange ) continue;

            if ( isDebug ) {
              info() << format( "  phi0 range ok%2d phiMin%8.3f phiMax%8.3f",
                                hasRange, range0.first, range0.second ) << endmsg;
              info() << format( "  phi2 range ok%2d phiMin%8.3f phiMax%8.3f",
                                hasRange, range2.first, range2.second ) << endmsg;
            }

            Tf::PatVeloPhiHitRange hitsPhi0 = stationPhi0->hits( sectorPhi0 );
            Tf::PatVeloPhiHitRange hitsPhi2 = stationPhi2->hits( sectorPhi2 );

            double offset0 = stationPhi0->sensor()->halfboxPhiOffset( sectorPhi0, rPhi0 );
            double offset2 = stationPhi2->sensor()->halfboxPhiOffset( sectorPhi2, rPhi2 );

            for ( cPhi0 = hitsPhi0.begin(); hitsPhi0.end() > cPhi0; cPhi0++) {
              if ( isDebug && matchKey( *cPhi0 ) ) printCoord( *cPhi0, "** Good Phi0" );
              if ( (*cPhi0)->hit()->isUsed() ) continue;
              coordPhi0 = circularRange.add( (*cPhi0)->coordHalfBox(), offset0);
              if ( !circularRange.contains( range0, coordPhi0 ) ) continue;

              for ( cPhi2 = hitsPhi2.begin(); hitsPhi2.end() > cPhi2; cPhi2++) {
                if ( isDebug && matchKey( *cPhi2 ) ) printCoord( *cPhi2, "** Good Phi2" );
                if ( (*cPhi2)->hit()->isUsed() ) continue;
                coordPhi2 = circularRange.add( (*cPhi2)->coordHalfBox(), offset2);
                if ( !circularRange.contains( range2, coordPhi2 ) ) continue;

                PatVeloOpenTrack newTrack( *cR0, *cR2, *cPhi0, *cPhi2);
                if ( m_maxSlope < fabs( newTrack.tx() ) ||
                     m_maxSlope < fabs( newTrack.ty() )    ) continue;

                // Search for coordinates in the central module, first in R, then in phi
                rPred    = newTrack.rAtZ( zR1 );
                phiPred  = newTrack.phiAtZ( zR1 );

                double rTol = m_rMatchTol * sensorR1->rPitch(rPred);

                if ( isDebug ) {
                  info() << format( "  0: z%7.1f  r%7.3f    Phi: n%5d phi%8.4f dir%8.4f dist%6.2f",
                                    zR0, r0, (*cPhi0)->stripNumber(), coordPhi0,
                                    (*cPhi0)->referencePhi(), (*cPhi0)->referenceR() ) << endmsg;
                  info() << format( "  2: z%7.1f  r%7.3f    Phi: n%5d phi%8.4f dir%8.4f dist%6.2f",
                                    zR2, r2, (*cPhi2)->stripNumber() , coordPhi2,
                                    (*cPhi2)->referencePhi(), (*cPhi2)->referenceR() ) << endmsg;
                  printCoord( *cR0,   "R0   " );
                  printCoord( *cR2,   "R2   " );
                  printCoord( *cPhi0, "Phi0 " );
                  printCoord( *cPhi2, "Phi2 " );
                  info() << format( "     x0%8.3f tx%10.6f y0%8.3f ty%10.6f",
                                    newTrack.x0(), newTrack.tx(), newTrack.y0(), newTrack.ty() ) << endmsg;
                  info() << "   Search R1 z " << zR1 << " rPred " << rPred << " tol " << rTol
                         << " in sensor " << stationR1->sensorNumber() << endmsg;
                }

                //== Search if enough R coordinates should have been found earlier in the search
                int nPrevR = 0;
                int sensNb = iSens0 + 2;
                while ( sensNb <= firstRSensor ) {
                  double r = newTrack.rAtZ( m_rHitManager->station( sensNb )->z() );
                  if ( sensorR0->rMax(sectorR0) < r ) break;
                  if ( sensorR0->rMin(sectorR0) > r ) break;
                  ++nPrevR;
                  sensNb += 2;
                }
                if ( m_maxMissedR <= nPrevR ) {
                  if ( isDebug ) info() << "... missed " << nPrevR << " R measures " << endmsg;
                  continue;
                }

                int sectorR1 = sectorR0;
                Tf::PatVeloRHit* bestRc1 = stationR1->closestHitHalfBox( sectorR1, rPred, rTol );
                if ( 0 == bestRc1 ) {
                  if ( sectorR0 != sectorR2 ) {
                    sectorR1 = sectorR2;
                    bestRc1 = stationR1->closestHitHalfBox( sectorR1, rPred, rTol );
                  }
                  if ( 0 == bestRc1 ) continue;
                }
                if ( isDebug ) printCoord( bestRc1, ".. found R1 " );

                //=== We have 3 aligned points in R sensors
                r1 = bestRc1->coordHalfBox();
                rPredPhi = newTrack.rAtZ( zPhi1 );
                int sectorPhi1 = 0;
                if ( stationPhi1->sensor()->rMax( 0 ) < rPredPhi ) sectorPhi1 = 1;
                double offset1 = stationPhi1->sensor()->halfboxPhiOffset( sectorPhi1, rPredPhi );

                int zoneR1 = sectorR1;
                if ( sensorR0->isRight() ) zoneR1 += 4;
                std::pair<double,double> range1;
                bool hasRange = m_trackTool->phiRange( rPredPhi, zoneR1, m_phiAngularTol,
                                                       stationPhi1, sectorPhi1, range1 );
                if ( isDebug ) {
                  info() << " For phi:  zone " << sectorPhi1 << " rPredPhi " << rPredPhi 
                         << " hasRange " << hasRange
                         << " phiMin " << range1.first << " PhiMax " << range1.second << endmsg;
                }
                if ( !hasRange ) continue;

                phiPred = circularRange.sub( newTrack.phiAtZ( zPhi1 ), offset1);
                double phiTol = m_phiMatchTol * stationPhi1->sensor()->phiPitch(rPredPhi);

                if ( isDebug ) info() << "  Phi1Pred " << phiPred << " phiTol " << phiTol  << endmsg;

                Tf::PatVeloPhiHit* bestPhic1 = stationPhi1->closestHitHalfBox( sectorPhi1, phiPred, phiTol);
                if (0 == bestPhic1) continue;

                double dr = ( r1 - rPred ) / rTol;
                double dp = phiPred - bestPhic1->coordHalfBox();
                if (  Gaudi::Units::pi < dp ) dp -= 2*Gaudi::Units::pi;
                if ( -Gaudi::Units::pi > dp ) dp += 2*Gaudi::Units::pi;
                dp = dp / phiTol;
                double chi2 = 9. * ( dr * dr + dp * dp );  // tol = +- 3 sigmas
                if ( m_maxChi2 < chi2 ) {
                  if ( isDebug ) info() << "... Chi2 too big : " << chi2 << endmsg;
                  continue;
                }
                 
                if ( isDebug ) {
                  info() << "  **** Found space triplet chi2 " << chi2 << " ****" << endmsg;
                  printCoord( *cR0,      "R0   " );
                  printCoord( bestRc1,   "R1   " );
                  printCoord( *cR2,      "R2   " );
                  printCoord( *cPhi0,    "Phi0 " );
                  printCoord( bestPhic1, "Phi1 " );
                  printCoord( *cPhi2,    "Phi2 " );
                }

                //=== We have 3 aligned points in Phi sensors
                newTrack.addRCoord(*cR0);
                newTrack.addRCoord(bestRc1);
                newTrack.addRCoord(*cR2);
                newTrack.addPhiCoord(*cPhi0);
                newTrack.addPhiCoord(bestPhic1);
                newTrack.addPhiCoord(*cPhi2);
                newTrack.setChi2( chi2 );

                // Look for other clusters until outside sensor
                // Clusters in R and Phi, per station
                int rSens       = iSens0-6;
                int sectorPhi   = sectorPhi2;
                int nbMissedR   = 0;
                int nbMissedPhi = 0;
                while ( 0 <= rSens ) {
                  stationR = m_rHitManager->station( rSens );
                  rSens -= 2;
                  if ( 0 == stationR ) break;
                  zR         = stationR->z();
                  sensorR    = stationR->sensor();

                  //== Adjust R sector according to phi estimated value...
                  // double phi = newTrack.phiAtZ( zR );
                  int zoneR  = sectorR2;
                  rPred = newTrack.rAtZ( zR );
                  if ( isDebug ) info() << "Search in zone " << zoneR << " at r= " << rPred << endmsg;

                  double rTol = m_rExtrapTol * sensorR->rPitch(rPred) * newTrack.rCoords()->size() / 3.;

                  Tf::PatVeloRHit* bestRc = stationR->closestHitHalfBox( zoneR, rPred, rTol );
                  if (0 == bestRc) {
                    ++nbMissedR;
                    if ( 2 < nbMissedR ) break;
                  } else {
                    nbMissedR = 0;
                    if ( isDebug ) printCoord( bestRc, "Added R coordinate   ");
                    newTrack.addRCoord(bestRc);
                  }

                  int sensorPhi  = stationR->sensor()->associatedPhiSensor()->sensorNumber();
                  stationPhi = m_phiHitManager->station( sensorPhi );

                  zPhi = stationPhi->z();
                  rPredPhi = newTrack.rAtZ( zPhi );
                  if ( stationPhi->sensor()->rMin( sectorPhi ) > rPredPhi ) {
                    if ( 0 == sectorPhi ) break;
                    sectorPhi = 0;
                  }
                  if ( stationPhi->sensor()->rMax( sectorPhi ) < rPredPhi ) {
                    if ( 1 == sectorPhi ) break;
                    sectorPhi = 1;
                  }
                  if ( sensorR0->isRight() ) zoneR += 4;
                  std::pair<double,double> range;
                  bool hasRange = m_trackTool->phiRange( rPredPhi, zoneR, m_phiAngularTol,
                                                         stationPhi, sectorPhi, range );
                  if (!hasRange) continue;

                  double phiTol = m_phiExtrapTol * stationPhi->sensor()->phiPitch(rPredPhi);
                  double offset = stationPhi->sensor()->halfboxPhiOffset( sectorPhi, rPredPhi );
                  phiPred = circularRange.sub(  newTrack.phiAtZ( zPhi ), offset);

                  Tf::PatVeloPhiHit* bestPhic = stationPhi->closestHitHalfBox( sectorPhi, phiPred, phiTol);
                  if ( 0 == bestPhic ) {
                    ++nbMissedPhi;
                    if ( 2 < nbMissedPhi ) break;
                  } else {
                    nbMissedPhi = 0;
                    if ( isDebug ) printCoord( bestPhic, "Added Phi coordinate ");
                    newTrack.addPhiCoord(bestPhic);
                  }
                }
                // Compute final chi2, backward flag, ...
                newTrack.finalFit();

                bool store = false;
                if ( 0 < bestTrack.nCoords() ) {
                  unsigned int minCoord = bestTrack.nCoords() - 2;
                  unsigned int maxCoord = bestTrack.nCoords() + 1;
                  if ( 8 > minCoord ) minCoord = minCoord + 1;

                  // if track is long enough, compare mainly chi2
                  if ( maxCoord <  newTrack.nCoords()    ) {
                    store = true;
                  } else if ( minCoord < newTrack.nCoords() ) {
                    if ( bestTrack.chi2() > newTrack.chi2() ) store = true;
                  }
                } else {
                  store = true;
                }
                if ( store ) {
                  if ( isDebug ) info() << " ...keep it as best, chi2 = " << newTrack.chi2() << endmsg;
                  bestTrack = newTrack;
                }
              }
            }

            if ( 0 < bestTrack.nCoords() ) {
              bool mustTag = ( 3 < bestTrack.rCoords()->size() &&
                               3 < bestTrack.phiCoords()->size() );
              if ( mustTag || m_maxChi2 > bestTrack.chi2() ) {
                matchShared( bestTrack, myTracks );
                if ( bestTrack.valid() ) {
                  myTracks.push_back( bestTrack );
                
                  if ( isDebug ) info() << "=== Stored track " << myTracks.size()
                                        << " chi2 " << bestTrack.chi2()
                                        << " firstZ " << bestTrack.firstZ()
                                        << " Backward " << bestTrack.backward()
                                        << endmsg;
                  std::vector<Tf::PatVeloRHit*>::const_iterator itC;
                  for ( itC = bestTrack.rCoords()->begin(); bestTrack.rCoords()->end() != itC; ++itC ) {
                    if ( isDebug ) printCoord( *itC, "R   " );
                    if ( mustTag ) (*itC)->hit()->setUsed( true );
                  }
                  std::vector<Tf::PatVeloPhiHit*>::const_iterator itP;
                  for ( itP = bestTrack.phiCoords()->begin(); bestTrack.phiCoords()->end() != itP; ++itP ) {
                    if ( isDebug ) printCoord( *itP, "Phi " );
                    if ( mustTag ) (*itP)->hit()->setUsed( true );
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  for ( std::vector<PatVeloOpenTrack>::iterator itSpace = myTracks.begin(); 
        myTracks.end() != itSpace; itSpace++ ) {
    PatVeloOpenTrack& tr = (*itSpace);
    tagManyUsed( tr );
    if ( !tr.valid() ) continue;
    LHCb::Track* patTr = new LHCb::Track();
    outputTracks->add( patTr );

    patTr->setType( LHCb::Track::Velo );
    patTr->setHistory( LHCb::Track::PatVeloOpen );
    patTr->setFlag( LHCb::Track::Backward, tr.backward() );
    patTr->setPatRecStatus( LHCb::Track::PatRecIDs );

    double z = tr.firstZ();
    Gaudi::XYZPoint offset = m_velo->halfBoxOffset( tr.half() );

    LHCb::State temp;
    temp.setLocation( LHCb::State::FirstMeasurement );
    temp.setX( tr.xAtZ( z ) + offset.x() );
    temp.setY( tr.yAtZ( z ) + offset.y() );
    temp.setZ( z );
    temp.setTx( tr.tx() );
    temp.setTy( tr.ty() );
    temp.setQOverP( 0. );
    Gaudi::TrackSymMatrix & cov = temp.covariance();
    cov( 0, 0 ) = tr.errX2();
    cov( 1, 1 ) = tr.errY2();
    cov( 2, 2 ) = tr.errTx2();
    cov( 3, 3 ) = tr.errTy2();
    patTr->addToStates( temp );

    temp.setLocation( LHCb::State::EndVelo );
    z = tr.lastZ();
    temp.setX( tr.xAtZ( z ) + offset.x() );
    temp.setY( tr.yAtZ( z ) + offset.y() );
    temp.setZ( z );
    temp.setTx( tr.tx() );
    temp.setTy( tr.ty() );
    temp.setQOverP( 0. );
    cov = temp.covariance();
    cov( 0, 0 ) = tr.errX2();
    cov( 1, 1 ) = tr.errY2();
    cov( 2, 2 ) = tr.errTx2();
    cov( 3, 3 ) = tr.errTy2();
    patTr->addToStates( temp );

    std::vector<Tf::PatVeloRHit*>::const_iterator itC;
    std::vector<Tf::PatVeloPhiHit*>::const_iterator itP;
    for ( itC = tr.rCoords()->begin(); tr.rCoords()->end() != itC; itC++ ) {
      patTr->addToLhcbIDs((*itC)->hit()->lhcbID());
    }
    for ( itP = tr.phiCoords()->begin(); tr.phiCoords()->end() != itP; itP++ ) {
      patTr->addToLhcbIDs((*itP)->hit()->lhcbID());
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatVeloOpenTracking::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//=========================================================================
//  Identify tracks sharing one R and one Phi hit. Keep only one...
//=========================================================================
void PatVeloOpenTracking::matchShared ( PatVeloOpenTrack& track, 
                                        std::vector<PatVeloOpenTrack>& tracks) {
  std::vector<PatVeloOpenTrack>::iterator itT;
  std::vector<Tf::PatVeloRHit*>::const_iterator itR, itR1;
  std::vector<Tf::PatVeloPhiHit*>::const_iterator itP, itP1;

  for ( itT = tracks.begin(); tracks.end() != itT; ++itT ) {
    if ( !(*itT).valid() ) continue;
    int nbMatch = 0;
    itR1 = track.rCoords()->begin();
    for ( itR = (*itT).rCoords()->begin(); (*itT).rCoords()->end() != itR; ++itR ) {
      while ( itR1 != track.rCoords()->end() &&
              (*itR1)->sensorNumber() > (*itR)->sensorNumber() ) ++itR1;
      if ( itR1 == track.rCoords()->end() ) break;
      if ( (*itR1)->hit()->lhcbID() == (*itR)->hit()->lhcbID() ) ++nbMatch;
    }
    if ( 0 == nbMatch ) continue;
    //== Same for phi
    nbMatch = 0;
    itP1 = track.phiCoords()->begin();
    for ( itP = (*itT).phiCoords()->begin(); (*itT).phiCoords()->end() != itP; ++itP ) {
      while ( itP1 != track.phiCoords()->end() &&
              (*itP1)->sensorNumber() > (*itP)->sensorNumber() ) ++itP1;
      if ( itP1 == track.phiCoords()->end() ) break;
      if ( (*itP1)->hit()->lhcbID() == (*itP)->hit()->lhcbID() ) ++nbMatch;
    }
    if ( 0 == nbMatch ) continue;
    //== Tag the worst one
    if ( track.nCoords() > (*itT).nCoords() ) {
      (*itT).setValid( false );
    } else if ( track.nCoords() < (*itT).nCoords() ) {
       track.setValid( false );
       break;
    } else if ( track.chi2() < (*itT).chi2() ) {
      (*itT).setValid( false );
    } else {
      track.setValid( false );
      break;
    }
  }
}

//=========================================================================
//  
//=========================================================================
void PatVeloOpenTracking::tagManyUsed( PatVeloOpenTrack& track ) {
  //== Ignore tracks where hits are tagged by the track.
  if ( 3 < track.rCoords()->size() &&
       3 < track.phiCoords()->size() ) return;
  
  int nUsed = 0;
  std::vector<Tf::PatVeloRHit*>::const_iterator itR;
  std::vector<Tf::PatVeloPhiHit*>::const_iterator itP;
  for ( itR = track.rCoords()->begin(); track.rCoords()->end() != itR; ++itR ) {
    if ( (*itR)->hit()->isUsed() ) ++nUsed;
  }
  for ( itP = track.phiCoords()->begin(); track.phiCoords()->end() != itP; ++itP ) {
    if ( (*itP)->hit()->isUsed() ) ++nUsed;
  }
  if ( 1 < nUsed ) track.setValid( false );
}
//=============================================================================
