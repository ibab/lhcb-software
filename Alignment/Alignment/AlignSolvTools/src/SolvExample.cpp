// $Id: SolvExample.cpp,v 1.10 2009-08-16 14:16:24 wouter Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "SolvExample.h"

//Matrix, vector classes
#include "AlignKernel/AlMat.h"
#include "AlignKernel/AlSymMat.h"
#include "AlignKernel/AlVec.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SolvExample
//
// 2007-03-07 : Adlene Hicheur
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( SolvExample )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SolvExample::SolvExample( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
declareProperty( "SolvMethod",m_solvtoolname="SpmInvTool");
}
//=============================================================================
// Destructor
//=============================================================================
SolvExample::~SolvExample() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode SolvExample::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm


// solver tool

 m_solver = tool<IAlignSolvTool>(m_solvtoolname);

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode SolvExample::execute() {

  debug() << "==> Execute" << endmsg;

  // First: examples of vector and matrix handling....

    int Nvec=8;
  AlVec b(Nvec);
  AlVec c(Nvec);

  for (unsigned(i) = 0; i < b.size(); ++i) { 
    b[i]=i+2;
    c[i]=i*3;
    debug()<<"AlVec b value, line "<<i<<" - " <<b[i]<<endmsg;
    debug()<<"AlVec c value, line "<<i<<" - " <<c[i]<<endmsg;    
  }
  b-=c;

  debug()<<"scalar product c*d..."<< (b*c)<<endmsg;


  AlSymMat md(Nvec);
  AlSymMat ma(Nvec);

  for (unsigned(i) = 0; i < md.size(); ++i) {
    debug()<<"AlSymMat md values line "<<i<<" -- ";
    for (unsigned(j) = 0; j <= i; ++j) {
      md[i][j]= i-j;
      if (i==j) ma[i][j]=3;
      debug()<<md[i][j]<<" - ";    
    }
    debug()<< "finish line" <<endmsg;
  }


  AlSymMat mdT;
  debug()<<"before md-ma"<<endmsg;
  md+=ma;
  mdT = (md+ma);
  debug()<<"AlSymMat md+ma Size "<< mdT.size()<<endmsg;

  for (unsigned(i) = 0; i < mdT.size(); ++i) {
    debug()<<"AlSymMat mdT values line "<<i<<" -- ";
    for (unsigned(j) = 0; j < mdT.size(); ++j) {
      debug()<<mdT[i][j]<<" - ";
     }
    debug()<<endmsg;
  }

  AlVec e;
  e = md*c;
  debug()<<"md*c Nelem..."<<e.size()<<endmsg;
  debug()<<"md*c values..."<<endmsg; 
  for (unsigned(i) = 0; i <e.size(); ++i) debug()<< e[i]<<" - ";
  debug()<<endmsg;  

  AlMat me = md*md;
  AlMat mk = ma*me;
  AlMat mj = me*ma;

  for (size_t i=0;i<me.nrow();i++) {
    debug()<<"AlMat me values line "<<i<<" -- ";
    for (size_t j=0;j<me.ncol();j++) {
      debug()<<me[i][j]<<" - ";
     }
    debug()<<endmsg;
  }

  for (size_t i=0;i<mk.nrow();i++) {
    debug()<<"AlSymMat*AlMat mk=ma*me values line "<<i<<" -- ";
    for (size_t j=0;j<mk.ncol();j++) {
      debug()<<mk[i][j]<<" - ";
     }
    debug()<<endmsg;
  }

  for (size_t i=0;i<mj.nrow();i++) {
    debug()<<"AlMat*AlSymMat mj=me*ma values line "<<i<<" -- ";
    for (size_t j=0;j<mj.ncol();j++) {
      debug()<<mj[i][j]<<" - ";
     }
    debug()<<endmsg;
  }

  me*=5.;
  AlSymMat mf(me.nrow());

  for (size_t i=0;i<me.nrow();i++) {
    debug()<<"AlMat me*5 values line "<<i<<" -- ";
    for (size_t j=0;j<me.ncol();j++) {
      debug()<<me[i][j]<<" - ";
      mf[i][j]=me[i][j];
     }
    debug()<<endmsg;
  }


  //Now examples of invertion and diagonalization
  
  AlSymMat mfi(mf);
  AlSymMat msp(mf);

  

  int ierr=0;


  
    mf.invert(ierr);


    

  if (ierr==0) {

  debug()<<"AlSymMat me^-1 values line "<< endmsg;

  for (unsigned(i) = 0; i <mf.size(); ++i) {
    debug()<<"AlSymMat mf values line "<<i<<" -- ";
    for (unsigned(j) =0; j < mf.size(); ++j) {
      debug()<<mf[i][j]<<" - ";
    }
    debug()<<endmsg;
  }

  debug()<<"AlSymMat me*me^-1 values line "<< endmsg;

  for (size_t i=0;i<(mf*mfi).nrow();i++) {
    debug()<<"AlSymMat me*me^-1 values line "<<i<<" -- ";
    for (size_t j=0;j<(mf*mfi).ncol();j++) {
      debug()<<(mf*mfi)[i][j]<<" - ";
    }
    debug()<<endmsg;
  }

  }

  debug()<<"Now using indirect inversion via diagonalization..."<<endmsg;
  
  int info = 999;
  char jobz = 'V';
  //declare transition matrix + vector to store eigenvalues
  size_t dim = mf.size();
  
  AlMat z(dim,dim);
  AlVec w(dim);

  AlSymMat mfi_bis(mfi);
  

  info = mfi.diagonalize(jobz,w,z);

  if (info==0) {
    debug()<< "*** successful diagonalization ***" << endmsg;
    AlSymMat invmat(dim);
    debug()<<"print eigenvalues: "<<endmsg;
    for (unsigned(i) = 0; i < invmat.size(); ++i) {
      debug()<<w[i]<< "  " <<endmsg;
      for (unsigned(j) = 0; j <= i; ++j) {
        for (size_t k=0;k<dim;k++) invmat[i][j]=invmat[i][j]+(z[k][i]*z[k][j]/w[k]);
      }
    } 

    debug()<<"end print eigenvalues: "<<endmsg;

    for (unsigned(i) = 0; i < invmat.size(); ++i) {
      //    debug()<<"AlSymMat me*me^-1 values line "<<i<<" -- ";
      for (unsigned(j) = 0; j < invmat.size(); ++j) {
        debug()<<(me*invmat)[i][j]<<"   ";
      }
      debug()<<endmsg;
    } 
  }

  //Calling the solver

  m_solver->compute(mf,e);
  debug()<<"Printing solution of mf*X = e..."<<endmsg; 
  for (unsigned(i) = 0; i <e.size(); ++i) debug()<< e[i]<<" - ";
  debug()<<endmsg;  

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode SolvExample::finalize() {

  debug() << "==> Finalize" << endmsg;
  //finalize the tool to close it properly
  m_solver->finalize();
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
