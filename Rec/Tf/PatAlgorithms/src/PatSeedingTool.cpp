// $Id: PatSeedingTool.cpp,v 1.6 2007-12-04 20:48:07 smenzeme Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "OTDet/DeOTDetector.h"
// from TrackEvent
#include "Event/StateParameters.h"
#include <boost/assign/list_of.hpp>
// local
#include "PatSeedingTool.h"
#include "PatFwdFitLine.h"
#include "PatFwdPlaneCounter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatSeedingTool
//
// 2006-10-13 : Olivier Callot
//-----------------------------------------------------------------------------


// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PatSeedingTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatSeedingTool::PatSeedingTool(  const std::string& type,
				 const std::string& name,
				 const IInterface* parent)
  : GaudiTool ( type, name , parent )
{

  declareInterface<IPatSeedingTool>(this);
  declareInterface<ITracksFromTrack>(this);

  declareProperty( "InputTracksName",   m_inputTracksName  = LHCb::TrackLocation::Forward  );

  declareProperty( "zMagnet",           m_zMagnet          = 5300. * Gaudi::Units::mm );
  declareProperty( "xMagTol",           m_xMagTol          = 2000. * Gaudi::Units::mm );
  declareProperty( "TolCollectOT",      m_tolCollectOT     = 3.    * Gaudi::Units::mm );
  declareProperty( "TolCollectIT",      m_tolCollectIT     = 0.3   * Gaudi::Units::mm );
  declareProperty( "InitialArrow",      m_initialArrow     = 0.0022                   );
  declareProperty( "CurveTol",          m_curveTol         =    5. * Gaudi::Units::mm );
  declareProperty( "zReference",        m_zReference       =  StateParameters::ZMidT);
  declareProperty( "ZOutput",           m_zOutputs        = boost::assign::list_of(StateParameters::ZBegT)
		   (StateParameters::ZMidT)(StateParameters::ZEndT));
  declareProperty( "dRatio",            m_dRatio           = -0.377e-3                );
  declareProperty( "TolExtrapolate",    m_tolExtrapolate   = 4.    * Gaudi::Units::mm );
  declareProperty( "yCorrection",       m_yCorrection      = .8e14 * Gaudi::Units::mm );

  declareProperty( "MinXPlanes",        m_minXPlanes       = 5                        );
  declareProperty( "CommonXFraction",   m_commonXFraction  = 0.69                     );
  declareProperty( "MaxRangeOT",        m_maxRangeOT       =  150. * Gaudi::Units::mm );
  declareProperty( "MaxRangeIT",        m_maxRangeIT       =   50. * Gaudi::Units::mm );
  declareProperty( "MaxChi2HitOT",      m_maxChi2HitOT     =   30.                    );
  declareProperty( "MaxChi2HitIT",      m_maxChi2HitIT     =   10.                    );
  declareProperty( "MinTotalPlanes",    m_minTotalPlanes   =   10                     );
  declareProperty( "MaxTrackChi2",      m_maxTrackChi2     =   20.                    );
  declareProperty( "MaxFinalChi2",      m_maxFinalChi2     =   15.                    );
  declareProperty( "MaxFinalTrackChi2", m_maxFinalTrackChi2=   10.                    );
  declareProperty( "MaxYAtOrigin",      m_maxYAtOrigin     =  400. * Gaudi::Units::mm );

  declareProperty( "MinMomentum",       m_minMomentum      =    0. * Gaudi::Units::MeV );

  declareProperty( "StateErrorX2",      m_stateErrorX2     =   4.0);
  declareProperty( "StateErrorY2",      m_stateErrorY2     = 400.);
  declareProperty( "StateErrorTX2",     m_stateErrorTX2    =  6.e-5);
  declareProperty( "StateErrorTY2",     m_stateErrorTY2    =  1.e-4);
  declareProperty( "StateErrorP",       m_stateErrorP      =  0.40);
  declareProperty( "MomentumScale",     m_momentumScale    = 40.  );
  declareProperty( "MomentumParams",     m_momentumParams  =  boost::assign::list_of
		   (-6.3453) (-4.77725) (-14.9039) (3.13647e-08) );
  declareProperty( "MeasureTime",       m_measureTime      = false                    );
  declareProperty( "UseForward",        m_useForward       = false                    );
  }
//=============================================================================
// Destructor
//=============================================================================
PatSeedingTool::~PatSeedingTool() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatSeedingTool::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTool

  m_tHitManager    = tool<Tf::TStationHitManager<PatForwardHit> >("PatTStationHitManager");

  m_seedTool = tool<PatSeedTool>( "PatSeedTool" );

  m_zForYMatch = 9000.;
  //== Max impact: first term is due to arrow ->curvature by / (700 mm)^2 then momentum to impact at z=0
  //== second term is decay of Ks: 210 MeV Pt, 2000 mm decay distance.
  m_maxImpact = 700. * 700. / ( m_momentumScale * m_initialArrow * m_minMomentum ) +
    2000. * 210 / m_minMomentum;

  if ( m_measureTime ) {
    m_timer = tool<ISequencerTimerTool>( "SequencerTimerTool" );
    m_timer->increaseIndent();
    m_timeInit   = m_timer->addTimer( "Use Fwd + Tsa tracks" );
    m_timeX      = m_timer->addTimer( "find X projections" );
    m_timeStereo = m_timer->addTimer( "find stereo" );
    m_timeItOt   = m_timer->addTimer( "find IT+OT" );
    m_timer->decreaseIndent();
  }

  if (m_momentumParams.size()<4) return Error ( "Not enough momentumParams values" );

  return StatusCode::SUCCESS;
}


void PatSeedingTool::prepareHits() {
 
  m_tHitManager->prepareHits();

  m_printing = msgLevel( MSG::DEBUG );

  if ( m_measureTime ) m_timer->start( m_timeInit );

  PatFwdHits::const_iterator itH;
  for (unsigned int sta = 0; sta < m_nSta; sta ++){
    for (unsigned int lay = 0; lay<m_nLay; lay++){
      for (unsigned int region = 0; region <m_nReg; region ++){
        Tf::TStationHitManager<PatForwardHit>::HitRange range = m_tHitManager->hits(sta, lay, region);
        for (itH = range.begin(); range.end() != itH; ++itH ) {
          PatFwdHit* hit = *itH;
	  if ( m_useForward && hit->hit()->testStatus(Tf::HitBase::UsedByPatForward))
	    hit->setIsUsed(true);
	  else
	    hit->setIsUsed(false);
          hit->setSelected( true );
          hit->setIgnored( false );
          const Tf::OTHit* otHit = (*itH)->hit()->othit();
          if(otHit) {
            if ( otHit->untruncatedDriftDistance() >  2.8 ||
                 otHit->untruncatedDriftDistance() < -0.6    ) {
	      hit->setIsUsed( true );
            } else if ( msgLevel( MSG::VERBOSE ) ) {
              debugFwdHit( hit, verbose() );
            }
          }
	}
      }
    }
  }

  if ( m_measureTime ) m_timer->stop( m_timeInit );
}

StatusCode PatSeedingTool::performTracking( std::vector<LHCb::Track*>& outputTracks) {

  unsigned int lay;
  unsigned int region;
  std::vector<PatSeedTrack>::iterator itS;
  PatFwdHits::const_iterator itH;

  for ( region = 0; region < m_nReg; ++region ) {

    m_tolCollect = m_tolCollectOT;
    m_maxChi2Hit = m_maxChi2HitOT;
    if ( 1 < region ) {
      m_tolCollect = m_tolCollectIT;
      m_maxChi2Hit = m_maxChi2HitIT;
    }

    for ( lay = 0; lay < m_nLay; lay+=3 ){  // loop on X layers
      std::vector<PatSeedTrack> candidates;

      if ( m_measureTime ) m_timer->start( m_timeX );

      findXCandidates( lay, region, candidates );

      if ( msgLevel( MSG::DEBUG ) ) {
        debug() << "Found " << candidates.size() << " x candidates" << endreq;
      }
      if ( m_measureTime ) {
        m_timer->stop( m_timeX );
        m_timer->start( m_timeStereo );
      }

      for ( itS = candidates.begin(); candidates.end() != itS; ++itS ) {

        PatSeedTrack& track = *itS;
        track.setValid( false );

        m_printing = msgLevel( MSG::DEBUG );
       
        if ( m_printing ) {
          info() << "--- X candidate " << itS - candidates.begin() << endreq;
          for ( itH = track.coordBegin(); track.coordEnd() != itH; ++itH ) {
            info() << format( "dist %7.3f ", track.distance( *itH ) );
            debugFwdHit( *itH, info() );
          }
        }

        //== Collect stereo hits

        PatFwdHits stereo;

        collectStereoHits( track, region, stereo );

        int minNbPlanes = m_minTotalPlanes - track.nPlanes();

        if ( m_printing ) {
          info() << "--- Initial list of stereo hits " << endreq;
          for ( itH = stereo.begin(); stereo.end() != itH; ++itH ) {
            if ( (*itH)->isIgnored() ) continue;
            debugFwdHit( *itH, info() );
          }
        }
        if ( stereo.size() < (unsigned int) minNbPlanes )  continue;

        bool ok = findBestRange( region, minNbPlanes, stereo );

        if ( !ok ) continue;

        if ( m_printing ) {
          info() << "--- After filtering of stereo hits " << endreq;
          for ( itH = stereo.begin(); stereo.end() != itH; ++itH ) {
            if ( (*itH)->isIgnored() ) continue;
            debugFwdHit( *itH, info() );
          }
        }

        //== restore real coordinate

        double y0, sl;
        ok = fitLineInY( stereo, y0, sl );

        if ( !ok ) {
          if ( m_printing ) info() << "  -- fitLineInY failed" << endreq;
          continue;
        }
        double yAtOrigin = y0;
        double yCorr     = m_yCorrection * sl * sl * track.curvature() * track.curvature();
        if ( sl > 0 ) {
          yAtOrigin = y0 - yCorr;
        } else {
          yAtOrigin = y0 + yCorr;
        }

        if ( m_printing ) {
          info() << format( "  Y0 %10.1f corr %10.1f yAtOrigin%10.1f  sl%10.7f  curv*E6 %8.4f",
                            y0, yCorr, yAtOrigin, sl, 1.e6*track.curvature() ) << endreq;
        }

        if ( m_maxYAtOrigin < fabs( yAtOrigin ) ) {
          if ( m_printing ) info() << "  -- yAtOrigin too big : " << yAtOrigin << endreq;
          continue;  // non pointing track -> garbage
        }

        if ( m_printing ) {
          info() << "Fitted line y0 " << y0 << " sl " << sl << endreq;
          for ( itH = stereo.begin(); stereo.end() != itH; ++itH ) {
            if ( (*itH)->isIgnored() ) continue;
            info() << format( "dist %7.3f ",  (*itH)->projection() - y0 - (*itH)->z() * sl );
            debugFwdHit( *itH, info() );

          }
        }
        //== Check for enough OT if not near boundary
        if ( region < m_nOTReg ) {
          double y = y0 + m_zForYMatch * sl;
          if ( 50. < fabs(y) ) {
            std::vector<int> nInStation(3,0);
            for ( itH = stereo.begin(); stereo.end() != itH; ++itH ) {
              if ( (*itH)->isIgnored() ) continue;
              nInStation[ (*itH)->hit()->station() ]++;
            }
            if ( 2 > nInStation[0] ||
                 2 > nInStation[1] ||
                 2 > nInStation[2]   ) {
              if ( m_printing ) info() << "Not enough OT stereo in some station : "
                                       << nInStation[0] << " "
                                       << nInStation[1] << " "
                                       << nInStation[2] << endreq;
              continue;
            }
          }
        }

        for ( itH = stereo.begin(); stereo.end() != itH; ++itH ) {
          if ( (*itH)->isIgnored() ) continue;
          track.addCoord( *itH );
        }


        if ( region < m_nOTReg &&
             17 > track.nCoords() &&
             2 * (track.nCoords()-track.nPlanes()+1) < track.nbOnSide() ) {
          if ( m_printing ) info()  << "Too many on side " << track.nbOnSide() << " with only "
                                    << track.nCoords() << " coordinates" << endreq;
          track.setValid( false );
          continue;
        }

        track.setYParams( y0, sl);
        track.sort();
        if ( m_printing ) info() << "-- Before fit, nPlanes = " << track.nPlanes() << endreq;
        ok = m_seedTool->fitTrack( track, m_maxFinalChi2, m_minTotalPlanes, false, m_printing );
	
	track.setValid( ok );
        if ( m_printing ) info() << "After fit status "<< ok << " chi2 " << track.chi2()
                                 << ", nPlanes = " << track.nPlanes() << endreq;
        if ( region < m_nOTReg  &&   // OT
             17 > track.nCoords() &&  // short track -> not too many neighbours
             2 * (track.nCoords() - track.nPlanes() +1 ) < track.nbOnSide() ) {
          if ( m_printing ) {
            info()  << "Too many on side " << track.nbOnSide() << " with only "
                    << track.nCoords() << " coordinates" << endreq;
            for ( itH = track.coordBegin(); track.coordEnd() != itH; ++itH ) {
              debugFwdHit( *itH, info() );
            }
          }
          track.setValid( false );
          continue;
        }

        if ( m_maxFinalTrackChi2 < track.chi2() || !ok) {
          if ( m_printing ) info() << "Chi2 too big" << endreq;
          track.setValid( false );
          continue;
        }
      }

      //== Sort tracks according to their chi2

      m_printing = msgLevel( MSG::DEBUG );

      std::vector<PatSeedTrack*> finalPt;
      finalPt.reserve( candidates.size() );

      for ( itS = candidates.begin(); candidates.end() != itS; ++itS ) {
        if ( (*itS).valid() ) finalPt.push_back( &(*itS) );
      }
      std::sort( finalPt.begin(), finalPt.end(), PatSeedTrack::decreasingQuality() );
            
      //== Keep only those with >50% unused clusters (in the current pass...)
      for ( std::vector<PatSeedTrack*>::iterator itTr = finalPt.begin();
            finalPt.end() != itTr; ++itTr ) {
        int nbUsed = 0;
        for ( itH = (*itTr)->coordBegin(); (*itTr)->coordEnd() != itH; ++itH ) {
          if ( (*itH)->isUsed() ) ++nbUsed;
        }
        if ( nbUsed > .5 * (*itTr)->nCoords() ) continue;

        if ( m_printing ) {
          info() << "** Store track, estimated P " << 1./( .038e6 * (*itTr)->curvature() )
                 << " nCoord " << (*itTr)->nCoords() << " chi2 " << (*itTr)->chi2() << endreq;
          for ( itH = (*itTr)->coordBegin(); (*itTr)->coordEnd() != itH; ++itH ) {
            debugFwdHit( *itH, info() );
          }
        }
        storeTrack( **itTr, outputTracks );
      }
      if ( m_measureTime ) m_timer->stop( m_timeStereo );
    }
  }

  //== Find remaining points in IT

  if ( m_measureTime ) m_timer->start( m_timeItOt );

  if ( m_printing ) info() << "=== remaining IT coord ====" << endreq;
  m_tolCollect = m_tolCollectIT;

  for ( region = m_nOTReg; region < m_nReg; ++region ) {
    std::vector<PatSeedTrack> candidates;
    for ( unsigned int sta = 0; sta < m_nSta; ++sta ) {
      for ( unsigned int lay = 0; lay < m_nLay; lay++ ) {
        Tf::TStationHitManager<PatForwardHit>::HitRange range = m_tHitManager->hits(sta, lay, region);
        for (itH = range.begin(); range.end() != itH; ++itH ) {
          if ( (*itH)->isUsed() ) continue;
          (*itH)->setX((*itH)->hit()->xAtYEq0());
          (*itH)->setZ((*itH)->hit()->zAtYEq0());
          const Tf::OTHit* otHit = (*itH)->hit()->othit();
          if(otHit) {
            (*itH)->setDriftDistance(otHit->untruncatedDriftDistance());
          }
        }
      }


      double zScaling = ( m_tHitManager->region(sta,3,region)->z() - m_tHitManager->region(sta,0,region)->z() /
                          ( m_tHitManager->region(sta, 0, region)->z() - m_zMagnet ));

      Tf::TStationHitManager<PatForwardHit>::HitRange rangeH0 = m_tHitManager->hits(sta,0,region);
      for ( PatFwdHits::const_iterator itH0 = rangeH0.begin();
            rangeH0.end() != itH0; ++itH0 ) {
        if ( (*itH0)->isUsed() ) continue;
        PatFwdHit* hit0 = *itH0;

        m_printing = msgLevel( MSG::VERBOSE );
	if ( m_printing ) info() << "*IT* hit sta " << sta << " lay 0 x = " << hit0->x() << endreq;
        double x3Min = hit0->x() + ( hit0->x() - m_xMagTol) * zScaling;
        double x3Max = hit0->x() + ( hit0->x() + m_xMagTol) * zScaling;
        Tf::TStationHitManager<PatForwardHit>::HitRange rangeH3 = m_tHitManager->hitsWithMinX( x3Min,sta,3,region );
        for ( PatFwdHits::const_iterator itH3 = rangeH3.begin(); rangeH3.end() != itH3; ++itH3 ) {

          if ( (*itH3)->isUsed() ) continue;
          PatFwdHit* hit3 = *itH3;
          if ( hit3->x() < x3Min ) continue;
          if ( hit3->x() > x3Max ) break;
          double z0 = hit0->z();
          double x0 = hit0->x();
          double sl = (hit3->x() - x0 ) / (hit3->z() - z0 );
          double x1Min = m_tHitManager->region(sta,1,region)->ymin() * m_tHitManager->region(sta,1,region)->sinT();
          double x1Max = m_tHitManager->region(sta,1,region)->ymax() * m_tHitManager->region(sta,1,region)->sinT();
          if ( x1Min > x1Max ) {
            double swap = x1Min;
            x1Min = x1Max;
            x1Max = swap;
          }
          x1Min = x1Min + x0 + ( m_tHitManager->region(sta,1,region)->z() - z0 ) *sl - m_tolCollect;
          x1Max = x1Max + x0 + ( m_tHitManager->region(sta,1,region)->z() - z0 ) *sl + m_tolCollect;
	  if ( m_printing ) info() << "     hit sta " << sta << " lay 3 x = " << hit3->x()
                                   << " x1Min " << x1Min << " x1Max " << x1Max << endreq;

          Tf::TStationHitManager<PatForwardHit>::HitRange rangeH1 = m_tHitManager->hitsWithMinX( x1Min, sta,1,region );
          for ( PatFwdHits::const_iterator itH1 = rangeH1.begin(); rangeH1.end() != itH1; ++itH1 ) {
            PatFwdHit* hit1 = *itH1;

            if ( hit1->isUsed() ) continue;
            hit1->setX(hit1->hit()->xAtYEq0());
            hit1->setZ(hit1->hit()->zAtYEq0());
            const Tf::OTHit* otHit = hit1->hit()->othit();
            if(otHit)
              hit1->setDriftDistance(otHit->untruncatedDriftDistance());
            if ( x1Min > hit1->x() ) continue;
            if ( x1Max < hit1->x() ) break;
            double y = ( (x0 + ( hit1->z() - z0 )*sl) - hit1->x() ) / hit1->hit()->dxDy();
            double pred = x0 + ( m_tHitManager->region(sta,2,region)->z() - z0 ) *sl + y * m_tHitManager->region(sta,2,region)->sinT();
            double x2Min = pred - 1.5*m_tolCollect;
            double x2Max = pred + 1.5*m_tolCollect;

            if ( m_printing ) info() << "     hit sta " << sta << " lay 1 x = " << hit1->x()
                                     << " x2Min " << x2Min << " x2Max " << x2Max << endreq;

            Tf::TStationHitManager<PatForwardHit>::HitRange rangeH2 = m_tHitManager->hitsWithMinX( x2Min, sta, 2, region );
            for ( PatFwdHits::const_iterator itH2 = rangeH2.begin(); rangeH2.end() != itH2; ++itH2 ) {

              PatFwdHit* hit2 = *itH2;
              if ( hit2->isUsed() ) continue;
              hit2->setX(hit2->hit()->xAtYEq0());
              hit2->setZ(hit2->hit()->zAtYEq0());
              const Tf::OTHit* otHit = hit2->hit()->othit();
              if(otHit)
                hit2->setDriftDistance(otHit->untruncatedDriftDistance());
              if ( x2Min > hit2->x() ) continue;
              if ( x2Max < hit2->x() ) break;
	      if ( m_printing )
                info() << "     hit sta " << sta << " lay 2 x = " << hit2->x() << endreq;
              PatSeedTrack track( hit0, hit1, hit2, hit3, m_zReference, m_dRatio, m_initialArrow );
              track.setYParams( 0, y/hit2->z() );
              if ( 3 > track.nbHighThreshold() ) {
                if ( m_printing ) info() << "   less than 3 with high threshold" << endreq;
                continue;
              }

              // Add neighbouring hits

              if ( itH0+1 != rangeH0.end() ) {
                if ( (*(itH0+1))->x() - hit0->x() < m_tolCollect ) {
                  ++itH0;
                  track.addCoord( *itH0 );
                  if ( m_printing ) info() << "   + hit sta " << sta << " lay 0 x = "
                                           << (*itH0)->x() << endreq;
                }
              }

              if ( itH1+1 != rangeH1.end() ) {
                if ( (*(itH1+1))->x() - hit1->x() < m_tolCollect ) {
                  ++itH1;
                  track.addCoord( *itH1 );
                  if ( m_printing ) info() << "   + hit sta " << sta << " lay 1 x = "
                                           << (*itH1)->x() << endreq;
                }
              }

              if ( itH2+1 != rangeH2.end() ) {
                if ( (*(itH2+1))->x() - hit2->x() < m_tolCollect ) {
                  ++itH2;
                  track.addCoord( *itH2 );
                  if ( m_printing ) info() << "   + hit sta " << sta << " lay 2 x = "
                                           << (*itH2)->x() << endreq;
                }
              }

              if ( itH3+1 != rangeH3.end() ) {
                if ( (*(itH3+1))->x() - hit3->x() < m_tolCollect ) {
                  ++itH3;
                  track.addCoord( *itH3 );
                  if ( m_printing ) info() << "   + hit sta " << sta << " lay 3 x = "
                                           << (*itH3)->x() << endreq;
                }
              }

              candidates.push_back( track );
              if ( m_printing ) {
                info() << "--- IT point -- " << endreq;
                for ( itH = track.coordBegin(); track.coordEnd() != itH; ++itH ) {
                  debugFwdHit( *itH, info() );
                }
              }
            }
          }
        }
      }
    }

    if ( m_printing ) info() << "===== Candidates Type " << region  << endreq;
    for ( std::vector<PatSeedTrack>::iterator itT = candidates.begin();
          candidates.end() != itT; ++itT ) {
      PatSeedTrack& track = *itT;
      m_printing = msgLevel( MSG::DEBUG );
     
      if ( m_printing ) info() << "   candidate " << itT - candidates.begin()
                               << " y(0) = " << track.yAtZ( 0. )
                               << " ySlope " << track.ySlope( 0. )
                               << endreq;
      PatFwdHit* firstHit = *track.coordBegin();

      int ITSta = firstHit->hit()->station();
      int nbSta = 1;
      int otTyp = 0;
      if ( track.yAtZ( firstHit->z() ) > 0 ) otTyp = 1;

      //== Some data in OT ??
      for ( int sta = 0; 3 > sta; ++sta ) {
        if ( sta == ITSta ) continue;
        int nbPlanes = track.nPlanes();

        for ( lay = 0 ; 4 > lay ; ++lay ) {
          if ( track.yAtZ( m_tHitManager->region(sta,lay,otTyp)->z() ) <
               m_tHitManager->region(sta,lay,otTyp)->ymin() - m_tolExtrapolate ||
               track.yAtZ( m_tHitManager->region(sta,lay,otTyp)->z() ) >
               m_tHitManager->region(sta,lay,otTyp)->ymax() + m_tolExtrapolate    ) continue;

          double xPred = track.xAtZ( m_tHitManager->region(sta,lay,otTyp)->z() ) +
            track.yAtZ( m_tHitManager->region(sta,lay,otTyp)->z() ) * m_tHitManager->region(sta,lay,otTyp)->sinT();
          if ( m_printing ) {
            info() << "** Search in sta " << sta << " lay " << lay << " xPred " << xPred << endreq;
          }

          Tf::TStationHitManager<PatForwardHit>::HitRange rangeXX =
            m_tHitManager->hitsWithMinX( xPred - m_tolExtrapolate - 3.,sta,lay,otTyp );
          for ( itH = rangeXX.begin(); rangeXX.end() != itH; ++itH ) {
            if ( (*itH)->isUsed() ) continue;
            PatFwdHit* hit = *itH;
            updateHitForTrack( hit, track.yAtZ(hit->z()),0);
            if ( - m_tolExtrapolate > track.distance( hit ) ) continue;
            if (   m_tolExtrapolate < track.distance( hit ) ) break;
            if ( m_printing ) {
              info() << "Add coord ";
              debugFwdHit( hit, info() );
            }
            track.addCoord( hit );
          }
        }
        if ( 3 <= track.nPlanes() - nbPlanes  ) {
          nbSta++;
          int minPlanes = 4* nbSta - 1;
          bool ok = m_seedTool->fitTrack( track, m_maxChi2HitOT, minPlanes, false, m_printing );
          if ( m_printing ) info() << "*** refitted track candidate status " << ok << endreq;
        }
      }
      // Try in IT again if needed
      if ( 2 == nbSta ) {
        for ( int sta = 0; 3 > sta; ++sta ) {
          if ( sta == ITSta ) continue;
          int nbAdded = 0;
          for ( lay = 0 ; 4 > lay ; ++lay ) {
            if ( track.yAtZ( m_tHitManager->region(sta, lay, region)->z() ) <
                 m_tHitManager->region(sta, lay, region)->ymin() - m_tolExtrapolate ||
                 track.yAtZ( m_tHitManager->region(sta, lay, region)->z() ) >
                 m_tHitManager->region(sta, lay, region)->ymax() + m_tolExtrapolate    ) continue;
            double xPred = track.xAtZ( m_tHitManager->region(sta, lay, region)->z() ) +
              track.yAtZ( m_tHitManager->region(sta, lay, region)->z() ) * m_tHitManager->region(sta, lay, region)->sinT();
            if ( m_printing ) {
              info() << "** Search in IT sta " << sta << " lay " << lay << " region " << region
                     << " xPred " << xPred << endreq;
            }

            Tf::TStationHitManager<PatForwardHit>::HitRange rangeYY =
              m_tHitManager->hitsWithMinX( xPred - m_tolExtrapolate, sta, lay, region );
            for ( itH = rangeYY.begin(); rangeYY.end() != itH; ++itH ) {
              if ( (*itH)->isUsed() ) continue;
              PatFwdHit* hit = *itH;
              updateHitForTrack( hit, track.yAtZ(hit->z()), 0);
              if ( - m_tolExtrapolate > track.distance( hit ) ) continue;
              if (   m_tolExtrapolate < track.distance( hit ) ) break;
              if ( m_printing ) {
                info() << "Add coord ";
                debugFwdHit( hit, info() );
              }
              track.addCoord( hit );
              nbAdded++;
            }
          }
          if ( 3 < nbAdded ) {
            nbSta++;
            int minPlanes = 4* nbSta +1-nbSta;
            m_seedTool->fitTrack( track, m_maxChi2HitOT, minPlanes, false, m_printing );
          }
        }
      }


      if ( m_printing ) {
        info() << " --- We have found " << nbSta << " stations." << endreq;
        for ( itH = track.coordBegin(); track.coordEnd() != itH; ++itH ) {
          debugFwdHit( *itH, info() );
        }
      }
      if ( 3 == nbSta ) storeTrack( track, outputTracks );
    }
  }
  if ( m_measureTime ) m_timer->stop( m_timeItOt );

  return StatusCode::SUCCESS;
}


//=========================================================================
// Store the track in the final container
//=========================================================================
void PatSeedingTool::storeTrack ( PatSeedTrack& track, std::vector<LHCb::Track*>& outputTracks) {
  LHCb::Track* out = new LHCb::Track();
  out->setType( LHCb::Track::Ttrack );
  out->setHistory( LHCb::Track::PatForward );
  out->setPatRecStatus( LHCb::Track::PatRecIDs );
  debug() << "==== Storing track " << outputTracks.size() << endreq;
  for ( PatFwdHits::iterator itH = track.coordBegin(); track.coordEnd() != itH; ++itH ) {
    out->addToLhcbIDs( (*itH)->hit()->lhcbID() );
    //== Tag used coordinates
    (*itH)->hit()->setStatus(Tf::HitBase::UsedByPatSeeding);
    (*itH)->setIsUsed(true);
    if ( msgLevel( MSG::DEBUG ) ) debugFwdHit( *itH, debug() );
  }

  LHCb::State temp;
  
  double zRef = track.z0();
  double ax   = track.xAtZ( zRef );
  double bx   = track.xSlope( zRef );
  double by   = track.ySlope( zRef );
  double x0   = ax - bx * zRef;    // intercept at z=0;
  double qOverP = m_momentumParams[0] +
    m_momentumParams[1] * bx * bx +
    m_momentumParams[2] * by * by +
    m_momentumParams[3] * x0 * x0;
  qOverP = 1.e-6 * x0 / qOverP;


  //== overestimated covariance matrix, as input to the Kalman fit

  Gaudi::TrackSymMatrix cov;
  cov(0,0) = m_stateErrorX2;
  cov(1,1) = m_stateErrorY2;
  cov(2,2) = m_stateErrorTX2;
  cov(3,3) = m_stateErrorTY2;
  double errQOverP = m_stateErrorP*qOverP;
  cov(4,4) = errQOverP * errQOverP;


  for (unsigned int i=0; i<m_zOutputs.size(); i++){
    double z = m_zOutputs[i];
  
    temp.setLocation( LHCb::State::AtT );
    temp.setState( track.xAtZ( z ), track.yAtZ( z ), z,
		   track.xSlope( z ), track.ySlope( z ), qOverP );
    temp.setCovariance( cov );
    out->addToStates( temp );
  }

  outputTracks.push_back( out );
}
//=========================================================================
//  Debug one coordinate
//=========================================================================
void PatSeedingTool::debugFwdHit ( PatFwdHit* coord, MsgStream& msg ) {
  msg << format( " Z %10.2f Xp %10.2f X%10.2f  St%2d lay%2d typ%2d used%2d",
                 coord->z(),
                 coord->projection(),
                 coord->x(),
                 coord->hit()->station(),
                 coord->hit()->layer(),
                 coord->hit()->region(),
                 coord->isUsed() );
  if ( 2 > coord->hit()->region() ) {
    msg << format( " P%2d N%2d RL%2d Drift %7.3f",
                   coord->hasPrevious(),
                   coord->hasNext(),
                   coord->rlAmb(),
                   coord->driftDistance() );
  }
  //else {
  //msg << format( " High Threshold %2d         ", coord->highThreshold() );
  //}

  //for ( std::vector<int>::const_iterator itM = coord->MCKeys().begin();
  //    coord->MCKeys().end() != itM; ++itM ) {
  //msg << " " << *itM;
  //}
  msg << endreq;
}

//=========================================================================
//  add the new track to the vector, if better, this means longer than
//  any other track sharing >70% of hits
//=========================================================================
bool PatSeedingTool::addIfBetter (  PatSeedTrack& track, std::vector<PatSeedTrack>& candidates ) {

  double zTrack = track.z0();
  double xTrack = track.xAtZ( zTrack );

  bool printing = msgLevel ( MSG::VERBOSE );
  
  if ( printing ) {
    info() << ".. Compare new track:" << endreq;
    for ( PatFwdHits::const_iterator itH = track.coordBegin(); track.coordEnd() != itH; ++itH ) {
      debugFwdHit( *itH, info() );
    }
  }

  for ( std::vector<PatSeedTrack>::iterator itT = candidates.begin();
        candidates.end() != itT; ++itT ) {
    int nCommon = 0;
    if ( 10 < fabs( (*itT).xAtZ( zTrack) - xTrack ) ) continue;

    //== compare the list of hits only if not too far...
    for ( PatFwdHits::const_iterator itH1 = (*itT).coordBegin();
          (*itT).coordEnd() != itH1; ++itH1 ) {
      for ( PatFwdHits::const_iterator itH2 = track.coordBegin(); track.coordEnd() != itH2; ++itH2 ) {
        if ( *itH2 == *itH1 ) ++nCommon;
      }
    }

    //== enough common in stored track: Keep the best (longest / best chi2)
    if ( nCommon >= m_commonXFraction * (*itT).nCoords() ||
         nCommon >= m_commonXFraction * track.nCoords() ) {
      if ( printing ) {
        info() << "Track " << itT - candidates.begin() << " size " <<  (*itT).nCoords()
               << " chi2 " << (*itT).chi2() << " has " << nCommon
               << " hit shared with current track of size " << track.nCoords()
               << " chi2 " << track.chi2() << endreq;
        for ( PatFwdHits::const_iterator itH1 = (*itT).coordBegin();
              (*itT).coordEnd() != itH1; ++itH1 ) {
          debugFwdHit( *itH1, info() );
        }
      }
      if ( track.nCoords() - track.chi2()/5 < (*itT).nCoords() - (*itT).chi2()/5 ) {
        if ( printing ) info() << "  new is shorter / higher chi2 -> ignore new" << endreq;
        return false;
      }
      if ( printing ) info() << "  erase old" << endreq;
      itT = candidates.erase( itT ) - 1; //== There will be a ++ in the loop!
    }
  }
  if ( printing ) info() << "  Store new one" << endreq;
  candidates.push_back( track );
  return true;
}

//=========================================================================
//  Find the x candidates in the specified layer and region. Results in candidates
//=========================================================================
void PatSeedingTool::findXCandidates ( unsigned int lay, unsigned int region, std::vector<PatSeedTrack>& candidates) {

  //== restore default measure

  PatFwdHits::const_iterator itH, itBeg, itH0, itH1, itH2;
  unsigned int sta, lay1;

  for ( sta = 0; sta < m_nSta; ++sta ) {
    for ( lay1 = 0; lay1 < m_nLay; lay1+=3 ) {
      Tf::TStationHitManager<PatForwardHit>::HitRange range = m_tHitManager->hits(sta, lay1, region);
      for ( itH = range.begin(); range.end() != itH; ++itH ) {
        if ( (*itH)->isUsed() ) continue;
        (*itH)->setX((*itH)->hit()->xAtYEq0());
        (*itH)->setZ((*itH)->hit()->zAtYEq0());
        const Tf::OTHit* otHit = (*itH)->hit()->othit();
        if(otHit) {
          (*itH)->setDriftDistance(otHit->driftDistance(0));
	}
      }
    }
  }

  double zScaling  = ( m_tHitManager->region(2,lay,region)->z() -  m_tHitManager->region(0,lay,region)->z() ) /
    ( m_tHitManager->region(0,lay,region)->z() - m_zMagnet );
  double zScaling2 = ( m_tHitManager->region(2,lay,region)->z() - m_tHitManager->region(0,lay,region)->z() ) /
    m_tHitManager->region(0,lay,region)->z();

  bool matchIn0 = false;
  bool matchIn2 = false;

  Tf::TStationHitManager<PatForwardHit>::HitRange range = m_tHitManager->hits(0,lay,region);
  for ( itH0 = range.begin(); range.end() != itH0; ++itH0 ) {
    PatFwdHit* hit0 = *itH0;
    if ( hit0->isUsed() )  continue;

    matchIn0 = msgLevel( MSG::VERBOSE );
    
    double x0 = hit0->hit()->xAtYEq0();
    double z0 = hit0->hit()->zAtYEq0();

    if ( hit0->hasNext() ) {
      hit0 = *(++itH0);
      if ( matchIn0 ) info() << "*** Hit0 " << x0 << " + " << hit0->x();
      x0 = .5 * ( x0 + hit0->hit()->xAtYEq0() );
      z0 = .5 * ( z0 + hit0->hit()->zAtYEq0()       );
    } else if ( matchIn0 ) {
      if ( matchIn0 ) info() << "*** Hit0 " << hit0->x();
    }
    m_printing = matchIn0;

    double x2Min = x0 + ( x0 - m_xMagTol) * zScaling;
    double x2Max = x0 + ( x0 + m_xMagTol) * zScaling;
    double x2Min2 = x0 + (x0 - m_maxImpact ) * zScaling2;
    double x2Max2 = x0 + (x0 + m_maxImpact ) * zScaling2;
    if ( x2Min < x2Min2 ) x2Min = x2Min2;
    if ( x2Max > x2Max2 ) x2Max = x2Max2;


    Tf::TStationHitManager<PatForwardHit>::HitRange rangeX2 = m_tHitManager->hitsWithMinX( x2Min, 2, lay, region );

    if ( m_printing ) info() << " x2Min " << x2Min << " x2Max " << x2Max << endreq;

    for ( itH2 = rangeX2.begin(); rangeX2.end() != itH2; ++itH2 ) {
      PatFwdHit* hit2 = *itH2;
      if ( hit2->isUsed() ) continue;
      if ( x2Max < hit2->hit()->xAtYEq0() ) break;
      double x2 = hit2->hit()->xAtYEq0();
      double z2 = hit2->hit()->zAtYEq0();

      m_printing = matchIn0;
      matchIn2 = false;
      
      if ( hit2->hasNext() ) {
        hit2 = *(++itH2);
	if ( m_printing ) info() << "    hit2 " << x2 << " + " << hit2->x();
        x2 = .5 * ( x2 + hit2->hit()->xAtYEq0() );
        z2 = .5 * ( z2 + hit2->hit()->zAtYEq0()       );
      } else if ( m_printing ) {
        info() << "    hit2 " <<  hit2->hit()->xAtYEq0();
      }

      //== Check center of magnet compatibility
      double slope = ( x2 - x0 ) / ( z2 - z0 );
      double intercept =  x0 - z0 * slope;
      double x1Pred = x0 + ( m_tHitManager->region(1,lay,region)->z() - z0 ) * slope
        + m_initialArrow * intercept;


      Tf::TStationHitManager<PatForwardHit>::HitRange rangeX1 = m_tHitManager->hitsWithMinX( x1Pred - m_curveTol, 1, lay, region );

      if ( m_printing ) info() << " x1Pred " << x1Pred << " tol " << m_curveTol
                               << " linear " << x0 + ( m_tHitManager->region(1,lay,region)->z() - z0 ) * slope
                               << endreq;

      for ( itH1 = rangeX1.begin(); rangeX1.end() != itH1; ++itH1 ) {
        PatFwdHit* hit1 = *itH1;

        if ( hit1->isUsed() ) continue;
        if ( x1Pred - m_curveTol > hit1->hit()->xAtYEq0() ) continue;
        if ( x1Pred + m_curveTol < hit1->hit()->xAtYEq0() ) break;

        double x1 = hit1->hit()->xAtYEq0();
        double z1 = hit1->hit()->zAtYEq0();
        if ( hit1->hasNext() ) {
          hit1 = *(++itH1);
          if ( m_printing ) info() << "         ==> found hit1 " << x1 << " + " << hit1->x() << endreq;
          x1 = .5 * ( x1 + hit1->hit()->xAtYEq0() );
          z1 = .5 * ( z1 + hit1->hit()->zAtYEq0()       );
        } else if ( m_printing ) {
          info() << "         ==> found hit1 " << x1 << endreq;
        }

        PatSeedTrack track( x0, x1, x2, z0, z1, z2, m_zReference, m_dRatio );
        track.setYParams( 0, 0);
        int nbMissed = 0;

        //start with the other 3 layers
        lay1 = 3-lay;
        for ( unsigned int nLay = 0; 2 > nLay ; ++nLay, lay1 = 3-lay1 ){
          for ( sta = 0; sta < m_nSta; ++sta ) {
            int found = 0;

            double xPred = track.xAtZ( m_tHitManager->region(sta,lay1, region)->z());
            Tf::TStationHitManager<PatForwardHit>::HitRange rangeX2 =
              m_tHitManager->hitsWithMinX( xPred - 2*m_tolCollect, sta, lay1, region );
            for ( itH = rangeX2.begin(); rangeX2.end() != itH; ++itH ) {
              PatFwdHit* hit = *itH;
              if ( hit->isUsed() ) continue;
              xPred = track.xAtZ( hit->hit()->zAtYEq0() );
              if ( xPred - m_tolCollect - hit->driftDistance() > hit->hit()->xAtYEq0() ) continue;
              if ( xPred + m_tolCollect + hit->driftDistance() < hit->hit()->xAtYEq0() ) break;
              track.addCoord( hit );
              ++found;
            }
            if ( 0 == found ) nbMissed++;
            if ( m_printing ) {
              info() << format( "         --- sta%2d lay%2d xPred%8.2f found%2d nbMissed%2d",
                                sta, lay1, track.xAtZ( ( m_tHitManager->region(sta,lay1, region)->z())),
                                found,  nbMissed )
                     << endreq;
            }
            if ( 1 < nbMissed ) break;
          }
          if ( 1 < nbMissed ) break;
        }
        if ( 1 < nbMissed ) continue;

        //== Fit the track
        if ( m_printing ) info() << " -- start fitting, nCoord = " << track.nCoords() << endreq;
        track.sort();
        bool fitted = m_seedTool->fitTrack( track, m_maxChi2Hit, m_minXPlanes, true, m_printing );
        if ( !fitted ) {
          if ( m_printing ) info() << "    -- fit failed" << endreq;
          continue;
        }
        if ( m_maxTrackChi2 < track.chi2() ) {
          if ( m_printing ) info() << "    -- global chi2 too high " << track.chi2() << endreq;
          continue;
        }

        //== Limited OT multiplicity -> check if compatible with in the central Y region
        if ( 2 > region && 8 > track.nCoords() ) {
          fitted = m_seedTool->fitTrack( track, m_maxFinalChi2, m_minXPlanes, true, m_printing );
          if ( !fitted ) {
            if ( m_printing ) info() << "    -- re-fit with final Chi2 failed" << endreq;
            continue;
          }  
          
	  if ( 2 * ( track.nCoords() - track.nPlanes() + 1 ) < track.nbOnSide() ) {
            if ( m_printing ) info() << "    -- only " << track.nCoords() << " and " << track.nbOnSide()
                                     << " neighbouring OT hits" << endreq;
            continue;
          }
          if ( 5 < track.chi2() ) {
            if ( m_printing ) info() << "    -- global chi2 too high " << track.chi2() << endreq;
            continue;
          }
        }

        if ( m_nOTReg < region && 3 > track.nbHighThreshold() ) {
          if ( m_printing ) info() << "    -- only " << track.nbHighThreshold()
                                   << " high threshold IT hits" << endreq;
          continue;
        }

        bool ok = addIfBetter( track, candidates );
        if ( ok ) {
          if ( m_printing ) {
            info() << " ****** stored N= " << candidates.size() -1 << endreq;
            for ( itH = track.coordBegin(); track.coordEnd() != itH; ++itH ) {
              debugFwdHit( *itH, info() );
            }
          }
        }
      }
    }
  }

  //== Final refit, as track may have been modified...
  for( std::vector<PatSeedTrack>::iterator itT = candidates.begin();
       candidates.end() != itT; ++itT ) {
    PatSeedTrack& track = *itT;

    track.sort();
    if ( track.valid() ) {
      m_printing = msgLevel( MSG::DEBUG );
            
      bool ok = m_seedTool->fitTrack( track, m_maxChi2Hit, m_minXPlanes, true, m_printing );
      if ( !ok ) track.setValid( false );
      // cut on chi2 per DoF
      if ( 15 < track.chi2() ) {
        track.setValid( false );
      }
    }
  }
}

//=========================================================================
//  Find all stereo hits compatible with the track.
//=========================================================================
void PatSeedingTool::collectStereoHits ( PatSeedTrack& track, unsigned int region, PatFwdHits& stereo ) {

  PatFwdHits::const_iterator itH;
  for ( int sta = 0; 3 > sta; ++sta ) {
    for ( int sLay = 1; 3 > sLay ; ++sLay ){
      int minRegion = region;
      int maxRegion = region+1;
      if ( 2 > region && 7 > track.nCoords() ) {  // central region -> try both OT halves
        minRegion = 0;
        maxRegion = 2;
      }
      for ( int testRegion = minRegion; maxRegion > testRegion; ++testRegion ) {
        Tf::TStationHitManager<PatForwardHit>::HitRange range = m_tHitManager->hits(sta, sLay, testRegion);
        for ( itH = range.begin(); range.end() != itH; ++itH ) {
          if ( (*itH)->isUsed() ) continue;
          (*itH)->setX((*itH)->hit()->xAtYEq0());
          (*itH)->setZ((*itH)->hit()->zAtYEq0());
          const Tf::OTHit* otHit = (*itH)->hit()->othit();
          if(otHit) {
            (*itH)->setDriftDistance(otHit->untruncatedDriftDistance());
          }
        }

        double xMin = m_tHitManager->region(sta, sLay, testRegion)->ymin() * 
          m_tHitManager->region(sta, sLay, testRegion)->sinT();
        double xMax = m_tHitManager->region(sta, sLay, testRegion)->ymax() * 
          m_tHitManager->region(sta, sLay, testRegion)->sinT();
        // Low number of coordinates in OT: Ask Y to be close to 0...
        if ( 0 == testRegion && 7 > track.nCoords() ) xMin = -50 * m_tHitManager->region(sta, sLay, testRegion)->sinT();
        if ( 1 == testRegion && 7 > track.nCoords() ) xMax =  50 * m_tHitManager->region(sta, sLay, testRegion)->sinT();

        if ( xMin > xMax ) {
          double swap = xMin;
          xMin = xMax;
          xMax = swap;
        }
        xMin = xMin + track.xAtZ( m_tHitManager->region(sta, sLay, testRegion)->z() ) - m_tolCollect;
        xMax = xMax + track.xAtZ( m_tHitManager->region(sta, sLay, testRegion)->z() ) + m_tolCollect;

        if ( m_printing ) info() << format( "Stereo in sta%2d lay%2d region%2d xMin %7.2f xmax %7.2f",
                                            sta, sLay, testRegion, xMin, xMax ) << endreq;

        Tf::TStationHitManager<PatForwardHit>::HitRange rangeW = m_tHitManager->hitsWithMinX( xMin, sta, sLay, testRegion);
        for ( itH = rangeW.begin(); rangeW.end() != itH; ++itH ) {
          PatFwdHit* hit = *itH;
          if ( hit->isUsed() ) continue;
          if ( xMin > hit->x() ) continue;
          if ( xMax < hit->x() ) break;
          double y = ( hit->x() - track.xAtZ( hit->z() ) );
          y = y * ( m_zForYMatch / hit->z() ) /  m_tHitManager->region(sta, sLay, testRegion)->sinT();
          hit->setProjection( y );
          hit->setIgnored( false );
          stereo.push_back( hit );
        }
      }
    }
  }
  std::sort( stereo.begin(), stereo.end(), 
	     Tf::increasingByProjection<PatForwardHit>());
}

//=========================================================================
//  Find the best range and remove (tag) all other hits
//=========================================================================
bool PatSeedingTool::findBestRange ( unsigned int region, int minNbPlanes, PatFwdHits& stereo ) {
  double maxRangeSize = m_maxRangeOT;
  if ( 1 < region ) maxRangeSize = m_maxRangeIT;

  PatFwdHits::const_iterator itBeg = stereo.begin();
  PatFwdHits::const_iterator itEnd = itBeg + minNbPlanes;

  //== get enough planes fired
  PatFwdPlaneCounter<PatForwardHit> count1( itBeg, itEnd );

  while ( itEnd != stereo.end() &&
          minNbPlanes > count1.nbDifferent() ) {
    count1.addHit( *itEnd++ );
  }
  if ( minNbPlanes > count1.nbDifferent() ) return false;

  //== Try to get the minNbPlanes to 6 if any range < maxRangeSize has it.

  if ( 6 > minNbPlanes ) {
    while ( itEnd != stereo.end() ) {
      while ( itEnd != stereo.end() &&
              maxRangeSize > (*(itEnd-1))->projection() - (*itBeg)->projection() &&
              minNbPlanes+1 > count1.nbDifferent() ) {
        count1.addHit( *itEnd++ );
      }
      if ( minNbPlanes < count1.nbDifferent() ) {  // more planes -> update
        minNbPlanes = count1.nbDifferent();
        if ( 6 == minNbPlanes ) break;
      }
      count1.removeHit( *itBeg++ );
    }
  }

  //== get enough planes fired
  itBeg = stereo.begin();
  itEnd = itBeg + minNbPlanes;
  PatFwdPlaneCounter<PatForwardHit> count( itBeg, itEnd );

  while ( itEnd != stereo.end() &&
          minNbPlanes > count.nbDifferent() ) {
    PatFwdHit* hit = *itEnd++;
    count.addHit( hit );
  }

  PatFwdHits::const_iterator itBest = itBeg;
  PatFwdHits::const_iterator itLast = itEnd;
  double minDist = (*(itEnd-1))->projection() - (*itBeg)->projection();

  if ( m_printing ) {
    info() << format( "        range minDist %7.2f from %8.3f to %8.3f minNbPlanes %2d",
                      minDist, (*itBeg)->projection(), (*(itEnd-1))->projection(), minNbPlanes )
           << endreq;
  }

  //== Better range ? Remove first, try to complete, measure spread...
  while ( itEnd != stereo.end() ) {
    count.removeHit( *itBeg++ );
    while ( itEnd != stereo.end() && minNbPlanes > count.nbDifferent() ) {
      PatFwdHit* hit = *itEnd++;
      count.addHit( hit );
    }
    if ( minNbPlanes == count.nbDifferent()  ) {
      if ( minDist > (*(itEnd-1))->projection() - (*itBeg)->projection() ) {
        minDist = (*(itEnd-1))->projection() - (*itBeg)->projection();
        itBest = itBeg;
        itLast = itEnd;
        if ( m_printing ) {
          info() << format( " better range minDist %7.2f from %8.3f to %8.3f minNbPlanes %2d",
                            minDist, (*itBeg)->projection(), (*(itEnd-1))->projection(),
                            minNbPlanes )
                 << endreq;
        }
      }
    }
  }
  itBeg = itBest;
  itEnd = itLast;

  //== add OT hits on side, except if 6 planes and we are near y=0
  double tolSide = m_tolCollect / fabs( (*itBeg)->hit()->dxDy() );
  if ( 6 == minNbPlanes &&
       50. > fabs( (*itBeg)->projection() ) &&
       50. > fabs( (*(itEnd-1))->projection() ) ) tolSide = 0.;

  double minProj = (*itBeg)->projection() - tolSide;
  while ( itBeg != stereo.begin() ) {
    PatFwdHit* hit = *(--itBeg);
    if ( minProj > hit->projection() ) {
      ++itBeg;
      break;
    }
  }
  double maxProj = (*(itEnd-1))->projection() + tolSide;
  while ( itEnd != stereo.end() ) {
    PatFwdHit* hit = *itEnd;
    if ( maxProj < hit->projection() )  break;
    ++itEnd;
  }
  if ( m_printing ) {
    info() << format( "  final range minDist %7.2f from %8.3f to %8.3f minNbPlanes %2d",
                      minDist, (*itBeg)->projection(), (*(itEnd-1))->projection(), minNbPlanes )
           << endreq;
  }

  PatFwdHits::const_iterator itH;
  for ( itH = stereo.begin(); itBeg != itH; ++itH ) {
    (*itH)->setIgnored( true );
  }
  for ( itH = itEnd; stereo.end() != itH; ++itH ) {
    (*itH)->setIgnored( true );
  }
  return true;
}

//=========================================================================
//  Fit a line in Y projection, returns the parameters.
//=========================================================================
bool PatSeedingTool::fitLineInY ( PatFwdHits& stereo, double& y0, double& sl) {
  double maxYDist = 20. * m_tolCollect;


  PatFwdHits::const_iterator itH;
  std::vector<double> largestDrift( 3, 0. );
  PatFwdHits seeds(3);

  for ( itH = stereo.begin(); stereo.end() != itH; ++itH ) {
    PatFwdHit* hit = *itH;
    if ( hit->isIgnored() ) continue;
    hit->setProjection( hit->projection() * hit->z() / m_zForYMatch );
    if ( 2 > hit->hit()->region() ) {
      hit->setRlAmb( 0 );
      int sta = hit->hit()->station();
      if ( largestDrift[sta] < hit->driftDistance() ) {
        largestDrift[sta] =  hit->driftDistance();
        seeds[sta] = hit;
      }
    }
  }

  if ( largestDrift[1] > largestDrift[0] &&
       largestDrift[1] > largestDrift[2] ) {
    if ( largestDrift[0] < largestDrift[2] ) {
      seeds[0] = seeds[1];
    } else {
      seeds[2] = seeds[1];
    }
  } else if ( largestDrift[1] > largestDrift[0] ) {
    seeds[0] = seeds[1];
  } else if ( largestDrift[1] > largestDrift[2] ) {
    seeds[2] = seeds[1];
  }

  double minChi2 = 1.e10;
  int bestMask    = 0;
  double bestSl = 0.;
  double bestY0 = 0.;
  if ( 0.1 < largestDrift[0] && 0.1 < largestDrift[2] ) {
    double z0 = seeds[0]->z();
    double z2 = seeds[2]->z();
    double x0 = seeds[0]->projection();
    double x2 = seeds[2]->projection();
    double d0 = seeds[0]->driftDistance()/seeds[0]->hit()->dxDy();
    double d2 = seeds[2]->driftDistance()/seeds[2]->hit()->dxDy();
    if ( m_printing ) {
      info() << format( "  0: z %8.2f y %7.2f d %7.2f", z0, x0, d0 ) << endreq;
      info() << format( "  2: z %8.2f y %7.2f d %7.2f", z2, x2, d2 ) << endreq;
    }

    for ( int mask = 0; 4 > mask; ++mask ) {
      int sign0 = 2*(mask&1)-1;
      int sign2 = 2*((mask>>1)&1) -1;
      double locY0 = x0 + sign0 * d0;
      double locY2 = x2 + sign2 * d2;
      sl = (locY2 - locY0 ) / ( z2 - z0);
      y0 = locY0 - sl * z0;
      double chi2 = 0.;
      for ( PatFwdHits::iterator itH = stereo.begin(); stereo.end() != itH; ++itH ) {
        if ( (*itH)->isIgnored() ) continue;
        double dist = fabs( (*itH)->projection() - y0 - (*itH)->z() * sl );
        double drift = (*itH)->driftDistance() / (*itH)->hit()->dxDy();
        if ( fabs(dist-drift) < fabs(dist+drift) ) {
          dist = dist - drift;
        } else {
          dist = dist + drift;
        }
        if ( m_printing ) info() << format( "    z%8.2f proj%8.2f drift%8.2f dist%8.2f",
                                            (*itH)->z(), (*itH)->projection(), drift, dist ) << endreq;
        chi2 += dist * dist;
      }
      if ( m_printing ) info() << format( "-- y fit: mask%2d y0 %8.2f sl%10.6f chi2 %12.2f",
                                          mask, y0, sl, chi2 ) << endreq;
      if ( chi2 < minChi2 ) {
        minChi2 = chi2;
        bestMask = mask;
        bestSl = sl;
        bestY0 = y0;
        seeds[0]->setRlAmb( 2*(bestMask&1)-1 );
        seeds[2]->setRlAmb( 2*((bestMask>>1)&1) -1 );
      }
    }
  }

  double worstDist = 1.e6;
  y0 = bestY0;
  sl = bestSl;
  int nOK = 0;
  while( worstDist > maxYDist ) {
    PatFwdFitLine line( 0., 0., 0.25e-4 );  // 200 mm around y=0
    for ( itH = stereo.begin(); stereo.end() != itH; ++itH ) {
      if ( (*itH)->isIgnored() ) continue;
      double proj = (*itH)->projection();
      double dist = (*itH)->projection() - y0 - (*itH)->z() * sl;
      double drift = (*itH)->driftDistance() / (*itH)->hit()->dxDy();
      if ( fabs(dist-drift) < fabs(dist+drift) ) {
        proj = proj - drift;
      } else {
        proj = proj + drift;
      }
      line.addPoint( (*itH)->z(), proj, (*itH)->hit()->weight() );
      nOK++;
    }
    if ( 4 > nOK ) return false;
    line.solve();
    y0 = line.ax();
    sl = line.bx();
    worstDist = 0.;
    PatFwdHits::iterator worst;
    for ( PatFwdHits::iterator itH = stereo.begin(); stereo.end() != itH; ++itH ) {
      if ( (*itH)->isIgnored() ) continue;
      double dist = fabs( (*itH)->projection() - y0 - (*itH)->z() * sl );
      double drift = (*itH)->driftDistance() / (*itH)->hit()->dxDy();
      if ( fabs(dist-drift) < fabs(dist+drift) ) {
        dist = dist - drift;
      } else {
        dist = dist + drift;
      }
      if ( dist > worstDist ) {
        worstDist = dist;
        worst = itH;
      }
    }
    if ( m_printing ) {
      info() << format( "Worst%7.2f ", worstDist );
      debugFwdHit( *worst, info() );
    }
    if ( worstDist > maxYDist ) {
      (*worst)->setIgnored( true );
    }
  }
  return true;
}

//=========================================================================
//  Extract the seed part from tracks, and store them
//=========================================================================
void PatSeedingTool::processTracks ( std::string location, std::vector<LHCb::Track*>& outputTracks ) {
  if ( !exist<LHCb::Tracks>( location ) ) return;

  LHCb::Tracks::iterator itT;
  LHCb::Tracks* inputTracks  = get<LHCb::Tracks>( location );
  for ( itT = inputTracks->begin(); inputTracks->end() != itT;  ++itT ) {
    //== Keep only ST and OT ids.
    std::vector<LHCb::LHCbID> ids;
    for ( std::vector<LHCb::LHCbID>::const_iterator itId = (*itT)->lhcbIDs().begin();
          (*itT)->lhcbIDs().end() != itId; ++itId ) {
      if ( (*itId).isVelo() ) continue;
      if ( (*itId).isTT()   ) continue;
      ids.push_back( *itId );
    }
    //== Check for clone, i.e. similar list of Ids.
    bool match = false;
    unsigned int maxMissed = int( (1.-m_commonXFraction) * ids.size() );

    std::vector<std::vector<LHCb::LHCbID> >::const_iterator itList;
    for ( itList = m_foundIds.begin(); m_foundIds.end() != itList; ++itList ) {
      unsigned int nCommon = 0;
      unsigned int nMissed = 0;
      for ( std::vector<LHCb::LHCbID>::const_iterator it1 = (*itList).begin();
            (*itList).end() != it1; ++it1 ) {
        bool found = false;
        for ( std::vector<LHCb::LHCbID>::const_iterator it2 = ids.begin();
              ids.end() != it2; ++it2 ) {
          if ( *it1 == *it2 ) {
            ++nCommon;
            found = true;
            break;
          }
        }
        if ( !found ) {
          --maxMissed;
          if ( nMissed > maxMissed ) break;  // too many missed so far ->abort the comparison.
        }
      }
      if ( nCommon > m_commonXFraction * ids.size() ) {
        match = true;
        break;
      }
    }
    if ( match ) continue;
    m_foundIds.push_back( ids );

    LHCb::Track* out = new LHCb::Track();
    out->addToAncestors( *itT );
    out->setType( LHCb::Track::Ttrack );
    out->setHistory( LHCb::Track::PatSeeding );
    out->setPatRecStatus( LHCb::Track::PatRecIDs );
    out->setLhcbIDs( ids );
    LHCb::State& state = (*itT)->stateAt(  LHCb::State::AtT );
    out->addToStates( state );
    outputTracks.push_back( out );
  }
}

//=============================================================================
StatusCode PatSeedingTool::tracksFromTrack(const LHCb::Track& /* seed */, 
                                           std::vector<LHCb::Track*>& tracks ){ 

  performTracking(tracks);
  
  return StatusCode::SUCCESS;
}


//=============================================================================
StatusCode PatSeedingTool::performTracking( LHCb::Tracks* outputTracks){ 
					   
  std::vector<LHCb::Track*> outvec;

  performTracking(outvec);
  
  for (unsigned int i=0; i<outvec.size(); i++)
    outputTracks->insert(outvec[i]);

  return StatusCode::SUCCESS;
}
