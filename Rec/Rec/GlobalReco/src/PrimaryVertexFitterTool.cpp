// $Id: PrimaryVertexFitterTool.cpp,v 1.1 2002-07-11 16:24:20 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartIF.h"

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
MyVertex* PrimaryVertexFitterTool::fitter(std::vector<MyTrack> m_tr, 
                                           int maxIteration,
                                           double chi2min,
                                           std::string m_fitModel,
                                           int m_minNumberOfTracks)
{
  StatusCode sc = StatusCode::SUCCESS;
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << " -*- PrimaryVertexFitterTool::fitter() -*-" << endreq;
  log << MSG::DEBUG 
      << "==> serching for preliminary position of vertex..."
      << endreq;  
  
  double UnitVector[3];
  int Iteration;
  std::string fitModel = m_fitModel;
  int minNumberOfTracks = m_minNumberOfTracks;
  int NumberOfTracksUsed;
  
  double chi2;

  // now preliminary position of PV    
  double SumW;
  double SumWZ;
  double SlXSq, SlYSq;
  double WX, WY, ZintX, ZintY;
  
  WX = WY = ZintX = ZintY = SlXSq = SlXSq = 0.0;
  SumW = SumWZ = 0.0;
  
  std::vector<MyTrack>::iterator itr;
  MyVertex* vtx = new MyVertex;

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
  
  for (itr = mtr.begin(); itr != mtr.end(); itr++) {
    if ((*itr).isUsed == true) {
      if ( ( (*itr).Slx != 0.0 ) && ( (*itr).Sly != 0.0 ) ) {
        SlXSq = (*itr).Slx * (*itr).Slx;
        SlYSq = (*itr).Sly * (*itr).Sly;

        WX = SlXSq / (1.0 + SlXSq);
        WY = SlYSq / (1.0 + SlYSq);

        ZintX = (*itr).Z - (*itr).X * (*itr).Slx;
        ZintY = (*itr).Z - (*itr).Y * (*itr).Sly;

        SumW += WX + WY;
        SumWZ += (WX * ZintX) + (WY * ZintY);
      }      
    }
  }
  
  if (SumW == 0.0) {
    log << MSG::ERROR
        << " SumW = 0.0 ==> No tracks found  - exiting..."
        << endreq;
    return vtx;
  } else {
    vtx->X = 0.0;
    vtx->Y = 0.0;
    vtx->Z = SumWZ/SumW;  
    
    log << MSG::DEBUG << "* * * Preliminary PV position * * *" << endreq;
    log << MSG::DEBUG << "[ " 
        << vtx->X << " , " 
        << vtx->Y << " , "
        << vtx->Z << " ] " << endreq;
  }

  ///< so... let's do it - mail minimalisation loop 
  double chi2old; ///< 
  chi2 = 10.0e10; ///< in the beginning a big one
  Iteration = 0;  ///<  in the begginnig God said...
  
  double Hessian[3][3]; ///< H hessian matrix
  double VD0[3];        ///< vector of D0
  double Cov[3][3];     ///< cov matrix = H^-1 
  HepVector3D DeltaPV;    ///< to update PV position
  
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
  while (chi2 > chi2min) {
 
    ///< check that there is more then 2 tracks to fit vertex
    if (NumberOfTracksUsed < minNumberOfTracks) {
      log << MSG::INFO
          << "==> Too few tracks left !"
          << endreq;
      log << MSG::INFO
          << "==> Failed to find PV position..."
          << endreq;
      vtx = NULL;
      return vtx;
    } // too few tracks left
    
    ++Iteration; ///< updating iteration counter

    log << MSG::DEBUG << "...begin of swimming..." << endreq;
    log << MSG::DEBUG 
        << "...mtr size before swimming #" << mtr.size() 
        << " ..." << endreq;
    
    for (itr = mtr.begin(); itr != mtr.end(); itr++) {
      if ((*itr).isUsed == true) {

        MyTrack wtra;

        // read all parameters
        wtra.X = (*itr).X;
        wtra.Y = (*itr).Y;
        wtra.Z = (*itr).Z;
        wtra.Slx = (*itr).Slx;
        wtra.Sly = (*itr).Sly;
        wtra.nBin = (*itr).nBin;
        wtra.errd0 = (*itr).errd0;

        wtra.unitVect[0] = (*itr).unitVect[0];
        wtra.unitVect[1] = (*itr).unitVect[1];
        wtra.unitVect[2] = (*itr).unitVect[2];

        wtra.vd0[0] = (*itr).vd0[0];
        wtra.vd0[1] = (*itr).vd0[1];
        wtra.vd0[2] = (*itr).vd0[2];
        

        wtra.Pt = (*itr).Pt;
        wtra.chi2 = (*itr).chi2;
        wtra.NbDeg = (*itr).NbDeg;
        wtra.idPart = (*itr).idPart;
        wtra.isUsed = true;
        
        log << MSG::DEBUG << " x  y z " 
            << wtra.X << " " << wtra.Y << " " << wtra.Z
            << " isUsed " << wtra.isUsed << endreq;
        
        // now swimming cov matrix from wtra.Z to vtx->Z
        double dz;
        log << MSG::DEBUG << "vtx x " << vtx->X 
            << " y " << vtx->Y
            << " z " << vtx->Z << endreq;

        dz = wtra.Z - vtx->Z;
        log << MSG::DEBUG << "------------------------------" << endreq;
        log << MSG::DEBUG << " dz " << dz << endreq;

        HepSymMatrix tmpCov(5,1);
        for (j = 0; j < 6; j++) {
          for (k = 0; k < 6; k++) {    
            tmpCov[j][k] = (*itr).Cov[j][k];
          }
        }
        wtra.Cov = tmpCov;
        
        //covariant matrix
        // Vxx
        tmpCov[0][0] = wtra.Cov[0][0]
          + (2 * dz * wtra.Cov[0][2])
          + (dz * dz * wtra.Cov[2][2]);
        // Vxx'
        tmpCov[0][2] = wtra.Cov[0][2]
          + (dz * wtra.Cov[2][2]);
        // Vyy
        tmpCov[1][1] = wtra.Cov[1][1]
          + (2 * dz * wtra.Cov[1][3])
          + (dz * dz * wtra.Cov[3][3]);
        // Vyy'
        tmpCov[1][4] = wtra.Cov[1][3]
          + (dz * wtra.Cov[3][3]);
        // Vxy
        tmpCov[0][1] = wtra.Cov[0][1] 
          + (dz * (wtra.Cov[0][3] + wtra.Cov[1][2]))
          + (dz * dz * wtra.Cov[2][3]);

        tmpCov[1][0] = tmpCov[0][1];
        tmpCov[2][0] = tmpCov[2][0];
        tmpCov[3][1] = tmpCov[1][3];

        for (j = 0; j < 5; j++) {
          log << MSG::DEBUG 
              << "--------------------------------------" << endreq;
          log << MSG::DEBUG 
              << tmpCov[j][0] << " | " 
              << tmpCov[j][1] << " | "
              << tmpCov[j][2] << " | "
              << tmpCov[j][3] << " | "
              << tmpCov[j][4] << endreq; 
        }
        
        HepSymMatrix tmpinv(5,1);
        tmpinv = tmpCov; // copy Cov matrix
        tmpinv.invert(j); // invert it
        /// we should check if it works...
        for (j = 0; j < 5; j++) {
          log << MSG::DEBUG 
              << "--------------------------------------" << endreq;
          log << MSG::DEBUG 
              << tmpinv[j][0] << " | " 
              << tmpinv[j][1] << " | "
              << tmpinv[j][2] << " | "
              << tmpinv[j][3] << " | "
              << tmpinv[j][4] << endreq; 
        }

        // x y in new z plane
        double X = wtra.X + (dz * wtra.Slx);
        double Y = wtra.Y + (dz * wtra.Sly);
        double Z = vtx->Z;
        
        HepVector3D tmpvd0;
        double tmpd0;
        // VectD0
        tmpvd0.set( wtra.X - vtx->X, 
                    wtra.Y - vtx->Y, 
                    0.0 );

        tmpd0 = sqrt((wtra.vd0(0) * wtra.vd0(0)) + (wtra.vd0(1) * wtra.vd0(1)));        

        wtra.d0 = sqrt(wtra.X * wtra.X + wtra.Y * wtra.Y);
        double ex = wtra.X  / wtra.d0;
        double ey = wtra.Y  / wtra.d0;
        wtra.errd0 =  sqrt(  (ex * ex * tmpCov[0][0]) 
                             + (ey * ey * tmpCov[1][1]));
        
        double chisqx, chisqy, chisqmix;
        double xold, yold, xnew, ynew;

        xold = wtra.X;
        yold = wtra.Y;
        xnew = wtra.X + dz * wtra.Slx;
        ynew = wtra.Y + dz * wtra.Sly;
        
        chisqx = pow((xold - xnew), 2) * tmpinv[0][0];
        chisqy = pow((yold - ynew), 2) * tmpinv[1][1];
        
        chisqmix = 2 * (xold - xnew) * (yold - ynew) * tmpinv[1][0];

        wtra.chi2 = chisqx + chisqy + chisqmix;
        
        wtra.X = X;
        wtra.Y = Y;
        wtra.Z = Z;

        for (j = 0; j < 6; j++) {
          for (k = 0; k < 6; k++) {
            wtra.Cov[j][k] = tmpCov[j][k];
          }
        }

        for (j = 0; j< 3 ; j++) {
          wtra.vd0[j] = tmpvd0[j];
        }
        
        // erase old parameters
        mtr.erase(itr);
        // insert new parameters  
        mtr.insert(itr, wtra);
         
      }
    } //< end of swimming Cov matrix

    NumberOfTracksUsed = 0;
    
    for (itr = mtr.begin(); itr != mtr.end(); itr++) {
      if ((*itr).isUsed == true ) {
        NumberOfTracksUsed++;
      } 
    }

    log << MSG::DEBUG << "Nb of Used tracks if fit #" 
        << NumberOfTracksUsed << endreq;

    ///< now compute VD0 and Hessian
    for (itr = mtr.begin(); itr != mtr.end(); itr++) {
      if ((*itr).isUsed == true) {

        double IP = (*itr).d0;
        double ErrIP = (*itr).errd0;
        double IPSq = pow(IP,2);
        double ErrIPSq = pow(ErrIP,2);
        double component3 = 1.0 / 
          (sqrt( 1.0 + (*itr).Slx * (*itr).Slx + (*itr).Sly * (*itr).Sly));
        double component1 = (*itr).Slx * component3;
        double component2 = (*itr).Sly * component3;

        UnitVector[0] = component1;
        UnitVector[1] = component2;
        UnitVector[2] = component3;
        // sum VD0 vector
        for (k = 0; k < 3; k++) {
          if (fitModel == "Gauss") {
            VD0[k] +=
              (2.0 / ErrIPSq) * (*itr).vd0(k) * 2.0 / (1.0 + IPSq/ErrIPSq);
          } else { // fitModel == "Breit-Wigner"
            VD0[k] += 2.0 * (*itr).vd0(k)/ ErrIPSq;
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
              Hessian[l][k] += 
                (2.0 / ErrIPSq ) * (delta_kl - UnitVector[k]*UnitVector[l]);
            } else { // m_fitModel == "Breit-Wigner"
              Hessian[l][k] +=
                ErrIPSq * (delta_kl - UnitVector[k]*UnitVector[l]) /
                (1.0 + (*itr).chi2);
            }
          } // end of countig Hessian  
        } 
      } //  hessian
    }
    
    log << MSG::DEBUG << " UnitVector x " << UnitVector[0] 
        << " y " << UnitVector[1] 
        << " z " << UnitVector[2] << endreq;
    
    
    // invert hessian - get covariance matrix
    double determinant;
    double r00,r01,r02;
    
    r00 = Hessian[1][1]*Hessian[2][2] - Hessian[2][1]*Hessian[1][2];
    r01 = Hessian[2][1]*Hessian[0][2] - Hessian[0][1]*Hessian[2][2];
    r02 = Hessian[0][1]*Hessian[1][2] - Hessian[1][1]*Hessian[0][2];
    
    
    determinant = r00 * Hessian[0][0]
      + r01 * Hessian[1][0]
      + r02 * Hessian[2][0];        
    
    if (determinant != 0.0) {
      Cov[0][0] = r00 / determinant;
      
      Cov[1][1] = 
        (Hessian[0][0]*Hessian[2][2] - Hessian[2][0]*Hessian[0][2]) 
        / determinant;
      
      Cov[2][2] = 
        (Hessian[0][0]*Hessian[1][1] -  Hessian[1][0]*Hessian[0][1]) 
        / determinant;
      
      Cov[0][1] =
        (Hessian[2][0]*Hessian[1][2] - Hessian[1][0]*Hessian[2][2])
        / determinant;
      
      Cov[1][0] = r01 / determinant;
        
      Cov[0][2] = 
        (Hessian[1][0]*Hessian[2][1] - Hessian[2][0]*Hessian[1][1])
        / determinant;
      
      Cov[2][0] = r02 / determinant;
      
      Cov[1][2] = 
        (Hessian[2][0]*Hessian[0][1] - Hessian[0][0]*Hessian[2][1])
        / determinant;
      
      Cov[2][1] = 
          (Hessian[1][0]*Hessian[0][2] - Hessian[0][0]*Hessian[1][2])
        / determinant;
        
    } else {
      log << MSG::DEBUG
          << "* * * det Hess = 0.0 - failed to  invese matrix * * *"
          << endreq;
      vtx = NULL;
      return vtx;
    }
    
    // now clear DeltaPV
    DeltaPV[0] = DeltaPV[1] = DeltaPV[2] = 0.0;
      
    // solve DeltaPV[] = Cov[][] * VD0[]
    for (k = 0; k < 3; k++) {
      for (l = 0; l < 3; l++) {
          DeltaPV[k] += Cov[k][l] * VD0[l];
      }
    } // end of solving

    log << MSG::DEBUG << "VD0 " << VD0[0] 
        << " " << VD0[1]
        << " " << VD0[2] << endreq;

    log << MSG::DEBUG << "DeltaPV " << DeltaPV[0] 
        << " " << DeltaPV[1]
        << " " << DeltaPV[2] << endreq;
    
    // now update position of PV
    vtx->X += DeltaPV[0];
    vtx->Y += DeltaPV[1];
    vtx->Z += DeltaPV[2];
    // end of updating PV
    
    //updating chi2 & rejecting tracks with biggest chi2
    chi2 = 0.0;
    std::vector<MyTrack>::iterator reject;
    
    for (itr = mtr.begin(); itr != mtr.end(); itr++) {
      if ((*itr).isUsed == true) {
        chi2 += (*itr).chi2;   
      }      
    }
    
    double diff = fabs(chi2old - chi2);

    log << MSG::DEBUG << "Current chi2 #" << chi2 << endreq;
    log << MSG::DEBUG << "Last chi2 #" << chi2old << endreq;
    log << MSG::DEBUG << "Diff #" << diff << endreq;
    
    if (diff <= 0.01) {
      log << MSG::INFO << "Ok, we have it!" << endreq;
      goto lab;
    } else {
      chi2old = chi2;
    }

    if (chi2 > chi2min) { 
      
      double chi2max; ///<  temp for biggest chi2 of single track 
      chi2max = 0.0;

      for (itr = mtr.begin(); itr !=  mtr.end(); itr++) {
        // MyTrack wtra = (*itr);
        if ((*itr).isUsed == true) {   
          if (chi2max < (*itr).chi2) { 
            chi2max = (*itr).chi2;
            reject = itr;      
          }
        }
      }
      log << MSG::DEBUG << "Reject tracks chi2 #"
          << (*reject).chi2 << endreq;

      MyTrack wtra;
      wtra.isUsed = false;

      mtr.erase(reject);
      mtr.insert(reject, wtra);
      NumberOfTracksUsed--; 
    }
    
    
    ///< now printing some info
    log << MSG::INFO << "####################################" << endreq;
    log << MSG::INFO << "   Iteration nr = " << Iteration << endreq;
    log << MSG::DEBUG << " Chi^2 minimal value = " << chi2min << endreq;
    log << MSG::INFO << " Chi^2 current value = " << chi2 << endreq;
    
    log << MSG::INFO << " Vertex position:" << endreq;
    log << MSG::INFO 
        << " [ " << vtx->X 
        << " , " << vtx->Y
        << " , " << vtx->Z 
        << " ]" << endreq;
    
    log << MSG::INFO << " Nr of tracks used to fit = "
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
      log << MSG::INFO
          << "===> Maximal iteration reached - exiting !"
          << endreq;
      log << MSG::INFO
          << "* * *  Please, increase MaxIteration in your options file * * *"
          << endreq;
      vtx = NULL;
      return vtx;
    } // max iteration reached -> PV NOT FOUND !!!
    
  };
  //////////////////////////////////////////////////////////////////////
  /// end of main minimalistion loop
  /////////////////////////////////////////////////////////////////////
  
 lab:
  
  log << MSG::INFO << " PV last position" << endreq;
  log << MSG::INFO << " [ " 
      << vtx->X << " , " 
      << vtx->Y << " , "
      << vtx->Z << " ] " << endreq;
  log << MSG::DEBUG << "==> It will be added to TES" << endreq;
  vtx->chi2 = chi2;
  HepSymMatrix tmpCov(3,1);

  for (i = 0; i < 3 ; i++) {  
    for (j = 0; j < 3; j++) {
      tmpCov[i][j] = Cov[i][j];
    }
  }
  
  vtx->Cov = tmpCov;
  vtx->NbDeg = 2 *  NumberOfTracksUsed - 3;

  std::vector<MyTrack>::iterator it;

  for (it = m_tr.begin(); it != m_tr.end(); it++) {
    if ((*it).isUsed == true) {
      vtx->tracks.push_back((*it).track);
    }    
  }
  
  log << MSG::DEBUG << "...exiting from PVFitterTool" << endreq;
  return vtx;
  
}
//=============================================================================
