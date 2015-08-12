#include <algorithm>

#ifdef __INTEL_COMPILER         // Disable ICC warning
  #pragma warning(disable:654)  // Tf::Tsa::ITsaSeedStep::execute only partially overridden
  #pragma warning(push)
#endif
#include "TsaOTStereoSearch.h"
#ifdef __INTEL_COMPILER         // Re-enable ICC warning 654
  #pragma warning(pop)
#endif

// BOOST !
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>

#include "SeedLineFit.h"

using namespace boost::assign;
using namespace boost;

using namespace Tf::Tsa;

DECLARE_TOOL_FACTORY( OTStereoSearch )

OTStereoSearch::OTStereoSearch(const std::string& type,
                               const std::string& name,
                               const IInterface* parent):
  StereoBase(type, name, parent),
  m_hitMan(NULL)
{
  declareProperty("otDataSvcType", m_dataSvcType = "Tf::Tsa::TStationHitManager");
  declareProperty("otDataSvcName", m_dataSvcName = "TsaDataManager");

  declareProperty( "syCut" , m_syCut = 0.07);
  declareProperty( "win" , m_win = 100.0);
  declareProperty( "nWin" , m_nWin = 6);
  declareProperty( "yTol", m_yTol = 20.0);
  declareProperty( "yTol2", m_yTol2 = 8.3);
  declareProperty("nHit", m_nHit = 15);
  declareProperty( "nY", m_nY = 4);
  declareProperty("maxDriftRadius", m_maxDriftRadius = 2.9);
  declareProperty("OnlyUnusedHits", m_onlyUnusedHits = false);

  m_scth = 1.0/TsaConstants::sth;

  // constructer
  declareInterface<ITsaSeedStep>(this);
}

OTStereoSearch::~OTStereoSearch(){
  // destructer
}


StatusCode OTStereoSearch::initialize()
{

  const StatusCode sc = StereoBase::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize",sc);
  }

  m_hitMan = tool<OTHitMan>(m_dataSvcType,m_dataSvcName);

  return sc;
}

StatusCode OTStereoSearch::execute(std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6]){

  //-------------------------------------------------------------------------
  //  Search for hits to form seed tracks in stereo stations
  //-------------------------------------------------------------------------


  for (int i = 0; i < 6; ++i){
    hits[i].reserve(1000);
  }
  loadData(hits);


  // pre-allocate vectors
  std::vector<SeedHit*> yHits[6];
  for (unsigned int ii = 0 ; ii < 6 ; ++ii) yHits[ii].reserve(8);
  std::vector<SeedHit*> select; select.reserve(24);

  for ( std::vector<SeedTrack*>::iterator it = seeds.begin(); seeds.end() != it; ++it ) {

    SeedTrack* seed = (*it);
    if (seed->live() == false) continue;

    // collect hits hit in y window
    for (unsigned int ii = 0 ; ii < 6 ; ++ii) yHits[ii].clear();
    if (collectHits(seed, hits, yHits) < m_nY) continue;

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
            if ( fabs( sy - y1w/z1 ) > m_syCut ) continue;
            
            //check if hits are within prediction
            if ( sy > m_syMax ) continue;
            if ( sy < m_syMin ) continue;

            // zero the tmp vector
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

            int nMax = 0;
            int s1Max = 0;
            int s2Max = 0;
            for ( int s1 = 0; s1 < 2; ++s1 ) {
              double y1 = hit1->y() + (2*s1-1)*hit1->r()*m_scth;
              for ( int s2 = 0; s2 < 2; ++s2 ) {
                const double y2 = hit2->y() + (2*s2-1)*hit2->r()*m_scth;
                const double sy = ( y2 - y1 ) / ( hit2->z() - z1 );
                int nh = 0;
                for ( std::vector<SeedHit*>::iterator it = select.begin(); select.end() != it; ++it ) {
                  SeedHit* hit = (*it);
                  double dy1 = hit->y() - hit->r()*m_scth - y1 - sy*( hit->z() - z1 );
                  double dy2 = dy1 + 2*hit->r()*m_scth;
                  if ( fabs(dy1) < m_yTol2 || fabs(dy2) < m_yTol2 ) ++nh;
                }
                if ( nh > nMax ) {
                  nMax = nh;
                  s1Max = s1;
                  s2Max = s2;
                } // of
              } // s2
            } // s1

            if ( nMax > nBest ) {
              nBest = nMax;
              iSel[0] = hit1;
              iSel[1] = hit2;
              sSel[0] = 2*s1Max-1;
              sSel[1] = 2*s2Max-1;
              nSel = 2;
              double y1 = hit1->y() + (2*s1Max-1)*hit1->r()*m_scth;
              double y2 = hit2->y() + (2*s2Max-1)*hit2->r()*m_scth;
              sy = ( y2 - y1 ) / ( hit2->z() - z1 );
              for ( std::vector<SeedHit*>::iterator it = select.begin(); select.end() != it; ++it ) {
                SeedHit* hit = (*it);
                double dy1 = hit->y() - hit->r()*m_scth - y1 - sy*( hit->z() - z1 );
                double dy2 = dy1 + 2*hit->r()*m_scth;
                if ( fabs(dy1) < m_yTol2 || fabs(dy2) < m_yTol2 ) {
                  iSel[nSel] = hit;
                  if ( fabs(dy1) < fabs(dy2) ) {
                    sSel[nSel] = -1;
                  } else {
                    sSel[nSel] = 1;
                  }
                  if ( nSel < 29 ) ++nSel;
                }
              } // for it
            } // best
          } //yHits
        } // yHits
      } // lay
    } // lay

    if ( nSel >= m_nY) {
      for ( int i = 0; i < nSel; ++i ) {
        SeedPnt pnt(iSel[i], sSel[i]);
        seed->addToYPnts(pnt);
      } // i
      int rc = m_fitLine->fit( seed );
      if ( seed->ny() < m_nY || rc < 1 || seed->nx()+seed->ny() < m_nHit  ) seed->setLive( false );
    }
    else {
      seed->setLive( false );
    }  // nbest
  } // it
  return StatusCode::SUCCESS;
}

void OTStereoSearch::loadData(std::vector<SeedHit*> hits[6]) const
{
  typedef OTHitMan::HitRange Range;
  typedef OTHitMan::Hits     Hits;
  for ( int station = 0; station < 3; ++station ) {
    for ( int layer = 1; layer < 3; ++layer ) {
      int lay = 2*station;
      if ( layer > 1 ) ++lay;
      const TfTsHitNumMap::TfRegions & tfRegions = m_hitNumMap.tfOTRegions(sector()-2);
      int nHits(0); // debug only
 
      for ( TfTsHitNumMap::TfRegions::const_iterator iTfR = tfRegions.begin();
            iTfR != tfRegions.end(); ++iTfR )
      {
        //debug() << " -> Tf : station=" << station << " layer=" << layer << " region=" << *iTfR << endreq;
        Range oRange = m_hitMan->hits(station,layer,*iTfR);
        nHits += oRange.size();
        for (Hits::const_iterator otIter = oRange.begin(); otIter != oRange.end(); ++otIter)
        {
 
          // no hit cleaning as yet
          //if ( clus->isHot() ) { debug() << "   -> IsHot -> Rejecting" << endreq; continue; }
          if ( (*otIter)->driftRadius() >= m_maxDriftRadius ) continue; 
          
	  if ((*otIter)->hit()->ignore()) continue;
	  if (m_onlyUnusedHits && (*otIter)->hit()->testStatus(Tf::HitBase::UsedByPatForward)) continue;
          SeedHit* hit = new SeedHit(*otIter);
          hits[lay].push_back( hit );
        }
      }

      std::stable_sort( hits[lay].begin(), hits[lay].end(), SeedFunctor::increasingX<const SeedHit*>() );
    } // layer
  } // station

}

// Not sure why this helps here, but it does suppress the warning!
#ifdef __INTEL_COMPILER         // Disable ICC warning
  #pragma warning(disable:279)  // BOOST_ASSERT controlling expression is constant
#endif
