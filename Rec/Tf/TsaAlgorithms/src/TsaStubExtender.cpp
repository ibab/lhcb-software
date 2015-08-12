#ifdef _WIN32
#pragma warning ( disable : 4244 ) // conversion double to float in root
#endif

// GaudiKernel
#include "GaudiKernel/SystemOfUnits.h"

#include "TsaStubExtender.h"

// Boost
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>
#include "boost/lexical_cast.hpp"
using namespace boost::assign;
using namespace boost;

using namespace Tf::Tsa;

DECLARE_TOOL_FACTORY( StubExtender )

StubExtender::StubExtender(const std::string& type,
                           const std::string& name,
                           const IInterface* parent):
  GaudiTool(type, name, parent),
  m_fitLine(0), m_parabolaFit(0)
{

  // constructer
  declareProperty("dSlopeCut", m_dSlopeCut = 0.03);
  declareProperty("seachTol", m_searchTol = 100.);
  declareProperty("nxCut", m_nxCut = 4);
  declareProperty("nyCut", m_nyCut = 4);
  declareProperty("txCut", m_txCut = 0.5);
  declareProperty("tyCut", m_tyCut = 0.04);
  declareProperty("yTol", m_yTol = 20.0);
  declareProperty("nTotalCut1", m_nTotalCut1 = 9);
  declareProperty("nTotalCut2", m_nTotalCut2 = 11);
  declareProperty("y0Cut1", m_y0Cut1 = 400);
  declareProperty("y0Cut2", m_y0Cut2 = 100);
  declareProperty("dxCut", m_dxCut = 1.00);
  declareProperty("dyCut", m_dyCut = 30.0);

  declareProperty("outlierCutLine", m_outlierCutLine = 6.0);
  declareProperty("outlierCutParabola", m_outlierCutParabola = 6.0);

  m_scth = 1.0/TsaConstants::sth;

  declareInterface<ITsaStubExtender>(this);

}

StubExtender::~StubExtender()
{
  // destructor
}

StatusCode StubExtender::finalize() 
{
  delete m_parabolaFit; m_parabolaFit = 0;
  delete m_fitLine;     m_fitLine     = 0;
  return GaudiTool::finalize();
}

StatusCode StubExtender::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize",sc);
  }

  m_parabolaFit = new SeedParabolaFit(TsaConstants::z0,
		  m_outlierCutParabola);
  m_fitLine = new SeedLineFit(msg(),TsaConstants::z0,
		  TsaConstants::sth, m_outlierCutLine);

  return sc;
}

StatusCode StubExtender::execute(int& sect, std::vector<SeedStub*> stubs[], std::vector<SeedHit*> hits[],
                                 std::vector<SeedHit*> sHits[], std::vector<SeedTrack*>& seeds ) 
{

  // Loop over IT stubs in each station
  std::vector<SeedHit*> select; select.reserve(16);

  for ( int stn = 0; stn < 3; ++stn ) {
    for ( std::vector<SeedStub*>::iterator it1 = stubs[stn].begin(); stubs[stn].end() != it1; ++it1 ) {
      SeedStub* stub = (*it1);

      // Only add OT hits from the sector containing the stub
      if ( stub->live() == false || (stub->y() > 0. && sect == 3) || (stub->y() < 0. && sect == 4) ) continue;

      select.clear();
      for ( int lay = 0; lay < 6; ++lay ) {

        double dSlope = 0.0;
        std::vector<SeedHit*>::iterator it2 = SeedFun::startX(hits[lay], stub->x(), stub->z1(), stub->sx(),m_searchTol);
        std::vector<SeedHit*>::iterator stop = SeedFun::stopX(hits[lay], stub->x(), stub->z1(), stub->sx(),m_searchTol);
        for ( ; stop != it2 ; ++it2 ) {
          SeedHit* hit = (*it2);
          if ( hit->onTrack() == true) continue;   // Hit not already used

          // Select OT hits in X layers that are within window defined using stub
          dSlope =  fabs(((hit->x() - stub->x())/(hit->z() - stub->z1())) -  stub->sx());
          if (dSlope < m_dSlopeCut) select.push_back(hit);
        }
      }

      if ( select.empty() == true ) continue;  // Require at least one OT X hit

      //  Two hits on track are given by IT hits on stub
      double x1 = stub->xHit1()->x();
      double x2 = stub->xHit2()->x();
      double z1 = stub->xHit1()->z();
      double z2 = stub->xHit2()->z();
      double dz = z2 - z1;
      double dxdz = (x2 - x1)/dz;
      double csth = sqrt( 1. + stub->sx()*stub->sx());
      int nMax = 0;
      int sSel[100];
      SeedHit* iSel[100];

      //  Loop over selected OT hits to define parabola
      for ( std::vector<SeedHit*>::iterator it3 = select.begin(); select.end() != it3; ++it3 ) {
        SeedHit* hit3 = (*it3);
        for ( int is = 0; is < 2; ++is ) {  // Include L/R ambiguity choice
          int sHit[99];
          SeedHit* iHit[99];
          double x3 = hit3->x() + (2*is-1)*hit3->r()*csth;
          double z3 = hit3->z();
          double ax = (dxdz-(x3-x1)/(z3-z1) )/(z2-z3);
          double bx = dxdz - ax*dz;
          int nh = 0;

          //  Count hits within window defined by parabola
          for ( std::vector<SeedHit*>::iterator it = select.begin(); select.end() != it; ++it ) {
            SeedHit* hit = (*it);
            if ( hit == hit3 ) continue;
            double dz = hit->z() - z1;
            double xSeed = dz*(ax*dz + bx) + x1;
            double dx1 = hit->x() - hit->r()*csth - xSeed;
            double dx2 = dx1 + 2*hit->r()*csth;
            if ( fabs(dx1) < m_dxCut || fabs(dx2) < m_dxCut ) {
              iHit[nh] = hit;
              sHit[nh] = 1;
              if ( fabs(dx1) < fabs(dx2) ) sHit[nh] = -1;
              if ( nh < 98 ) ++nh;
            }
          }
          if ( nh > nMax ) {  // Choose parabola that gives most hits on track
            nMax = nh;
            for ( int i = 0; i < nh; ++i ) {
              iSel[0] = hit3;
              sSel[0] = 2*is-1;
              iSel[i+1] = iHit[i];
              sSel[i+1] = sHit[i];
            }
          }
        }
      }
      if ( nMax == 0 ) continue;

      std::vector<SeedPnt> xPnts; xPnts.reserve(nMax+4);
      xPnts += SeedPnt(stub->xHit1()) , SeedPnt(stub->xHit2()) ;
      for ( int i = 0; i < nMax+1; ++i ) {
        xPnts.push_back( SeedPnt(iSel[i], sSel[i]) );
      }

      SeedTrack* seed = new SeedTrack(xPnts,6+stn);

      // Fit parabola to all X hits on track
      if ( m_parabolaFit->fit(seed,csth) < 1 || seed->nx() < m_nxCut || fabs( seed->sx() ) > m_txCut ) {
        delete seed;
        continue;
      }

      //  Next select stereo hits on candidate
      std::vector<SeedPnt> yPnts; yPnts.reserve(16);
      yPnts += SeedPnt(stub->yHit1()), SeedPnt(stub->yHit2()) ;

      //  Loop over stereo hits in each (double) layer
      for ( int lay = 0; lay < 6; ++lay ) {

        if (hits[lay].empty() == true) continue;

        const double zFirst = hits[lay].front()->z();
        const double slope = seed->xSlope(zFirst,TsaConstants::z0);
        Gaudi::XYZVector vec(1., TsaConstants::tilt*slope, -slope);
        //Gaudi::XYZPoint point(seed->x(zFirst,TsaConstants::z0), hits[lay].front()->clus()->yMid(), zFirst );
        Gaudi::XYZPoint point(seed->x(zFirst,TsaConstants::z0), hits[lay].front()->yMid(), zFirst );
        Gaudi::Plane3D plane = Gaudi::Plane3D(vec,point);
        Gaudi::XYZPoint iPoint;
        const double tol =  0.1*hits[lay].front()->length();;

        std::vector<SeedHit*>::iterator stop =  SeedFun::endStereo(sHits[lay], point.x(),tol);
        for (std::vector<SeedHit*>::iterator it = SeedFun::startStereo(sHits[lay], point.x(),tol);
             it != stop; ++it){
          SeedHit* hit = (*it);
          if ( hit->onTrack() == true ) continue;   // Hit not already used

          //if ( !Tsa::ClusFun::intersection(hit->clus(),plane,iPoint) ) continue;
          if ( !Tf::intersection(hit->tfHit(),plane,iPoint) ) continue;
         

          double y = iPoint.y();
          double dy = y - hit->r()*m_scth - ( stub->y() / stub->z2() )*iPoint.z();
          double dy2 = dy + 2*hit->r()*m_scth;

          //  Require that they are within window defined by line from origin to the IT stub
          if ( fabs( dy ) < m_dyCut || fabs( dy2 ) < m_dyCut ) {
            hit->setY( y );
            hit->setZ( iPoint.z() );
            if ( fabs( dy2 ) < fabs( dy ) ) {
              yPnts.push_back( SeedPnt(hit,1) );
            }
            else {
              yPnts.push_back( SeedPnt(hit,-1) );
            }
          }
        } // hits
      } // layer
      seed->setYPnts( yPnts );

      //  Fit line to Y hits
      if ( m_fitLine->fit( seed ) < 1 || seed->ny() < m_nyCut|| fabs( seed->sy() ) > m_tyCut
           || fabs( seed->y0() ) > m_y0Cut1 ||
           seed->nx()+seed->ny() < m_nTotalCut1 ) {
        delete seed;
        continue;
      }

      //  Check consistency of y-coordinate with active region of OT X-wires
      int nKill = 0;
      std::vector<SeedPnt>& pnts = seed->xPnts();
      for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
        SeedPnt & pnt = *it;
        if ( pnt.skip() || pnt.hit()->OT() == 0 ) continue;
        double ySeed = seed->y(pnt.hit()->z(), 0.);
        //if ( ( ySeed > 0. && ySeed < pnt.hit()->clus()->yMin()-m_yTol ) ||
        //    ( ySeed < 0. && ySeed > pnt.hit()->clus()->yMax()+m_yTol ) ) {
        if ( ( ySeed > 0. && ySeed < pnt.hit()->yMin()-m_yTol ) ||
             ( ySeed < 0. && ySeed > pnt.hit()->yMax()+m_yTol ) ) {
          pnt.setSkip( true );  // kill points that are in unphysical region
          ++nKill;
        }
      }
      if ( nKill > 0 ) {
        if ( seed->nx() - nKill < 4 ) {
          delete seed;
          continue;
        }

        int rc = m_parabolaFit->fit( seed, csth );  //  Refit seed
        if ( rc < 1 || seed->nx() < m_nxCut || fabs( seed->sx() ) > m_txCut) {
          delete seed;
          continue;
        }
      }

      //  Same for stereso wires
      nKill = 0;
      std::vector<SeedPnt>& ypnts = seed->yPnts();
      for ( std::vector<SeedPnt>::iterator it = ypnts.begin(); ypnts.end() != it; ++it ) {
        SeedPnt & pnt = *it;
        if ( pnt.skip() || pnt.hit()->OT() == 0 ) continue;
        //if ( ( pnt.coord() > 0. && pnt.coord() < pnt.hit()->clus()->yMin()-m_yTol ) ||
        //     ( pnt.coord() < 0. && pnt.coord() > pnt.hit()->clus()->yMax()+m_yTol ) ) {
        if ( ( pnt.coord() > 0. && pnt.coord() < pnt.hit()->yMin()-m_yTol ) ||
             ( pnt.coord() < 0. && pnt.coord() > pnt.hit()->yMax()+m_yTol ) ) {
          pnt.setSkip( true );
          ++nKill;
        }
      }
      if ( nKill > 0 ) {
        if ( seed->ny() - nKill < 3 ) {
          delete seed;
          continue;
        }

        if ( m_fitLine->fit(seed) < 1 || seed->ny() < m_nyCut || fabs( seed->sy() ) > m_tyCut ||
             fabs( seed->y0() ) > m_y0Cut1 ||
             seed->nx()+seed->ny() < m_nTotalCut1 ) {
          delete seed;
          continue;
        }
      }

      //  Tighter cuts if stub not from first station (most overlap tracks have stub in first station)
      if ( stn > 0 && ( fabs( seed->y0() ) > m_y0Cut2 || seed->nx()+seed->ny() < m_nTotalCut2 )) {
        delete seed;
        continue;
      }


      // set the dth
      const double sx = (seed->x(9400.,0.0) - seed->x(7900.,0.0))/1500.;
      const double dth = atan( sx ) - atan( (x1 - (z1-TsaConstants::zMagnet)*sx)*TsaConstants::oneOverZMagnet );
      seed->setDth(dth);
      seeds.push_back( seed );  //  Add to vector of seed candidates
    }
  }

  return StatusCode::SUCCESS;
}
