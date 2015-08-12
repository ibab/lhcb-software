// GaudiKernel
#include "GaudiKernel/SystemOfUnits.h"

#include "TsaStubLinker.h"

// Boost
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>
#include "boost/lexical_cast.hpp"
using namespace boost::assign;
using namespace boost;

using namespace Tf::Tsa;

DECLARE_TOOL_FACTORY( StubLinker )

StubLinker::StubLinker(const std::string& type,
                       const std::string& name,
                       const IInterface* parent):
  GaudiTool(type, name, parent),
  m_fitLine(NULL), m_parabolaFit(NULL)
{

  // constructor

  declareProperty("deltaSx",  m_deltaSx = 0.03);
  declareProperty("deltaXCon", m_deltaXCon = 0.08);
  declareProperty("outlierCutParabola", m_outlierCutParabola = 6.0);
  declareProperty("outlierCutLine", m_outlierCutLine = 6.0);

  declareInterface<ITsaStubLinker>(this);

}

StubLinker::~StubLinker(){
  // destructor
}

StatusCode StubLinker::finalize() 
{
  delete m_parabolaFit; m_parabolaFit = 0;
  delete m_fitLine;     m_fitLine     = 0;
  return GaudiTool::finalize();
}

StatusCode StubLinker::initialize(){

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize",sc);
  }

  m_parabolaFit = new SeedParabolaFit(TsaConstants::z0,
		  m_outlierCutParabola);
  m_fitLine = new SeedLineFit(msg(),TsaConstants::z0,
		  TsaConstants::sth, m_outlierCutLine);

  return sc;
}

StatusCode StubLinker::execute(std::vector<SeedStub*> stubs[], std::vector<SeedTrack*>& seeds) {

  for (unsigned int i = 0; i < 3; ++i) std::stable_sort(stubs[i].begin(), stubs[i].end(), SeedFunctor::increasingSX<const SeedStub*>() );  //  Add stub candidate to vector

  //  Loop over first station for linking
  for ( int stn = 0; stn < 2; ++stn ) {

    //  Loop over stubs in first station
    for ( std::vector<SeedStub*>::iterator it1 = stubs[stn].begin(); stubs[stn].end() != it1; ++it1 ) {
      SeedStub* stub1 = (*it1);
      if ( stub1->live() == false ) continue;
      const double x1 = stub1->x();
      const double y1 = stub1->y();
      const double z1 = stub1->z1();

      // starting value
      double testVal = stub1->sx() - m_deltaSx;
      std::vector<SeedStub*>::iterator it2  = std::lower_bound(stubs[stn+1].begin(),stubs[stn+1].end(),
                                                               testVal , SeedFunctor::compBySX_LB<const SeedStub*>());
      //  Loop over stubs in next station
      for ( ; stubs[stn+1].end() != it2; ++it2 ) {
        SeedStub* stub2 = (*it2);
        if ( stub2->live() == false ) continue;

        // apply cuts to link stubs
        if ( stub2->sx() - stub1->sx() > m_deltaSx) continue;
        if (fabs(xSlopeCon(stub1,stub2)) > m_deltaXCon) continue;

        const double z2 = stub2->z1();
        const double x2 = stub2->x();

        const double y2 = stub2->y();
        double dy = stub2->y()  - ( y1 + (stub2->z2()-stub1->z2()) * y1/stub1->z2());
        if ( fabs( dy ) < 3) {
          stub1->setLive( false );
          stub2->setLive( false );

          //  Two linked stubs are enough to make a seed candidate
          std::vector<SeedPnt> xPnts; xPnts.reserve(12);
          xPnts += SeedPnt(stub1->xHit1() ), SeedPnt( stub1->xHit2() ), SeedPnt( stub2->xHit1() ), SeedPnt( stub2->xHit2() );

          std::vector<SeedPnt> yPnts; yPnts.reserve(12);
          yPnts += SeedPnt(stub1->yHit1() ), SeedPnt( stub1->yHit2() ), SeedPnt( stub2->yHit1() ), SeedPnt( stub2->yHit2() );

          SeedTrack* seed = new SeedTrack(xPnts,yPnts, 5);

          double sav = (x2-x1)/(z2-z1);

          //  If the linked stubs are from stations 1+2, check for a third stub in station 3
          if ( stn == 0 ) {
            double testVal2 = stub2->sx() - m_deltaSx;
            std::vector<SeedStub*>::iterator it3  = std::lower_bound(stubs[2].begin(),stubs[2].end(),
                                                                     testVal2 , SeedFunctor::compBySX_LB<const SeedStub*>());

            for ( ; stubs[2].end() != it3; ++it3 ) {
              SeedStub* stub3 = (*it3);
              if ( stub3->live() == false ) continue;

              // apply linking cuts
              if (fabs(stub3->sx()- stub2->sx()) > m_deltaSx) continue;

              const double z3 = stub3->z1();
              double zav = 0.5*( z2 + z3 );
              double dx = x2 + stub2->sx()*(zav-z2) - ( stub3->x() + stub3->sx()*(zav-z3) );
              if ( fabs( dx ) > 3.) continue;

              dy = stub3->y() - ( y2 + (stub3->z2()-stub2->z2()) * y2/stub2->z2() );
              if ( fabs( dy ) < 3. ) {
                stub3->setLive( false );
                SeedPnt pnt1(stub3->xHit1());
                seed->addToXPnts(pnt1);
                SeedPnt pnt2(stub3->xHit2());
                seed->addToXPnts(pnt2);
                SeedPnt ypnt1(stub3->yHit1());
                seed->addToYPnts(ypnt1);
                SeedPnt ypnt2(stub3->yHit2());
                seed->addToYPnts(ypnt1);

                sav = (stub3->x()-x1)/(z3-z1);

                break;
              }
            }
          }
          double csth = sqrt( 1. + sav*sav );
          if ( m_parabolaFit->fit(seed,csth) < 1 ) {
            delete seed;
            continue;
          }

          // and fit a line to Y hits
          if ( m_fitLine->fit(seed) < 1 ) {
            delete seed;
            continue;
          }

          // set the dth
          const double sx = (seed->x(9400.,0.0) - seed->x(7900.,0.0))/1500.;
          const double dth = atan( sx ) - atan( (x1 - (z1-TsaConstants::zMagnet)*sx)*TsaConstants::oneOverZMagnet );
          seed->setDth(dth);
          //seed->setSelect( true );     //  just select all of these candidates
          seeds.push_back( seed );  //  Add to vector of seed candidates
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

