#ifdef _WIN32
#pragma warning ( disable : 4244 ) // conversion double to float in root
#endif

// GaudiKernel
#include "GaudiKernel/SystemOfUnits.h"

#include "TsaStubFind.h"

using namespace Tf::Tsa;

DECLARE_TOOL_FACTORY( StubFind )

StubFind::StubFind(const std::string& type,
                   const std::string& name,
                   const IInterface* parent):
  GaudiTool(type, name, parent)
{
  // constructer
  declareProperty("sector", m_sector = 0);
  declareProperty("yTol", m_yTol = 10.0);
  declareProperty("xSearch_xsParam", m_xsParam = 1.0/3125.0);
  declareProperty("sxCut", m_sxCut = 0.4);
  declareProperty("dAngle", m_dAngle = 0.3);
  declareProperty("syCut", m_syCut = 0.25);
  declareProperty("yCut", m_yCut = 5.0);

  // constructer
  declareInterface<ITsaStubFind>(this);

}

StubFind::~StubFind() { }

StatusCode StubFind::execute(std::vector<SeedHit*> hits[],
                             std::vector<SeedHit*> sHits[],
                             std::vector<SeedStub*> stubs[]) {


  //  Loop over IT stations
  for ( int stn = 0; stn < 3; ++stn ) {
    int lay = 2*stn;
    //  debug() << "Layer = " << lay   << " # hits  = " << hits[lay].size() << endreq;
    //  debug() << "Layer = " << lay+1 << " # hits  = " << hits[lay+1].size() << endreq;
    // debug() << "Layer = " << lay   << " # shits = " << sHits[lay].size() << endreq;
    // debug() << "Layer = " << lay+1 << " # shits = " << sHits[lay+1].size() << endreq;

    //  Loop over hits in first X layer of station
    std::vector<SeedHit*>::iterator start = hits[lay+1].begin();
    for ( std::vector<SeedHit*>::iterator it1 = hits[lay].begin(); hits[lay].end() != it1; ++it1 ) {
      SeedHit* hit1 = (*it1);
      if ( hit1->onTrack() == true ) continue;  // only use hits that haven't been previously used
   

      double x1 = hit1->x();
      double z1 = hit1->z();

      //  Loop over hits in second X layer of station
      bool first = true;
      for ( std::vector<SeedHit*>::iterator it2 = start; hits[lay+1].end() != it2; ++it2 ) {
        SeedHit* hit2 = (*it2);
        if ( hit2->onTrack() == true ) continue;
   
        double sx = (hit2->x()-x1) /( hit2->z() - z1);
        if ( sx < -m_sxCut ) continue;
        if ( first ) {
          first = false;
          start = it2;
        }
        if ( sx > m_sxCut ) break;  // cuts to select stub candidates
        if ( fabs(x1*m_xsParam - sx) > m_dAngle ) continue; 

        //  Now for the stereo hits
        if (sHits[lay].empty()) continue; 

        SeedHit* firstHit3 = hits[lay].front();
        Gaudi::XYZVector vec(1., TsaConstants::tilt*sx, -sx);
        double x3 = x1 + sx*(firstHit3->z()-z1);   //  Use straight line for stub in X projection
        //Gaudi::XYZPoint point3(x3, firstHit3->clus()->yMid(), firstHit3->z());
        Gaudi::XYZPoint point3(x3, firstHit3->yMid(), firstHit3->z());
        Gaudi::Plane3D plane3 = Gaudi::Plane3D(vec,point3);
        Gaudi::XYZPoint iPoint;
        //double tol = 0.2*hits[lay].front()->clus()->length();
        double tol = 0.2*hits[lay].front()->length();

        for ( std::vector<SeedHit*>::iterator it3 = SeedFun::startStereo(sHits[lay],x3,tol);
              sHits[lay].end() != it3; ++it3 ) {
          SeedHit* hit3 = (*it3);
          if ( hit3->onTrack() == true ) continue;
  

          if ( !Tf::intersection(hit3->tfHit(),plane3,iPoint) ) continue; 
        

          double y3 = iPoint.y();
          //if ( y3 > hit3->clus()->yMax() + m_yTol ) continue;
          //if ( y3 < hit3->clus()->yMin() - m_yTol ) break;
          if ( y3 > hit3->yMax() + m_yTol ) continue;
          if ( y3 < hit3->yMin() - m_yTol ) break;

          hit3->setY( y3 );
          hit3->setZ( iPoint.z() );

          if (sHits[lay+1].empty() == true) continue;

          SeedHit* firstHit4 = hits[lay].front();;
          double x4 = x1 + sx*(firstHit4->z()-z1);
          //Gaudi::XYZPoint point4(x4, firstHit4->clus()->yMid(), firstHit4->z());
          Gaudi::XYZPoint point4(x4, firstHit4->yMid(), firstHit4->z());
          Gaudi::Plane3D plane4 = Gaudi::Plane3D(vec,point4);

          for ( std::vector<SeedHit*>::iterator it4 = SeedFun::startStereo(sHits[lay+1],x4,tol);
                sHits[lay+1].end() != it4; ++it4 ) {
            SeedHit* hit4 = (*it4);
            if ( hit4->onTrack() == true ) continue;


            if ( !Tf::intersection(hit4->tfHit(),plane4,iPoint) )continue; 
        

            double y4 = iPoint.y();
            // if ( y4 > hit4->clus()->yMax() + m_yTol ) break;
            // if ( y4 < hit4->clus()->yMin() - m_yTol ) continue;
            if ( y4 > hit4->yMax() + m_yTol ) break;
            if ( y4 < hit4->yMin() - m_yTol ) continue;
            hit4->setY( y4 );
            hit4->setZ( iPoint.z() );

            double sy = ( y4 - y3 ) / ( hit4->z() - hit3->z() );
            if ( fabs( sy ) > m_syCut ) continue;
            double dy = y4 - (y3 + ( hit4->z() - hit3->z())*y3/hit3->z() );
            if ( fabs( dy ) > m_yCut ) continue;

        

            //  Make a stub candidate
            SeedHit* xHit1 = hit1->clone();
            SeedHit* xHit2 = hit2->clone();
            SeedHit* yHit1 = hit3->clone();
            SeedHit* yHit2 = hit4->clone();
            SeedStub* stub = new SeedStub(x1, y3, z1, hit3->z(),
                                          sx, sy, xHit1, xHit2, yHit1, yHit2, true );
            // without cloning ...
            //SeedStub* stub = new SeedStub(x1, y3, z1, hit3->z(),
            //                              sx, sy, hit1, hit2, hit3, hit4, false );

            stubs[stn].push_back( stub );  //  Add stub candidate to vector
          }
        }
      }
    }
    std::stable_sort(stubs[stn].begin(), stubs[stn].end(), SeedFunctor::increasingSX<const SeedStub*>() );  //  Add stub candidate to vector
  } // sort

  return StatusCode::SUCCESS;
}
