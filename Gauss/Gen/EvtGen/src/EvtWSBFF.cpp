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
// Module: EvtWSBFF.cc
//
// Description: Routine to implement semileptonic form factors
//              according to the model ISGW2
//
// Modification history:
//
//    DJL       April 17, 1998       Module created
//
//------------------------------------------------------------------------
// 
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtWSBFF.hh"
#include "EvtGen/EvtString.hh"

void EvtWSBFF::getscalarff( EvtId parent, EvtId daught,
                       double t, double mass, double *fpf,
			    double *f0f ) {

  double mx,mb;
  double m0m2,m1m2,m0p2,m1p2;
  double h0,h1;

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

  if (parent==B0||parent==B0B||parent==BP||parent==BM) {
    if (daught==PIP||daught==PI0||daught==PIM||daught==ETA||daught==ETAPR) {
      m0m2 = 5.27*5.27;
      m1m2 = 5.32*5.32;
      m0p2 = 5.99*5.99;
      m1p2 = 5.71*5.71;
      h0 = 0.33;
      h1 = 0.33;
    }
    else{
      if (daught==D0B||daught==DM||daught==DP||daught==D0) {
        m0m2 = 6.30*6.30;
        m1m2 = 6.34*6.34;
        m0p2 = 6.80*6.80;
        m1p2 = 6.73*6.73;
        h0 = 0.69;
        h1 = 0.69;
      }
      else{
        report(ERROR,"EvtGen") << "Not implemented daught in wsbff.\n";
      }
    }
  }
  else{
    report(ERROR,"EvtGen") << "Not implemented parent in wsbff.\n";
  }

  mb = EvtPDL::getNominalMass( parent );
  mx = mass;

  *fpf = h1/(1-(t/m1m2));
  *f0f = h0/(1-(t/m0m2));

  return;
}



 void EvtWSBFF::getvectorff(EvtId parent, EvtId daught,
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

  double m0m2,m1m2,m0p2,m1p2;
  double hv,ha1,ha2,ha0;
  double mb,mx;

  if (parent==B0||parent==B0B||parent==BP||parent==BM) {
    if (daught==DSTP||daught==DSTM||daught==DST0||daught==DSTB){
      m0m2=6.30*6.30;
      m1m2=6.34*6.34;
      m0p2=6.80*6.80;
      m1p2=6.73*6.73;
      hv = 0.71;
      ha1 = 0.65;
      ha2 = 0.69;
      ha0 = 0.62;
    }
    else{
      if (daught==OMEG||daught==RHO0||daught==RHOP||daught==RHOM){
        m0m2=5.27*5.27;
        m1m2=5.33*5.33;
        m0p2=5.99*5.99;
        m1p2=5.71*5.71;
        hv = 0.33;
        ha1 = 0.28;
        ha2 = 0.28;
        ha0 = 0.28;
      }
      else{
        report(ERROR,"EvtGen") << "Not implemented daught in get_wsb_ff_3S1.\n";
      }
    }
  }
  else{
    report(ERROR,"EvtGen") << "Not implemented parent in get_ks_ff_3S1.\n";
  }

  mb=EvtPDL::getNominalMass(parent);
  mx=mass;

  *a0f = (ha0)/(1.0-(t/m0m2));
  *vf = (hv)/(1.0-(t/m1m2));
  *a1f = (ha1)/(1.0-(t/m1p2));
  *a2f = (ha2)/(1.0-(t/m1p2));

  return;
 }

