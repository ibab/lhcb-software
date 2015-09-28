#include <algorithm>

#ifdef __INTEL_COMPILER         // Disable ICC warning
  #pragma warning(disable:654)  // Tf::Tsa::ITsaSeedStep::execute only partially overridden
  #pragma warning(push)
#endif
#include "TsaITStereoSearch.h"
#ifdef __INTEL_COMPILER         // Re-enable ICC warning 654
  #pragma warning(pop)
#endif
#include "SeedLineFit.h"
#include "TsaKernel/SeedFunctor.h"
#include "TsaKernel/SeedFun.h"

// BOOST !
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>
using namespace boost::assign;
using namespace boost;

using namespace Tf::Tsa;

DECLARE_TOOL_FACTORY( ITStereoSearch )

ITStereoSearch::ITStereoSearch(const std::string& type,
                               const std::string& name,
                               const IInterface* parent):
  StereoBase(type, name, parent)
{
  declareProperty("itDataSvcType", m_dataSvcType = "Tf::Tsa::TStationHitManager");
  declareProperty("itDataSvcName", m_dataSvcName = "TsaDataManager");

  declareProperty( "syCut" , m_syCut = 0.007);
  declareProperty( "win" , m_win = 4.0);
  declareProperty( "nWin" , m_nWin = 2);
  declareProperty( "yTol", m_yTol = 0.9);
  declareProperty( "nY", m_nY = 4);

  declareProperty("OnlyUnusedHits", m_onlyUnusedHits = false);

  // constructer
  declareInterface<ITsaSeedStep>(this);

}

ITStereoSearch::~ITStereoSearch(){
  // destructer
}

StatusCode ITStereoSearch::initialize()
{
  const StatusCode sc = StereoBase::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize",sc);
  }

  m_hitMan = tool<ITHitMan>(m_dataSvcType,m_dataSvcName);

  return sc;
}

StatusCode ITStereoSearch::execute(std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6]){

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
    if (this->collectHits(seed, hits, yHits) < m_nY) continue;

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

            //check if hits are within prediction
            if ( sy > m_syMax ) continue;
            if ( sy < m_syMin ) continue;
            
            select.clear();
            for ( int lay = lay1; lay <= lay2; ++lay ) {
              for ( std::vector<SeedHit*>::iterator itY = yHits[lay].begin(); yHits[lay].end() != itY; ++itY ) {
                SeedHit* hit = (*itY);
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
              for ( std::vector<SeedHit*>::iterator itSel = select.begin(); select.end() != itSel; ++itSel ) {
                iSel[nSel] = *itSel;
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

void ITStereoSearch::loadData(std::vector<SeedHit*> hits[6]) const
{
  typedef ITHitMan::HitRange Range;
  typedef ITHitMan::Hits     Hits;
  for ( int station = 0; station < 3; ++station ) {
    for ( int layer = 1; layer < 3; ++layer ) {
      int lay = 2*station;
      if ( layer > 1 ) ++lay;
      if ( sector() < 3 ) {
        for ( int ib = 0; ib < 2; ++ib ) {
          if ( sector() > 0 && ib > 0 ) continue;
          int box = 2 + sector() - ib;
          // CRJ : Convert Tsa region into a Tf regions
          const TfTsHitNumMap::TfRegions & tfRegions = m_hitNumMap.tfITRegions(box);
          int nHits(0); // debug only
	  //          debug() << "IT hits for Tsa : station=" << station+1 << " layer=" << layer << " region=" << box << endmsg;
          for ( TfTsHitNumMap::TfRegions::const_iterator iTfR = tfRegions.begin();
                iTfR != tfRegions.end(); ++iTfR )
          {
            //debug() << " -> Tf : station=" << station << " layer=" << layer << " region=" << *iTfR << endmsg;
            Range iRange = m_hitMan->hits(station,layer,*iTfR);
            nHits += iRange.size();
            for ( Hits::const_iterator itIter = iRange.begin(); itIter != iRange.end(); ++itIter)
            {
	      //              debug() << "  -> " << (*itIter)->hit()->lhcbID() << endmsg;
              //if ((*itIter)->isHot() == false) { // CRJ : Need to decide what to do about cleaning
	      if ((*itIter)->hit()->ignore()) continue;
	      if (m_onlyUnusedHits && (*itIter)->hit()->testStatus(Tf::HitBase::UsedByPatForward)) continue;
              SeedHit* hit = new SeedHit(*itIter);
              hits[lay].push_back( hit );
              //}
            }
          }
	  //          debug() << " -> Found " << nHits << " hits" << endmsg;
        }
      }
      std::stable_sort( hits[lay].begin(), hits[lay].end(), SeedFunctor::increasingX<const SeedHit*>() );
    } // layer
  }
}

// Not sure why this helps here, but it does suppress the warning!
#ifdef __INTEL_COMPILER         // Disable ICC warning
  #pragma warning(disable:279)  // BOOST_ASSERT controlling expression is constant
#endif
