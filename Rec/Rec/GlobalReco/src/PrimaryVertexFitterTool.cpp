// $Id: PrimaryVertexFitterTool.cpp,v 1.3 2002-10-22 21:56:38 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartIF.h"
#include "CLHEP/Units/PhysicalConstants.h"
// local
#include "PrimaryVertexFitterTool.h"



//-----------------------------------------------------------------------------
// Implementation file for class : PrimaryVertexFitterTool
//
// 03/04/2002 : Krzysztof Ciba
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static   ToolFactory<PrimaryVertexFitterTool>          s_factory ;
const    IToolFactory& PrimaryVertexFitterToolFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrimaryVertexFitterTool::PrimaryVertexFitterTool( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : AlgTool ( type, name , parent ) {

  declareInterface<IPrimaryVertexFitterTool>(this);
}
//=============================================================================

//=============================================================================
//MyVertex* PrimaryVertexFitterTool::fitter(std::vector<MyTrack> m_tr, 
StatusCode PrimaryVertexFitterTool::fitter(std::vector<MyTrack> m_tr, 
                                           int maxIteration,
                                           double chi2min,
                                           std::string m_fitModel,
                                           int m_minNumberOfTracks,
                                           MyVertex* vtx)
{
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << " -*- PrimaryVertexFitterTool::fitter() -*-" << endreq;
  log << MSG::DEBUG 
      << "==> serching for preliminary position of vertex..."
      << endreq;  
  
  HepVector3D UnitVector;
  int Iteration;
  std::string fitModel = m_fitModel;
  int minNumberOfTracks = m_minNumberOfTracks;
  int NumberOfTracksUsed = 0;
  double chi2;

  // now preliminary position of PV    
  double sumw, sumwz, wx, wy, zintx, zinty;
  
  wx = wy = zintx = zinty = 0.0;
  sumw = sumwz = 0.0;
  
  std::vector<MyTrack>::iterator itr;
  //  MyVertex* vtx = new MyVertex;

  std::vector<MyTrack> mtr;

  if (0 != mtr.size()) {
    mtr.clear();    
  }
  
  for (itr = m_tr.begin(); itr != m_tr.end(); itr++) {
    
    MyTrack wtra = (*itr);
    
    mtr.push_back(wtra);  
    if (wtra.isUsed == true) {
      NumberOfTracksUsed++;
    }    
  }
  
  int i,j,k,l;

 start:
  
  for (itr = mtr.begin(); itr != mtr.end(); itr++) {
    if ((*itr).isUsed == true) {
      if ( ( (*itr).slope.x() != 0.0 ) 
           && ( (*itr).slope.y() != 0.0 ) ) {

        double uvx = (*itr).unitVect.x();
        double uvy = (*itr).unitVect.y();
        
        double uvxsq = pow(uvx, 2);
        double uvysq = pow(uvy, 2);

        wx = uvxsq / (1.0 + uvxsq);
        wy = uvysq / (1.0 + uvysq);
 
        zintx = - (*itr).pos.x() / (*itr).slope.x() + (*itr).pos.z();
        zinty = - (*itr).pos.y() / (*itr).slope.y() + (*itr).pos.z();
        
        sumw += wx + wy;
        sumwz += (wx * zintx) + (wy * zinty);
      }      
    }
  }
  

  if (sumw == 0.0) {
    log << MSG::ERROR
        << " sumw = 0.0 ==> No tracks found  - exiting..."
        << endreq;
    //    return vtx;
    return StatusCode::FAILURE;
  } else {
    HepVector3D pos(0.0, 0.0, sumwz/sumw);
    vtx->pos = pos;
    log << MSG::INFO << "pre pos of vtx " << vtx->pos << endreq;
  }


  ///< so... let's do it - mail minimalisation loop 
  double chi2old = 10.0e9; ///< 
  chi2 = 10.0e7; ///< in the beginning a big one


  HepMatrix Hessian(3,3); ///< H hessian matrix
  HepMatrix Cov(3,3);     ///< cov matrix of vertex fit
  HepVector VD0(3);        ///< vector of D0
  HepVector DeltaPV(3);    ///< to update PV position
  
  // just in case clear all  matrices and vectors
  for (i = 0; i < 3; i++) {
    VD0[i] = 0.0;
    DeltaPV[i] = 0.0;
    for (j = 0; j < 3; j++){
      Hessian[i][j] = 0.0;
      Cov[i][j] =  0.0;
    }
  }
  
  if (fitModel == "Gauss") {
    chi2min = 3.0; ///< Gauss
  } else {
    chi2min = 5.0; ///< Breit-Wigner
  }
  
  ///////////////////////////////////////////////////////////////////////
  ///
  ///< main minimalization loop
  ///
  /////////////////////////////////////////////////////////////////////
  for (Iteration = 0; Iteration < 5; Iteration++  ) { 

    ///< check that there is more then 2 tracks to fit vertex
    if (NumberOfTracksUsed < minNumberOfTracks) {
      log << MSG::INFO
          << "==> Too few tracks left !"
          << endreq;
      log << MSG::INFO
          << "==> Failed to find PV position..."
          << endreq;
      vtx = NULL;
      //      return vtx;
      return StatusCode::FAILURE;
    } // too few tracks left

    for (itr = mtr.begin(); itr != mtr.end(); itr++) {
      if ((*itr).isUsed == true) {
        
        MyTrack* wtra = itr;
        
        log << MSG::DEBUG 
            << "tr  " << wtra->pos 
            << " isUsed " << wtra->isUsed << endreq;
        
        // now swimming cov matrix from wtra.Z to vtx->Z
        double dz;
        
        dz =  vtx->pos.z() - wtra->pos.z();
        log << MSG::DEBUG << "swimming dz = " << dz << endreq;
        
        HepSymMatrix tmpCov(5,1);
        for (j = 0; j < 6; j++) {
          for (k = 0; k < 6; k++) {    
            tmpCov[j][k] = wtra->Cov[j][k];
          }
        }
        
        log << MSG::DEBUG << wtra->Cov << endreq;

        //covariant matrix
        // Vxx
        tmpCov[0][0] = wtra->Cov[0][0]
          + (2 * dz * wtra->Cov[0][2])
          + (dz * dz * wtra->Cov[2][2]);
        // Vxx'
        tmpCov[0][2] = wtra->Cov[0][2]
          + (dz * wtra->Cov[2][2]);
        // Vyy
        tmpCov[1][1] = wtra->Cov[1][1]
          + (2 * dz * wtra->Cov[1][3])
          + (dz * dz * wtra->Cov[3][3]);
        // Vyy'
        tmpCov[1][4] = wtra->Cov[1][3]
          + (dz * wtra->Cov[3][3]);
        // Vxy
        tmpCov[0][1] = wtra->Cov[0][1] 
          + (dz * (wtra->Cov[0][3] + wtra->Cov[1][2]))
          + (dz * dz * wtra->Cov[2][3]);

        tmpCov[1][0] = tmpCov[0][1];
        tmpCov[2][0] = tmpCov[2][0];
        tmpCov[3][1] = tmpCov[1][3];

        log << MSG::DEBUG << "cov mat after swimming # " << endreq;
        log << MSG::DEBUG << tmpCov << endreq;


        ///< attemp o use he best formula o count chi2 
        double chisqx, chisqy, chisqmix;
        double xold, yold, xnew, ynew;
        
        xold = wtra->pos.x();
        yold = wtra->pos.y();
        xnew = wtra->pos.x() + dz * wtra->slope.x();
        ynew = wtra->pos.y() + dz * wtra->slope.y();
        
        chisqx = pow((xold - xnew), 2) / tmpCov[0][0];
        chisqy = pow((yold - ynew), 2) / tmpCov[1][1];        
        chisqmix = 2 * (xold - xnew) * (yold - ynew) / tmpCov[1][0];

        wtra->chi2 = (chisqx + chisqy + chisqmix);

        log << MSG::DEBUG << "chi2 from LHC-b ASL #" << wtra->chi2  << endreq;

        HepVector3D DiffV = wtra->pos - vtx->pos;
        
        log << MSG::DEBUG << "start point  " << wtra->pos << endreq;
        log << MSG::DEBUG << "vtx pos " << vtx->pos << endreq;
        log << MSG::DEBUG << "DiffV " << DiffV << endreq;
       
        wtra->vd0 = wtra->unitVect.cross(DiffV.cross(wtra->unitVect)); 

        // if (wtra->vd0.z() < 0.0) {
        //  wtra->vd0 = -wtra->vd0;
        // }
        
        log << MSG::DEBUG << "vd0 " << wtra->vd0 << endreq;
        
        wtra->d0 = wtra->vd0.mag(); 

        log << MSG::DEBUG << "d0 " << wtra->d0 << endreq;

        // d0 err
        
        double err_geom; // = 0.02;
        double Z;
        
        if (wtra->pos.z() != 0.0) {
          Z = fabs(vtx->pos.z() - wtra->pos.z());
        } else {
          Z = wtra->Z;
        }
        
        double zl = Z / wtra->L;
        double zll = (Z + wtra->L) / wtra->L;
        
        double zlsq = pow(zl, 2);
        double zllsq = pow(zll, 2);
        
        err_geom = 0.0146 * sqrt(zlsq + zllsq);

        double err_scat;
        double CosTheta = wtra->unitVect.z();
        double X0 = (double)wtra->nbSta/(fabs(CosTheta));
        double DistFromPV = fabs((wtra->pos.z() - vtx->pos.z()))
          / fabs(CosTheta);
      
        err_scat = 0.0085 * 100.0 * 1000.0 * sqrt(X0) * 
          DistFromPV / wtra->p / GeV;

        log << MSG::INFO
            << " err_geom " << err_geom 
            << " err_scat " << err_scat 
            << endreq;

        wtra->errd0 = sqrt(err_geom * err_geom + err_scat * err_scat);

        log << MSG::INFO 
            << "do # " << wtra->d0 
            << " err # " << wtra->errd0 << endreq;
        
        double frac = pow(wtra->d0 / wtra->errd0, 2);
        wtra->chi2 = frac;

        log << MSG::INFO << "chi2 of track " << wtra->chi2 << endreq;
        
        if (wtra->chi2 >= (18.0)) {
          log << MSG::INFO << "...get rid of  track!" << endreq;
          wtra->isUsed = false;
          goto start;
        }      
      }
    } //< end of swimming Cov 

    NumberOfTracksUsed = 0;
    for (itr = mtr.begin(); itr != mtr.end(); itr++) {
      if ((*itr).isUsed == true ) {
        NumberOfTracksUsed++;
      } 
    }

    log << MSG::DEBUG << "Nb of Used tracks if fit #" 
        << NumberOfTracksUsed << endreq;

    //  clear all  matrices and vectors
    for (i = 0; i < 3; i++) {
      VD0[i] = 0.0;
      DeltaPV[i] = 0.0;
      for (j = 0; j < 3; j++){
        Hessian[i][j] = 0.0;
        Cov[i][j] =  0.0;
      }
    }
    ///< now compute VD0 and Hessian
    for (itr = mtr.begin(); itr != mtr.end(); itr++) {
      MyTrack* wtra = itr;
      if (wtra->isUsed == true) {
        double IP = wtra->d0;
        double ErrIP = wtra->errd0;
        double IPSq = pow(IP, 2);
        double ErrIPSq = pow(ErrIP, 2);
        UnitVector = wtra->unitVect;

        // sum VD0 vector
        for (k = 0; k < 3; k++) {
          if (fitModel == "Gauss") {
            VD0[k] += (2.0 / ErrIPSq) * wtra->vd0(k) 
              * 2.0 / (1.0 + IPSq/ErrIPSq);
          } else { // fitModel == "Breit-Wigner"
            VD0[k] += 2.0 * wtra->vd0(k)/ ErrIPSq;
          }
        }
        
        double delta_kl;
        // prepare Hessian
        for (k = 0; k < 3; k++) {
          for (l = 0; l < 3; l++ ) {
            if (k == l) { // delta_kl
              delta_kl = 1.0;
            } else {
              delta_kl = 0.0;
            } // end of delta_kl
            
            if (fitModel == "Gauss") {
              Hessian[l][k] += (2.0 / ErrIPSq ) 
                * (delta_kl - UnitVector[k]*UnitVector[l])
                * 2.0/(1.0 + wtra->chi2);
            } else { // m_fitModel == "Breit-Wigner"
              Hessian[l][k] +=
                ErrIPSq * (delta_kl - UnitVector[k]*UnitVector[l]) /
                (1.0 + wtra->chi2);
            }
          } // end of counting Hessian  
        } 
      } //  hessian
    }

    //< find out DeltaPV
    DeltaPV = solve(Hessian , VD0);
    log << MSG::DEBUG << "DeltaPV " << DeltaPV << endreq;
    // update PV  position
    vtx->pos[0] +=  DeltaPV[0];
    vtx->pos[1] +=  DeltaPV[1];
    vtx->pos[2] +=  DeltaPV[2];
    
    // invert hessian - get covariance matrix (error)
    Cov = Hessian;
    Cov.invert(j);   

    ///<updating chi2 & rejecting tracks with biggest chi2
    chi2 = 0.0;
//      std::vector<MyTrack>::iterator reject;    
    for (itr = mtr.begin(); itr != mtr.end(); itr++) {
      if ((*itr).isUsed == true) {
        chi2 += (*itr).chi2;   
      }      
    }
    chi2 /= (NumberOfTracksUsed-3);
    

    double diff = chi2old - chi2;

    log << MSG::INFO << "Current chi2 #" << chi2 << endreq;
    log << MSG::INFO << "Last chi2 #" << chi2old << endreq;
    log << MSG::INFO << "Diff #" << diff << endreq;
    
    if ((diff > 0.0)&&(diff<=0.1)) { //< end of analysis
      log << MSG::INFO << "Ok, we have it!" << endreq;
      goto lab;
    } else 
      if ((diff > 0.0)&&(diff > 0.1)) { ///< just do next iteration
        chi2old = chi2;
        chi2min = chi2 / (NumberOfTracksUsed - 3);
        continue;
      };

    //log << MSG::INFO << "chi2min " << chi2min << endreq;    
    //double chi2max; ///<  temp for biggest chi2 of single track 
    //chi2max = 0.0;
    //    for (itr = mtr.begin(); itr !=  mtr.end(); itr++) {   
    //  if ((*itr).isUsed == true) { 
    //    if (chi2max < (*itr).chi2) { 
    //      chi2max = (*itr).chi2;
    //      reject = itr;
          // (*itr).isUsed = false;
    //    }
    //  }  
    // }
    
    //reject.isUsed = false;
    //log << MSG::INFO << "Reject tracks chi2 #"
    //    << (*reject).chi2 << endreq;
  
    
  
    

    ///< now printing some info
    log << MSG::DEBUG << "####################################" << endreq;
    log << MSG::INFO << "   Iteration nr = " << Iteration << endreq;
    log << MSG::DEBUG << " Chi^2 minimal value = " << chi2min << endreq;
    log << MSG::INFO << " Chi^2 current value = " << chi2 << endreq;
    log << MSG::INFO << " Vertex position:" << endreq;
    log << MSG::INFO << vtx->pos << endreq;
    log << MSG::DEBUG << " Nr of tracks used to fit = "
        << NumberOfTracksUsed+1 << endreq;
    log << MSG::DEBUG << " Covariant matrix" << endreq;
    for(i = 0; i < 3; i++) {
      log << MSG::DEBUG << "------------------------------------" << endreq;
      log << MSG::DEBUG
          << Cov[i][0] << " | "
          << Cov[i][1] << " | "
          << Cov[i][2] << endreq;
    }
    
    log << MSG::DEBUG << "#####################################" << endreq;
      
    ///< too many iterations ?
    if  (Iteration > maxIteration) {
      log << MSG::DEBUG
          << "===> Maximal iteration reached - exiting !"
          << endreq;
      log << MSG::DEBUG
          << "* * *  Please, increase MaxIteration in your options file * * *"
          << endreq;
      vtx = NULL;
      //      return vtx;
      return StatusCode::FAILURE;
    } // max iteration reached -> PV NOT FOUND !!!
    
  };
  //////////////////////////////////////////////////////////////////////
  /// end of main minimalistion loop
  /////////////////////////////////////////////////////////////////////
  
 lab:
  log << MSG::INFO << "vtx last pos " << vtx->pos << endreq;
  
  vtx->chi2 = chi2/(NumberOfTracksUsed-3);
  HepSymMatrix tmpCov(3,1);
  
  for (i = 0; i < 3 ; i++) {  
    for (j = 0; j < 3; j++) {
      tmpCov[i][j] = Cov[i][j];
    }
  }
  
  vtx->Cov = tmpCov;
  vtx->NbDeg =  NumberOfTracksUsed - 3;
  
  std::vector<MyTrack>::iterator it;
  
  for (it = m_tr.begin(); it != m_tr.end(); it++) {
    if ((*it).isUsed == true) {
      vtx->tracks.push_back((*it).track);
    }    
  }
  
  log << MSG::DEBUG << "...exiting from PVFitterTool" << endreq;
  //  return vtx;
  return StatusCode::SUCCESS;
}
//=============================================================================
