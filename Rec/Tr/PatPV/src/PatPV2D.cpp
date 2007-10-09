// $Id: PatPV2D.cpp,v 1.1.1.1 2007-10-09 18:46:14 smenzeme Exp $
// Include files
 
// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/AlgFactory.h"

// from gsl
#include "gsl/gsl_math.h"

// from Event
#include "Event/Track.h"
#include "VeloDet/DeVelo.h"

// local
#include "PatPV2D.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatPV2D
//
// 2005-06-22 : Olivier Callot from Mariusz Witek (original method by Ivan Kisel)
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PatPV2D );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatPV2D::PatPV2D( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "maxNumPv"        , m_maxNumPv      =  3             );
  declareProperty( "maxIter"         , m_maxIter       =  3             );
  declareProperty( "minMult"         , m_minMult       =  7             );
  declareProperty( "x0MS"            , m_x0MS          =  0.01          );
  declareProperty( "multiQualityPV"  , m_multiQualityPV   =  10         );
  declareProperty( "minBackQualityPV", m_minBackQualityPV =   2         );
  declareProperty( "dzQualityPV"     , m_dzQualityPV      =  20.*Gaudi::Units::mm     );
  declareProperty( "SaveSeedsAsPV"   , m_saveSeedsAsPV    =  false      );
  declareProperty( "InputTracksName"    , m_inputTracksName     =  LHCb::TrackLocation::RZVelo);
  declareProperty( "OutputVerticesName" , m_outputVerticesName  =  LHCb::RecVertexLocation::Velo2D );
}
//=============================================================================
// Destructor
//=============================================================================
PatPV2D::~PatPV2D() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatPV2D::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  //  double X0 = 0.0048; // 440 um of Si
  double X0 = m_x0MS;
  m_scatCons = (13.6*sqrt(X0)*(1.+0.038*log(X0))) / ( 0.400 * Gaudi::Units::GeV );

  
 
  
  //== Get Velo detector element, to get the R sector angle
  DeVelo* velo = getDet<DeVelo>( DeVeloLocation::Default );

  std::vector<double> dum( 8, 0. );
  m_phiOfSector = dum;  // make it 8 words..


  //== First R sensor 
  std::vector<DeVeloRType*>::const_iterator sensorRIt = velo->rSensorsBegin();

  for (unsigned int izone = 0; izone<4; izone++) {
    m_phiOfSector[izone] = 0.5*((*sensorRIt)->phiMinZone(izone) 
                                + (*sensorRIt)->phiMaxZone(izone) );
    m_phiOfSector[izone+4] = m_phiOfSector[izone] + M_PI;
  };

  // Access PVSeedTool
  m_pvSeedTool = tool<PVSeedTool>( "PVSeedTool", this );
  if( !m_pvSeedTool ) {
    err() << "Unable to retrieve the PVSeedTool" << endmsg;
    return  StatusCode::FAILURE;
  }
  m_vclusters.reserve(500);

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatPV2D::execute() {

  debug() << "==> Execute" << endmsg;

  m_inputTracks   = get<LHCb::Tracks>( m_inputTracksName );
  m_outputVertices  = new LHCb::RecVertices();
  put(m_outputVertices, m_outputVerticesName);

  std::vector<myTrack> myTracks;
  myTracks.reserve( m_inputTracks->size() );
  std::vector< myVertex > myVertices;
  myVertices.reserve( 10);
  
  std::vector<myTrack>::iterator itMyTr;
  std::vector<myVertex>::iterator itMyPv;

  // to aviod edge effects
  double farZ =  (200. -20.)*Gaudi::Units::mm;
  double maxZ =  (200. -10.)*Gaudi::Units::mm;

  m_vclusters.clear();

  // select 2d tracks and fill local bookkeeping table
  for ( std::vector<LHCb::Track*>::const_iterator itT = m_inputTracks->begin();
        m_inputTracks->end() != itT; itT++ ) {
    LHCb::Track* pTr2d = (*itT);

    pTr2d->setFlag( LHCb::Track::Invalid, false );

    double zFirst  = pTr2d->firstState().z();
    double rFirst  = pTr2d->firstState().x();
    double trFirst = pTr2d->firstState().tx();
    double d2r0    = pTr2d->firstState().errX2();
    double d2tr    = pTr2d->firstState().errTx2();
    int    sector  = pTr2d->specific();
    double phi     = m_phiOfSector[ sector ];
    // add MS from RF foil, constant Pt
    double addScat = m_scatCons * trFirst;
    double z0      = zFirst - rFirst/trFirst; // where r = 0.
    
    double dr2ms   = ( addScat * (zFirst-z0) ) * ( addScat * (zFirst-z0) );
    d2r0 += dr2ms;
    // mark distant tracks as bad tracks to avoid edge effects
    if( farZ < fabs(z0)) {
      pTr2d->setFlag( LHCb::Track::Invalid, true );
    } else if( maxZ > fabs(z0) ) {
      myTrack tmpTrack;
      tmpTrack.pTr2d     = pTr2d;
      tmpTrack.pvidx     = 0;
      tmpTrack.chi2      = 0.;
      tmpTrack.pvweight  = 1.0;
      tmpTrack.z0        = z0;
      tmpTrack.tr        = trFirst;
      tmpTrack.d2r0      = d2r0;
      tmpTrack.d2tr      = d2tr;
      tmpTrack.cosPhi    = cos(phi);
      tmpTrack.sinPhi    = sin(phi);
      tmpTrack.sector    = sector;
      tmpTrack.backward  = pTr2d->checkFlag( LHCb::Track::Backward);
      myTracks.push_back(tmpTrack);

      double sigsq;
      // use standard errors for seeding
      m_pvSeedTool->errorForPVSeedFinding(trFirst, 0.0, sigsq);

      vtxCluster clu;
      clu.z = z0;
      clu.sigsq = sigsq;
      clu.sigsqmin = clu.sigsq;
      clu.ntracks = 1;
      m_vclusters.push_back(clu);
    }
  }

  /////////////////////////////////
  // Get zseeds of PV candidates //
  /////////////////////////////////

  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << m_vclusters.size() << " 2D tracks used for seeding" << endmsg;
  }
  
  std::vector<double> zseeds;
  m_pvSeedTool->findClusters(m_vclusters,zseeds);

  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << zseeds.size() << " z seeds found: " << zseeds << endmsg;
  }
  
  // Monitor quality of zseeds only.
  if(m_saveSeedsAsPV) {
    storeDummyVertices(zseeds);
    return StatusCode::SUCCESS;
  }


  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << myTracks.size() << " input 2D tracks " << endmsg;
  }

  int pvidx;

  std::vector<double>::iterator itzseed;  
  itzseed = zseeds.begin();
  while (myVertices.size() < m_maxNumPv && itzseed != zseeds.end() ) {

    pvidx = myVertices.size()+1;

    double zseed = *itzseed;
    if ( msgLevel( MSG::DEBUG ) ) {
      debug() << "Found seed: " << zseed << endmsg;
    }

    double zv = zseed;
    double xv = 0.0;
    double yv = 0.0;

    double sigmazv = 0.100 * Gaudi::Units::mm;
    double sigmaxv = 0.070 * Gaudi::Units::mm;
    double sigmayv = 0.070 * Gaudi::Units::mm;

    double sumX[4], sumY[4], sumWX[4], sumWY[4];
    double meanX[4], meanY[4], sigma2X[4], sigma2Y[4];

    for ( itMyTr = myTracks.begin(); itMyTr != myTracks.end(); itMyTr++) {
      if(itMyTr->pvidx > 0) {
        itMyTr->pvweight = 0.0;  // Ignore already assigned tracks
      } else {
        itMyTr->pvweight = 1.0;
      }
    }


    for (int iter=1; iter<=m_maxIter; iter++){

      double zvcut  = 7.5 * sigmazv; // from pull distribution
      double xyvcut = 7.5 * sigmaxv;

      // xy pv finding

      for(int pr = 0; pr<4; pr++){
        sumX[pr]  = 0.0;
        sumY[pr]  = 0.0;
        sumWX[pr] = 0.0;
        sumWY[pr] = 0.0;
      }

      for ( itMyTr = myTracks.begin(); itMyTr != myTracks.end(); itMyTr++) {

        if ( itMyTr->pvidx>0 || itMyTr->pvweight < 0.0001) continue;


        double z0   = itMyTr->z0;
        double tr   = itMyTr->tr;
        double d2r0 = itMyTr->d2r0;
        double d2tr = itMyTr->d2tr;

        //check wether it is compatible with zv:
        double cosa = itMyTr->cosPhi;
        double sina = itMyTr->sinPhi;
        double rvpr = xv * cosa + yv * sina; // projection of the pv point
        double dzv  = rvpr / tr;
        double zvpr = z0 + dzv;
        double d2zvpr = ( d2r0 + dzv * dzv *d2tr ) / (tr*tr) +
          (d2tr) * (zvpr/tr)*(zvpr/tr);

        //if (fabs(zvpr-zv) >= zvcut+5.0*sqrt(d2zvpr)){// outside errors
        double ddz = fabs(zvpr-zv)-zvcut; 
        if ( ddz > 0. && ddz * ddz >= 25.*d2zvpr ) {
          itMyTr->pvweight = 0.0;
          continue;
        }

        // compatible with the previous estimation of (xv,yv)?
        double r   = tr * (zv-z0);  //r at primary vertex position
        double d2r = d2r0 + (zv-z0) * (zv-z0) * d2tr;

        double tolR = xyvcut + 5.0 * sqrt(d2r);  // Tol = 5 time computed error.
        if ( fabs(r-rvpr) >= tolR ){
          itMyTr->pvweight = 0.0;
          continue;
        }

        double dist2   = ( r - rvpr ) * ( r - rvpr );
        double xyvcut2 = tolR * tolR;
        // bi-weight Tukey
        double coeff   = (1.0-(dist2/xyvcut2))*(1.0-(dist2/xyvcut2));

        double x = r*cosa;
        double y = r*sina;

        // calculate centers of gravity for each sector and then summ vectors

        int    pr  = itMyTr->sector % 4;
        double wx  = coeff * itMyTr->pvweight / ( d2r * cosa * cosa );
        double wy  = coeff * itMyTr->pvweight / ( d2r * sina * sina );

        itMyTr->chi2 = dist2/d2r;
        itMyTr->pvweight = coeff; //store to select good tracks for z calc.

        sumX[pr]  += x * wx;
        sumY[pr]  += y * wy;
        sumWX[pr] += wx;
        sumWY[pr] += wy;

      } // loop over tracks

      double xpoint[2], ypoint[2], sigma2xp[2], sigma2yp[2];
      bool pointfound[2];
      for(int proj = 0; proj < 4; proj++){
        if (proj<2){//reset x,y,sigma2yx
          xpoint[proj]=0.0;
          sigma2xp[proj]=0.0;
          ypoint[proj]=0.0;
          sigma2yp[proj]=0.0;
          pointfound[proj] = false;
        }
        meanX[proj]=0.0;
        meanY[proj]=0.0;
        sigma2X[proj]=0.0;
        sigma2Y[proj]=0.0;
        if (sumWX[proj] != 0.0){
          meanX[proj]   = sumX[proj]/sumWX[proj];
          sigma2X[proj] = 1.0/sumWX[proj];
          meanY[proj]   = sumY[proj]/sumWY[proj];
          sigma2Y[proj] = 1.0/sumWY[proj];
          int ipoint = proj - 2*(proj/2);
          xpoint[ipoint]   += meanX[proj];
          sigma2xp[ipoint] += sigma2X[proj];
          ypoint[ipoint]   += meanY[proj];
          sigma2yp[ipoint] += sigma2Y[proj];
          pointfound[ipoint] = true;
        }
      }

      xv = yv = sigmaxv = sigmayv = 0.0;
      for (int ipoint = 0; ipoint < 2; ipoint++){
        if (pointfound[ipoint]){
          xv += xpoint[ipoint]/sigma2xp[ipoint];
          yv += ypoint[ipoint]/sigma2yp[ipoint];
          sigmaxv += 1.0/sigma2xp[ipoint];
          sigmayv += 1.0/sigma2yp[ipoint];
        }
      }

      if (sigmaxv != 0.0){
        xv /= sigmaxv; 
        sigmaxv = sqrt(1.0/sigmaxv);
        yv /= sigmayv; 
        sigmayv = sqrt(1.0/sigmayv);
      } else{
        sigmaxv = 1000.0; sigmayv = 1000.0;
      }

      //estimate zv position
      double sumCenter = 0.0;
      double sumValue  = 0.0;

      for ( itMyTr = myTracks.begin(); itMyTr != myTracks.end(); itMyTr++) {

        if(itMyTr->pvidx>0 || itMyTr->pvweight<0.0001) continue;

        double z0  = itMyTr->z0;
        //double r0  = itMyTr->r0;
        double tr  = itMyTr->tr;
        double d2r0  = itMyTr->d2r0;
        double d2tr  = itMyTr->d2tr;

        double rvpr = xv * itMyTr->cosPhi + yv * itMyTr->sinPhi;
        double z    = z0+(rvpr/*-r0*/)/tr;
        double d2z  = (d2r0+(z-z0)*(z-z0)*d2tr)/(tr*tr) +
          (d2tr)*(z/tr)*(z/tr);

        double dz   = sqrt(d2z);
        if (fabs(z-zv) >= zvcut+5.0*dz){// outside errors
          itMyTr->pvweight = 0.0;
          continue;
        }

        double zvcut2 = (zvcut+5.0*dz)*(zvcut+5.0*dz);
        double dist2 = (z-zv)*(z-zv);
        double coeff = (1.0-(dist2/zvcut2))*(1.0-(dist2/zvcut2));
        double w = (1.0/d2z)*coeff*itMyTr->pvweight;// bi-weight Tukey
        sumCenter += z*w;
        sumValue  += w;

        itMyTr->pvweight = coeff;

      }

      zv = sumCenter/sumValue;
      sigmazv = sqrt(1.0/sumValue);

      if ( msgLevel( MSG::DEBUG ) ) {
        debug() << format( "    iter %i X=%7.3f +-%5.3f  Y=%7.3f +-%5.3f",
                           iter, xv, sigmaxv, yv, sigmayv)
                << endmsg;
      }
        
      if ( msgLevel( MSG::DEBUG ) ) {
        debug() << format( " -> ZV = %7.3f +-%5.3f", zv, sigmazv) << endmsg;
      }
      
    } // end iter loop

    int multFitPv =0;
    int multFitPvQuality =0;
    for ( itMyTr = myTracks.begin(); itMyTr != myTracks.end(); itMyTr++) {
      if (itMyTr->pvweight>0.01 && itMyTr->chi2 < 25. ) {
        multFitPv++;
        itMyTr->pvidx  = pvidx;
      }
      if (itMyTr->pvweight>0.01 && itMyTr->chi2 < 4. ) {
        multFitPvQuality++;
      }
    }

    // check if PV is close to another PV require better quality 
    bool isClose = false;
    for( itMyPv = myVertices.begin(); itMyPv != myVertices.end(); itMyPv++) {
      if ( itMyPv->multi >  multFitPv && fabs(zv - itMyPv->z) < m_dzQualityPV ) {
        isClose = true;
      }
    } 

    // store PV
    int checkMult = multFitPv;
    int multCut = m_minMult;
    if (isClose) {
      checkMult = multFitPvQuality;
      multCut = m_multiQualityPV;
    }
    if( checkMult >= multCut) {
      myVertex tmpVertex;
      tmpVertex.x = xv;
      tmpVertex.y = yv;
      tmpVertex.z = zv;
      tmpVertex.sigmax = sigmaxv;
      tmpVertex.sigmay = sigmayv;
      tmpVertex.sigmaz = sigmazv;
      tmpVertex.multi = multFitPv;
      // calculate chi2
      double chi2 = 0.;
      for ( itMyTr = myTracks.begin(); itMyTr != myTracks.end(); itMyTr++) {
        if ( itMyTr->pvidx == pvidx ) {

          double z0   = itMyTr->z0;
          double tr   = itMyTr->tr;
          double d2r0 = itMyTr->d2r0;
          double d2tr = itMyTr->d2tr;

          //check wether it is compatible with zv:
          double cosa = itMyTr->cosPhi;
          double sina = itMyTr->sinPhi;
          double rvpr = xv * cosa + yv * sina; // projection of the pv point
          double r   = tr * (zv-z0);  //r at primary vertex position
          double d2r = d2r0 + (zv-z0) * (zv-z0) * d2tr;
          double dist2   = ( r - rvpr ) * ( r - rvpr );
          chi2 += dist2/d2r;

        }
      }
      tmpVertex.chi2 = chi2;
      myVertices.push_back(tmpVertex);
    }
    itzseed++;
  } // end while

  // store to PatVertexContainer
  if ( msgLevel( MSG::DEBUG ) ) debug() << " L1 PV vertices: " << endmsg;

  int kpv = 0;
  int savedpv = 0 ;
  for( itMyPv = myVertices.begin(); itMyPv != myVertices.end(); itMyPv++) {
    kpv++;

    // require backward tracks for low multiplicity vertices close to 
    // other vertex
    bool badPV = false;
    if(  myVertices.size() > 1 && itMyPv->multi < m_multiQualityPV) {
      std::vector<myVertex>::iterator itMyPvOther;
      for( itMyPvOther = myVertices.begin(); itMyPvOther != myVertices.end();
           itMyPvOther++) {
        if( itMyPvOther != itMyPv) {
          double dz = itMyPvOther->z - itMyPv->z;
          if ( dz > 0.0 && fabs(dz) < m_dzQualityPV ) {
            // count backward tracks
            int nBack = 0;
            for ( itMyTr = myTracks.begin(); itMyTr != myTracks.end();
                  itMyTr++) {
              if (itMyTr->backward )  nBack++;
            }
            if(nBack < m_minBackQualityPV) badPV = true;
          }
        }
      }
    }

    if(badPV) continue;
    savedpv++ ;

    LHCb::RecVertex* trVtx = new LHCb::RecVertex();
    trVtx->clearTracks();

    Gaudi::XYZPoint pos( itMyPv->x, itMyPv->y, itMyPv->z);
    trVtx->setPosition(pos);
    Gaudi::SymMatrix3x3 cov;
    cov(0,0) = gsl_pow_2(itMyPv->sigmax);
    cov(1,1) = gsl_pow_2(itMyPv->sigmay);
    cov(2,2) = gsl_pow_2(itMyPv->sigmaz);
    trVtx->setCovMatrix(cov);

    for ( itMyTr = myTracks.begin(); itMyTr != myTracks.end();
          itMyTr++) {
      if (itMyTr->pvidx == kpv){
        trVtx->addToTracks(itMyTr->pTr2d);
      }
    }

    trVtx->setNDoF(itMyPv->multi-3);
    trVtx->setChi2(itMyPv->chi2);
    m_outputVertices->insert(trVtx);

    if ( msgLevel( MSG::DEBUG ) ) {
      debug() << format( "Vtx #%2i :  X %7.3f +-%5.3f  Y %7.3f +-%5.3f  Z %9.3f +-%5.3f Mult%3i",
                         kpv, itMyPv->x, itMyPv->sigmax, itMyPv->y, itMyPv->sigmay, 
                         itMyPv->z, itMyPv->sigmaz, itMyPv->multi)
              << endmsg;
    }
  }
  setFilterPassed(savedpv>0);

  return StatusCode::SUCCESS;
};


void PatPV2D:: storeDummyVertices(std::vector<double> &zseeds) {
 
  std::vector<double>::iterator itzseed;
  for (itzseed = zseeds.begin(); itzseed != zseeds.end(); itzseed++) {
    LHCb::RecVertex* trVtx = new LHCb::RecVertex();
    trVtx->clearTracks();

    Gaudi::XYZPoint pos(0.,0.,*itzseed);
    trVtx->setPosition(pos);
    Gaudi::SymMatrix3x3 errMat;
    trVtx->setCovMatrix(errMat);
    trVtx->setNDoF(1);
    trVtx->setChi2(99999.);
    std::vector<LHCb::Track*>::const_iterator itMyTr;
    for(itMyTr = m_inputTracks->begin(); itMyTr != m_inputTracks->end();
        itMyTr++) {
      LHCb::Track* pTr2d = (*itMyTr);
      double zFirst  = pTr2d->firstState().z();
      double rFirst  = pTr2d->firstState().x();
      double trFirst = pTr2d->firstState().tx();
      double z0      = zFirst - rFirst/trFirst;
      if(fabs(z0 - *itzseed) < 3.0*Gaudi::Units::mm) {
        trVtx->addToTracks(*itMyTr);
      }
    }
    m_outputVertices->insert(trVtx);

  }
}

void PatPV2D::errorForPVSeedFinding(double tx, double ty, double &sigz2) {

  double pMean = 3000.;

  double tanTheta2 = tx * tx + ty * ty;
  double sinTheta2 =  tanTheta2 / ( 1. + tanTheta2 );

  // assume that first hit in VD ad 8 mm
  double distr = 8.*Gaudi::Units::mm;
  double dist2        = distr*distr/sinTheta2;
  double sigma_ms2    = m_scatCons * m_scatCons * dist2 / (pMean*pMean);
  double fslope2      = 0.0005*0.0005;
  double sigma_slope2 = fslope2*dist2;

  sigz2 = (sigma_ms2 + sigma_slope2) / sinTheta2; 

}



//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatPV2D::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
