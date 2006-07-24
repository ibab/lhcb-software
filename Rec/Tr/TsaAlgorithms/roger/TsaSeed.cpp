// $:  $
     
// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/Tuples.h"

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
#include "TsaKernel/ILHCbIDsToMCParticles.h"
#include "TsaKernel/IOTExpectedHits.h"
#include "TsaKernel/Types.h"
#include "TsaKernel/ClusFun.h"

#include "Kernel/Plane3DTypes.h"

#include "Event/MCParticle.h"
#include "Event/MCTrackInfo.h"
#include "gsl/gsl_cdf.h"

// factory defs
static const AlgFactory<TsaSeed> s_factory;
const IAlgFactory& TsaSeedFactory = s_factory;

TsaSeed::TsaSeed(const std::string& name,
                                     ISvcLocator* pSvcLocator):
  GaudiTupleAlg(name, pSvcLocator)
{
  
  // TsaInitialization constructor
  declareProperty("otDataSvc", m_otDataSvcName = "OTDataSvc");
  declareProperty("itDataSvc", m_itDataSvcName = "ITDataSvc");
  declareProperty("LikCut", m_likCut = 999.);
}


TsaSeed::~TsaSeed()
{
  // destructer
}

StatusCode TsaSeed::initialize()
{
  // Initialization
  
  StatusCode sc = GaudiTupleAlg::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize");
  }
  m_itDataSvc = tool<IITDataSvc>(m_itDataSvcName);
  m_otDataSvc = tool<IOTDataSvc>(m_otDataSvcName);
   
  m_linkTool = tool<ILHCbIDsToMCParticles>("LHCbIDsToMCParticles");
  m_expectedHits = tool<IOTExpectedHits>("OTExpectedHits");
  m_ev = 0;
  
  return StatusCode::SUCCESS;
}

StatusCode TsaSeed::execute(){
//-------------------------------------------------------------------------
//  Track seeding in the OT
//-------------------------------------------------------------------------

  ++m_ev;  // Event count
  
  // Loop over OT sectors
  for ( int sector = 1; sector < 3; ++sector ) {
    std::vector<SeedHit*> hits[6], hit2[6];
    std::vector<SeedTrack*> seeds;
    aWatch.start();

    search( sector, hits, seeds );
    searchStereo( sector, hit2, seeds );
    likelihood( seeds );
    select( seeds, hits, hit2 );
  
    aWatch.stop();
    double time = aWatch.lasttime();  // Time taken in seeding algorithm

    monitor( sector, seeds, time, hits, hit2 ); 

    // Delete the temporary objects that have been created
    for ( std::vector<SeedTrack*>::iterator it = seeds.begin(); seeds.end() != it; ++it ) {
      SeedTrack* seed = (*it);
      delete seed;
    }
    for ( int lay = 0; lay < 6; ++lay ) {
      for ( std::vector<SeedHit*>::iterator it = hits[lay].begin(); hits[lay].end() != it; ++it ) {
        SeedHit* hit = (*it);
        delete hit;
      }
      for ( std::vector<SeedHit*>::iterator it = hit2[lay].begin(); hit2[lay].end() != it; ++it ) {
        SeedHit* hit = (*it);
        delete hit;
      }
    }
  }

  /*
  plot(seeds.size(), "seeds", 0., 10000., 100);
  SeedTracks* seedsCont = new SeedTracks();
  seedsCont->reserve(seeds.size());
  std::vector<SeedTrack*>::iterator it = seeds.begin(); 
  while (it != seeds.end()){
    SeedTrack* seed = (*it);
    plot(seed->lik(),"lik", -500., 500., 100);
    if (seed->lik() < m_likCut){
      seedsCont->insert(seed);
    }
    else {
      delete seed;
    }
    ++it;
  } // while
  put(seedsCont,SeedTrackLocation::Default);
  */

  return StatusCode::SUCCESS;
}

void TsaSeed::select( std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[], std::vector<SeedHit*> hit2[] ) {
//-------------------------------------------------------------------------
//  Select seeds
//-------------------------------------------------------------------------

  if ( seeds.size() < 1 ) return;
  for ( std::vector<SeedTrack*>::iterator it = seeds.begin(); seeds.end() != it; ++it ) {
    SeedTrack* seed = *it;
    if ( seed->live() == 0 ) continue;
    for ( std::vector<SeedPnt>::iterator it = seed->xPnts().begin(); seed->xPnts().end() != it; ++it ) {
      SeedPnt & pnt = *it;
      if ( pnt.skip() ) continue;
      pnt.hit()->seeds().push_back( seed );
    }
    for ( std::vector<SeedPnt>::iterator it = seed->yPnts().begin(); seed->yPnts().end() != it; ++it ) {
      SeedPnt & pnt = *it;
      if ( pnt.skip() ) continue;
      pnt.hit()->seeds().push_back( seed );
    }
  }
  for ( int lay = 0; lay < 6; ++lay ) {
    for ( std::vector<SeedHit*>::iterator it = hits[lay].begin(); hits[lay].end() != it; ++it ) {
      SeedHit* hit = (*it);
      if ( hit->seeds().size() < 2 ) continue;
      std::vector<SeedTrack*> share = hit->seeds();
      for ( std::vector<SeedTrack*>::iterator it1 = share.begin(); share.end()-1 != it1; ++it1 ) {
        SeedTrack* seed1 = *it1;
        if ( seed1->live() == 0 ) continue;
        for ( std::vector<SeedTrack*>::iterator it2 = it1+1; share.end() != it2; ++it2 ) {
          SeedTrack* seed2 = *it2;
          if ( seed2->live() == 0 ) continue;
          seed1->links().push_back( seed2 );
          seed2->links().push_back( seed1 );
        }
      }
    }
    for ( std::vector<SeedHit*>::iterator it = hit2[lay].begin(); hit2[lay].end() != it; ++it ) {
      SeedHit* hit = (*it);
      if ( hit->seeds().size() < 2 ) continue;
      std::vector<SeedTrack*> share = hit->seeds();
      for ( std::vector<SeedTrack*>::iterator it1 = share.begin(); share.end()-1 != it1; ++it1 ) {
        SeedTrack* seed1 = *it1;
        if ( seed1->live() == 0 ) continue;
        for ( std::vector<SeedTrack*>::iterator it2 = it1+1; share.end() != it2; ++it2 ) {
          SeedTrack* seed2 = *it2;
          if ( seed2->live() == 0 ) continue;
          seed1->yLinks().push_back( seed2 );
          seed2->yLinks().push_back( seed1 );
        }
      }
    }
  }
  for ( std::vector<SeedTrack*>::iterator it = seeds.begin(); seeds.end() != it; ++it ) {
    SeedTrack* seed = *it;
    if ( seed->live() == 0 ) continue;
    std::vector<SeedTrack*> clones;
    if ( seed->links().size() > 3 ) {
      int nLink = 0;
      SeedTrack* iLink[500];
      int nShare[500];
      for ( std::vector<SeedTrack*>::iterator it2 = seed->links().begin(); seed->links().end() != it2; ++it2 ) {
        SeedTrack* seed2 = *it2;
        bool found = false;
        if ( nLink > 0 ) {
          for ( int i = 0; i < nLink; ++i ) {
            if ( seed2 == iLink[i] ) {
              nShare[i]++;
              found = true;
              break;
            }
          }
        }
        if ( found == false ) {
          iLink[nLink] = seed2;
          nShare[nLink] = 1;
          if ( nLink < 499 ) nLink++;
        }
      }
      if ( nLink > 0 ) {
        for ( int i = 0; i < nLink; ++i ) {
          if ( nShare[i] > 3 ) clones.push_back( iLink[i] );
        }
      }
    }
    if ( seed->yLinks().size() > 3 ) {
      int nLink = 0;
      SeedTrack* iLink[500];
      int nShare[500];
      for ( std::vector<SeedTrack*>::iterator it2 = seed->yLinks().begin(); seed->yLinks().end() != it2; ++it2 ) {
        SeedTrack* seed2 = *it2;
        bool found = false;
        if ( nLink > 0 ) {
          for ( int i = 0; i < nLink; ++i ) {
            if ( seed2 == iLink[i] ) {
              nShare[i]++;
              found = true;
              break;
            }
          }
        }
        if ( found == false ) {
          iLink[nLink] = seed2;
          nShare[nLink] = 1;
          if ( nLink < 499 ) nLink++;
        }
      }
      if ( nLink > 0 ) {
        for ( int i = 0; i < nLink; ++i ) {
          if ( nShare[i] > 3 ) clones.push_back( iLink[i] );
        }
      }
    }
    seed->setLinks( clones );
  }

  // Sort seeds according to their likelihood
  std::sort( seeds.begin(), seeds.end(), TsaSeed::decreasingLikelihood() );

  for ( std::vector<SeedTrack*>::iterator it = seeds.begin(); seeds.end() != it; ++it ) {
    SeedTrack* seed = *it;
    if ( seed->live() ) {    // Check seed is still available
      seed->setSelect( 1 );  // Mark it as selected
      seed->setLive( 0 );
      if ( seed->links().size() < 1 ) continue;  // Linked candidates (that share hits) 
      for ( std::vector<SeedTrack*>::iterator it2 = seed->links().begin(); seed->links().end() != it2; ++it2 ) {
        (*it2)->setLive( 0 );  // Kill candidates that share hits with selected one
      }
    }
  }
};
  
bool TsaSeed::decreasingLikelihood::operator() ( SeedTrack* first, SeedTrack* second ) const {
//-------------------------------------------------------------------------
//  Define the sort sequence for seed candidates
//-------------------------------------------------------------------------
  if ( 0 == second ) return true;
  if ( 0 == first )  return false;
  return first->lik() > second->lik();
}

void TsaSeed::likelihood( std::vector<SeedTrack*>& seeds ) {
//-------------------------------------------------------------------------
//  Determine likelihood of seed candidates
//-------------------------------------------------------------------------

  double z0 = 0.;
  for ( std::vector<SeedTrack*>::iterator it = seeds.begin(); seeds.end() != it; ++it ) {
    SeedTrack* seed = *it;
    if ( seed->live() == 0 ) continue;

    std::vector<SeedPnt> pnts = seed->xPnts();
    for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
      SeedPnt & pnt = *it;
      double dy = seed->y0() + seed->sy()*pnt.hit()->z() - pnt.hit()->clus()->yMid();
      double z = pnt.hit()->z() + .0036*dy;
      pnt.setZ( z );
    }
    seed->setXPnts( pnts );
    double sx = seed->sx() + 2.*seed->tx()*8600.;
    double cth = 1. / sqrt( 1. + sx*sx );
    int rc = refitParabola( seed, cth );
    if ( rc < 1 ) {
      seed->setLive( 0 );
      continue;
    }
    int nxExp = 0;
    int nyExp = 0;
    int nxFound = 0;
    int nyFound = 0;
    double xLik = 0.;
    double yLik = 0.;
    double y0 = seed->y0();
    double z1 = z0 + 1000.;
    double y1 = y0 + seed->sy()*(z1-z0);
    
    // Fill seed track parameters into format required by tool to determine expected hits
    Tsa::Line aLine( y0, y1, z0, z1 );
    Tsa::Parabola aParab( seed->tx(), seed->sx(), seed->x0() ); 
    for ( int station = 1; station < 4; ++station ) {
      for ( int layer = 0; layer < 4; ++layer ) {
        LHCb::OTChannelID testChan(station,layer,0,0,0);
        typedef std::vector<IOTExpectedHits::OTPair> OTPairs;
        OTPairs output;
        m_expectedHits->collect(aParab,aLine,testChan,output);   
        OTPairs::iterator iter = output.begin();
        for (;iter != output.end(); ++iter ){
          LHCb::OTChannelID chan = iter->first; // channel that is hit
          double dist = iter->second;     // signed distance to wire
          bool found = 0;

          // X projection
          if ( layer == 0 || layer == 3 ) {
            for ( std::vector<SeedPnt>::iterator it = seed->xPnts().begin(); seed->xPnts().end() != it; ++it ) {
              SeedPnt & pnt = *it;
              LHCb::OTChannelID hitChan = pnt.hit()->channel();
              LHCb::OTChannelID tChan = LHCb::OTChannelID(hitChan.station(),hitChan.layer(), hitChan.quarter(),
                                                          hitChan.module(), hitChan.straw());              
              if ( tChan == chan ) {
                found = 1;
                break;
              }
            }
          } else {
            for ( std::vector<SeedPnt>::iterator it = seed->yPnts().begin(); seed->yPnts().end() != it; ++it ) {
              SeedPnt & pnt = *it;
              LHCb::OTChannelID hitChan = pnt.hit()->channel();
              LHCb::OTChannelID tChan = LHCb::OTChannelID(hitChan.station(),hitChan.layer(), hitChan.quarter(),
                                                          hitChan.module(), hitChan.straw());              
              if ( tChan == chan ) {
                found = 1;
                break;
              }
            }
          } 

          // Likelihood contribution for this hit (hard coded for now...)
          if (found) {
            if ( layer == 0 || layer == 3 ) {
              nxFound++;
              nxExp++;
            } else {
              nyFound++;
              nyExp++;
            }
          } else {
            if ( layer == 0 || layer == 3 ) {
              nxExp++;
              xLik += log( 0.261 + exp( 5.1 * fabs(dist) - 11.87 ) );
            } else {
              nyExp++;
              yLik += log( 0.261 + exp( 5.1 * fabs(dist) - 11.87 ) );
            }
          }
        }
      }
    }
    if ( nxExp < 4 || nyExp < 4 || ( double(nxFound) / nxExp < 0.5 && 
                                     double(nyFound) / nyExp < 0.5 ) ) {
      seed->setLive( 0 );
    } else {
      double prx = log( gsl_cdf_chisq_Q( seed->xChi2(), seed->nx()-3 ));
      double pry = log( gsl_cdf_chisq_Q( seed->yChi2(), seed->ny()-2 ));
      double lik = xLik + yLik + ( prx + pry )/3.;
      int n = nxExp + nyExp;
      int r = nxFound + nyFound;
      double fnum = 1.;
      double fden = 1.;
      for ( int i = 0; i < n-r; ++i ) {
        fnum *= double( n-i );
        fden *= double( i+1 );
      }
      
      // Add binomial term to likelihood according to number of found hits on seed
      double eff = 0.9;
      lik += log( fnum/fden ) + r*log( eff ) + (n-r)*log( 1.-eff );
      seed->setLik( lik );
    }
  }
};

void TsaSeed::searchStereo( int& sector, std::vector<SeedHit*> hits[], std::vector<SeedTrack*>& seeds ) {
//-------------------------------------------------------------------------
//  Search for hits to form seed tracks in stereo stations of the OT 
//-------------------------------------------------------------------------

  double sth = 0.087156;
  for ( int station = 1; station < 4; ++station ) {
    for ( int layer = 1; layer < 3; ++layer ) {
      int lay = 2*(station-1);
      if ( layer > 1 ) lay++;
      Tsa::OTRange oRange = m_otDataSvc->partition(station,layer,sector);
      for (Tsa::OTClusters::const_iterator otIter = oRange.begin(); otIter != oRange.end(); ++otIter){
        Tsa::OTCluster* clus = *otIter;
        if ( clus->driftRadius() > 2.7 ) continue;
        SeedHit* hit = new SeedHit; 
        hit->setX( clus->xMid() );
        hit->setZ( clus->zMid() );
        hit->setR( clus->driftRadius() );
        hit->setChannel( clus->channel() );
        hit->setClus( clus );
        LHCb::MCParticle* aPart = particle(clus);
        if ( aPart ) hit->setPart( aPart->key() );
        hit->setIndex( station );
        hits[lay].push_back( hit );
      }
      if ( hits[lay].size() < 1 ) continue;
      std::sort( hits[lay].begin(), hits[lay].end(), TsaSeed::increasingX() );
    }
  }
  double yMin[2] = {-2500., -40.};
  double yMax[2] = {40., 2500.};
  for ( std::vector<SeedTrack*>::iterator it = seeds.begin(); seeds.end() != it; ++it ) {
    SeedTrack* seed = (*it);
    double z0 = 0.;
    std::vector<SeedHit*> yHits[6];
    for ( int lay = 0; lay < 6; ++lay ) {
      bool uLay = false;
      if ( lay == 0 || lay == 2 || lay == 4 ) uLay = true;
      for ( std::vector<SeedHit*>::iterator it = hits[lay].begin(); hits[lay].end() != it; ++it ) {
        SeedHit* hit = (*it);
        double zWire = hit->z();
        double xSeed = seed->x0() + seed->sx()*(zWire-z0) + seed->tx()*(zWire-z0)*(zWire-z0);
        double slope = seed->sx() + 2.*seed->tx()*(zWire-z0);
        Tsa::OTCluster* clus = hit->clus();
        Gaudi::XYZVector vec(1., 0.0036*slope, -slope);
        Gaudi::XYZPoint point(xSeed, clus->yMid(), zWire);
        Gaudi::Plane3D plane = Gaudi::Plane3D(vec,point);
          
        // intersect with plane with trajectory
        Gaudi::XYZPoint iPoint;
        if ( !Tsa::ClusFun::intersection(clus,plane,iPoint) ) continue; 
        double y = iPoint.y();
        if ( uLay ) {
          if ( y > yMax[sector-1] ) continue;
          if ( y < yMin[sector-1] ) break;
        } else {
          if ( y > yMax[sector-1] ) break;
          if ( y < yMin[sector-1] ) continue;
        }
        hit->setY( y );
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
            if ( fabs( sy - y1w/z1 ) > 0.1 ) continue;

            std::vector<SeedHit*> select;
            for ( int lay = lay1; lay <= lay2; ++lay ) {
              for ( std::vector<SeedHit*>::iterator it = yHits[lay].begin(); yHits[lay].end() != it; ++it ) {
                SeedHit* hit = (*it);
                if ( lay == lay1 && hit == hit1 ) continue;
                if ( lay == lay2 && hit == hit2 ) continue;
                if ( fabs( hit->y()-y1w-sy*(hit->z() - z1) ) < 100. ) select.push_back( hit );
              }
            }
            if ( select.size() < 6 ) continue;
            int nMax = 0;
            int s1Max = 0;
            int s2Max = 0;
            for ( int s1 = 0; s1 < 2; ++s1 ) {
              double y1 = hit1->y() + (2*s1-1)*hit1->r()/sth;
              for ( int s2 = 0; s2 < 2; ++s2 ) {
                double y2 = hit2->y() + (2*s2-1)*hit2->r()/sth;
                double sy = ( y2 - y1 ) / ( hit2->z() - z1 );
                int nh = 0;
                for ( std::vector<SeedHit*>::iterator it = select.begin(); select.end() != it; ++it ) {
                  SeedHit* hit = (*it);
                  double dy1 = hit->y() - hit->r()/sth - y1 - sy*( hit->z() - z1 );
                  double dy2 = dy1 + 2*hit->r()/sth;
                  if ( fabs(dy1) < 10. || fabs(dy2) < 10. ) nh++;
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
              double y1 = hit1->y() + (2*s1Max-1)*hit1->r()/sth;
              double y2 = hit2->y() + (2*s2Max-1)*hit2->r()/sth;
              sy = ( y2 - y1 ) / ( hit2->z() - z1 );
              for ( std::vector<SeedHit*>::iterator it = select.begin(); select.end() != it; ++it ) {
                SeedHit* hit = (*it);
                double dy1 = hit->y() - hit->r()/sth - y1 - sy*( hit->z() - z1 );
                double dy2 = dy1 + 2*hit->r()/sth;
                if ( fabs(dy1) < 10. || fabs(dy2) < 10. ) {
                  iSel[nSel] = hit;
                  if ( fabs(dy1) < fabs(dy2) ) {
                    sSel[nSel] = -1;
                  } else {
                    sSel[nSel] = 1;
                  }
                  if ( nSel < 29 ) nSel++;
                }
              }
            }
          }
        }
      }
    }
    if ( nBest > 2 ) {
      std::vector<SeedPnt> yPnts;
      for ( int i = 0; i < nSel; ++i ) {
        SeedPnt pnt;
        pnt.setHit( iSel[i] );
        pnt.setSign( sSel[i] );
        yPnts.push_back( pnt );
      }
      seed->setYPnts( yPnts );
      int rc = fitLine( seed );
      if ( seed->ny() < 6 || rc < 1 ) seed->setLive( 0 );
    } else {
      seed->setLive( 0 );
    }
  }
};

void TsaSeed::search( int& sector, std::vector<SeedHit*> hits[], std::vector<SeedTrack*>& seeds ) {
//-------------------------------------------------------------------------
//  Search for hits to form seed tracks in X stations of the OT 
//-------------------------------------------------------------------------

  for ( int station = 1; station < 4; ++station ) {
    for ( int layer = 0; layer < 4; layer+=3 ) {
      int lay = 2*(station-1);
      if ( layer > 0 ) lay++;
      Tsa::OTRange oRange = m_otDataSvc->partition(station,layer,sector);
      for (Tsa::OTClusters::const_iterator otIter = oRange.begin(); otIter != oRange.end(); ++otIter){
        Tsa::OTCluster* clus = *otIter;
        if ( clus->driftRadius() > 2.7 ) continue;
        SeedHit* hit = new SeedHit; 
        hit->setX( clus->xMid() );
        hit->setZ( clus->zMid() );
        hit->setR( clus->driftRadius() );
        hit->setChannel( clus->channel() );
        hit->setClus( clus );
        LHCb::MCParticle* aPart = particle(clus);
        if ( aPart ) hit->setPart( aPart->key() );
        hits[lay].push_back( hit );
      }
      if ( hits[lay].size() < 1 ) continue;
      std::sort( hits[lay].begin(), hits[lay].end(), TsaSeed::increasingX() );
    }
  }
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
          if ( sx < -0.8 ) continue;
          if ( first ) {
            first = false;
            start = it2;
          }
          if ( sx > 0.8 ) break;
          bool punt = false;
          std::vector<SeedHit*> skip = hit1->skip();
          if ( skip.size() > 0 ) {
            for ( std::vector<SeedHit*>::iterator it = skip.begin(); skip.end() != it; ++it ) {
              if ( hit2 == (*it) ) {
                punt = true;
                break;
              }
            }
          }
          if ( punt ) continue;
          if ( fabs(hit1->x()/3125. - sx) > 0.7 ) continue;
          double dth = atan( sx ) - atan( (x1 - (z1-5300.)*sx)/5300. );
          if ( fabs( dth ) > 0.7 ) continue;
          double dx = x2-x1;
          double dz = z2-z1;
          double cth = dz / sqrt( dx*dx + dz*dz );
          std::vector<SeedHit*> select[3];
          std::vector<SeedHit*>::iterator it = it1;
          while ( it < hits[lay1].end()-1 && (*(it+1))->x() - x1 < 10. ) {
            it++;
            SeedHit* hit = (*it);
            select[0].push_back( hit );
          }
          it = it1;
          while ( it > hits[lay1].begin() && x1 - (*(it-1))->x() < 10. ) {
            it--;
            SeedHit* hit = (*it);
            select[0].push_back( hit );
          }
          it = it2;
          while ( it < hits[lay2].end()-1 && (*(it+1))->x() - x2 < 10. ) {
            it++;
            SeedHit* hit = (*it);
            select[2].push_back( hit );
          }
          it = it2;
          while ( it > start && x2 - (*(it-1))->x() < 10. ) {
            it--;
            SeedHit* hit = (*it);
            select[2].push_back( hit );
          }
          for ( int lay = lay1+1; lay < lay2; ++lay ) {
            int stn = 1;
            if ( lay < 2 ) stn = 0;
            if ( lay > 3 ) stn = 2;
            for ( std::vector<SeedHit*>::iterator it = hits[lay].begin(); hits[lay].end() != it; ++it ) {
              SeedHit* hit = (*it);
              double dx = hit->x() - x1 - sx*( hit->z() - z1 );
              if ( dx < -10.) continue;
              if ( dx >  10.) break;
              hit->setUse1( 0 );
              hit->setUse2( 0 );
              select[stn].push_back( hit );
            }
          }
          if ( select[1].size() < 1 || select[0].size()+select[1].size()+select[2].size() < 5 ) continue;

          int nhSel = 0;
          SeedHit* hSel = 0;
          int sSel[3] = {0, 0, 0};
          for ( std::vector<SeedHit*>::iterator it3 = select[1].begin(); select[1].end() != it3; ++it3 ) {
            SeedHit* hit3 = (*it3);
            if ( hit3->use1() && hit3->use2() ) continue;
            double x3 = hit3->x();
            double z3 = hit3->z();
            double ax = ( (x2-x1)/(z2-z1)-(x3-x1)/(z3-z1) )/(z2-z3);
            double bx = (x2-x1)/(z2-z1) - ax*(z2-z1);
            int nhMax[3] = {0, 0, 0};
            int sMax[3] = {0, 0, 0};
            for ( int stn = 0; stn < 3; ++stn ) {
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
                    nh++;
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
            if ( nhMax[0]+nhMax[1]+nhMax[2] > nhSel ) {
              nhSel = nhMax[0]+nhMax[1]+nhMax[2];
              sSel[0] = sMax[0];
              sSel[1] = sMax[1];
              sSel[2] = sMax[2];
              hSel = hit3;
            }
          }
          if ( nhSel < 1 ) continue;

          std::vector<SeedPnt> xPnts;
          SeedPnt pnt;
          pnt.setHit( hit1 );
          pnt.setSign( sSel[0] );
          xPnts.push_back( pnt );
          pnt.setHit( hit2 );
          pnt.setSign( sSel[2] );
          xPnts.push_back( pnt );
          pnt.setHit( hSel );
          pnt.setSign( sSel[1] );
          xPnts.push_back( pnt );

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
              double z = hit->z();
              double xSeed = ax*(z-z1)*(z-z1) + bx*(z-z1) + x1;
              double dx1 = hit->x() + hit->r()/cth - xSeed;
              double dx2 = dx1 - 2*hit->r()/cth;
              if ( fabs(dx1) < 1. || fabs(dx2) < 1. ) {
                hit->setUse1( 1 );
                pnt.setHit( hit );
                if ( fabs(dx1) < fabs(dx2) ) {
                  pnt.setSign( 1 );
                } else {
                  pnt.setSign( -1 );
                }
                xPnts.push_back( pnt );
              }
            }
          }
          if ( xPnts.size() < 7 ) continue;

          SeedTrack* seed = new SeedTrack; 
          seed->setXPnts( xPnts );
          int rc = fitParabola( seed, cth );

          if ( rc < 1 || seed->nx() < 7 || fabs( seed->x0() ) > 4000. || fabs( seed->sx() ) > 0.7 || 
               fabs( seed->tx() ) > 2.2E-5 || fabs( 53000.*seed->tx() - dth ) > 0.8 ) {
            delete seed;
            continue;
          }
          seed->setLik( dth );
          seeds.push_back( seed );

          hit1->setUse1( 1 );
          hit2->setUse1( 1 );
          select[0].push_back( hit1 );
          select[2].push_back( hit2 );
          for ( std::vector<SeedHit*>::iterator it = select[0].begin(); select[0].end() != it; ++it ) {
            SeedHit* hit = (*it);
            if ( hit->use1() == 0 && hit->use2() == 0 ) continue;
            std::vector<SeedHit*> skip2 = hit->skip();
            for ( std::vector<SeedHit*>::iterator it2 = select[2].begin(); select[2].end() != it2; ++it2 ) {
              SeedHit* hitb = (*it2);
              if ( hitb->use1() == 0 && hitb->use2() == 0 ) continue;
              skip2.push_back( hitb );
            }
            hit->setSkip( skip2 );
          }
        }
      }
    }
  }
};

int TsaSeed::fitLine( SeedTrack* seed ) {
//-------------------------------------------------------------------------
//  Fit a line y = y0 + s*(z-z0) through hits on seed track
//-------------------------------------------------------------------------

  std::vector<SeedPnt> pnts = seed->yPnts(); 
  if ( pnts.size() < 2 ) return -1;
  bool update = 0;
  int nIt = 0;
  double z0 = 0.;
  double sth = 0.087156;
  do {
    nIt++;
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
      ny++;
      double y = pnt.hit()->y() + pnt.sign()*pnt.hit()->r()/sth;
      double z = pnt.hit()->z();
      double dz2 = (z-z0)*(z-z0);
      double dd = 0.04/(sth*sth);
      if ( pnt.sign() == 0 ) dd = 1.0/(sth*sth);
      a += y*(z-z0) / dd;
      b += (z-z0) / dd;
      c += dz2 / dd;
      d += y / dd;
      e += 1. / dd;
    }
    if ( ny < 2 ) return -2;
    double det = c*e - b*b;
    if ( fabs( det ) < 1.e-12 || fabs(b) < 1.e-12 ) {
      return -3;
    } else { 
      double s = ( a*e - b*d ) / det;
      double y0 = ( a - c*s ) / b;
      seed->setNy( ny );
      seed->setY0( y0 );
      seed->setSy( s );
    }

    double dMax = 0.;
    std::vector<SeedPnt>::iterator itMax;
    for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
      SeedPnt & pnt = *it;
      if ( pnt.skip() ) continue;
      double y = pnt.hit()->y();
      double z = pnt.hit()->z();
      double r = pnt.hit()->r()/sth;
      double ySeed = seed->y0() + seed->sy()*(z-z0);
      if ( pnt.sign() == 0 ) {
        update = 1;
        pnt.setSign( 1 );
        if ( fabs( y+r - ySeed ) > fabs( y-r - ySeed ) ) pnt.setSign( -1 );
      } else {
        if ( fabs( y+pnt.sign()*r - ySeed ) > fabs( y-pnt.sign()*r - ySeed ) ) {
          update = 1;
          pnt.setSign( -pnt.sign() );
        }
      }
      double dy = y + pnt.sign()*r - ySeed;
      if ( fabs(dy) > dMax ) {
        dMax = fabs(dy);
        itMax = it;
      }
    }
    if ( update == 0 && dMax > 8. ) {
      update = 1;
      (*itMax).setSkip( 1 );
    }
  } while ( update && nIt < 12 ); 
  if ( nIt == 12 ) return -4;

  double chi2 = 0.;
  for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
    SeedPnt & pnt = *it;
    double y = pnt.hit()->y() + pnt.sign()*pnt.hit()->r()/sth;
    double z = pnt.hit()->z();
    pnt.setCoord( y );
    pnt.setZ( z );
    if ( pnt.skip() ) continue;
    double ch = y - seed->y0() - seed->sy()*(z-z0);
    chi2 += ch*ch*sth*sth / 0.04;
  }
  seed->setYChi2( chi2 );
  seed->setYPnts( pnts );
  return nIt;
};

int TsaSeed::fitParabola( SeedTrack* seed, double &cth ) {
//-------------------------------------------------------------------------
//  Fit a parabola x = x0 + s*(z-z0) + t*(z-z0)**2 through hits on seed track
//-------------------------------------------------------------------------

  std::vector<SeedPnt> pnts = seed->xPnts(); 
  if ( pnts.size() < 3 ) return -1;
  bool update = 0;
  int nIt = 0;
  double z0 = 0.;
  do {
    nIt++;
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
      nx++;
      double x = pnt.hit()->x() + pnt.sign()*pnt.hit()->r()/cth;
      double z = pnt.hit()->z();
      double dz2 = (z-z0)*(z-z0);
      double dz3 = dz2*(z-z0);
      double dz4 = dz3*(z-z0);
      double dd = 0.04/(cth*cth);
      if ( pnt.sign() == 0 ) dd = 1.0/(cth*cth);
      a += x / dd;
      b += 1. / dd;
      c += (z-z0) / dd;
      d += dz2 / dd;
      e += x*(z-z0) / dd;
      f += dz3 / dd;
      g += x*dz2 / dd;
      h += dz4 / dd;
    }
    if ( nx < 3 ) return -2;
    if ( fabs( (d*d-f*c)*(f*d-h*c)-(f*f-h*d)*(d*c-f*b) ) < 1.e-12 ||
         fabs( f*c-d*d ) < 1.e-12 || fabs(d) < 1.e-12 ) return -3;

    double x0 = ( (f*f-h*d)*(f*a-d*e)-(d*d-f*c)*(h*e-f*g) ) /
      ( (d*d-f*c)*(f*d-h*c)-(f*f-h*d)*(d*c-f*b) );
    double s = ( (f*a-d*e) + (d*c-f*b)*x0 ) / (f*c-d*d);
    double t = ( a - b*x0 - c*s ) / d;
    seed->setNx( nx );
    seed->setX0( x0 );
    seed->setSx( s );
    seed->setTx( t );

    double dMax = 0.;
    std::vector<SeedPnt>::iterator itMax;
    for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
      SeedPnt & pnt = *it;
      if ( pnt.skip() ) continue;
      double x = pnt.hit()->x();
      double z = pnt.hit()->z();
      double r = pnt.hit()->r()/cth;
      double xSeed = x0 + s*(z-z0) + t*(z-z0)*(z-z0);
      if ( pnt.sign() == 0 ) {
        update = 1;
        pnt.setSign( 1 );
        if ( fabs( x+r - xSeed ) > fabs( x-r - xSeed ) ) pnt.setSign( -1 );
      } else {
        if ( fabs( x+pnt.sign()*r - xSeed ) > fabs( x-pnt.sign()*r - xSeed ) ) {
          update = 1;
          pnt.setSign( -pnt.sign() );
        }
      }
      double dx = x+pnt.sign()*r - xSeed;
      if ( fabs(dx) > dMax ) {
        dMax = fabs(dx);
        itMax = it;
      }
    }
    if ( update == 0 && dMax > 0.6/cth ) {
      update = 1;
      (*itMax).setSkip( 1 );
    }
  } while ( update && nIt < 12 );
  if ( nIt == 12 ) return -4;
  
  double chi2 = 0.;
  for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
    SeedPnt & pnt = *it;
    double x = pnt.hit()->x() + pnt.sign()*pnt.hit()->r()/cth;
    pnt.setCoord( x );
    if ( pnt.skip() ) continue;
    double z = pnt.hit()->z();
    double dz2 = (z-z0)*(z-z0);
    double ch = x - seed->x0() - seed->sx()*(z-z0) - seed->tx()*dz2;
    chi2 += ch*ch*cth*cth / 0.04;
  }
  seed->setXChi2( chi2 );
  seed->setXPnts( pnts );
  return nIt;
};

int TsaSeed::refitParabola( SeedTrack* seed, double &cth ) {
//-------------------------------------------------------------------------
//  Fit a parabola x = x0 + s*(z-z0) + t*(z-z0)**2 through hits on seed track
//-------------------------------------------------------------------------

  std::vector<SeedPnt> pnts = seed->xPnts(); 
  if ( pnts.size() < 3 ) return -1;
  double z0 = 0.;
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
    double dz2 = (z-z0)*(z-z0);
    double dz3 = dz2*(z-z0);
    double dz4 = dz3*(z-z0);
    double dd = 0.04/(cth*cth);
    a += x / dd;
    b += 1. / dd;
    c += (z-z0) / dd;
    d += dz2 / dd;
    e += x*(z-z0) / dd;
    f += dz3 / dd;
    g += x*dz2 / dd;
    h += dz4 / dd;
  }
  if ( fabs( (d*d-f*c)*(f*d-h*c)-(f*f-h*d)*(d*c-f*b) ) < 1.e-12 ||
       fabs( f*c-d*d ) < 1.e-12 || fabs(d) < 1.e-12 ) return -3;

  double x0 = ( (f*f-h*d)*(f*a-d*e)-(d*d-f*c)*(h*e-f*g) ) /
    ( (d*d-f*c)*(f*d-h*c)-(f*f-h*d)*(d*c-f*b) );
  double s = ( (f*a-d*e) + (d*c-f*b)*x0 ) / (f*c-d*d);
  double t = ( a - b*x0 - c*s ) / d;
  seed->setX0( x0 );
  seed->setSx( s );
  seed->setTx( t );
  
  double chi2 = 0.;
  for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
    SeedPnt & pnt = *it;
    if ( pnt.skip() ) continue;
    double x = pnt.coord();
    double z = pnt.z();
    double dz2 = (z-z0)*(z-z0);
    double ch = x - seed->x0() - seed->sx()*(z-z0) - seed->tx()*dz2;
    chi2 += ch*ch*cth*cth / 0.04;
  }
  seed->setXChi2( chi2 );
  return 1;
};

void TsaSeed::monitor( int& sector, std::vector<SeedTrack*>& seeds, double& time, 
                       std::vector<SeedHit*> hits[], std::vector<SeedHit*> hit2[] ) {
//-------------------------------------------------------------------------
//  Monitor the track seeding performance 
//-------------------------------------------------------------------------

  const LHCb::MCParticles*  mcparts  = get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default );
  MCTrackInfo trackInfo( eventSvc(), msgSvc() );

  Tsa::STClusters* itHits = get<Tsa::STClusters>(Tsa::STClusterLocation::IT);

  Tuple nt1 = nTuple("Event");
  Tuple nt2 = nTuple("Particle");
  Tuple nt3 = nTuple("Seed");
  
  int nPart = 0;
  int iPart[1000];
  int idPart[1000];
  double yPart[1000];
  double syPart[1000];
  double pPart[1000];
  double fPart[1000];
  SeedTrack* sPart[1000];

  // Define the "signal" particles, which seeding is tuned to find
  for ( LHCb::MCParticles::const_iterator mcpart = mcparts->begin();
        mcpart != mcparts->end(); ++mcpart ){
    int nR = trackInfo.nbVeloR(*mcpart);
    if ( nR < 3 ) continue;
    int nPhi = trackInfo.nbVeloPhi(*mcpart);
    if ( nPhi < 3 ) continue;
    int idMC = 0;
    int mcid = (*mcpart)->particleID().pid();
    if ( abs(mcid) == 11 )   idMC = -1;  // e
    if ( abs(mcid) == 13 )   idMC = -2;  // mu
    if ( abs(mcid) == 211 )  idMC =  3;  // pi
    if ( abs(mcid) == 321 )  idMC =  4;  // K
    if ( abs(mcid) == 2212 ) idMC =  5;  // p
    if ( idMC == 0 ) continue;           // Only use charged stable particles
    int motherID = 0;
    const LHCb::MCParticle* mother = (*mcpart)->mother();
    if ( mother ) motherID = mother->particleID().pid();
    if ( idMC == -2 && motherID == 443 ) idMC = -6;
    if ( idMC ==  3 && motherID == 310 ) idMC =  7;
    if ( mcid < 0 ) idMC = -idMC;        // Signed with charge 
    double yMC = (*mcpart)->originVertex()->position().Y();
    double zMC = (*mcpart)->originVertex()->position().Z();
    double pMC = (*mcpart)->p()/1000.;
    double pyMC = (*mcpart)->momentum().Py()/1000.;
    double pzMC = (*mcpart)->momentum().Pz()/1000.;
    int iMC = (*mcpart)->key();
      
    // Count hits created by particle in OT and IT
    int nx[3] = {0, 0, 0};  // X hits per OT station
    int ns[3] = {0, 0, 0};  // Stereo hits per OT station
    int ni = 0;             // IT hits
    double zMin = 9999.;
    double xMin = 0.;
    double yMin = 0.;
    for ( int station = 1; station < 4; ++station ) {
      for ( int layer = 0; layer < 4; ++layer ) {
        Tsa::OTRange oRange =  m_otDataSvc->partition(station,layer,sector);
        for (Tsa::OTClusters::const_iterator otIter = oRange.begin(); otIter != oRange.end(); ++otIter){
          Tsa::OTCluster* clus = *otIter;
          LHCb::MCParticle* aPart = particle(clus);
          if (aPart && aPart->key() == iMC) {
            if (layer == 0 || layer == 3) {
              if ( clus->zMid() < zMin ) {
                zMin = clus->zMid();
                xMin = clus->xMid();
                yMin = yMC + (pyMC/pzMC)*(zMin - zMC);
              }
              nx[station-1]++; 
            } else { 
              ns[station-1]++;   
            }
          }
        }
      }
    }
    // Count IT hits
    for (Tsa::STClusters::const_iterator tIter = itHits->begin(); tIter != itHits->end() ; ++tIter){
      Tsa::STCluster* clus = *tIter;
      LHCb::MCParticle* aPart = particle(clus);
      if (aPart && aPart->key() == iMC) ni++;
    }
    // Selection cuts for MC particles that are used for tuning the seeding algorithm
    if ( nx[0] > 0 && ns[0] > 0 && nx[1] > 0 && ns[1] > 0 && nx[2] > 0 && ns[2] > 0 &&
         ni == 0 && pMC > 2. ) {
      iPart[nPart] = iMC;           // Store information for those particles
      idPart[nPart] = idMC;
      yPart[nPart] = yMC - (pyMC/pzMC)*zMC;
      syPart[nPart] = pyMC / pzMC;
      pPart[nPart] = pMC;
      fPart[nPart] = 0.;
      sPart[nPart] = 0;
      if (nPart < 999) nPart++;
    }
  }

  // Find purity of seeds
  for ( std::vector<SeedTrack*>::iterator it = seeds.begin(); seeds.end() != it; ++it ) {
    SeedTrack* seed = (*it);
    if ( seed->select() == 0 ) continue;
    int nHit = 0;
    int iHit[60];
    std::vector<SeedPnt> pnts = seed->xPnts(); 
    for ( std::vector<SeedPnt>::iterator it2 = pnts.begin(); pnts.end() != it2; ++it2 ) {
      SeedPnt pnt = (*it2);
      if ( pnt.skip() ) continue;
      iHit[nHit] = pnt.hit()->part();
      if ( nHit < 59 ) nHit++;
    }
    pnts = seed->yPnts(); 
    for ( std::vector<SeedPnt>::iterator it2 = pnts.begin(); pnts.end() != it2; ++it2 ) {
      SeedPnt pnt = (*it2);
      if ( pnt.skip() ) continue;
      iHit[nHit] = pnt.hit()->part();
      if ( nHit < 59 ) nHit++;
    }
    int nm = 0;
    int nMatch[60];
    int iMatch[60];
    for ( int ih = 0; ih < nHit; ++ih ) {
      if ( iHit[ih] > -1 ) {
        if ( nm == 0 ) {
          iMatch[0] = iHit[ih];
          nMatch[0] = 1;
          nm++;
        } else {
          bool found = 0;
          for ( int i = 0; i < nm; ++i ) {
            if ( iHit[ih] == iMatch[i] ) {
              found = 1;
              nMatch[i]++;
              break;
            }
          }
          if ( found == 0 ) {
            iMatch[nm] = iHit[ih];
            nMatch[nm] = 1;
            if ( nm < 59 ) nm++;
          }
        }
      }
    } 
    int nMax = 0;
    int iMax = -2;
    for ( int i = 0; i < nm; ++i ) {
      if ( nMatch[i] > nMax ) {
        nMax = nMatch[i];
        iMax = iMatch[i];
      }
    }
    double purity = 0.;
    if ( nMax > 0 ) purity = double(nMax) / nHit;
    seed->setPur( purity );
    seed->setPart( iMax );
    if ( iMax >= 0 ) {
      for ( int ip = 0; ip < nPart; ++ip ) {
        if ( iPart[ip] == iMax ) {
          seed->setMatch( 1 );
          if ( purity > fPart[ip] ) {
            fPart[ip] = purity;
            sPart[ip] = seed;
          }
        }
      }
    }
  }
  for ( int ip = 0; ip < nPart; ++ip ) {
    nt2->column ( "ev" , m_ev );
    nt2->column ( "id" , idPart[ip] );
    nt2->column ( "y" , yPart[ip] );
    nt2->column ( "sy" , syPart[ip] );
    nt2->column ( "p" , pPart[ip] );
    if ( sPart[ip] == 0 ) {
      nt2->column ( "n" , 0 );
      nt2->column ( "nx" , 0 );
      nt2->column ( "pur" , 0. );
      nt2->column ( "x0" , 0. );
      nt2->column ( "sx" , 0. );
      nt2->column ( "tx" , 0. );
      nt2->column ( "ch2" , 0. );
    } else {
      sPart[ip]->setMatch( 2 );
      nt2->column ( "n" , sPart[ip]->xPnts().size() );
      nt2->column ( "nx" , sPart[ip]->nx() );
      nt2->column ( "pur" , sPart[ip]->pur() );
      nt2->column ( "x0" , sPart[ip]->x0() );
      nt2->column ( "sx" , sPart[ip]->sx() );
      nt2->column ( "tx" , sPart[ip]->tx() );
      nt2->column ( "ch2" , sPart[ip]->xChi2() );
    }
    nt2->write();
  }
  int nSeeds[2] = {0, 0};
  int nGhosts[2] = {0, 0};
  for ( std::vector<SeedTrack*>::iterator it = seeds.begin(); seeds.end() != it; ++it ) {
    SeedTrack* seed = (*it);
    if ( seed->select() == 0 ) continue;
    nSeeds[0]++;
    if ( seed->pur() < 0.7 ) nGhosts[0]++;
    if ( seed->lik() > -23. ) {
      nSeeds[1]++;
      if ( seed->pur() < 0.7 ) nGhosts[1]++;
    }
    double qp = 0.;
    for ( int ip = 0; ip < nPart; ++ip ) {
      if ( iPart[ip] == seed->part() ) {
        qp = pPart[ip];
        if ( idPart[ip] < 0 ) qp = -pPart[ip];
      }
    }
    nt3->column ( "ev" , m_ev );
    nt3->column ( "mx" , seed->xPnts().size() );
    nt3->column ( "my" , seed->yPnts().size() );
    nt3->column ( "nx" , seed->nx() );
    nt3->column ( "ny" , seed->ny() );
    nt3->column ( "x0" , seed->x0() );
    nt3->column ( "y0" , seed->y0() );
    nt3->column ( "sx" , seed->sx() );
    nt3->column ( "sy" , seed->sy() );
    nt3->column ( "tx" , seed->tx() );
    nt3->column ( "chx" , seed->xChi2() );
    nt3->column ( "chy" , seed->yChi2() );
    nt3->column ( "pur" , seed->pur() );
    nt3->column ( "sel" , seed->select() );
    nt3->column ( "mat" , seed->match() );
    nt3->column ( "lik" , seed->lik() );
    nt3->column ( "qp" , qp );
    nt3->column ( "nc" , seed->links().size() );
    nt3->write();
  }
  int nxHits = 0;
  int nyHits = 0;
  for ( int lay = 0; lay < 6; ++lay ) {
    for ( std::vector<SeedHit*>::iterator it = hits[lay].begin(); hits[lay].end() != it; ++it ) nxHits++;
    for ( std::vector<SeedHit*>::iterator it = hit2[lay].begin(); hit2[lay].end() != it; ++it ) nyHits++;
  }
  nt1->column ( "ev" , m_ev );
  nt1->column ( "h" , sector );
  nt1->column ( "np" , nPart );
  nt1->column ( "nx" , nxHits );
  nt1->column ( "ny" , nyHits );
  nt1->column ( "ns" , nSeeds[0] );
  nt1->column ( "ns2" , nSeeds[1] );
  nt1->column ( "ng" , nGhosts[0] );
  nt1->column ( "ng2" , nGhosts[1] );
  nt1->column ( "nt" , seeds.size() );
  nt1->column ( "t" , time );
  nt1->write();
};

bool TsaSeed::increasingX::operator() ( SeedHit* first, SeedHit* second ) const {
//-------------------------------------------------------------------------
//  Define the sort sequence for hits
//-------------------------------------------------------------------------
  if ( 0 == second ) return true;
  if ( 0 == first )  return false;
  return first->x() < second->x();
}

StatusCode TsaSeed::finalize(){
   
  std::cout << "Time: " << name() << " "  << aWatch.sum() << std::endl;
  return GaudiTupleAlg::finalize();
}

LHCb::MCParticle* TsaSeed::particle(Tsa::Cluster* aCluster){
 
 ILHCbIDsToMCParticles::LinkMap tMap;
 LHCb::LHCbID theID = aCluster->id();
 m_linkTool->link(theID,tMap);

 LHCb::MCParticle* aParticle = 0;
 if (tMap.empty() == false) {
   ILHCbIDsToMCParticles::LinkMap::iterator iter = tMap.begin();
   aParticle = iter->first;
 }
 return aParticle;
}
