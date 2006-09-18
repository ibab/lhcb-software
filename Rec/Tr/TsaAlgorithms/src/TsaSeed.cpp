// $:  $
     
// Gaudi
#include "GaudiKernel/AlgFactory.h"

// boost
#include "boost/lexical_cast.hpp"

// CLHEP
#include "Kernel/PhysicalConstants.h"

// Tsa
#include "TsaKernel/TsaDataFunctor.h"
#include "TsaSeed.h"

#include "TsaKernel/TimeSummary.h"
#include "TsaKernel/Parabola.h"
#include "TsaKernel/Line.h"
#include "TsaKernel/IITDataSvc.h"
#include "TsaKernel/IOTDataSvc.h"
#include "TsaKernel/OTCluster.h"
#include "TsaKernel/STCluster.h"

#include "TsaKernel/IOTExpectedHits.h"
#include "TsaKernel/IITExpectedHits.h"
#include "TsaKernel/Types.h"
#include "TsaKernel/ClusFun.h"

#include "Kernel/Plane3DTypes.h"


#include "gsl/gsl_cdf.h"


// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>
using namespace boost::assign;
using namespace boost;

using namespace LHCb;
using namespace boost::lambda;



// factory defs
static const AlgFactory<TsaSeed> s_factory;
const IAlgFactory& TsaSeedFactory = s_factory;

TsaSeed::TsaSeed(const std::string& name,
                                     ISvcLocator* pSvcLocator):
  TsaBaseAlg(name, pSvcLocator)
{
  
  // TsaInitialization constructor
  declareProperty("otDataSvcType", m_otDataSvcType = "OTDataSvc");
  declareProperty("itDataSvcType", m_itDataSvcType = "ITDataSvc");
  declareProperty("otDataSvcName", m_otDataSvcName = "OTDataSvc");
  declareProperty("itDataSvcName", m_itDataSvcName = "ITDataSvc");
  declareProperty("LikCut", m_likCut = 999.);

  declareProperty("seedTracksLocation", m_seedTrackLocation = SeedTrackLocation::Default);
  declareProperty("seedHitLocation",  m_seedHitLocation = SeedHitLocation::Default); 
  declareProperty("seedStubLocation", m_seedStubLocation = SeedStubLocation::Default);


  // geometry and other stuff
  m_sth = 0.087156;
  m_z0 = 0.0;

  //  Cuts for candidate selection (in xSearch)
  declareProperty("xSearch_sxCut", m_xSearch_sxCut = list_of(0.3)(0.8));
  declareProperty("xSearch_xsCut",m_xSearch_xsCut = list_of(0.3)(0.7));
  declareProperty("xSearch_x0Cut",m_xSearch_x0Cut = list_of(1500.)(4500.));
  declareProperty("xSearch_sx2Cut",m_xSearch_sx2Cut = list_of(0.12)(0.8));
  declareProperty("xSearch_dthCut",m_xSearch_dthCut = list_of(0.4)(0.7));
  declareProperty("xSearch_tdCut",m_xSearch_tdCut = list_of(0.2)(0.8));
  declareProperty("xSearch_win",m_xSearch_win = list_of(0.6)(10.));
  declareProperty("xSearch_win2",m_xSearch_win2 = list_of(5.)(10.));
  declareProperty("xSearch_nWin",m_xSearch_nWin = list_of(2)(5));
  declareProperty("xSearch_nWin2",m_xSearch_nWin2 = list_of(4)(7));
  declareProperty("xSearch_nxCut",m_xSearch_nxCut = list_of(5)(7));

}


TsaSeed::~TsaSeed()
{
  // destructer
}

StatusCode TsaSeed::initialize()
{
  // Initialization
  
  StatusCode sc = TsaBaseAlg::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize");
  }
  m_itDataSvc = tool<IITDataSvc>(m_itDataSvcType,m_itDataSvcName);
  m_otDataSvc = tool<IOTDataSvc>(m_otDataSvcType,m_otDataSvcName);
   
  m_expectedHits = tool<IOTExpectedHits>("OTExpectedHits");
  m_expectedITHits = tool<IITExpectedHits>("ITExpectedHits");
  
  return StatusCode::SUCCESS;
}

StatusCode TsaSeed::execute(){
//-------------------------------------------------------------------------
//  Steering routine for track seeding
//-------------------------------------------------------------------------

//  startTimer();

  SeedTracks* seedSel = new SeedTracks();    //  Selected seed candidates
  seedSel->reserve(1000);

  std::vector<SeedStub*> stubs[3];            //  IT stubs per station
  for (unsigned iS = 0; iS < 3; ++iS) stubs[iS].reserve(100);
 
  SeedStubs* stubsCont = new SeedStubs();  
  stubsCont->reserve(200);

  SeedHits* hitsCont = new SeedHits();
  hitsCont->reserve(10000);

  // Loop over sectors of tracker (0-2 are IT, 3-4 are OT)
  for ( int sector = 0; sector < 5; ++sector ) {

    std::vector<SeedHit*> hits[6], sHits[6];  //  Hits per layer in X and stereo
    for (int i = 0; i < 6; ++i){
      hits[i].reserve(1000);
      sHits[i].reserve(1000);
    }

    std::vector<SeedTrack*> seeds;            //  Seed candidates within the sector
    seeds.reserve(1000);

    searchX( sector, hits, seeds );           //  Search for seed candidates in X projection

    searchStereo( sector, sHits, seeds );     //  Add stereo hits to X candidates

    likelihood( sector, seeds );              //  Calculate likelihood of seed candidates

    select( seeds );                          //  Select seed candidates according to likelihood

    //  For IT, use remaining hits to search for track stubs 
    if ( sector <= 2 ) findStubs( sector, hits, sHits, stubs );

    //  After the IT stub finding is finished, try to link the stubs to make seed candidates
    if ( sector == 2 ) linkStubs( stubs, seeds );

    //  For those IT stubs that remain, try to extend them into the OT
    if ( sector > 2 ) extendStubs( sector, stubs, hits, sHits, seeds );

    // Delete the temporary objects that have been created
    for ( std::vector<SeedTrack*>::iterator it = seeds.begin(); seeds.end() != it; ++it ) {
        seedSel->insert( *it);
    }

    for ( int lay = 0; lay < 6; ++lay ) {
      for ( std::vector<SeedHit*>::iterator it = hits[lay].begin(); hits[lay].end() != it; ++it ) hitsCont->insert(*it);
      for ( std::vector<SeedHit*>::iterator it = sHits[lay].begin(); sHits[lay].end() != it; ++it ) hitsCont->insert(*it);
    }
  }

  
  // clean up the stubs...
  for ( int stn = 0; stn < 3; ++stn ) {
    for ( std::vector<SeedStub*>::iterator it = stubs[stn].begin(); stubs[stn].end() != it; ++it ) {
      stubsCont->insert(*it);
    }
  }


  // put output in the store
  put(seedSel,m_seedTrackLocation);
  put(hitsCont, m_seedHitLocation); 
  put(stubsCont, m_seedStubLocation);

  //stopTimer();

  //  double nHit = hitsCont->size();
  //  plot(timer().lasttime()/1000 , "timer", 0., 10., 200  );
  // plot2D(nHit, timer().lasttime()/1000 , "timer", 0., 20000., 0., 10., 200, 200 );

  return StatusCode::SUCCESS;
}

void TsaSeed::findStubs( int& sect, std::vector<SeedHit*> hits[], std::vector<SeedHit*> sHits[], 
                         std::vector<SeedStub*> stubs[] ) {
//-------------------------------------------------------------------------
//  Search for track stubs in each IT station
//-------------------------------------------------------------------------

  double yTol[5] = {1.0,1.0,1.0,20.0,20.};

  //  Loop over IT stations
  for ( int stn = 0; stn < 3; ++stn ) {
    int lay = 2*stn;

    //  Loop over hits in first X layer of station
    std::vector<SeedHit*>::iterator start = hits[lay+1].begin();
    for ( std::vector<SeedHit*>::iterator it1 = hits[lay].begin(); hits[lay].end() != it1; ++it1 ) {
      SeedHit* hit1 = (*it1);
      if ( hit1->seeds().size() > 0 ) continue;  // only use hits that haven't been previously used
      double x1 = hit1->x();
      double z1 = hit1->z();

      //  Loop over hits in second X layer of station
      bool first = true;
      for ( std::vector<SeedHit*>::iterator it2 = start; hits[lay+1].end() != it2; ++it2 ) {
        SeedHit* hit2 = (*it2);
        if ( hit2->seeds().size() > 0 ) continue;
        double sx = (hit2->x()-x1) /( hit2->z() - z1);
        if ( sx < -0.5 ) continue;
        if ( first ) {
          first = false;
          start = it2;
        }
        if ( sx > 0.5 ) break;                        // cuts to select stub candidates
        if ( fabs(x1/3125. - sx) > 0.3 ) continue;

        //  Now for the stereo hits
        if (sHits[lay].empty() == true) continue;
        
        SeedHit* firstHit3 = hits[lay].front();;
        Gaudi::XYZVector vec(1., 0.0036*sx, -sx);
        double x3 = x1 + sx*(firstHit3->z()-z1);   //  Use straight line for stub in X projection
        Gaudi::XYZPoint point3(x3, firstHit3->clus()->yMid(), firstHit3->z());
        Gaudi::Plane3D plane3 = Gaudi::Plane3D(vec,point3);
        Gaudi::XYZPoint iPoint;
 
        for ( std::vector<SeedHit*>::iterator it3 = sHits[lay].begin(); sHits[lay].end() != it3; ++it3 ) {
          SeedHit* hit3 = (*it3);
          if ( hit3->seeds().empty() == false ) continue;
	 
          if ( !Tsa::ClusFun::intersection(hit3->clus(),plane3,iPoint) ) continue; 
          double y3 = iPoint.y();
          if ( y3 > hit3->clus()->yMax() + yTol[sect] ) continue;
          if ( y3 < hit3->clus()->yMin() - yTol[sect] ) break;
          hit3->setY( y3 );
          hit3->setZ( iPoint.z() );

          if (sHits[lay+1].empty() == true) continue;
        
	  SeedHit* firstHit4 = hits[lay].front();;
          double x4 = x1 + sx*(firstHit4->z()-z1);
       
          Gaudi::XYZPoint point4(x4, firstHit4->clus()->yMid(), firstHit4->z());
          Gaudi::Plane3D plane4 = Gaudi::Plane3D(vec,point4);

          for ( std::vector<SeedHit*>::iterator it4 = sHits[lay+1].begin(); sHits[lay+1].end() != it4; ++it4 ) {
            SeedHit* hit4 = (*it4);
            if ( hit4->seeds().size() > 0 ) continue;
          
            if ( !Tsa::ClusFun::intersection(hit4->clus(),plane4,iPoint) ) continue; 
            double y4 = iPoint.y();
            if ( y4 > hit4->clus()->yMax() + yTol[sect] ) break;
            if ( y4 < hit4->clus()->yMin() - yTol[sect] ) continue;
            hit4->setY( y4 );
            hit4->setZ( iPoint.z() );

            double sy = ( y4 - y3 ) / ( hit4->z() - hit3->z() );
            if ( fabs( sy ) > 0.25 ) continue;
            double dy = y4 - (y3 + ( hit4->z() - hit3->z())*y3/hit3->z() );
            if ( fabs( dy ) > 3. ) continue;

            //  Make a stub candidate
            SeedHit* xHit1 = hit1->clone(); 
            SeedHit* xHit2 = hit2->clone(); 
            SeedHit* yHit1 = hit3->clone(); 
            SeedHit* yHit2 = hit4->clone(); 

            SeedStub* stub = new SeedStub(x1, y3, z1, hit3->z(),
                                          sx, sy, xHit1, xHit2, yHit1, yHit2 );
           
            stubs[stn].push_back( stub );  //  Add stub candidate to vector
          }
        }
      }
    }
  }
};

void TsaSeed::linkStubs( std::vector<SeedStub*> stubs[], std::vector<SeedTrack*>& seeds ) {
//-------------------------------------------------------------------------
//  Link track stubs in IT stations to form seed track candidates
//-------------------------------------------------------------------------

//  Loop over first station for linking
  for ( int stn = 0; stn < 2; ++stn ) {

    //  Loop over stubs in first station
    for ( std::vector<SeedStub*>::iterator it1 = stubs[stn].begin(); stubs[stn].end() != it1; ++it1 ) {
      SeedStub* stub1 = (*it1);
      if ( stub1->live() == 0 ) continue;
      double x1 = stub1->x();
      double y1 = stub1->y();
      double z1 = stub1->z1();

      //  Loop over stubs in next station
      for ( std::vector<SeedStub*>::iterator it2 = stubs[stn+1].begin(); stubs[stn+1].end() != it2; ++it2 ) {
        SeedStub* stub2 = (*it2);
        if ( stub2->live() == 0 ) continue;
        double x2 = stub2->x();
        double y2 = stub2->y();
        double z2 = stub2->z1();
        double zav = 0.5*( z1 + z2 );
        double dx = x1 + stub1->sx()*(zav-z1) - ( x2 + stub2->sx()*(zav-z2) );
        double dsx = stub2->sx() - stub1->sx();
        double dy = y2 - ( y1 + (stub2->z2()-stub1->z2()) * y1/stub1->z2() );

        //  Apply cuts to select linked stubs
        if ( fabs( dx ) < 3. && fabs( dy ) < 3. && fabs( dsx ) < 0.03 ) {
          stub1->setLive( 0 );
          stub2->setLive( 0 );

          //  Two linked stubs are enough to make a seed candidate
	  //          seed->setSector( 5 );  //  indicates a seed candidate made from linked IT stubs
          
          std::vector<SeedPnt> xPnts; xPnts.reserve(12);
          SeedPnt pnt1( stub1->xHit1() );
          SeedPnt pnt2( stub1->xHit2() );
          SeedPnt pnt3( stub2->xHit1() );
          SeedPnt pnt4( stub2->xHit2() );
          xPnts += pnt1, pnt2, pnt3, pnt4;
 
          std::vector<SeedPnt> yPnts; yPnts.reserve(12);
          SeedPnt ypnt1( stub1->yHit1() );
          SeedPnt ypnt2( stub1->yHit2() );
          SeedPnt ypnt3( stub2->yHit1() );
          SeedPnt ypnt4( stub2->yHit2() );
          yPnts += ypnt1, ypnt2, ypnt3, ypnt4;
 
          SeedTrack* seed = new SeedTrack(xPnts,yPnts, 5); 

          double sav = (x2-x1)/(z2-z1);

          //  If the linked stubs are from stations 1+2, check for a third stub in station 3
          if ( stn == 0 ) {
            for ( std::vector<SeedStub*>::iterator it3 = stubs[2].begin(); stubs[2].end() != it3; ++it3 ) {
              SeedStub* stub3 = (*it3);
              if ( stub3->live() == 0 ) continue;
              double x3 = stub3->x();
              double y3 = stub3->y();
              double z3 = stub3->z1();
              zav = 0.5*( z2 + z3 );
              dx = x2 + stub2->sx()*(zav-z2) - ( x3 + stub3->sx()*(zav-z3) );
              dsx = stub3->sx() - stub2->sx();
              dy = y3 - ( y2 + (stub3->z2()-stub2->z2()) * y2/stub2->z2() );

              //  Apply the same linking cuts for stations 2+3 as for stations 1+2
              if ( fabs( dx ) < 3. && fabs( dy ) < 3. && fabs( dsx ) < 0.03 ) {
                stub3->setLive( 0 );
                SeedPnt pnt1(stub3->xHit1());
                seed->addToXPnts(pnt1);
                SeedPnt pnt2(stub3->xHit2());
                seed->addToXPnts(pnt2);
                SeedPnt ypnt1(stub3->yHit1());
                seed->addToYPnts(ypnt1);
                SeedPnt ypnt2(stub3->yHit2());
                seed->addToYPnts(ypnt1);

                sav = (x3-x1)/(z3-z1);
                break;
              }
            }
          }
          double cth = 1. / sqrt( 1. + sav*sav );
          if ( fitParabola(seed,cth) < 1 ) {
            delete seed;
            continue;
          }

          // and fit a line to Y hits
          if ( fitLine(seed) < 1 ) {
            delete seed;
            continue;
          }
          seed->setLik( 0. );       //  For now, don't bother calculating likelihood,
          seed->setSelect( 1 );     //  just select all of these candidates
          seeds.push_back( seed );  //  Add to vector of seed candidates
        }
      }
    }
  }
};

void TsaSeed::extendStubs( int& sect, std::vector<SeedStub*> stubs[], std::vector<SeedHit*> hits[], 
                           std::vector<SeedHit*> sHits[], std::vector<SeedTrack*>& seeds ) {
//-------------------------------------------------------------------------
//  Extend IT track stubs to form seed track candidates by adding OT hits
//-------------------------------------------------------------------------

// Loop over IT stubs in each station  
  for ( int stn = 0; stn < 3; ++stn ) {
    for ( std::vector<SeedStub*>::iterator it1 = stubs[stn].begin(); stubs[stn].end() != it1; ++it1 ) {
      SeedStub* stub = (*it1);

      // Only add OT hits from the sector containing the stub
      if ( stub->live() == 0 || (stub->y() > 0. && sect == 3) || (stub->y() < 0. && sect == 4) ) continue;
      
      std::vector<SeedHit*> select; select.reserve(24);
      for ( int lay = 0; lay < 6; ++lay ) {
        for ( std::vector<SeedHit*>::iterator it2 = hits[lay].begin(); hits[lay].end() != it2; ++it2 ) {
          SeedHit* hit = (*it2);
          if ( !hit->seeds().empty() ) continue;   // Hit not already used 
          double xStub = stub->x() + ( hit->z() - stub->z1() ) * stub->sx();

          // Select OT hits in X layers that are within window defined using stub 
          if ( fabs(( hit->x() - xStub )/( hit->z() - stub->z1() )) < 0.03 ) select.push_back( hit );
        }
      }
      if ( select.empty() == true ) continue;  // Require at least one OT X hit
      
      //  Two hits on track are given by IT hits on stub
      double x1 = stub->xHit1()->x();
      double x2 = stub->xHit2()->x();
      double z1 = stub->xHit1()->z();
      double z2 = stub->xHit2()->z();
      double cth = 1. / sqrt( 1. + stub->sx()*stub->sx() );
      int nMax = 0;
      int sSel[100];
      SeedHit* iSel[100];

      //  Loop over selected OT hits to define parabola
      for ( std::vector<SeedHit*>::iterator it3 = select.begin(); select.end() != it3; ++it3 ) {
        SeedHit* hit3 = (*it3);
        for ( int is = 0; is < 2; ++is ) {  // Include L/R ambiguity choice
          int sHit[99];
          SeedHit* iHit[99];
          double x3 = hit3->x() + (2*is-1)*hit3->r()/cth;
          double z3 = hit3->z();
          double ax = ( (x2-x1)/(z2-z1)-(x3-x1)/(z3-z1) )/(z2-z3);
          double bx = (x2-x1)/(z2-z1) - ax*(z2-z1);
          int nh = 0;

          //  Count hits within window defined by parabola
          for ( std::vector<SeedHit*>::iterator it = select.begin(); select.end() != it; ++it ) {
            SeedHit* hit = (*it);
            if ( hit == hit3 ) continue;
            double z = hit->z();
            double xSeed = ax*(z-z1)*(z-z1) + bx*(z-z1) + x1;
            double dx1 = hit->x() - hit->r()/cth - xSeed;
            double dx2 = dx1 + 2*hit->r()/cth;
            if ( fabs(dx1) < 1. || fabs(dx2) < 1. ) {
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
      if ( nMax < 1 ) continue;
      
      std::vector<SeedPnt> xPnts; xPnts.reserve(24);
      xPnts += SeedPnt(stub->xHit1()) , SeedPnt(stub->xHit2()) ;
      for ( int i = 0; i < nMax+1; ++i ) {
        xPnts.push_back( SeedPnt(iSel[i], sSel[i]) );
      }
      SeedTrack* seed = new SeedTrack(xPnts,6+stn); 

      // Fit parabola to all X hits on track 
      if ( fitParabola(seed,cth) < 1 || seed->nx() < 4 || fabs( seed->sx() ) > 0.8 ) {
        delete seed;
        continue;
      }

      //  Next select stereo hits on candidate
      std::vector<SeedPnt> yPnts; yPnts.reserve(24);
      yPnts += SeedPnt(stub->yHit1()), SeedPnt(stub->yHit2()) ;
     
      //  Loop over stereo hits in each (double) layer
      for ( int lay = 0; lay < 6; ++lay ) {

        if (hits[lay].empty() == true) continue;

        const double zFirst = hits[lay].front()->z();
        const double slope = seed->xSlope(zFirst,m_z0);
        Gaudi::XYZVector vec(1., 0.0036*slope, -slope);
        Gaudi::XYZPoint point(seed->x(zFirst,m_z0), hits[lay].front()->clus()->yMid(), zFirst );
        Gaudi::Plane3D plane = Gaudi::Plane3D(vec,point);
        Gaudi::XYZPoint iPoint;

        for ( std::vector<SeedHit*>::iterator it = sHits[lay].begin(); sHits[lay].end() != it; ++it ) {
          SeedHit* hit = (*it);
          if ( !hit->seeds().empty() ) continue;   // Hit not already used    
        
          if ( !Tsa::ClusFun::intersection(hit->clus(),plane,iPoint) ) continue; 
          double y = iPoint.y();
          double dy = y - hit->r()/m_sth - ( stub->y() / stub->z2() )*iPoint.z();
          double dy2 = dy + 2*hit->r()/m_sth;

          //  Require that they are within window defined by line from origin to the IT stub
          if ( fabs( dy ) < 40. || fabs( dy2 ) < 40. ) {
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
      if ( fitLine( seed ) < 1 || seed->ny() < 3 || fabs( seed->sy() ) > 0.04 || fabs( seed->y0() ) > 400. ||
           seed->nx()+seed->ny() < 8 ) {
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
        if ( ( ySeed > 0. && ySeed < pnt.hit()->clus()->yMin()-20. ) || 
             ( ySeed < 0. && ySeed > pnt.hit()->clus()->yMax()+20. ) ) {
          pnt.setSkip( 1 );  // kill points that are in unphysical region
          ++nKill;
        }
      }
      if ( nKill > 0 ) {
        if ( seed->nx() - nKill < 4 ) {
          delete seed;
          continue;
        }

        int rc = fitParabola( seed, cth );  //  Refit seed
        if ( rc < 1 || seed->nx() < 4 || fabs( seed->sx() ) > 0.8 ) {
          delete seed;
          continue;
        }
      }

      //  Same for stereo wires
      nKill = 0;
      std::vector<SeedPnt>& ypnts = seed->yPnts();
      for ( std::vector<SeedPnt>::iterator it = ypnts.begin(); ypnts.end() != it; ++it ) {
        SeedPnt & pnt = *it;
        if ( pnt.skip() || pnt.hit()->OT() == 0 ) continue;
        if ( ( pnt.coord() > 0. && pnt.coord() < pnt.hit()->clus()->yMin()-20. ) || 
             ( pnt.coord() < 0. && pnt.coord() > pnt.hit()->clus()->yMax()+20. ) ) {
          pnt.setSkip( 1 );
          ++nKill;
        }
      }
      if ( nKill > 0 ) {
        if ( seed->ny() - nKill < 3 ) {
          delete seed;
          continue;
        }

        if ( fitLine(seed) < 1 || seed->ny() < 3 || fabs( seed->sy() ) > 0.04 || fabs( seed->y0() ) > 400. ||
             seed->nx()+seed->ny() < 8 ) {
          delete seed;
          continue;
        }
      }

      //  Tighter cuts if stub not from first station (most overlap tracks have stub in first station)
      if ( stn > 0 && ( fabs( seed->y0() ) > 100. || seed->nx()+seed->ny() < 11 )) {
        delete seed;
        continue;
      }
      seed->setLik( 0. );       //  For now, don't bother calculating likelihood,
      seed->setSelect( 1 );     //  just select all of these candidates
      seeds.push_back( seed );  //  Add to vector of seed candidates
    }
  }
};

void TsaSeed::searchX( int& sect, std::vector<SeedHit*> hits[], std::vector<SeedTrack*>& seeds ) {
//-------------------------------------------------------------------------
//  Search for hits to form seed tracks in X stations
//-------------------------------------------------------------------------



  //  Load hits with data from Tsa clusters
  for ( int station = 1; station < 4; ++station ) {
    for ( int layer = 0; layer < 4; layer+=3 ) {
      int lay = 2*(station-1);
      if ( layer > 0 ) ++lay;
      if ( sect < 3 ) {
        for ( int ib = 0; ib < 2; ++ib ) {
          if ( sect > 0 && ib > 0 ) continue;
          int box = 2 + sect - ib;
          Tsa::STRange iRange =  m_itDataSvc->partition(station,layer+1,box);
          for (Tsa::STClusters::const_iterator itIter = iRange.begin(); itIter != iRange.end(); ++itIter){
            if ((*itIter)->isHot() == false) {
              SeedHit* hit = new SeedHit(*itIter); 
              hits[lay].push_back( hit );
	    }
          }
        }
      } else {
        Tsa::OTRange oRange = m_otDataSvc->partition(station,layer,sect-2);
        for (Tsa::OTClusters::const_iterator otIter = oRange.begin(); otIter != oRange.end(); ++otIter){
          Tsa::OTCluster* clus = *otIter;
          if ( clus->isHot() == false && clus->driftRadius() < 2.7 ) {
            SeedHit* hit = new SeedHit(clus);; 
            hits[lay].push_back( hit );
	  }
        }
      }
      std::sort( hits[lay].begin(), hits[lay].end(), TsaSeed::increasingX() );
    }
  }
  int tr = 1;               //  Flag to indicate which tracker sector belongs to
  if ( sect < 3 ) tr = 0;
    
  for ( int lay1 = 0; lay1 < 2; ++lay1 ) {
    for ( int lay2 = 5; lay2 > 3; --lay2 ) {
 
      std::vector<SeedHit*>::iterator start = hits[lay2].begin();
      for ( std::vector<SeedHit*>::iterator it1 = hits[lay1].begin(); hits[lay1].end() != it1; ++it1 ) {
        SeedHit* hit1 = (*it1);
        double x1 = hit1->x();
        double z1 = hit1->z();

        bool first = true;
        for ( std::vector<SeedHit*>::iterator it2 = start; hits[lay2].end() != it2; ++it2 ) {
          SeedHit* hit2 = (*it2);
          double x2 = hit2->x();
          double z2 = hit2->z();
          double sx = (x2-x1) / (z2-z1);
          if ( sx < -m_xSearch_sxCut[tr] ) continue;
          if ( first ) {
            first = false;
            start = it2;
          }
          if ( sx > m_xSearch_sxCut[tr] ) break;
          bool punt = false;
          std::vector<SeedHit*>& skip = hit1->skip();
          for ( std::vector<SeedHit*>::iterator it = skip.begin(); skip.end() != it; ++it ) {
            if ( hit2 == (*it) ) {
              punt = true;
              break;
            }
          } // for
 
          if ( punt ) continue;
          if ( fabs(x1/3125. - sx) > m_xSearch_xsCut[tr] ) continue;
          double dth = atan( sx ) - atan( (x1 - (z1-5300.)*sx)/5300. );
          if ( fabs( dth ) > m_xSearch_dthCut[tr] ) continue;
          double dx = x2-x1;
          double dz = z2-z1;
          double cth = dz / sqrt( dx*dx + dz*dz );
          std::vector<SeedHit*> select[3];
          for (unsigned int ii = 0; ii < 3; ++ii) select[ii].reserve(8);

	  //          collectXHits(hits[lay1], it1, x1, win[tr], select[0]);
	  //  collectXHits(hits[lay2], it2, x2, win[tr], select[2]);
	  
          std::vector<SeedHit*>::iterator it = it1; 
          if (it !=  hits[lay1].end()){
            ++it;
            while ( it != hits[lay1].end() && (*it)->x() - x1 < m_xSearch_win[tr] ) {
              select[0].push_back( *it );
              ++it;
	    }
          }

          it = it1; 
          if (it != hits[lay1].begin()){
            --it;
            while ( it != hits[lay1].begin() && x1 - (*it)->x() < m_xSearch_win[tr] ) {
              select[0].push_back( *it );
              --it;
	    }
          }

          it = it2;
          if (it !=  hits[lay2].end()){
            ++it;
            while ( it != hits[lay2].end() && (*it)->x() - x2 < m_xSearch_win[tr] ) {
              select[2].push_back( *it );
              ++it;
	    }
          }

          it = it2; 
          if (it != hits[lay2].begin()){
            --it;
            while ( it != hits[lay2].begin() && x2 - (*it)->x() < m_xSearch_win[tr] ) {
              select[2].push_back( *it );
              --it;
	    }
          }
	  

          for ( int lay = lay1+1; lay < lay2; ++lay ) {
            int stn = 1;
            if ( lay < 2 ) stn = 0;
            if ( lay > 3 ) stn = 2;
            double cut = m_xSearch_win[tr];
            if ( stn == 1 ) cut = m_xSearch_win2[tr];
            std::vector<SeedHit*>::iterator it = startX(hits[lay], x1, z1, sx);  
            for ( ; hits[lay].end() != it; ++it ) {
              SeedHit* hit = (*it);
              double dx = hit->x() - x1 - sx*( hit->z() - z1 );
              if ( dx < -cut ) continue;
              if ( dx >  cut ) break;
              hit->setUse1( 0 );
              hit->setUse2( 0 );
              select[stn].push_back( hit );
            }
          }
          if ( select[1].empty() == true || select[0].size()+select[1].size()+select[2].size() < (unsigned int)m_xSearch_nWin[tr] ) continue;

          int nhSel = 0;
          SeedHit* hSel = 0;
          int sSel[3] = {0, 0, 0};
          SeedHit* iSel[100];
          for ( std::vector<SeedHit*>::iterator it3 = select[1].begin(); select[1].end() != it3; ++it3 ) {
            SeedHit* hit3 = (*it3);
            if ( hit3->use1() && hit3->use2() ) continue;
            double x3 = hit3->x();
            double z3 = hit3->z();
            double ax = ( (x2-x1)/(z2-z1)-(x3-x1)/(z3-z1) )/(z2-z3);
            double bx = (x2-x1)/(z2-z1) - ax*(z2-z1);
            int nhMax[3] = {0, 0, 0};
            int sMax[3] = {0, 0, 0};
            SeedHit* ih[3][100];
            for ( int stn = 0; stn < 3; ++stn ) {
              if ( sect < 3 ) {
                int nh = 0;
                for ( std::vector<SeedHit*>::iterator it = select[stn].begin(); select[stn].end() != it; ++it ) {
                  SeedHit* hit = (*it);
                  if ( hit == hit3 ) continue;
   
                  double xSeed = ax*(hit->z()-z1)*(hit->z()-z1) + bx*(hit->z()-z1) + x1;
                  double dx = hit->x() - xSeed;
                  if ( fabs(dx) < 0.3 ) {
                    ih[stn][nh] = hit;
                    if ( nh < 99 ) ++nh;
                    hit->setUse1( 1 );
                    hit->setUse2( 1 );
                  }
                }
                nhMax[stn] = nh;
              } else {
                SeedHit* hStn = hit1;
                if ( stn == 1 ) hStn = hit3;
                if ( stn == 2 ) hStn = hit2;
                int nMax = 0;
                for ( int is = 0; is < 2; ++is ) {
                  if ( stn == 1 && is == 0 && hit3->use1() ) continue;
                  if ( stn == 1 && is == 1 && hit3->use2() ) continue;
                  int nh = 0;
                  for ( std::vector<SeedHit*>::iterator it = select[stn].begin(); select[stn].end() != it; ++it ) {
                    SeedHit* hit = (*it);
                    if ( hit == hit3 ) continue;
                    double z = hit->z();
                    double xSeed = ax*(z-z1)*(z-z1) + bx*(z-z1) + x1 + (2*is-1)*hStn->r()/cth;
                    double dx1 = hit->x() - hit->r()/cth - xSeed;
                    double dx2 = dx1 + 2*hit->r()/cth;
                    if ( fabs(dx1) < 1. || fabs(dx2) < 1. ) {
                      ++nh;
                      if ( fabs(dx1) < 1. ) hit->setUse1( 1 );
                      if ( fabs(dx2) < 1. ) hit->setUse2( 1 );
                    }
                  }
                  if ( nh > nMax ) {
                    nMax = nh;
                    nhMax[stn] = nh;
                    sMax[stn] = 2*is-1;
                  }
                }
              }
            }
            if ( nhMax[0]+nhMax[1]+nhMax[2] > nhSel ) {
              nhSel = nhMax[0]+nhMax[1]+nhMax[2];
              sSel[0] = sMax[0];
              sSel[1] = sMax[1];
              sSel[2] = sMax[2];
              hSel = hit3;
              if ( sect < 3 ) {
                int nh = 0;
                for ( int stn = 0; stn < 3; ++stn ) {
                  for ( int i = 0; i < nhMax[stn]; ++i ) {
                    iSel[nh] = ih[stn][i];
                    ++nh;
                  }
                }
              }
            }
          }
          if ( nhSel < 1 ) continue;

          std::vector<SeedPnt> xPnts; xPnts.reserve(24);
          xPnts +=  SeedPnt(hit1, sSel[0]),  SeedPnt( hit2, sSel[2] ), SeedPnt( hSel, sSel[1] );

          if ( sect < 3 ) {
            for ( int i = 0; i < nhSel; ++i ) {
              SeedPnt pnt( iSel[i] );
              xPnts.push_back( pnt );
            }
          } else {
            x1 += sSel[0]*hit1->r()/cth;
            x2 += sSel[2]*hit2->r()/cth;
            double x3 = hSel->x() + sSel[1]*hSel->r()/cth;
            double z3 = hSel->z();
            double ax = ( (x2-x1)/(z2-z1)-(x3-x1)/(z3-z1) )/(z2-z3);
            double bx = (x2-x1)/(z2-z1) - ax*(z2-z1);
            for ( int stn = 0; stn < 3; ++stn ) {
              for ( std::vector<SeedHit*>::iterator it = select[stn].begin(); select[stn].end() != it; ++it ) {
                SeedHit* hit = (*it);
                if ( hit == hSel ) continue;
  
                double xSeed = ax*(hit->z()-z1)*(hit->z()-z1) + bx*(hit->z()-z1) + x1;
                double dx1 = hit->x() + hit->r()/cth - xSeed;
                double dx2 = dx1 - 2*hit->r()/cth;
                if ( fabs(dx1) < 1. || fabs(dx2) < 1. ) {
                  hit->setUse1( 1 );
                  if ( fabs(dx1) < fabs(dx2) ) {
                    SeedPnt pnt( hit, 1 );
                    xPnts.push_back(pnt);
                  } else {
                    SeedPnt pnt(hit, -1 );
                    xPnts.push_back( pnt );
                  }
                }
              }
            }
          }
          if ( xPnts.size() < (unsigned int)m_xSearch_nWin2[tr] ) continue;

          SeedTrack* seed = new SeedTrack(xPnts,sect); 
          seed->setDth( dth );
          int rc = fitParabola( seed, cth );
          if ( rc < 1 || seed->nx() < m_xSearch_nxCut[tr] || fabs( seed->x0() ) > m_xSearch_x0Cut[tr] || 
               fabs( seed->sx() ) > m_xSearch_sx2Cut[tr] || fabs( 53000.*seed->tx() - dth ) > m_xSearch_tdCut[tr] ) {
            delete seed;
            continue;
          }
          seeds.push_back( seed );

          hit1->setUse1( 1 );
          hit2->setUse1( 1 );
          select[0].push_back( hit1 );
          select[2].push_back( hit2 );
          for ( std::vector<SeedHit*>::iterator it = select[0].begin(); select[0].end() != it; ++it ) {
            SeedHit* hit = (*it);
            if ( hit->use1() == 0 && hit->use2() == 0 ) continue;
            std::vector<SeedHit*>& skip2 = hit->skip();
            for ( std::vector<SeedHit*>::iterator it2 = select[2].begin(); select[2].end() != it2; ++it2 ) {
              SeedHit* hitb = (*it2);
              if ( hitb->use1() == 0 && hitb->use2() == 0 ) continue;
              skip2.push_back( hitb );
            }
          }
        }
      }
    }
  }
};

void TsaSeed::searchStereo( int& sect, std::vector<SeedHit*> hits[], std::vector<SeedTrack*>& seeds ) {
//-------------------------------------------------------------------------
//  Search for hits to form seed tracks in stereo stations of the OT 
//-------------------------------------------------------------------------

  double syCut[2] = {0.01, 0.1};
  double win[2] = {4., 100.};
  int nWin[2] = {2, 6};

  int tr = 1;
  if ( sect < 3 ) tr = 0;
  for ( int station = 1; station < 4; ++station ) {
    for ( int layer = 1; layer < 3; ++layer ) {
      int lay = 2*(station-1);
      if ( layer > 1 ) ++lay;
      if ( sect < 3 ) {
        for ( int ib = 0; ib < 2; ++ib ) {
          if ( sect > 0 && ib > 0 ) continue;
          int box = 2 + sect - ib;
          Tsa::STRange iRange =  m_itDataSvc->partition(station,layer+1,box);
          for (Tsa::STClusters::const_iterator itIter = iRange.begin(); itIter != iRange.end(); ++itIter){
            if ((*itIter)->isHot() == false) {
              SeedHit* hit = new SeedHit(*itIter); 
              hits[lay].push_back( hit );
	    }
          }
        }
      } else {
        Tsa::OTRange oRange = m_otDataSvc->partition(station,layer,sect-2);
        for (Tsa::OTClusters::const_iterator otIter = oRange.begin(); otIter != oRange.end(); ++otIter){
          Tsa::OTCluster* clus = *otIter;
          if (clus->isHot() == false  &&  clus->driftRadius() < 2.7 ) {
            SeedHit* hit = new SeedHit(clus); 
            hits[lay].push_back( hit );
	  }
        }
      }
      std::sort( hits[lay].begin(), hits[lay].end(), TsaSeed::increasingX() );
    }
  }

  double yTol[5] = {1.0,1.0,1.0,20.0,20.};


  for ( std::vector<SeedTrack*>::iterator it = seeds.begin(); seeds.end() != it; ++it ) {
    SeedTrack* seed = (*it);
    std::vector<SeedHit*> yHits[6];
    for (unsigned int ii = 0 ; ii < 6 ; ++ii) yHits[ii].reserve(16);
    for ( int lay = 0; lay < 6; ++lay ) {

      if (hits[lay].empty() == true) continue;

      bool uLay = false;
      if ( lay == 0 || lay == 2 || lay == 4 ) uLay = true;
 
      const double zFirst = hits[lay].front()->z();
      const double slope = seed->xSlope(zFirst,m_z0);
      Gaudi::XYZVector vec(1., 0.0036*slope, -slope);
      Gaudi::XYZPoint point(seed->x(zFirst,m_z0), hits[lay].front()->clus()->yMid(), zFirst );
      Gaudi::Plane3D plane = Gaudi::Plane3D(vec,point);
      Gaudi::XYZPoint iPoint;

      for ( std::vector<SeedHit*>::iterator it = startStereo(hits[lay],point.x()); hits[lay].end() != it; ++it ) {
        SeedHit* hit = (*it);
        if ( !Tsa::ClusFun::intersection(hit->clus(),plane,iPoint) ) continue; 
	//  const double y = iPoint.y();
        if ( uLay ) {
          if ( iPoint.y() > hit->clus()->yMax() + yTol[sect] ) continue;
          if ( iPoint.y() < hit->clus()->yMin() - yTol[sect] ) break;
        } else {
          if ( iPoint.y() > hit->clus()->yMax() + yTol[sect] )break;
          if ( iPoint.y() < hit->clus()->yMin() - yTol[sect]) continue;
	}
        hit->setY( iPoint.y() );
        hit->setZ( iPoint.z() );
        yHits[lay].push_back( hit );
      }
    }

    int nBest = 0;
    int nSel = 0;
    SeedHit* iSel[30];
    int sSel[30];
    for ( int lay1 = 0; lay1 < 2; ++lay1 ) {
      for ( int lay2 = 5; lay2 > 3; --lay2 ) {
        for ( std::vector<SeedHit*>::iterator it1 = yHits[lay1].begin(); yHits[lay1].end() != it1; ++it1 ) {
          SeedHit* hit1 = (*it1);
          double y1w = hit1->y();
          double z1 = hit1->z();
          for ( std::vector<SeedHit*>::iterator it2 = yHits[lay2].begin(); yHits[lay2].end() != it2; ++it2 ) {
            SeedHit* hit2 = (*it2);
            double sy = (hit2->y()-y1w) / (hit2->z()-z1);
            if ( fabs( sy - y1w/z1 ) > syCut[tr] ) continue;

            std::vector<SeedHit*> select; select.reserve(18);
            for ( int lay = lay1; lay <= lay2; ++lay ) {
              for ( std::vector<SeedHit*>::iterator it = yHits[lay].begin(); yHits[lay].end() != it; ++it ) {
                SeedHit* hit = (*it);
                if ( lay == lay1 && hit == hit1 ) continue;
                if ( lay == lay2 && hit == hit2 ) continue;
                if ( fabs( hit->y()-y1w-sy*(hit->z() - z1) ) < win[tr] ) select.push_back( hit );
              }
            }
            if ( select.size() < (unsigned int)nWin[tr] ) continue;

            if ( sect < 3 ) {
              if ( select.size() > (unsigned int)nBest ) {
                nBest = select.size();
                iSel[0] = hit1;
                iSel[1] = hit2;
                nSel = 2;
                for ( std::vector<SeedHit*>::iterator it = select.begin(); select.end() != it; ++it ) {
                  SeedHit* hit = (*it);
                  iSel[nSel] = hit;
                  if ( nSel < 29 ) ++nSel;
                }
              }
            } else {
              int nMax = 0;
              int s1Max = 0;
              int s2Max = 0;
              for ( int s1 = 0; s1 < 2; ++s1 ) {
                double y1 = hit1->y() + (2*s1-1)*hit1->r()/m_sth;
                for ( int s2 = 0; s2 < 2; ++s2 ) {
                  double y2 = hit2->y() + (2*s2-1)*hit2->r()/m_sth;
                  double sy = ( y2 - y1 ) / ( hit2->z() - z1 );
                  int nh = 0;
                  for ( std::vector<SeedHit*>::iterator it = select.begin(); select.end() != it; ++it ) {
                    SeedHit* hit = (*it);
                    double dy1 = hit->y() - hit->r()/m_sth - y1 - sy*( hit->z() - z1 );
                    double dy2 = dy1 + 2*hit->r()/m_sth;
                    if ( fabs(dy1) < 10. || fabs(dy2) < 10. ) ++nh;
                  }
                  if ( nh > nMax ) {
                    nMax = nh;
                    s1Max = s1;
                    s2Max = s2;
                  }
                }
              }
              if ( nMax > nBest ) {
                nBest = nMax;
                iSel[0] = hit1;
                iSel[1] = hit2;
                sSel[0] = 2*s1Max-1;
                sSel[1] = 2*s2Max-1;
                nSel = 2;
                double y1 = hit1->y() + (2*s1Max-1)*hit1->r()/m_sth;
                double y2 = hit2->y() + (2*s2Max-1)*hit2->r()/m_sth;
                sy = ( y2 - y1 ) / ( hit2->z() - z1 );
                for ( std::vector<SeedHit*>::iterator it = select.begin(); select.end() != it; ++it ) {
                  SeedHit* hit = (*it);
                  double dy1 = hit->y() - hit->r()/m_sth - y1 - sy*( hit->z() - z1 );
                  double dy2 = dy1 + 2*hit->r()/m_sth;
                  if ( fabs(dy1) < 10. || fabs(dy2) < 10. ) {
                    iSel[nSel] = hit;
                    if ( fabs(dy1) < fabs(dy2) ) {
                      sSel[nSel] = -1;
                    } else {
                      sSel[nSel] = 1;
                    }
                    if ( nSel < 29 ) ++nSel;
                  }
                } // for
              } //if 
            } // if
          } //yHits
        } // yHits
      } // lay
    } // lay
    if ( nBest > 2 ) {
      for ( int i = 0; i < nSel; ++i ) {
        int sign = 0;
        if (sect > 2) sign = sSel[i];
        SeedPnt pnt(iSel[i],sign);
        seed->addToYPnts(pnt);
      }

      int rc = fitLine( seed );
      if ( seed->ny() < 5 || rc < 1 || ( sect > 2 && seed->nx()+seed->ny() < 15 ) ) seed->setLive( 0 );
    } else {
      seed->setLive( 0 );
    }
  }
};

bool TsaSeed::decreasingLikelihood::operator() ( SeedTrack* first, SeedTrack* second ) const {
//-------------------------------------------------------------------------
//  Define the sort sequence for seed candidates
//-------------------------------------------------------------------------
  return ( 0 == second ? true : 0 == first ? false : first->lik() > second->lik());
}

void TsaSeed::likelihood( int& sect, std::vector<SeedTrack*>& seeds ) {
//-------------------------------------------------------------------------
//  Determine likelihood of seed candidates
//-------------------------------------------------------------------------
 
  for ( std::vector<SeedTrack*>::iterator it = seeds.begin(); seeds.end() != it; ++it ) {
    SeedTrack* seed = *it;
    if ( seed->live() == 0 ) continue;

    std::vector<SeedPnt>& pnts = seed->xPnts();
    for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
      SeedPnt & pnt = *it;
      double dy = seed->y(pnt.hit()->z(),m_z0) - pnt.hit()->clus()->yMid();
      double z = pnt.hit()->z() + .0036*dy;
      pnt.setZ( z );
    }

    double sx = seed->xSlope(8600.,0.0);
    double cth = 1. / sqrt( 1. + sx*sx );
    if ( refitParabola(seed,cth) < 1 ) {
      seed->setLive( 0 );
      continue;
    }
    int nxExp = 0;
    int nyExp = 0;
    int nxFound = 0;
    int nyFound = 0;
    double xLik = 0.;
    double yLik = 0.;
    
    // Fill seed track parameters into format required by tool to determine expected hits
    Tsa::Line aLine( seed->sy(), seed->y0() );
    Tsa::Parabola aParab( seed->tx(), seed->sx(), seed->x0() ); 
    std::vector<SeedPnt>& ypnts = seed->yPnts();
    for ( int station = 1; station < 4; ++station ) {
      for ( int layer = 0; layer < 4; ++layer ) {

        if ( sect < 3 ) {
          LHCb::STChannelID testChan(LHCb::STChannelID::typeIT, station,layer+1,0,0,0);
          typedef std::vector<IITExpectedHits::ITPair> ITPairs;
          ITPairs output;
          m_expectedITHits->collect(aParab,aLine,testChan,output,sect);   

          int oldx = -1;
          int oldy = -1;
          for (ITPairs::iterator iter = output.begin(); iter != output.end(); ++iter ){
	    // LHCb::STChannelID chan = iter->first; 
            if ( layer == 0 || layer == 3 ) {
              if ( int(iter->second) != oldx ) {
                oldx = int(iter->second);
                ++nxExp;
              }

              std::vector<SeedPnt>::iterator iterX = std::find_if(pnts.begin(),
                                                                  pnts.end(), 
                                                                  bind(&SeedPnt::sameChan, _1, iter->first)); 

              if (iterX != pnts.end() ) ++nxFound;
	     

            } else {

              if ( int(iter->second) != oldy ) {
                oldy = int(iter->second);
                ++nyExp;
              }

              std::vector<SeedPnt>::iterator iterY = std::find_if(ypnts.begin(),
                                                                  ypnts.end(), 
                                                                  bind(&SeedPnt::sameChan, _1, iter->first)); 

              if (iterY != ypnts.end() ) ++nyFound;
        
            }
          }
        } else {

          LHCb::OTChannelID testChan(station,layer,0,0,0);
          typedef std::vector<IOTExpectedHits::OTPair> OTPairs;
	  OTPairs output;
	  m_expectedHits->collect(aParab,aLine,testChan,output, sect);   
	  for (OTPairs::iterator iter = output.begin() ;iter != output.end(); ++iter ){
	   
            if ( layer == 0 || layer == 3 ) {
	      // x projection
	      std::vector<SeedPnt>::iterator iterX = std::find_if(pnts.begin(),
                                                              pnts.end(), 
                                                              bind(&SeedPnt::sameStraw, _1, iter->first)); 
              ++nxExp;
              if (iterX != pnts.end() ) {
                 ++nxFound;
	      }
              else {
                 xLik += log( 0.261 + exp( 5.1 * fabs(iter->second) - 11.87 ) );
	      }
            }  
            else {
              // y projection
              std::vector<SeedPnt>::iterator iterY = std::find_if(ypnts.begin(),
                                                             ypnts.end(), 
                                                             bind(&SeedPnt::sameStraw, _1, iter->first));
               ++nyExp;
              if (iterY != ypnts.end() ) {
                ++nyFound;
	      }
              else {
                yLik += log( 0.261 + exp( 5.1 * fabs(iter->second) - 11.87 ) );
	      } // found or not
	   } // switch x or y projection 
         } // OTPairs
	} // if
      } // layer
    }// station

    if ( seed->nx() < 4 || seed->ny() < 3 || nxExp < 4 || nyExp < 4 || 
      ( double(nxFound) / nxExp < 0.5 && double(nyFound) / nyExp < 0.5) ) {
      seed->setLive( 0 );
    } else {
      double prx = log( gsl_cdf_chisq_Q( seed->xChi2(), seed->nx()-3 ));
      double pry = log( gsl_cdf_chisq_Q( seed->yChi2(), seed->ny()-2 ));
      double lik = xLik + yLik + ( prx + pry )/3.;

      int n = nxExp + nyExp;
      int r = nxFound + nyFound;
      if ( r > n ) r = n;
      double fnum = 1.;
      double fden = 1.;
      for ( int i = 0; i < n-r; ++i ) {
        fnum *= double( n-i );
        fden *= double( i+1 );
      }
      
      // Add binomial term to likelihood according to number of found hits on seed
      double eff = 0.9;
      if ( sect < 3 ) eff = 0.99;
      lik += log( fnum/fden ) + r*log( eff ) + (n-r)*log( 1.-eff );

      double arg1 = fabs( seed->y0() );
      double arg2 = fabs( 53000.*seed->tx() - seed->dth() );
      
      double f1 = ( exp( 5.335-0.005433*arg1 ) + exp( 7.551-0.04098*arg1 ) ) / ( exp(5.335) + exp(7.552) );
      double f2 = ( exp( 6.884-35.49*arg2 ) + exp( 5.62-6.928*arg2 ) ) / ( exp(6.884) + exp(5.62) );
      
      lik += log( f1 ) + log( f2 );
      if ( lik < -30. ) seed->setLive( 0 );

      seed->setLik( lik );
    }
  }
};

void TsaSeed::select( std::vector<SeedTrack*>& seeds ) {
//-------------------------------------------------------------------------
//  Select seeds
//-------------------------------------------------------------------------

  // Sort seeds according to their likelihood
  std::sort( seeds.begin(), seeds.end(), TsaSeed::decreasingLikelihood() );

  for ( std::vector<SeedTrack*>::iterator it = seeds.begin(); seeds.end() != it; ++it ) {
    SeedTrack* seed = *it;
    if ( seed->live() ) {    // Check seed is still available

      seed->setLive( 0 );
      int nLink = 0;
      SeedTrack* iLink[500];
      int nShare[500];
      bool kill = false;
      for ( std::vector<SeedPnt>::iterator itp = seed->xPnts().begin(); seed->xPnts().end() != itp; ++itp ) {
        SeedPnt & pnt = *itp;
        if ( pnt.skip() ) continue;
        if ( pnt.hit()->seeds().size() < 1 ) continue;
        for ( std::vector<SeedTrack*>::iterator it2 = pnt.hit()->seeds().begin(); pnt.hit()->seeds().end() != it2; ++it2 ) {
          SeedTrack* seed2 = *it2;
          bool found = false;
          if ( nLink > 0 ) {
            for ( int i = 0; i < nLink; ++i ) {
              if ( seed2 == iLink[i] ) {
                nShare[i]++;
                found = true;
                if ( nShare[i] > 3 ) kill = true;
                break;
              }
            }
          }
          if ( found == false ) {
            iLink[nLink] = seed2;
            nShare[nLink] = 1;
            if ( nLink < 499 ) ++nLink;
          }
          if ( kill ) break;
        }
        if ( kill ) break;
      }
      if ( kill ) continue;

      nLink = 0;
      for ( std::vector<SeedPnt>::iterator itp = seed->yPnts().begin(); seed->yPnts().end() != itp; ++itp ) {
        SeedPnt & pnt = *itp;
        if ( pnt.skip() ) continue;
        if ( pnt.hit()->seeds().size() < 1 ) continue;
        for ( std::vector<SeedTrack*>::iterator it2 = pnt.hit()->seeds().begin(); pnt.hit()->seeds().end() != it2; ++it2 ) {
          SeedTrack* seed2 = *it2;
          bool found = false;
          if ( nLink > 0 ) {
            for ( int i = 0; i < nLink; ++i ) {
              if ( seed2 == iLink[i] ) {
                nShare[i]++;
                found = true;
                if ( nShare[i] > 3 ) kill = true;
                break;
              }
            }
          }
          if ( found == false ) {
            iLink[nLink] = seed2;
            nShare[nLink] = 1;
            if ( nLink < 499 ) ++nLink;
          }
          if ( kill ) break;
        }
        if ( kill ) break;
      }
      if ( kill ) continue;

      seed->setSelect( 1 );  // Mark it as selected

      for ( std::vector<SeedPnt>::iterator itp = seed->xPnts().begin(); seed->xPnts().end() != itp; ++itp ) {
        SeedPnt & pnt = *itp;
        if ( pnt.skip() ) continue;
        pnt.hit()->seeds().push_back( seed );
      }
      for ( std::vector<SeedPnt>::iterator itp = seed->yPnts().begin(); seed->yPnts().end() != itp; ++itp ) {
        SeedPnt & pnt = *itp;
        if ( pnt.skip() ) continue;
        pnt.hit()->seeds().push_back( seed );
      }
    }
  }
};
  
int TsaSeed::fitLine( SeedTrack* seed ) {
//-------------------------------------------------------------------------
//  Fit a line y = y0 + s*(z-m_z0) through hits on seed track
//-------------------------------------------------------------------------

  std::vector<SeedPnt>& pnts = seed->yPnts(); 
  if ( pnts.size() < 2 ) return -1;
  bool update = false;
  int nIt = 0;


  do {
    ++nIt;
    update = 0;

    double a = 0.;
    double b = 0.;
    double c = 0.;
    double d = 0.;
    double e = 0.;
    int ny = 0;
    
    for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
      SeedPnt & pnt = *it;
      if ( pnt.skip() ) continue;
      ++ny;
      double y = pnt.hit()->y();
      if ( pnt.hit()->OT() ) y += pnt.sign()*pnt.hit()->r()/m_sth;
      double z = pnt.hit()->z();
      double dz2 = (z-m_z0)*(z-m_z0);
      double dd = 0.0036/(m_sth*m_sth);
      if ( pnt.hit()->OT() ) {
        dd = 0.04/(m_sth*m_sth);
        if ( pnt.sign() == 0 ) dd = 1.0/(m_sth*m_sth);
      }
      double invDD = 1/dd;
      a += y*(z-m_z0)*invDD;
      b += (z-m_z0)* invDD;
      c += dz2 * invDD;
      d += y * invDD;
      e += 1. *invDD;
    }
    if ( ny < 2 ) return -2;
    double det = c*e - b*b;
    if ( fabs( det ) < 1.e-12 || fabs(b) < 1.e-12 ) {
      return -3;
    } else { 
      const double s = ( a*e - b*d ) / det;
      const double y0 = ( a - c*s ) / b;
      seed->setNy( ny );
      seed->setYParams(s,y0);
    }

    double dMax = 0.;
    std::vector<SeedPnt>::iterator itMax;
    for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
      SeedPnt & pnt = *it;
      if ( pnt.skip() ) continue;
      if ( seed->sector() > 5 && pnt.hit()->OT() == 0 ) continue;
      double y = pnt.hit()->y();
      //double z = pnt.hit()->z();
      double ySeed = seed->y(pnt.hit()->z(),m_z0);
      double dy = ( y - ySeed ) / ( 0.06/m_sth );
      if ( pnt.hit()->OT() ) {
        double r = pnt.hit()->r()/m_sth;
        if ( pnt.sign() == 0 ) {
          update = 1;
          pnt.setSign( 1 );
          if ( fabs( y+r - ySeed ) > fabs( y-r - ySeed ) ) pnt.setSign( -1 );
        } else {
          if ( fabs( y+pnt.sign()*r - ySeed ) > fabs( y-pnt.sign()*r - ySeed ) ) {
            update = 1;
            //pnt.setSign( -pnt.sign() );
            pnt.flipSign();
          }
        }
        dy = ( y + pnt.sign()*r - ySeed ) / ( 0.2/m_sth );
      }
      if ( fabs(dy) > dMax ) {
        dMax = fabs(dy);
        itMax = it;
      }
    }
    if ( update == 0 && dMax > 4. ) {
      update = 1;
      (*itMax).setSkip( 1 );
    }
  } while ( update && nIt < 12 ); 
  if ( nIt == 12 ) return -4;

  double chi2 = 0.;
  for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
    SeedPnt & pnt = *it;
    double y = pnt.hit()->y();
    if ( pnt.hit()->OT() ) y += pnt.sign()*pnt.hit()->r()/m_sth;
    const double z = pnt.hit()->z();
    pnt.setCoord( y );
    pnt.setZ( z );
    if ( pnt.skip() ) continue;
    const double ch = y - seed->y(z,m_z0);
    if ( pnt.hit()->OT() ) {
      chi2 += ch*ch*m_sth*m_sth / 0.04;
    } else {
      chi2 += ch*ch*m_sth*m_sth / 0.0036;
    }
  }
  seed->setYChi2( chi2 );

  return nIt;
};

int TsaSeed::fitParabola( SeedTrack* seed, double &cth ) {
//-------------------------------------------------------------------------
//  Fit a parabola x = x0 + s*(z-m_z0) + t*(z-m_z0)**2 through hits on seed track
//-------------------------------------------------------------------------

  std::vector<SeedPnt>& pnts = seed->xPnts(); 
  if ( pnts.size() < 3 ) return -1;
  bool update = 0;
  int nIt = 0;
  
  do {
    ++nIt;
    update = 0;
    double a = 0.;
    double b = 0.;
    double c = 0.;
    double d = 0.;
    double e = 0.;
    double f = 0.;
    double g = 0.;
    double h = 0.;
    int nx = 0;
    for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
      SeedPnt & pnt = *it;
      if ( pnt.skip() ) continue;
      ++nx;
      double x = pnt.hit()->x();
      if ( pnt.hit()->OT() ) x += pnt.sign()*pnt.hit()->r()/cth;
      double z = pnt.hit()->z();
      double dz2 = (z-m_z0)*(z-m_z0);
      double dz3 = dz2*(z-m_z0);
      double dz4 = dz3*(z-m_z0);
      double dd = 0.0036;
      if ( pnt.hit()->OT() ) {
        dd = 0.04/(cth*cth);
        if ( pnt.sign() == 0 ) dd = 1.0/(cth*cth);
      }
      double invdd = 1.0/dd;
      a += x * invdd;
      b += 1. * invdd;
      c += (z-m_z0) *invdd;
      d += dz2 *invdd;
      e += x*(z-m_z0) *invdd;
      f += dz3 *invdd;
      g += x*dz2 *invdd;
      h += dz4 * invdd;

    }
    if ( nx < 3 ) return -2;
    if ( fabs( (d*d-f*c)*(f*d-h*c)-(f*f-h*d)*(d*c-f*b) ) < 1.e-12 ||
         fabs( f*c-d*d ) < 1.e-12 || fabs(d) < 1.e-12 ) return -3;

    double x0 = ( (f*f-h*d)*(f*a-d*e)-(d*d-f*c)*(h*e-f*g) ) /
      ( (d*d-f*c)*(f*d-h*c)-(f*f-h*d)*(d*c-f*b) );
    double s = ( (f*a-d*e) + (d*c-f*b)*x0 ) / (f*c-d*d);
    double t = ( a - b*x0 - c*s ) / d;
    seed->setNx( nx );
    seed->setXParams(t,s,x0);
    
    double dMax = 0.;
    std::vector<SeedPnt>::iterator itMax;
    for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
      SeedPnt & pnt = *it;
      if ( pnt.skip() ) continue;
      if ( seed->sector() > 5 && pnt.hit()->OT() == 0 ) continue;
      double x = pnt.hit()->x();
      double z = pnt.hit()->z();
      double xSeed = seed->x(z,m_z0);
      double dx = (x - xSeed) / 0.06;
      if ( pnt.hit()->OT() ) {
        double r = pnt.hit()->r()/cth;
        if ( pnt.sign() == 0 ) {
          update = 1;
          pnt.setSign( 1 );
          if ( fabs( x+r - xSeed ) > fabs( x-r - xSeed ) ) pnt.setSign( -1 );
        } else {
          if ( fabs( x+pnt.sign()*r - xSeed ) > fabs( x-pnt.sign()*r - xSeed ) ) {
            update = 1;
	    //pnt.setSign( -pnt.sign() );
            pnt.flipSign();
          }
        }
        dx = (x + pnt.sign()*r - xSeed)*cth / 0.2;
      }
      if ( fabs(dx) > dMax ) {
        dMax = fabs(dx);
        itMax = it;
      }
    }
    if ( update == 0 && dMax > 3. ) {
      update = 1;
      (*itMax).setSkip( 1 );
    }
  } while ( update && nIt < 20 );
  if ( nIt == 20 ) return -4;
  
  double chi2 = 0.;
  for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
    SeedPnt & pnt = *it;
    double x = pnt.hit()->x();
    if ( pnt.hit()->OT() ) x += pnt.sign()*pnt.hit()->r()/cth;
    pnt.setCoord( x );
    if ( pnt.skip() ) continue;
    double z = pnt.hit()->z();
    double ch = x - seed->x(z,m_z0);
    if ( pnt.hit()->OT() ) {
      chi2 += ch*ch*cth*cth / 0.04;
    } else {
      chi2 += ch*ch / 0.0036;
    }
  }
  seed->setXChi2( chi2 );
  //  seed->setXPnts( pnts );
  return nIt;
};

int TsaSeed::refitParabola( SeedTrack* seed, double &cth ) {
//-------------------------------------------------------------------------
//  Fit a parabola x = x0 + s*(z-m_z0) + t*(z-m_z0)**2 through hits on seed track
//-------------------------------------------------------------------------

  std::vector<SeedPnt>& pnts = seed->xPnts(); 
  if ( pnts.size() < 3 ) return -1;

  double a = 0.;
  double b = 0.;
  double c = 0.;
  double d = 0.;
  double e = 0.;
  double f = 0.;
  double g = 0.;
  double h = 0.;
  for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
    SeedPnt & pnt = *it;
    if ( pnt.skip() ) continue;
    double x = pnt.coord();
    double z = pnt.z();
    double dz2 = (z-m_z0)*(z-m_z0);
    double dz3 = dz2*(z-m_z0);
    double dz4 = dz3*(z-m_z0);
    double dd = 0.0036;
    if ( pnt.hit()->OT() ) dd = 0.04/(cth*cth);
    double invDD = 1./dd;
    a += x * invDD;
    b += 1. * invDD;
    c += (z-m_z0) * invDD;
    d += dz2 * invDD;
    e += x*(z-m_z0) *invDD;
    f += dz3 *invDD;
    g += x*dz2 *invDD;
    h += dz4 * invDD;
  }
  if ( fabs( (d*d-f*c)*(f*d-h*c)-(f*f-h*d)*(d*c-f*b) ) < 1.e-12 ||
       fabs( f*c-d*d ) < 1.e-12 || fabs(d) < 1.e-12 ) return -3;

  double x0 = ( (f*f-h*d)*(f*a-d*e)-(d*d-f*c)*(h*e-f*g) ) /
    ( (d*d-f*c)*(f*d-h*c)-(f*f-h*d)*(d*c-f*b) );
  double s = ( (f*a-d*e) + (d*c-f*b)*x0 ) / (f*c-d*d);
  double t = ( a - b*x0 - c*s ) / d;

  seed->setXParams(t,s,x0);
   
  double chi2 = 0.;
  for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
    SeedPnt & pnt = *it;
    if ( pnt.skip() ) continue;
    double ch = pnt.coord() - seed->x(pnt.z(),m_z0);
    if ( pnt.hit()->OT() ) {
      chi2 += ch*ch*cth*cth / 0.04;
    } else {
      chi2 += ch*ch / 0.0036;
    }
  }
  seed->setXChi2( chi2 );
  return 1;
};


bool TsaSeed::increasingX::operator() ( SeedHit* first, SeedHit* second ) const {
//-------------------------------------------------------------------------
//  Define the sort sequence for hits
//-------------------------------------------------------------------------
  return ( 0 == second ? true : 0 == first ? false : first->x() < second->x());
}

StatusCode TsaSeed::finalize(){
   
  return TsaBaseAlg::finalize();
}


std::vector<SeedHit*>::iterator TsaSeed::startX(std::vector<SeedHit*>& hits, const double x1,
                                                const double z1, const double sx) const{

  if (hits.empty() == true) return hits.end();

  const double xTest = x1 + (sx*(hits.front()->z() - z1)) - 20.; 
  return std::lower_bound(hits.begin(),hits.end(), xTest , compByX_LB());
}


 std::vector<SeedHit*>::iterator TsaSeed::startStereo(std::vector<SeedHit*>& hits, const double x) const{

   //   return hits.begin();
 
   // find the start
  if (hits.empty() == true) return hits.end();

   const double xTest = x - 240.0;
   return std::lower_bound(hits.begin(),hits.end(), xTest , compByXMin_LB());
 }


