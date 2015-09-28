// GaudiKernel
#include "GaudiKernel/SystemOfUnits.h"

#ifdef __INTEL_COMPILER         // Disable ICC warning
  #pragma warning(disable:654)  // Tf::Tsa::ITsaSeedStep::execute only partially overridden
  #pragma warning(push)
#endif
#include "TsaOTXSearch.h"
#ifdef __INTEL_COMPILER         // Re-enable ICC warning 654
  #pragma warning(pop)
#endif

// BOOST !
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>

#include "SeedParabolaFit.h"

using namespace boost::assign;
using namespace boost;

// macro for member function calls
#define CALL_MEMBER_FN(object,ptrToMember)  (object.*(ptrToMember))

using namespace Tf::Tsa;

DECLARE_TOOL_FACTORY( OTXSearch )

OTXSearch::OTXSearch(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  XSearchBase(type, name, parent)
{
  declareProperty("otDataSvcType", m_dataSvcType = "Tf::Tsa::TStationHitManager");
  declareProperty("otDataSvcName", m_dataSvcName = "TsaDataManager");

  declareProperty("sxCut", m_sxCut = 0.85);
  declareProperty("xsCut", m_xsCut = 0.7);
  declareProperty("x0Cut", m_x0Cut = 4500.);
  declareProperty("sx2Cut",m_sx2Cut = 0.8);
  declareProperty("dthCut", m_dthCut = 0.70);
  declareProperty("tdCut",m_tdCut = 0.75);
  declareProperty("win", m_win = 10.0);
  declareProperty("win2",m_win2 = 6.0);
  declareProperty("nWin",m_nWin = 5);
  declareProperty("nWin2",m_nWin2 = 7);
  declareProperty("nxCut",m_nxCut = 6);
  declareProperty("nT2",m_nT2Conf = 1);

  declareProperty("maxDriftRadius", m_maxDriftRadius = 2.9*Gaudi::Units::mm);
  declareProperty("deltaX", m_deltaX = 0.75*Gaudi::Units::mm);
  declareProperty("collectPolicy", m_collectPolicy = "Curved");

  declareProperty("OnlyUnusedHits", m_onlyUnusedHits = false);

}

OTXSearch::~OTXSearch(){
  // destructer
}


StatusCode OTXSearch::initialize()
{
  const StatusCode sc = XSearchBase::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize",sc);
  }

  m_hitMan = tool<OTHitMan>(m_dataSvcType,m_dataSvcName);

  return sc;
}

StatusCode OTXSearch::execute(std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6]){

  //-------------------------------------------------------------------------
  //  Search for hits to form seed tracks in X stations
  //-------------------------------------------------------------------------


  for (int i = 0; i < 6; ++i){
    hits[i].reserve(1000);
  }
  loadData(hits);

  std::vector<SeedHit*> select[3];
  for (unsigned int ii = 0; ii < 3; ++ii) select[ii].reserve(12);

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
          if ( sx < m_sxMin ) continue;
          if ( first ) {
            first = false;
            start = it2;
          }
          if ( sx > m_sxMax ) break;

          std::vector<SeedHit*>& skip = hit1->skip();
          std::vector<SeedHit*>::iterator itSkip = std::find(skip.begin(),skip.end(),hit2);
          if (itSkip != skip.end()) continue;

          if ( fabs(x1*m_xsParam - sx) > m_xsCut) continue;
          const double dth = atan( sx ) - atan( (x1 - (z1-TsaConstants::zMagnet)*sx)*TsaConstants::oneOverZMagnet );
          if ( fabs( dth ) > m_dthCut ) continue;

          for (unsigned int j = 0; j < 3; ++j) select[j].clear();

          // collect hits in station 3
          for ( int lay = 2; lay < 4; ++lay ) {
            CALL_MEMBER_FN(*this,m_collectFun)(hits[lay],x1,z1,sx, m_win2,select[1]);
          }

          if (select[1].size() <= (unsigned int) m_nT2Conf) continue;

          if (lay1 == 0){
            CALL_MEMBER_FN(*this,m_collectFun)(hits[lay1+1],x1,z1,sx, m_win2,select[0]);
            std::for_each(select[0].begin(), select[0].end(), std::bind2nd(std::mem_fun(&SeedHit::setUse),false) );
          }

          if (lay2 == 5){
            CALL_MEMBER_FN(*this,m_collectFun)(hits[lay2-1],x1,z1,sx,m_win2,select[2]);
            std::for_each(select[2].begin(), select[2].end(), std::bind2nd(std::mem_fun(&SeedHit::setUse),false) );
          }

          collectXHits(hits[lay1], it1, x1, z1, m_win, sx, select[0]);
          collectXHits(hits[lay2], it2, x2, z1, m_win, sx ,select[2]);

          if (select[0].size()+select[1].size()+select[2].size() < (unsigned int)m_nWin ) continue;

          // flag t3 hits as not used
          // std::for_each(select[1].begin(), select[1].end(), std::bind2nd(std::mem_fun(&SeedHit::setUse),false) );
          std::for_each(select[1].begin(), select[1].end(), bind(&SeedHit::setUse,_1,false) );


          const double dz = z2-z1;
          double csth = sqrt(1.0 + sx*sx);

          int nhSel = 0;
          SeedHit* hSel = 0;
          int sSel[3] = {0, 0, 0};
          //          SeedHit* iSel[100];
          for ( std::vector<SeedHit*>::iterator it3 = select[1].begin(); select[1].end() != it3; ++it3 ) {
            SeedHit* hit3 = (*it3);
            if ( hit3->use1() && hit3->use2() ) continue;
            double x3 = hit3->x();
            double z3 = hit3->z();
            double ax = ( sx-(x3-x1)/(z3-z1) )/(z2-z3);
            double bx = sx - ax*dz;
            int nhMax[3] = {0, 0, 0};
            int sMax[3] = {0, 0, 0};
            //            SeedHit* ih[3][100];
            for ( int stn = 0; stn < 3; ++stn ) {
              SeedHit* hStn = hit1;
              if ( stn == 1 ) hStn = hit3;
              if ( stn == 2 ) hStn = hit2;
              int nMax = 0;
              for ( int is = 0; is < 2; ++is ) {
                if ( stn == 1 && is == 0 && hit3->use1() ) continue;
                if ( stn == 1 && is == 1 && hit3->use2() ) continue;
                int nh = 0;
                double cx =  x1 + (2*is-1)*hStn->r()*csth;
                for ( std::vector<SeedHit*>::iterator it = select[stn].begin(); select[stn].end() != it; ++it ) {
                  SeedHit* hit = (*it);
                  if ( hit == hit3 ) continue;
                  const double dz = hit->z() - z1;
                  const double xSeed = dz*(ax*dz + bx) + cx;
                  const double rcsth = hit->r()*csth ;
                  double dx1 = hit->x() - rcsth - xSeed;
                  double dx2 = dx1 + 2*rcsth;
                  //   double dx1 = hit->x() - hit->r()*csth - xSeed;
                  //double dx2 = dx1 + 2*hit->r()*csth;
                  if ( fabs(dx1) < m_deltaX || fabs(dx2) < m_deltaX ) {
                    ++nh;
                    if ( fabs(dx1) < m_deltaX ) hit->setUse1( true );
                    if ( fabs(dx2) < m_deltaX ) hit->setUse2( true );
                  }
                }
                if ( nh > nMax ) {
                  nMax = nh;
                  nhMax[stn] = nh;
                  sMax[stn] = 2*is-1;
                }  // if
              } // is
            } // loop stn

            if ( nhMax[0]+nhMax[1]+nhMax[2] > nhSel ) {
              nhSel = nhMax[0]+nhMax[1]+nhMax[2];
              sSel[0] = sMax[0];
              sSel[1] = sMax[1];
              sSel[2] = sMax[2];
              hSel = hit3;

              //              int nh = 0;
              //              for ( int stn = 0; stn < 3; ++stn ) {
              //                for ( int i = 0; i < nhMax[stn]; ++i ) {
              //                  iSel[nh] = ih[stn][i];
              //                  ++nh;
              //                } // i
              //              } //stn
            } // if
          } // select
          if ( nhSel < 1 ) continue;

          std::vector<SeedPnt> xPnts; xPnts.reserve(24);
          xPnts +=  SeedPnt(hit1, sSel[0]),  SeedPnt( hit2, sSel[2] ), SeedPnt( hSel, sSel[1] );


          x1 += sSel[0]*hit1->r()*csth;
          x2 += sSel[2]*hit2->r()*csth;
          double x3 = hSel->x() + sSel[1]*hSel->r()*csth;
          double z3 = hSel->z();
          const double dxdz2 = (x2-x1)/dz;
          double ax = (dxdz2-(x3-x1)/(z3-z1) )/(z2-z3);
          double bx = dxdz2 - ax*dz;
          for ( int stn = 0; stn < 3; ++stn ) {
            for ( std::vector<SeedHit*>::iterator it = select[stn].begin(); select[stn].end() != it; ++it ) {
              SeedHit* hit = (*it);
              if ( hit == hSel ) continue;

              const double dz = hit->z()-z1;
              double xSeed = dz*(ax*dz + bx) + x1;
              double dx1 = hit->x() + hit->r()*csth - xSeed;
              double dx2 = dx1 - 2*hit->r()*csth;
              if ( fabs(dx1) < m_deltaX || fabs(dx2) < m_deltaX ) {
                hit->setUse1( true );
                if ( fabs(dx1) < fabs(dx2) ) {
                  SeedPnt pnt( hit, 1 );
                  xPnts.push_back(pnt);
                } else {
                  SeedPnt pnt(hit, -1 );
                  xPnts.push_back( pnt );
                } // if
              } // if
            }
          } // stn
          if ( xPnts.size() < (unsigned int)m_nWin2 ) continue;

          SeedTrack* seed = new SeedTrack(xPnts,sector(), dth);

          int rc = m_parabolaFit->fit( seed, csth );
          if (rc < 1 || goodTrack(seed) == false){
            delete seed;
          }
          else {
            seeds.push_back( seed );
            tagUsedHits(hit1, hit2, select);
          }
        } //hits
      } //hits
    } // lay
  } // lay

  // cleanup(hits);
  return StatusCode::SUCCESS;
}

void OTXSearch::loadData(std::vector<SeedHit*> hits[6]) const
{
  typedef OTHitMan::HitRange Range;
  typedef OTHitMan::Hits     Hits;

  //  Load hits with data from Tsa clusters
  for ( int station = 0; station < 3; ++station ) {
    for ( int layer = 0; layer < 4; layer+=3 ) {
      int lay = 2*station;
      if ( layer > 0 ) ++lay;
      const TfTsHitNumMap::TfRegions & tfRegions = m_hitNumMap.tfOTRegions(sector()-2);
      int nHits(0); // debug only
 
      for ( TfTsHitNumMap::TfRegions::const_iterator iTfR = tfRegions.begin();
            iTfR != tfRegions.end(); ++iTfR )
      {
        //debug() << " -> Tf : station=" << station << " layer=" << layer << " region=" << *iTfR << endmsg;
        Range oRange = m_hitMan->hits(station,layer,*iTfR);
        nHits += oRange.size();
        for (Hits::const_iterator otIter = oRange.begin(); otIter != oRange.end(); ++otIter)
        {
 
          // no cleaning as yet
          //if ( clus->isHot() ) { debug() << "   -> IsHot -> Rejecting" << endmsg; continue; }
          if ( (*otIter)->driftRadius() >= m_maxDriftRadius ) continue; 
          
	  if ((*otIter)->hit()->ignore()) continue;
	  if (m_onlyUnusedHits && (*otIter)->hit()->testStatus(Tf::HitBase::UsedByPatForward)) continue;
          SeedHit* hit = new SeedHit(*otIter);
          hits[lay].push_back( hit );
        }
      }

      std::stable_sort( hits[lay].begin(), hits[lay].end(), SeedFunctor::increasingX<const SeedHit*>() );
    } // layer
  } //stn
}

#undef CALL_MEMBER_FN

// Not sure why this helps here, but it does suppress the warning!
#ifdef __INTEL_COMPILER         // Disable ICC warning
  #pragma warning(disable:279)  // BOOST_ASSERT controlling expression is constant
#endif

