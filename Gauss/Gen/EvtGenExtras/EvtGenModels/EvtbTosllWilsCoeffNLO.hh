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
// Module: EvtGen/EvtbTosllWilsCoeffNLO.hh
//
// Description:
//
// Modification history:
//
//    N.Nikitin           March 27 2008         Module created
//
//------------------------------------------------------------------------

#ifndef EVTBTOSLLWILCNLO_HH
#define EVTBTOSLLWILCNLO_HH
class EvtComplex;

// all definicions !
class EvtbTosllWilsCoeffNLO{

 public:

  double As( double mu, int Nf, int ias);
  double Li2(double w);
  
  double C1(double mu, double Mw, int Nf, int ias);
  double C2(double mu, double Mw, int Nf, int ias);
  double C3(double mu, double Mw, int Nf, int ias);
  double C4(double mu, double Mw, int Nf, int ias); 
  double C5(double mu, double Mw, int Nf, int ias);
  double C6(double mu, double Mw, int Nf, int ias);

  double A(double z);
  double B(double z);
  double C_Bur(double z);
  double D_Bur(double z);
  double E(double z);
  double F_Bur(double z);
  double Y(double z);
  double Z(double z);

  double C7gamma(double mu, double Mw, double mt, int Nf, int ias );

  double Pe(double eta);
  double P0ndr(double asW, double eta);

  double C9v(double mu, double Mw, double mt, int Nf, int ias);
  double C10a(double mt, double Mw);

  double Reh(double mu, double m2, double z, double s);
  double Imh(double z, double s);
  double Reh0(double mu, double m2, double s);
  double Imh0(void);
  double ReHadd(int res_swch, double q2, double ml);
  double ImHadd(int res_swch, double q2, double ml);
  double omega(double s);

  double ReC9eff(int res_swch, double q2, double M1, double m2, double mu, 
	         double mc, double mt, double Mw, double ml, int Nf, int ias);
  double ImC9eff(int res_swch, double q2, double M1, double m2, double mu, 
	         double mc, double Mw, double ml, int Nf, int ias);

  EvtComplex GetC9Eff(int res_swch, double q2, double M1, double m2, double mu, 
	              double mc, double mt, double Mw, double ml, 
	              int Nf, int ias);
  EvtComplex GetC10Eff(double mt, double Mw);
  EvtComplex GetC7Eff(double mu, double Mw, double mt, int Nf, int ias);
 
};

#endif
