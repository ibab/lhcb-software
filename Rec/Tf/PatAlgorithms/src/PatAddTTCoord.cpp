// $Id: PatAddTTCoord.cpp,v 1.2 2009-04-20 06:24:33 cattanem Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "TfKernel/RecoFuncs.h"

#include "PatAddTTCoord.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatAddTTCoord
//
// 2006-06-29 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PatAddTTCoord );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatAddTTCoord::PatAddTTCoord( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IAddTTClusterTool>(this);
  declareProperty( "ZTTField"  , m_zTTField   =  1650. * Gaudi::Units::mm  );
  declareProperty( "TTParam"   , m_ttParam    =  30.                       );
  declareProperty( "TTTol"     , m_ttTol      =  2.                        );
  declareProperty( "TTTolSlope", m_ttTolSlope =  20000.                    );
  declareProperty( "ZTTProj"   , m_zTTProj    =  2500.                     );
  declareProperty( "TTMaxChi2" , m_ttMaxChi2  =  3.                        );
}
//=============================================================================
// Destructor
//=============================================================================
PatAddTTCoord::~PatAddTTCoord() {}

//=========================================================================
//
//=========================================================================
StatusCode PatAddTTCoord::initialize ( ) {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  debug() << "==> Initialize" << endmsg;


  m_ttHitManager = tool<Tf::TTStationHitManager <PatTTHit> >("PatTTStationHitManager");

  return StatusCode::SUCCESS;
}
//=========================================================================
//  Add the TT ids to the track (not implemented with STClusters
//=========================================================================
StatusCode PatAddTTCoord::addTTClusters( LHCb::Track& ,
                                         std::vector<LHCb::STCluster*>& ,
                                         std::vector<double>& ) {
  return Warning("The method AddTTClusters returning clusters and chi2 is NOT implemented.");
}

//=========================================================================
//  distance track to cluster, not implemented
//=========================================================================
double PatAddTTCoord::distanceToStrip( const LHCb::Track& ,
                                       const LHCb::STCluster&  ) {
  warning() << "The method distanceToStrip is NOT implemented." << endmsg;
  return 1.e30;
}
//=========================================================================
//  Add the TT hits on the track, only the ids.
//=========================================================================
StatusCode PatAddTTCoord::addTTClusters( LHCb::Track& track ) {

  
  m_ttHitManager->prepareHits();
  
  PatTTHits myTtCoords;
  bool debugging = msgLevel( MSG::DEBUG );

  if ( debugging ) debug() << "--- Entering addTTClusters ---" << endmsg;

  double tol = m_ttTol + m_ttTolSlope / track.p();

  //== Get all TT hits whose projection is compatible with the track

  PatTTHits::const_iterator itTT;
  PatTTHits selected;
  LHCb::State state = track.closestState( m_zTTProj );

  if ( debugging ) debug() << "State z " << state.z() << " x " << state.x() <<
                     " y " << state.y() << " tx " << state.tx() << " ty " <<
                     state.ty() << " p " << track.p() << endmsg;

  Tf::TTStationHitManager<PatTTHit>::HitRange range = m_ttHitManager->hits();
  for ( itTT = range.begin(); range.end() != itTT; ++itTT ) {
    PatTTHit* tt = *itTT;
    double z = tt->z();

    double yPred = state.y() + ( z - state.z() ) * state.ty();
    if( !tt->hit()->isYCompatible( yPred, tol) )
      continue ;

    double tyTr = state.ty();
    updateTTHitForTrack( tt, state.y()-state.z()*state.ty(), tyTr );
    
    double xPred = state.x() + ( z-state.z() ) * state.tx() + m_ttParam * state.qOverP() * ( z - m_zTTField );
    if ( fabs( xPred - tt->x() ) < tol ) {
      double projDist = ( xPred - tt->x() ) * ( m_zTTProj - m_zTTField ) / ( z - m_zTTField );
      tt->setProjection( projDist );
      selected.push_back( tt );
    }
  }
  std::sort( selected.begin(), selected.end(), Tf::increasingByProjection<PatTTHit>() );

  PatTTHits::iterator itSel;

  // if ( debugging ) {
  //info() << " preselected " << selected.size() << " TTCord, tol = " << tol << endmsg;
  //for ( itSel = selected.begin(); selected.end() != itSel; ++itSel ) {
  //  PatTTHit* tt = *itSel;
  //  info() << format( "  hit proj %7.2f plane %2d", tt->projection(), tt->planeCode() );
  //  for ( std::vector<int>::const_iterator itM = tt->MCKeys().begin();
  //        tt->MCKeys().end() != itM; ++itM ) {
  //    info() << " " << *itM;
  //  }
  //  info() << endmsg;
  //}
  //}

  //== Find a group of close projections, with enough planes

  PatTTHits::iterator itBeg, itEnd, worst;

  double chi2   = 0.;

  if ( 2 < selected.size() ) {

    //== Fit a straight line with the points, remove worst until good

    PatTTHits goodTT;
    double bestChi2 = m_ttMaxChi2;

    for (  itBeg = selected.begin(); itBeg+2 < selected.end(); ++itBeg ) {
      double firstProj = (*itBeg)->projection();
      goodTT.clear();
      int nbPlane = 0;
      std::vector<int> firedPlanes(4, 0);
      itEnd = itBeg;
      double maxProj =  firstProj + m_ttTol + .25 * fabs(firstProj );
      while ( itEnd != selected.end() ) {
        if ( (*itEnd)->projection() > maxProj ) break;
        if ( 0 == firedPlanes[ (*itEnd)->planeCode() ] ) {
          firedPlanes[ (*itEnd)->planeCode() ] = 1;
          nbPlane++;
        }
        goodTT.push_back( *itEnd++ );
      }
      if ( 3 > nbPlane ) continue;
      if ( myTtCoords.size() > goodTT.size() ) continue;
      if ( debugging ) info() << "Start fit, first proj " << firstProj << " nbPlane " << nbPlane
                              << " size " << goodTT.size() << endmsg;

      double offset = 0.;
      double slope  = 0.;
      double offsetY= 0.;
      double fixedWeight = 9./tol/tol;  // tol is assumed to be 3 sigmas of track constraint
      chi2 = 1.e20;

      while ( 1.e10 < chi2 ) {
        double s0   = fixedWeight;  // fix X = 0 with fixedWeight
        double sz   = 0.;
        double st   = 0.;
        double sz2  = fixedWeight * (m_zTTProj-m_zTTField)*(m_zTTProj-m_zTTField); // fix slope by point a z=TTfield
        double szt  = 0.;
        double st2  = fixedWeight;  // fix Y = 0 with fixedWeight
        double sx   = 0.;
        double sxz  = 0.;
        double sxt  = 0.;

        for ( itSel = goodTT.begin(); goodTT.end() != itSel; ++itSel ) {
          PatTTHit* tt = *itSel;
          double w    = tt->hit()->weight();
          double dz   = tt->z() - m_zTTProj;
          double t    = tt->hit()->sinT();
          double dist = tt->projection();
          s0   += w;
          sz   += w * dz;
          st   += w * t;
          sz2  += w * dz * dz;
          szt  += w * dz * t ;
          st2  += w * t  * t ;
          sx   += w * dist;
          sxz  += w * dist * dz;
          sxt  += w * dist * t ;
        }
        double a1 = sz  * sz - s0  * sz2;
        double b1 = st  * sz - s0  * szt;
        double c1 = sx  * sz - s0  * sxz;
        double a2 = sz  * st - s0  * szt;
        double b2 = st  * st - s0  * st2;
        double c2 = sx  * st - s0  * sxt;

        double den = a1 * b2 - a2 * b1;
        slope   = ( c1 * b2 - c2 * b1 ) / den;
        offsetY = ( a1 * c2 - a2 * c1 ) / den;
        offset  = (sx - slope * sz - offsetY * st ) / s0;

        chi2 = fixedWeight * ( offset * offset +
                               offsetY * offsetY +
                               (m_zTTProj-m_zTTField)*(m_zTTProj-m_zTTField)*slope * slope );
        int nDoF = 0;
        double worstDiff = -1.;
        std::vector<int> differentPlanes(4, 0);
        for ( itSel = goodTT.begin(); goodTT.end() != itSel; ++itSel ) {
          PatTTHit* tt = *itSel;
          if ( 0 ==  differentPlanes[tt->planeCode()]++ ) ++nDoF;
        }

        unsigned int nIndep = nDoF;
        for ( itSel = goodTT.begin(); goodTT.end() != itSel; ++itSel ) {
          PatTTHit* tt = *itSel;
          double w    = tt->hit()->weight();
          double dz   = tt->z() - m_zTTProj;
          double dist = tt->projection() - offset - slope * dz - offsetY * tt->hit()->sinT();
          if ( ( 1 < differentPlanes[tt->planeCode()] ||
                 nIndep == goodTT.size() ) &&
               worstDiff < w * dist * dist ) {
            worstDiff = w * dist * dist;
            worst = itSel;
          }
          chi2 += w * dist * dist;
        }

        chi2 /= nDoF;

        if ( debugging && worstDiff > 0. ) {
          info() << format( " chi2 %10.2f nDoF%2d wors %8.2f proj %6.2f offset %8.3f slope %10.6f offsetY %10.6f",
                            chi2, nDoF, worstDiff, (*worst)->projection(), offset, slope, offsetY)
                 << endmsg;
        }
        //== remove last point if bad fit...
        if ( worstDiff > 0. &&
             bestChi2 < chi2 &&
             goodTT.size() > 3  ) {
          goodTT.erase( worst );
          chi2 = 1.e11;  // new iteration
        }
      }

      if ( bestChi2 > chi2 && goodTT.size() >= myTtCoords.size() ) {
        if ( debugging ) {
          info() << "*** Store this candidate, nbTT = " << goodTT.size() << " chi2 " << chi2 << endmsg;
          for ( itSel = goodTT.begin(); goodTT.end() != itSel; ++itSel ) {
            PatTTHit* tt = *itSel;
            double z     = tt->z();
            double dz    = tt->z() - m_zTTProj;
            double dist  = tt->projection() - offset - slope * dz - offsetY * tt->hit()->sinT();
            double mPred = tt->x() + dist;
            MsgStream& msg = info() << tt->planeCode()
                                    << format( " z%7.0f  x straight %7.2f pred %7.2f  x %7.2f diff %7.2f ",
                                               z, state.x() + state.tx() * ( z-state.z() ), mPred, tt->hit()->xMid(), dist );
            //     for ( std::vector<int>::const_iterator itM = tt->MCKeys().begin();
            //    tt->MCKeys().end() != itM; ++itM ) {
            //  msg << " " << *itM;
            //}
            msg << endmsg;
          }
        }
        myTtCoords = goodTT;
        bestChi2 = chi2;
      }
    }
  }

  for ( PatTTHits::iterator itT = myTtCoords.begin();
        myTtCoords.end() != itT; ++itT ) {
    PatTTHit* myHit = (*itT);
    track.addToLhcbIDs( myHit->hit()->lhcbID() );
    myHit->hit()->setUsed( true );
  }
  return StatusCode::SUCCESS;
}


//=============================================================================
