//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information:
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtLb2plnuLQCD.cc
//
// Description: Routine to implement Lb->p l nu form factors
//              according to predictions from LQCD
//
// Modification history:
//
//   William Sutcliffe     27/07/2013        Module created
//                                      
//
//--------------------------------------------------------------------------
#include "EvtGenBase/EvtPatches.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenModels/EvtLb2plnuLQCDFF.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtId.hh"
#include "EvtGenBase/EvtIdSet.hh"
#include "EvtGenBase/EvtConst.hh"
#include <string>
#include <math.h>
#include <stdlib.h>
using std::endl;

void EvtLb2plnuLQCDFF::getdiracff(EvtId parent, EvtId daught,
				double q2, double /* mass */ , 
				double *f1, double *f2, double *f3, 
				double *g1, double *g2, double *g3 ) {

  // Define Event IDs for Lb and p 
  static EvtId LAMB=EvtPDL::getId("Lambda_b0");
  static EvtId LAMBB=EvtPDL::getId("anti-Lambda_b0");
  static EvtId PRO=EvtPDL::getId("p+");
  static EvtId PROB=EvtPDL::getId("anti-p-");


  if( (parent==LAMB && daught==PRO) 
       || (parent==LAMBB && daught==PROB) ) 
  {
      // Calculate Lb->p form factors based on LQCD predictions
      // Predictions taken from W. Detmold, C-J. Lin, S. Meinel and M.Wingate, arXiv:1306.0446 (2013)

      double MLamB = EvtPDL::getMass(parent);
      double MPro = EvtPDL::getMass(daught);
      double Y1 = 2.97;
      double X1 = 1.36;
      double Y2 = -0.28;
      double X2 = 0.81;
      double EnMn = (MLamB * MLamB + MPro*MPro - q2)/(2.0 * MLamB) - MPro;
      double F1 = Y1/( (X1 + EnMn) * (X1 + EnMn)); 
      double F2 = Y2/( (X2 + EnMn) * (X2 + EnMn)); 
      double pi = atan(1.0) * 4.0;
      double alphas = 0.214;
      double cv = 2.0/3.0 * alphas/pi; 
      double cgam  = 1.0 - (alphas/pi)* (4.0/3.0);

      *f1 = cgam*(F1 - F2);
      *f2 = cv*F1 + (2.0*cgam + cv)*F2;
      *f3 = 0.0;
      *g1 = cgam*(F1 + F2);
      *g2 = -cv*F1 + (2.0*cgam + cv)*F2;
      *g3 = 0.0;

  }
  else 
  {
  report(ERROR,"EvtGen") << "Only Lb -> p transitions allowed in EvtLb2plnuLQCDFF.\n";  
  ::abort();
  }

  return ;
}


void EvtLb2plnuLQCDFF::getraritaff( EvtId , EvtId ,
				  double , double , 
				  double* , double* , double* , double* , 
				  double* , double* , double* , double*  ) {

  report(ERROR,"EvtGen") << "Not implemented :getraritaff in EvtLb2plnuLQCDFF.\n";  
  ::abort();

}

void EvtLb2plnuLQCDFF::getscalarff(EvtId, EvtId, double, double, double*, double*) {

  report(ERROR,"EvtGen") << "Not implemented :getscalarff in EvtLb2plnuLQCDFF.\n";  
  ::abort();

}

void EvtLb2plnuLQCDFF::getvectorff(EvtId, EvtId, double, double, double*, double*,
				 double*, double*) {

  report(ERROR,"EvtGen") << "Not implemented :getvectorff in EvtLb2plnuLQCDFF.\n";  
  ::abort();

}

void EvtLb2plnuLQCDFF::gettensorff(EvtId, EvtId, double, double, double*, double*,
				 double*, double*) {

  report(ERROR,"EvtGen") << "Not implemented :gettensorff in EvtLb2plnuLQCDFF.\n";  
  ::abort();

}

void EvtLb2plnuLQCDFF::getbaryonff(EvtId, EvtId, double, double, double*, 
				 double*, double*, double*){
  
  report(ERROR,"EvtGen") << "Not implemented :getbaryonff in EvtLb2plnuLQCDFF.\n";  
  ::abort();

}
