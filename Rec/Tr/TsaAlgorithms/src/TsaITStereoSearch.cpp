// $Id: TsaITStereoSearch.cpp,v 1.2 2007-01-16 08:06:37 mneedham Exp $

#include <algorithm>

// GaudiKernel
#include "GaudiKernel/ToolFactory.h"

#include "TsaITStereoSearch.h"
#include "SeedParabolaFit.h"
#include "TsaKernel/SeedTrack.h"
#include "SeedLineFit.h"
#include "TsaKernel/SeedFunctor.h"
#include "TsaKernel/SeedFun.h"


#include "TsaKernel/ClusFun.h"
#include "TsaKernel/TsaConstants.h"
#include "TsaKernel/IITDataSvc.h"
#include "TsaKernel/STCluster.h"

// BOOST !
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>
using namespace boost::assign;
using namespace boost;

DECLARE_TOOL_FACTORY( TsaITStereoSearch );

TsaITStereoSearch::TsaITStereoSearch(const std::string& type,
                               const std::string& name,
                               const IInterface* parent):
  TsaStereoBase(type, name, parent)
{
  declareProperty("itDataSvcType", m_dataSvcType = "ITDataSvc");
  declareProperty("itDataSvcName", m_dataSvcName = "ITDataSvc");
 
  declareProperty( "syCut" , m_syCut = 0.01);
  declareProperty( "win" , m_win = 4.0);
  declareProperty( "nWin" , m_nWin = 2);
  declareProperty( "yTol", m_yTol = 1.0);
  declareProperty( "nY", m_nY = 4);

  // constructer
  declareInterface<ITsaSeedStep>(this);

};

TsaITStereoSearch::~TsaITStereoSearch(){
  // destructer
}


StatusCode TsaITStereoSearch::initialize(){

  StatusCode sc = TsaStereoBase::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize",sc);
  }

  // data service tool !
  m_dataSvc = tool<IITDataSvc>(m_dataSvcType,m_dataSvcName);

  return StatusCode::SUCCESS;
}

StatusCode TsaITStereoSearch::execute(std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6]){

//-------------------------------------------------------------------------
//  Search for hits to form seed tracks in stereo stations
//-------------------------------------------------------------------------

  for (int i = 0; i < 6; ++i){
    hits[i].reserve(200);
  }
  loadData(hits);

  std::vector<SeedHit*> yHits[6];
  for (unsigned int ii = 0 ; ii < 6 ; ++ii) yHits[ii].reserve(8);
  std::vector<SeedHit*> select; select.reserve(12);

  for ( std::vector<SeedTrack*>::iterator it = seeds.begin(); seeds.end() != it; ++it ) {

    SeedTrack* seed = (*it);
    if (seed->live() == false) continue;

    // collect hits hit in y window
    for (unsigned int ii = 0 ; ii < 6 ; ++ii) yHits[ii].clear();
    if (collectHits(seed, hits, yHits) < m_nY) continue;

    int nBest = 0;
    int nSel = 0;
    SeedHit* iSel[30];
    
    for ( int lay1 = 0; lay1 < 2; ++lay1 ) {
      for ( int lay2 = 5; lay2 > 3; --lay2 ) {
        for ( std::vector<SeedHit*>::iterator it1 = yHits[lay1].begin(); yHits[lay1].end() != it1; ++it1 ) {
          SeedHit* hit1 = (*it1);
          double y1w = hit1->y();
          double z1 = hit1->z();
          for ( std::vector<SeedHit*>::iterator it2 = yHits[lay2].begin(); yHits[lay2].end() != it2; ++it2 ) {
            SeedHit* hit2 = (*it2);
            double sy = (hit2->y()-y1w) / (hit2->z()-z1);
            if ( fabs( sy - y1w/z1 ) > m_syCut ) continue;

            select.clear();
            for ( int lay = lay1; lay <= lay2; ++lay ) {
              for ( std::vector<SeedHit*>::iterator it = yHits[lay].begin(); yHits[lay].end() != it; ++it ) {
                SeedHit* hit = (*it);
                if ( lay == lay1 && hit == hit1 ) continue;
                if ( lay == lay2 && hit == hit2 ) continue;
                if ( fabs( hit->y()-y1w-sy*(hit->z() - z1) ) < m_win ) select.push_back( hit );
              }
            }
            if ( select.size() < (unsigned int)m_nWin ) continue;

            if ( select.size() > (unsigned int)nBest ) {
              nBest = select.size();
              iSel[0] = hit1;
              iSel[1] = hit2;
              nSel = 2;
              for ( std::vector<SeedHit*>::iterator it = select.begin(); select.end() != it; ++it ) {
                iSel[nSel] = *it;
                if ( nSel < 29 ) ++nSel;
              }
            }
	  } // yhits
	} // yhits
      } // lay
    } // lay
  
    if ( nSel >= m_nY) {
      for ( int i = 0; i < nSel; ++i ) {
        SeedPnt pnt(iSel[i]);
        seed->addToYPnts(pnt);
      } // i
      int rc = m_fitLine->fit( seed );
      if ( rc < 1 || seed->ny() < m_nY ) seed->setLive( false );
    } 
    else {
      seed->setLive( false );
    }  // nbest
  } // it
  return StatusCode::SUCCESS;
}

void TsaITStereoSearch::loadData(std::vector<SeedHit*> hits[6]) const{

  for ( int station = 1; station < 4; ++station ) {
    for ( int layer = 1; layer < 3; ++layer ) {
      int lay = 2*(station-1);
      if ( layer > 1 ) ++lay;
      if ( sector() < 3 ) {
        for ( int ib = 0; ib < 2; ++ib ) {
          if ( sector() > 0 && ib > 0 ) continue;
          int box = 2 + sector() - ib;
          Tsa::STRange iRange =  m_dataSvc->partition(station,layer+1,box);
          for (Tsa::STClusters::const_iterator itIter = iRange.begin(); itIter != iRange.end(); ++itIter){
            if ((*itIter)->isHot() == false) {
              SeedHit* hit = new SeedHit(*itIter); 
              hits[lay].push_back( hit );
	    }
          }
        }
      }
      std::sort( hits[lay].begin(), hits[lay].end(), SeedFunctor::increasingX<const SeedHit*>() );
    }
  }
}

