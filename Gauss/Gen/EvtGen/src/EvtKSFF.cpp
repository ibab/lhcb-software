//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtKSFF.cc
//
// Description: Routine to implement semileptonic form factors
//              according to the model ISGW2
//
// Modification history:
//
//    DJL     April 17, 1998        Module created
//
//------------------------------------------------------------------------
// 
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtKSFF.hh"
#include "EvtGen/EvtId.hh"
#include "EvtGen/EvtString.hh"

void EvtKSFF::getscalarff(EvtId parent,EvtId daught,
                       double t, double mass, double *fpf,
			    double *f0f ) {

  //added by Lange Jan4,2000
  static EvtId B0=EvtPDL::getId("B0");
  static EvtId B0B=EvtPDL::getId("anti-B0");
  static EvtId BP=EvtPDL::getId("B+");
  static EvtId BM=EvtPDL::getId("B-");

  static EvtId D0=EvtPDL::getId("D0");
  static EvtId D0B=EvtPDL::getId("anti-D0");
  static EvtId DP=EvtPDL::getId("D+");
  static EvtId DM=EvtPDL::getId("D-");

  static EvtId PI0=EvtPDL::getId("pi0");
  static EvtId PIP=EvtPDL::getId("pi+");
  static EvtId PIM=EvtPDL::getId("pi-");

  static EvtId ETA=EvtPDL::getId("eta");
  static EvtId ETAPR=EvtPDL::getId("eta'");

  double mx,mb;
  double mff2 = 0.0;

  if (parent==B0||parent==B0B||parent==BP||parent==BM) {
    if (daught==PIP||daught==PI0||daught==PIM||daught==ETA||daught==ETAPR) {
      mff2 = 5.33*5.33;
    }
    else{
      if (daught==D0B||daught==DM||daught==DP||daught==D0) {
        mff2 = 6.34*6.34;
      }
      else{
        report(ERROR,"EvtGen") << "Not implemented daught in ksff.\n";
      }
    }
  }
  else{
    report(ERROR,"EvtGen") << "Not implemented parent in ksff.\n";
  }

  mb = EvtPDL::getNominalMass( parent );
  mx = mass;

  *fpf = (mff2/(mff2-t));
  *f0f = 0.0;
  return;
}



 void EvtKSFF::getvectorff(EvtId parent,EvtId daught,
                       double t, double mass, double *a1f,
			     double *a2f, double *vf, double *a0f ){

  //added by Lange Jan4,2000
  static EvtId B0=EvtPDL::getId("B0");
  static EvtId B0B=EvtPDL::getId("anti-B0");
  static EvtId BP=EvtPDL::getId("B+");
  static EvtId BM=EvtPDL::getId("B-");

  static EvtId DST0=EvtPDL::getId("D*0");
  static EvtId DSTB=EvtPDL::getId("anti-D*0");
  static EvtId DSTP=EvtPDL::getId("D*+");
  static EvtId DSTM=EvtPDL::getId("D*-");

  static EvtId RHO0=EvtPDL::getId("rho0");
  static EvtId RHOP=EvtPDL::getId("rho+");
  static EvtId RHOM=EvtPDL::getId("rho-");
  static EvtId OMEG=EvtPDL::getId("omega");


  double mff2 = 0.0;
  double mb,mx;

  if (parent==B0||parent==B0B||parent==BP||parent==BM) {
    if (daught==DSTP||daught==DSTM||daught==DST0||daught==DSTB){
      mff2=6.34*6.34;
    }
    else{
      if (daught==OMEG||daught==RHO0||daught==RHOP||daught==RHOM){
      mff2=5.33*5.33;
      }
      else{
        report(ERROR,"EvtGen") << "Not implemented daught in get_ks_ff_3S1.\n";
      }
    }
  }
  else{
    report(ERROR,"EvtGen") << "Not implemented parent in get_ks_ff_3S1.\n";
  }

  mb=EvtPDL::getNominalMass(parent);
  mx=mass;

  double q2depend;

  q2depend = (mff2/(mff2-t));

  *a1f= q2depend;
  *vf= q2depend*q2depend;
  *a2f=q2depend*q2depend;
  *a0f=0.0;

  return;
 }

