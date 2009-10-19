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
// Module: EvtbTosllWilsCoeffNLO.cpp
//
// Description: The calculation of the Wilson coefficients for 
//              b -> (d,s) ell+ ell-  transitions in the SM at NLO 
//              according to the paper: 
//                    A.J.Buras, M.Munz, Phys.Rev.D52, p.189 (1995).
//              
//
//
// Modification history:
//
//  N.Nikitin        March 17, 2008                Module created
//                   June  14, 2008                New equation for As
//
//------------------------------------------------------------------------
// 
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtVector4C.hh"
#include "EvtGenBase/EvtTensor4C.hh"
#include "EvtGenBase/EvtId.hh"
#include "EvtGenBase/EvtDiLog.hh"
// The header file for current class memeber functions description
#include "EvtGenModels/EvtbTosllWilsCoeffNLO.hh"
#include <cstdlib>

//   **************************************************************
//   *                                                            *
//   *     The strong running coupling constant from PDG          * 
//   *                                                            * 
//   * mu - the scale parameter  ( in GeV );                      * 
//   * Nf - number of "effective" flavours ( Nf=5 for b-quark);   * 
//   * the colors number = 3;                                     * 
//   * ias - the number for alpha_s(M_Z) choice:                  * 
//   *     = 0              PDG 1sigma minimal alpha_s(M_Z);      * 
//   *     = 1              PDG average value  alpha_s(M_Z);      * 
//   *     = 2              PDG 1sigma maximal alpha_s(M_Z).      * 
//   *                                                            *
//   **************************************************************
 double EvtbTosllWilsCoeffNLO::As( double mu, int Nf, int ias)
	 {
	   double as, ll;
	   double b0, b1, b2;    /* terms in the series of the beta-function */
           double alpha_strong[]={0.1156,0.1176,0.1196}; /* at M_Z scale */
           double MZ=91.19;      /* in GeV */  

	   b0=11.-2.*((double)Nf)/3.;
	   b1=51.-19.*((double)Nf)/3.;
	   b2=2857.-5033.*((double)Nf)/9.+325.*pow(((double)Nf),2.)/27.;

           // RG Equation solution
           alpha_strong[ias]=alpha_strong[ias]/(4.0*EvtConst::pi);
           ll=0.0-log(MZ/mu)+(b0*b2-b1*b1)*alpha_strong[ias]/(2.0*pow(b0,3.0));
           ll=ll+1.0/(2.0*b0*alpha_strong[ias]);
           ll=ll+b1*log(alpha_strong[ias])/(2.0*b0*b0);

           // Running coupling constant from M_Z to mu
           as=pow((log(log(2.0*ll))-0.5),2.0)+b2*b0/(8.0*b1*b1)-5.0/4.0;
           as=as*pow((b1/(b0*b0*ll)),2.0);
           as=1.0-b1*log(2.0*ll)/(b0*b0*ll)-as;
           as=2.0*EvtConst::pi*as/(b0*ll);

           if(as <= 0.0){
              report(ERROR,"EvtGen") << "The function EvtbTosllWilsCoeffNLO::As" 
              << "\n Unexpected value of the running coupling constant!"
              << "\n alpha_s("<< mu <<") = " << as <<";"
              << "\n Nf =" << Nf << ",   ias = " << ias <<";"
              << "\n ln(mu/lambda_QCD) = " << ll <<";"
              <<std::endl;
              ::abort();
           }

	   return as;
	 }


//    ************************************************************
//    *                                                          *
//    *                  Spencer function                        * 
//    *              in serial representation                    * 
//    *                    ( w <= 1.0 )                          * 
//    *                                                          * 
//    *                                                          *
//    ************************************************************
  double EvtbTosllWilsCoeffNLO::Li2(double w)
	{
	  double Lii=0.0;
	  double k=1.0;

	  while(k<=20.0)
	    {
	      Lii=Lii+pow(w,k)/pow(k,2.0);
	      k++;
	    };

	  /* printf("\n Spencer function value: Lii(%f)=%f \n\n",w,Lii); */

	  return Lii;
	}


  /*                       Coefficient C1(mu)                             *
   *            by A.J.Buras and M.Munz, Phys.Rev. D52, 186.              */ 
  double EvtbTosllWilsCoeffNLO::C1( double mu, double Mw, int Nf, int ias)
	 {
	    double CC1; 
	    double eta; 
	    double asW;  /* the strong coupling constant at the scale Mw */ 
	    double asmu; /* the strong coupling constant at the scale mu */ 
	    int i; 
 
	    double a[]={14.0/23.0, 16.0/23.0, 6.0/23.0, -12.0/23.0, 
                        0.4086, -0.4230, -0.8994, 0.1456}; 
	    double k1[]={0.0,0.0,0.5,-0.5,0.0,0.0,0.0,0.0}; 
 
	    asW =As(Mw,Nf,ias); 
	    asmu=As(mu,Nf,ias); 
	    eta =asW/asmu; 
 
	    CC1=0.0; 
	    i=0; 
	    while(i<8) 
	      { 
		CC1=CC1+k1[i]*pow(eta,a[i]); 
		i++; 
	      }; 
 
	    return CC1; 
	 } 
 
 
 
  /*                       Coefficient C2(mu)                             * 
   *            by A.J.Buras and M.Munz, Phys.Rev. D52, 186.              */ 
   double EvtbTosllWilsCoeffNLO::C2(double mu, double Mw, int Nf, int ias) 
	 { 
	    double CC2; 
	    double eta; 
	    double asW;  /* the strong coupling constant at the scale Mw */ 
	    double asmu; /* the strong coupling constant at the scale mu */ 
	    int i; 
 
	    double a[]={14.0/23.0, 16.0/23.0, 6.0/23.0, -12.0/23.0, 
                        0.4086, -0.4230, -0.8994, 0.1456}; 
	    double k2[]={0.0,0.0,0.5,0.5,0.0,0.0,0.0,0.0}; 
 
	    asW =As(Mw,Nf,ias); 
	    asmu=As(mu,Nf,ias); 
	    eta =asW/asmu; 
 
	    CC2=0.0; 
	    i=0; 
	    while(i<8) 
	      { 
		CC2=CC2+k2[i]*pow(eta,a[i]); 
		i++; 
	      }; 
 
	    return CC2; 
	 } 
 
  /*                       Coefficient C3(mu)                             * 
   *            by A.J.Buras and M.Munz, Phys.Rev. D52, 186.              */ 
   double EvtbTosllWilsCoeffNLO::C3(double mu, double Mw, int Nf, int ias) 
	 { 
	    double CC3; 
	    double eta; 
	    double asW;  /* the strong coupling constant at the scale Mw */ 
	    double asmu; /* the strong coupling constant at the scale mu */ 
	    int i; 
 
	    double a[]={14.0/23.0, 16.0/23.0, 6.0/23.0, -12.0/23.0, 
                         0.4086, -0.4230, -0.8994, 0.1456}; 
	    double k3[]={0.0, 0.0, -1.0/14.0, 1.0/6.0, 0.0510, -0.1403, -0.0113, 0.0054}; 
 
	    asW =As(Mw,Nf,ias); 
	    asmu=As(mu,Nf,ias); 
	    eta =asW/asmu; 
 
	    CC3=0.0; 
	    i=0; 
	    while(i<8) 
	      { 
		CC3=CC3+k3[i]*pow(eta,a[i]); 
		i++; 
	      }; 
 
	    return CC3; 
	 } 
 
 
  /*                       Coefficient C4(mu)                             * 
   *            by A.J.Buras and M.Munz, Phys.Rev. D52, 186.              */ 
   double EvtbTosllWilsCoeffNLO::C4(double mu, double Mw, int Nf, int ias) 
	 { 
	    double CC4; 
	    double eta; 
	    double asW;  /* the strong coupling constant at the scale Mw */ 
	    double asmu; /* the strong coupling constant at the scale mu */ 
	    int i; 
 
	    double a[]={14.0/23.0, 16.0/23.0, 6.0/23.0, -12.0/23.0, 
                        0.4086, -0.4230, -0.8994, 0.1456}; 
	    double k4[]={0.0, 0.0, -1.0/14.0, -1.0/6.0, 0.0984, 0.1214, 0.0156, 0.0026}; 
 
	    asW =As(Mw,Nf,ias); 
	    asmu=As(mu,Nf,ias); 
	    eta =asW/asmu; 
 
	    CC4=0.0; 
	    i=0; 
	    while(i<8) 
	      { 
		CC4=CC4+k4[i]*pow(eta,a[i]); 
		i++; 
	      }; 
 
	    return CC4; 
	 } 
 
 
  /*                       Coefficient C5(mu)                             * 
   *            by A.J.Buras and M.Munz, Phys.Rev. D52, 186.              */ 
   double EvtbTosllWilsCoeffNLO::C5(double mu, double Mw, int Nf, int ias) 
	 { 
	    double CC5; 
	    double eta; 
	    double asW;  /* the strong coupling constant at the scale Mw */ 
	    double asmu; /* the strong coupling constant at the scale mu */ 
	    int i; 
 
	    double a[]={14.0/23.0, 16.0/23.0, 6.0/23.0, -12.0/23.0, 
                        0.4086, -0.4230, -0.8994, 0.1456}; 
	    double k5[]={0.0, 0.0, 0.0, 0.0, -0.0397, 0.0117, -0.0025, 0.0304}; 
 
	    asW =As(Mw,Nf,ias); 
	    asmu=As(mu,Nf,ias); 
	    eta =asW/asmu; 
 
	    CC5=0.0; 
	    i=0; 
	    while(i<8) 
	      { 
		CC5=CC5+k5[i]*pow(eta,a[i]); 
		i++; 
	      }; 
 
	    return CC5; 
	 } 
 
 
   /*                       Coefficient C6(mu)                             * 
    *            by A.J.Buras and M.Munz, Phys.Rev. D52, 186.              */ 
  double EvtbTosllWilsCoeffNLO::C6(double mu, double Mw, int Nf, int ias) 
	 { 
	    double CC6; 
	    double eta; 
	    double asW;  /* the strong coupling constant at the scale Mw */ 
	    double asmu; /* the strong coupling constant at the scale mu */ 
	    int i; 
 
	    double a[]={14.0/23.0, 16.0/23.0, 6.0/23.0, -12.0/23.0, 
                        0.4086, -0.4230, -0.8994, 0.1456}; 
	    double k6[]={0.0, 0.0, 0.0, 0.0, 0.0335, 0.0239, -0.0462, -0.0112}; 
 
	    asW =As(Mw,Nf,ias); 
	    asmu=As(mu,Nf,ias); 
	    eta =asW/asmu; 
 
	    CC6=0.0; 
	    i=0; 
	    while(i<8) 
	      { 
		CC6=CC6+k6[i]*pow(eta,a[i]); 
		i++; 
	      }; 
 
	    return CC6; 
	 } 
 
 
     /* by A.J.Buras and M.Munz, Phys.Rev. D52, 186.  */ 
  double EvtbTosllWilsCoeffNLO::A(double z)
	{
	  double AA;

	  AA=z*(8.0*pow(z, 2.0)+5.0*z-7.0)/(12.0*pow((z-1.0),3.0));
	  AA=AA+pow(z,2.0)*(2.0-3.0*z)*log(z)/(2.0*pow((z-1.0),4.0));

	  return AA;
	}

  /* by A.J.Buras and M.Munz, Phys.Rev. D52, 186.  */
  double EvtbTosllWilsCoeffNLO::B(double z)
	{
	  double BB;

	  BB=z/(4.0*(1.0-z))+z*log(z)/(4.0*pow((1.0-z),2.0));

	  return BB;
	}

  /* by A.J.Buras and M.Munz, Phys.Rev. D52, 186.  */
  double EvtbTosllWilsCoeffNLO::C_Bur(double z)
	{
	  double CC;

	  CC=z*(z-6.0)/(8.0*(z-1.0));
	  CC=CC+z*(3.0*z+2.0)*log(z)/(8.0*pow((z-1.0),2.0));

	  return CC; 
	} 
 
  /* by A.J.Buras and M.Munz, Phys.Rev. D52, 186.  */ 
  double EvtbTosllWilsCoeffNLO::D_Bur(double z)
	{ 
	  double DD; 
 
	  DD=(25.0*pow(z,2.0)-19.0*pow(z,3.0))/(36.0*pow((z-1.0),3.0));
	  DD=DD+pow(z,2.0)*(5.0*pow(z,2.0)-2.0*z-6.0)*log(z)/(18.0*pow((z-1.0),4.0));
	  DD=DD-(4.0/9.0)*log(z);

	  return DD; 
	} 
 
  /* by A.J.Buras and M.Munz, Phys.Rev. D52, 186.  */ 
  double EvtbTosllWilsCoeffNLO::E(double z)
	{ 
	  double EE; 
 
	  EE=z*(18.0-11.0*z-z*z)/(12.0*pow((1.0-z),3.0));
	  EE=EE+pow(z,2.0)*(15.0-16.0*z+4.0*z*z)*log(z)/(6.0*pow((1.0-z),4.0));
	  EE=EE-(2.0/3.0)*log(z);

	  return EE; 
	} 
 
  /* by A.J.Buras and M.Munz, Phys.Rev. D52, 186.  */ 
  double EvtbTosllWilsCoeffNLO::F_Bur(double z)
	{ 
	  double FF; 
 
	  FF=z*(pow(z,2.0)-5.0*z-2.0)/(4.0*pow((z-1.0),3.0));
	  FF=FF+3.0*pow(z,2.0)*log(z)/(2.0*pow((z-1.0),4.0));

	  return FF; 
	} 
 
  /* by A.J.Buras and M.Munz, Phys.Rev. D52, 186.  */ 
  double EvtbTosllWilsCoeffNLO::Y(double z)
	{ 
	  double YY; 
 
	  YY=C_Bur(z)-B(z); 
 
	  return YY; 
	} 
 
   /* by A.J.Buras and M.Munz, Phys.Rev. D52, 186.  */ 
  double EvtbTosllWilsCoeffNLO::Z(double z)
	{ 
	  double ZZ; 
 
	  ZZ=C_Bur(z)+0.25*D_Bur(z); 
 
	  return ZZ; 
	} 
 
 
  /*            Coefficient  C7gamma(mu) in the SM                        * 
   *            by A.J.Buras and M.Munz, Phys.Rev. D52, 186.              */ 
   double EvtbTosllWilsCoeffNLO::C7gamma(double mu, double Mw, double mt, int Nf, int ias )
	 { 
	    double C7, C70, C80, sum; 
            double AA, FF;
	    double x, eta; 
	    double asW, asmu; 
	    int i; 
 
	    double a[]={14.0/23.0, 16.0/23.0, 6.0/23.0, -12.0/23.0, 
			0.4086, -0.4230, -0.8994, 0.1456}; 
	    double h[]={2.2996, -1.0880, -3.0/7.0, -1.0/14.0, -0.6494, 
		       -0.0380, -0.0186, -0.0057}; 
 
	    x   =pow(mt/Mw, 2.0); 
	    asW =As(Mw,Nf,ias); 
	    asmu=As(mu,Nf,ias); 
	    eta =asW/asmu;

            AA=A(x);
            FF=F_Bur(x); 
 
	    C70=-0.5*AA; 
	    C80=-0.5*FF; 
 
	    C7=pow(eta,(16.0/23.0))*C70; 
	    C7=C7+(8.0/3.0)*(pow(eta,(14.0/23.0))-pow(eta,(16.0/23.0)))*C80; 
 
	    sum=0.0; 
	    i=0; 
	    while(i<8) 
	      { 
		sum=sum+h[i]*pow(eta,a[i]); 
		i++; 
	      }; 
	    C7=C7+sum; 
 
	    return C7; 
	 } 
 
 
	  /*             Coefficient P_E                   * 
	   * by A.J.Buras and M.Munz, Phys.Rev. D52, 186;  * 
	   *            see formula (2.12).                */ 
  double EvtbTosllWilsCoeffNLO::Pe(double eta)
	{ 
	   double sum; 
	   double Pee; 
	   int i; 
 
	   double a[]={14.0/23.0, 16.0/23.0, 6.0/23.0, -12.0/23.0, 
			0.4086, -0.4230, -0.8994, 0.1456}; 
	   double q[]={0.0, 0.0, 0.0, 0.0, 0.0318, 0.0918, -0.2700, 0.0059}; 
 
	   sum=0.0; 
	   i=0; 
	   while(i<8) 
	     { 
	       sum=sum+q[i]*pow(eta,(a[i]+1.0)); 
	       i++; 
	     }; 
	   Pee=0.1405+sum; 
 
	   return Pee; 
	} 
 
 
	  /*         Coefficient P^{NDR}_0                  * 
	   * by A.J.Buras and M.Munz, Phys.Rev. D52, 186;   * 
	   *	       see formula (2.11).                  */ 
   double EvtbTosllWilsCoeffNLO::P0ndr(double asW, double eta)
	{ 
	   double P00ndr; 
	   double sum; 
	   int i; 
 
	   double a[]={14.0/23.0, 16.0/23.0, 6.0/23.0, -12.0/23.0, 
		       0.4086, -0.4230, -0.8994, 0.1456}; 
	   double p[]={0.0, 0.0, -80.0/203.0, 8.0/33.0, 0.0433, 
		       0.1384, 0.1648, -0.0073}; 
	   double r[]={0.0, 0.0, 0.8966, -0.1960, -0.2011, 0.1328, 
		      -0.0292, -0.1858}; 
	   double s[]={0.0, 0.0, -0.2009, -0.3579, 0.0490, -0.3616, 
		      -0.3554, 0.0072}; 
 
	   sum=0.0; 
	   i=0; 
	   while(i<8) 
	     { 
	       sum=sum+p[i]*pow(eta,(a[i]+1.0)); 
	       i++; 
	     }; 
	   P00ndr=EvtConst::pi*(-0.1875+sum)/asW; 
	   P00ndr=P00ndr+1.2468; 
	   sum=0.0; 
	   i=0; 
	   while(i<8) 
	     { 
	       sum=sum+(r[i]+s[i]*eta)*pow(eta,a[i]); 
	       i++; 
	     }; 
	   P00ndr=P00ndr+sum; 
 
	   return P00ndr; 
	 } 
 
 
    /*      Coefficient  C_{9V} (in the NDR schime)     * 
     *   by A.J.Buras and M.Munz, Phys.Rev. D52, 186;   * 
     *	         see formula (2.10).                    */ 
   double EvtbTosllWilsCoeffNLO::C9v(double mu, double Mw, double mt, int Nf, int ias)
	 { 
	   double C9; 
	   double x, eta; 
	   double asW, asmu; 
	   double sin2W=0.224;  /* the square of the weak angle */
           double P00ndr, YY, ZZ, Pee, EE;  
 
	   x=   pow(mt/Mw, 2.0); 
	   asW= As(Mw,Nf,ias); 
	   asmu=As(mu,Nf,ias); 
	   eta= asW/asmu;
 
           P00ndr=P0ndr(asW,eta);
           YY    =Y(x);
           ZZ    =Z(x);
           Pee   =Pe(eta);
           EE    =E(x);
 
	   /* C9 */ 
	   C9=P00ndr+(YY/sin2W)-4.0*ZZ+Pee*EE; 
 
	   return C9; 
	 } 
 
 
     /*                Coefficient C_{10A}               * 
      *   by A.J.Buras and M.Munz, Phys.Rev. D52, 186;   * 
      *	                see formula (2.8).               */ 
   double EvtbTosllWilsCoeffNLO::C10a(double mt, double Mw)
	 { 
	   double C10; 
	   double x; 
	   double sin2W=0.224;  /* the square of the Winberg angle */ 
           double YY;
 
	   x=pow(mt/Mw, 2.0);

           YY=Y(x); 
 
	   C10=-YY/sin2W; 
 
	   return C10; 
	 } 
 
 
   /*              Re(h(z,\hat s)) by Buras           */ 
   double EvtbTosllWilsCoeffNLO::Reh(double mu, double m2, double z, double s)
	{ 
	  double reh, swh; 
	  double x;        /* Buras variable "x" in (2.29) */ 
	   
	  x=4.0*pow(z,2.0)/s; 

	  reh=-8.0*log(m2/mu)/9.0-8.0*log(z)/9.0;
	  reh=reh+8.0/27.0+4.0*x/9.0;

	  swh=2.0*(2.0+x)*sqrt(fabs(1.0-x))/9.0;

	  if(x<=1.0)
	    {
	      swh=swh*log(fabs((sqrt(1.0-x)+1.0)/(sqrt(1.0-x)-1.0)));
	    }
	  else
	    { 
	      swh=swh*2.0*atan(1.0/sqrt(x-1.0)); 
	    }; 
 
	  reh=reh-swh; 
 
	  return reh; 
	} 
 
 
   /*       Im(h(z,\hat s)) by Buras           */ 
   double EvtbTosllWilsCoeffNLO::Imh(double z, double s)
	{ 
	  double x;        /* Buras variable "x" in (2.29) */ 
	  double imh; 
 
          x=4.0*pow(z,2.0)/s; 
 
	  if(x<=1.0) 
	    { 
	      imh=2.0*EvtConst::pi*(2.0+x)*sqrt(fabs(1.0-x))/9.0; 
	    } 
	  else 
	    { 
	      imh=0.0; 
	    }; 
	  return imh; 
	} 
 
 
   /*           Re(h(0,\hat s)) by Buras           */ 
   double EvtbTosllWilsCoeffNLO::Reh0(double mu, double m2, double s)
      { 
	double reh0;
	reh0=8.0/27.0-8.0*log(m2/mu)/9.0-4.0*log(s)/9.0;
	return reh0;
      } 
 
 
 
   /*       Im(h(0,\hat s)) by Buras	   */ 
   double EvtbTosllWilsCoeffNLO::Imh0(void)
      { 
	double imh0; 
 
	imh0=4.0*EvtConst::pi/9.0; 
	return imh0; 
      } 
 
 
   /*                        REAL PART                                * 
    * of the resonant hadronic contribution to the C_{9V} coefficient * 
    *                                                                 * 
    *       res_swch=0 -- switch OFF of the resonant contribution;   * 
    *       res_swch=1 -- switch ON  of the resonant contribution.    * 
    *                                                                 */ 
   double EvtbTosllWilsCoeffNLO::ReHadd(int res_swch, double q2, double ml)
      { 
	 int i;
	 double RReHadd=0.0; 
	 double a,b,Znam; 
	 double Gamma_ll[2];
 
	 double alpha_qed=1.0/137.0; 
 
	 /* Total branching ratios of J/\psi and \psi^{'}--mesons in GeV-s */ 
	 double Gamma[]={0.000088, 0.000277}; 
	 /* The branching fractions for decays V->l^+l^-, i={e,\mu} in GeV--s */ 
	 double Gamma_ll_e_mu[]={0.000005, 0.000002}; 
	 /* The branching fractions for decays V->tau^+tau^- in GeV--s */ 
	 double Gamma_ll_tau[]={0.0, 0.000002}; 
	 /* Masses of J/\psi and \psi^{'}--mesons */ 
	 double M[]={3.09688, 3.686}; 
	  
	 if(ml<1.0) 
	   { 
	     i=0; 
	     while(i<2) 
	       { 
	         Gamma_ll[i]=Gamma_ll_e_mu[i]; 
		 i++; 
	       }; 
	   } 
	  else 
	   { 
	     i=0; 
	     while(i<2) 
	       { 
	         Gamma_ll[i]=Gamma_ll_tau[i]; 
		 i++; 
	       }; 
	   }; 
 
	 switch(res_swch) 
	   { 
	     case 0: RReHadd=0.0; 
		     break; 
	     case 1: i=0; 
		     RReHadd=0.0; 
		     while(i<2) 
		       { 
			 a=pow(M[i],2.0)-q2; 
			 b=M[i]*Gamma[i]; 
			 Znam=pow(a,2.0)+pow(b,2.0); 
			 Znam=Gamma_ll[i]*M[i]*a/Znam; 
			 RReHadd=RReHadd+Znam; 
			 i++; 
		       }; 
		     break; 
	    }; 
 
//       The sign plus are corresponded to the relation:   
//	         \kappa*(3C_1+C_2)=1              
//       with sign of Wilson coefficien C_2(M_W)=+1 as at work            
//       A.J.Buras and M.Munz, Phys.Rev. D52, 186.
	 RReHadd=RReHadd*3.0*EvtConst::pi/pow(alpha_qed, 2.0); 
 
	 return RReHadd; 
      } 

 
   /*                   IMAGINARY PART                              * 
    * of the resonant hadronic contribution to the C_9V coefficient * 
    *                                                               * 
    *  res_swch -- switching parameter:                             * 
    *       res_swch=0 -- switch OFF for the resonant contribution; * 
    *       res_swch=1 -- switch ON for the resonant contribution.  * 
    *                                                               */ 
   double EvtbTosllWilsCoeffNLO::ImHadd(int res_swch, double q2, double ml)
      { 
	 int i; 
 
	 double IImHadd=0.0; 
	 double a,b,Znam; 
	 double Gamma_ll[2]; 
 
	 double alpha_qed=1.0/137.0; 
 
	 /* Total branching ratios of J/\psi and \psi^{'}--mesons in GeV-s */ 
	 double Gamma[]={0.000088, 0.000277}; 
	 /* The branching fractions for decays V->l^+l^-, i={e,\mu} in GeV--s */ 
	 double Gamma_ll_e_mu[]={0.000005, 0.000002}; 
	 /* The branching fractions for decays V->tau^+tau^- in GeV--s */ 
	 double Gamma_ll_tau[]={0.0, 0.000002}; 
	 /* Masses of J/\psi and \psi^{'}--mesons */ 
	 double M[]={3.09688, 3.686}; 
 
         if(ml<1.0) 
	   { 
	     i=0; 
	     while(i<2) 
	       { 
	         Gamma_ll[i]=Gamma_ll_e_mu[i]; 
		 i++; 
	       }; 
	   } 
	  else 
	   { 
	     i=0; 
	     while(i<2) 
	       { 
	         Gamma_ll[i]=Gamma_ll_tau[i]; 
		 i++; 
	       }; 
	   }; 
	    
	 switch(res_swch) 
	   { 
	     case 0: IImHadd=0.0; 
		     break; 
	     case 1: i=0; 
		     IImHadd=0.0; 
		     while(i<2) 
		       { 
			 a=pow(M[i],2.0)-q2; 
			 b=M[i]*Gamma[i]; 
			 Znam=pow(a,2.0)+pow(b,2.0); 
			 Znam=Gamma_ll[i]*M[i]*b/Znam; 
			 IImHadd=IImHadd+Znam; 
			 i++; 
		       }; 
		     break; 
	    }; 

//       The sign plus are corresponded to the relation:   
//	         \kappa*(3C_1+C_2)=1              
//       with sign of Wilson coefficien C_2(M_W)=+1 as at work            
//       A.J.Buras and M.Munz, Phys.Rev. D52, 186. 
	 IImHadd=IImHadd*3.0*EvtConst::pi/pow(alpha_qed, 2.0); 
 
	 return IImHadd; 
      } 
 
 
       /*            Function omega(s)                      * 
	* by  A.J.Buras, M.Munz, Phys.Rev.D52 (1995), p189. */ 
double EvtbTosllWilsCoeffNLO::omega(double s)
	{ 
	   double oomega; 
 
           if(s>1.0){
             s=0.999999;
           }
	   oomega=-2.0*pow(EvtConst::pi,2.0)/9.0-4.0*Li2(s)/3.0;
	   oomega=oomega-2.0*log(s)*log(1.0-s)/3.0;
	   oomega=oomega-(5.0+4.0*s)*log(1.0-s)/(3.0*(1.0+2.0*s));
	   oomega=oomega-2.0*s*(1.0+s)*(1.0-2.0*s)*log(s)/
		  (3.0*pow((1.0-s),2.0)*(1.0+2.0*s));
	   oomega=oomega+(5.0+9.0*s-6.0*s*s)/(6.0*(1.0-s)*(1.0+2.0*s));

	   return oomega;
	}

 
       /*         REAL PART of the coefficient C_9V:                * 
	*           C9 -> C9eff=ReC9eff+i*ImC9eff                   * 
	*  by  A.J.Buras, M.Munz, Phys.Rev.D52 (1995), p189         * 
	*                                                           * 
	*   res_swch -- switching parameter:                        * 
	* res_swch=0 -- switch OFF for the resonant contribution,   * 
	* res_swch=1 -- switch ON for the resonant contribution;    * 
	*                                                           * 
	*         q2 -- the square of transition 4-momentum;        * 
        *         M1 -- B-meson mass;                               *
	*         m2 -- b-quark mass (in the heavy meson M1), GeV;  * 
	*         mu -- scale parameter, GeV;                       * 
	*         mc -- c-quark mass, GeV;                          * 
	*         mt -- t-quark mass, GeV;                          * 
	*         Mw -- mass of the W--meson, GeV;                  * 
	*         ml -- leptonic mass, GeV;                         * 
	*         Nf -- number of "effective" flavors               * 
	*                          (for b-quark Nf=5);              * 
	*        ias -- switching parameter for Lms[]               * 
	*                          in the As(..) function.          * 
	*                                                           */ 
double EvtbTosllWilsCoeffNLO::ReC9eff(int res_swch, double q2, double M1, double m2, double mu, 
	                        double mc, double mt, double Mw, double ml, int Nf, int ias)
	 { 
	   double s;          /* Buras variable "\hat s" in (2.26)      */ 
	   double z;          /* Buras variable "z"      in (2.26)      */ 
	   double tilde_eta;  /* Buras variable " \tilde\eta" in (2.33) */ 
	   double c1,c2,c3,c4,c5,c6,c9,RReHadd,RReC9eff; 
 
	   s=q2/pow(M1,2.0); 
	   z=mc/m2; 
	   tilde_eta=1.0+EvtbTosllWilsCoeffNLO::As(mu,Nf,ias)
                        *EvtbTosllWilsCoeffNLO::omega(s)/EvtConst::pi; 
 
	   /* Initialisation of the Wilson coefficients */ 
	   c1=EvtbTosllWilsCoeffNLO::C1(mu,Mw,Nf,ias); 
	   c2=EvtbTosllWilsCoeffNLO::C2(mu,Mw,Nf,ias); 
	   c3=EvtbTosllWilsCoeffNLO::C3(mu,Mw,Nf,ias); 
	   c4=EvtbTosllWilsCoeffNLO::C4(mu,Mw,Nf,ias); 
	   c5=EvtbTosllWilsCoeffNLO::C5(mu,Mw,Nf,ias); 
	   c6=EvtbTosllWilsCoeffNLO::C6(mu,Mw,Nf,ias); 
	   c9=EvtbTosllWilsCoeffNLO::C9v(mu,Mw,mt,Nf,ias);
           RReHadd=EvtbTosllWilsCoeffNLO::ReHadd(res_swch,q2,ml); 
 
	   RReC9eff=c9*tilde_eta; 
	   RReC9eff=RReC9eff+(3.0*c1+c2+3.0*c3+c4+3.0*c5+c6)*EvtbTosllWilsCoeffNLO::Reh(mu,m2,z,s); 
	   RReC9eff=RReC9eff-0.5*(4.0*c3+4.0*c4+3.0*c5+c6)*EvtbTosllWilsCoeffNLO::Reh(mu,m2,1.0,s); 
	   RReC9eff=RReC9eff-0.5*(c3+3.0*c4)*EvtbTosllWilsCoeffNLO::Reh0(mu,m2,s); 
	   RReC9eff=RReC9eff+2.0*(3.0*c3+c4+3.0*c5+c6)/9.0; 
 
	   RReC9eff=RReC9eff+RReHadd;

           report(NOTICE,"EvtGen") 
             << "\n =============================================================="
             << "\n =============================================================="
             << "\n\n The function EvtbTosllWilsCoeffNLO::ReC9eff(...) passed."
             << "\n Particle masses:"
             << "\n q2                = " << q2
             << "\n leptonic mass  ml = " << ml
             << "\n c - quark mass mc = " << mc
             << "\n b - quark mass mb = " << m2
             << "\n t - quark mass mt = " << mt
             << "\n W - boson mass Mw = " << Mw
             << "\n ==============================================================="
             << "\n Input parameters:"
             << "\n scale parameter         mu = " << mu
             << "\n number of flavors       Nf = " << Nf
             << "\n resonant switching         = " << res_swch
             << "\n parameter for alpha_s(M_Z) = " << ias
             << "\n ================================================================"
             << "\n Wilson Coefficients:"
             << "\n RReHadd   = " << RReHadd
             << "\n RReC9eff  = " << RReC9eff
             << "\n tilde_eta = " << tilde_eta
             << "\n s         = " << s
             << "\n c1        = " << c1
             << "\n c2        = " << c2
             << "\n c3        = " << c3
             << "\n c4        = " << c4
             << "\n c5        = " << c5
             << "\n c6        = " << c6
             << "\n c9        = " << c9
             << "\n ================================================================="
             << "\n ================================================================="
             << std::endl;
 
 
	   return RReC9eff; 
 
	 } 
 
 
       /*       IMAGINARY PART of the coefficient C_9V:             * 
	*           C9 -> C9eff=ReC9eff+i*ImC9eff                   * 
	*  by  A.J.Buras, M.Munz, Phys.Rev.D52 (1995), p189         * 
	*                                                           * 
	*   res_swch -- switching parameter:                        * 
	* res_swch=0 -- switch OFF for the resonant contribution;   * 
	* res_swch=1 -- switch ON for the resonant contribution;    * 
	*                                                           * 
	*         q2 -- the square of transition 4-momentum;        * 
        *         M1 -- B-meson mass;                               *
	*         m2 -- b-quark mass ( in the heavy meson M1), GeV; * 
	*         mu -- scale parameter, GeV;                       * 
	*         mc -- c-quark mass, GeV;                          * 
	*         mt -- t-quark mass, GeV;                          * 
	*         Mw -- mass of the W--meson, GeV;                  * 
	*         ml -- leptonic mass, GeV;                         * 
	*         Nf -- number of "effective" flavors               * 
	*                          (for b-quark Nf=5);              * 
	*        ias -- switching parameter for Lms[]               * 
	*                          in the As(..) function.          * 
	*                                                           */ 
double EvtbTosllWilsCoeffNLO::ImC9eff(int res_swch, double q2, double M1, double m2, 
                        double mu, double mc, double Mw, double ml, int Nf, int ias)
	 { 
	   double s;          /* Buras variable "\hat s"in (2.26)    */ 
	   double z;          /* Buras variable "z" in (2.26)        */ 
	   double c1,c2,c3,c4,c5,c6,IImHadd,IImC9eff; 
 
	   s=q2/pow(M1,2.0); 
	   z=mc/m2; 
 
	   /* Initialisation of the Wilson coefficients */ 
	   c1=EvtbTosllWilsCoeffNLO::C1(mu,Mw,Nf,ias); 
	   c2=EvtbTosllWilsCoeffNLO::C2(mu,Mw,Nf,ias); 
	   c3=EvtbTosllWilsCoeffNLO::C3(mu,Mw,Nf,ias); 
	   c4=EvtbTosllWilsCoeffNLO::C4(mu,Mw,Nf,ias); 
	   c5=EvtbTosllWilsCoeffNLO::C5(mu,Mw,Nf,ias); 
	   c6=EvtbTosllWilsCoeffNLO::C6(mu,Mw,Nf,ias);
           IImHadd=EvtbTosllWilsCoeffNLO::ImHadd(res_swch,q2,ml);
 
	   IImC9eff=(3.0*c1+c2+3.0*c3+c4+3.0*c5+c6)*EvtbTosllWilsCoeffNLO::Imh(z,s); 
	   IImC9eff=IImC9eff-0.5*(4.0*c3+4.0*c4+3.0*c5+c6)*EvtbTosllWilsCoeffNLO::Imh(1.0,s); 
	   IImC9eff=IImC9eff-0.5*(c3+3.0*c4)*EvtbTosllWilsCoeffNLO::Imh0(); 
 
	   IImC9eff=IImC9eff+IImHadd; 
 
	   return IImC9eff; 
 
	 }



       /*     Complex representation for the coefficient C_9V:      * 
	*               C9eff=ReC9eff+i*ImC9eff                     * 
	*  by  A.J.Buras, M.Munz, Phys.Rev.D52 (1995), p189         * 
	*                                                           * 
	*   res_swch -- switching parameter:                        * 
	* res_swch=0 -- switch OFF for the resonant contribution;   * 
	* res_swch=1 -- switch ON for the resonant contribution;    * 
	*                                                           * 
	*         q2 -- the square of transition 4-momentum;        * 
	*         m2 -- b-quark mass ( in the heavy meson M1), GeV; * 
        *         M1 -- B-meson mass;                               *
	*         mu -- scale parameter, GeV;                       * 
	*         mc -- c-quark mass, GeV;                          * 
	*         mt -- t-quark mass, GeV;                          * 
	*         Mw -- mass of the W--meson, GeV;                  * 
	*         ml -- leptonic mass, GeV;                         * 
	*         Nf -- number of "effective" flavors               * 
	*                          (for b-quark Nf=5);              * 
	*        ias -- switching parameter for Lms[]               * 
	*                          in the As(..) function.          * 
	*                                                           */ 
EvtComplex EvtbTosllWilsCoeffNLO::GetC9Eff(int res_swch, double q2, double M1, double m2, 
                                           double mu, double mc, double mt, double Mw, 
                                           double ml, int Nf, int ias) 
        {
           double RReC9eff, IImC9eff;
           EvtComplex unit1(1.0,0.0);
           EvtComplex uniti(0.0,1.0);
           EvtComplex c9eff;

           RReC9eff=EvtbTosllWilsCoeffNLO::ReC9eff(res_swch, q2, M1, m2, mu, mc, mt, Mw, ml, Nf, ias);
           IImC9eff=EvtbTosllWilsCoeffNLO::ImC9eff(res_swch, q2, M1, m2, mu, mc, Mw, ml, Nf, ias);
	                                  
           c9eff=RReC9eff*unit1+IImC9eff*uniti;
           return c9eff;
        }



       /*     Complex representation for the coefficient C_10A:     *
        *                    C_10A=ReC_10                           *
	*  by  A.J.Buras, M.Munz, Phys.Rev.D52 (1995), p189         * 
	*                                                           * 
	*         mt -- t-quark mass, GeV;                          * 
	*         Mw -- mass of the W--meson, GeV;                  *
        *                                                           */
EvtComplex EvtbTosllWilsCoeffNLO::GetC10Eff(double mt, double Mw) 
       {
          double ReC10;
          EvtComplex c10eff;
          EvtComplex unit1(1.0,0.0);

          ReC10=EvtbTosllWilsCoeffNLO::C10a(mt, Mw);     

          c10eff=unit1*ReC10;

          return c10eff;
       }




       /*    Complex representation for the coefficient C7gamma:    * 
	*                  C7gamma=ReC7gamma                        * 
	*  by  A.J.Buras, M.Munz, Phys.Rev.D52 (1995), p189         * 
	*                                                           * 
	*         mu -- scale parameter, GeV;                       * 
	*         mt -- t-quark mass, GeV;                          * 
	*         Mw -- mass of the W--meson, GeV;                  * 
	*         Nf -- number of "effective" flavors               * 
	*                          (for b-quark Nf=5);              * 
	*        ias -- switching parameter for Lms[]               * 
	*                          in the As(..) function.          * 
	*                                                           */ 
EvtComplex EvtbTosllWilsCoeffNLO::GetC7Eff(double mu, double Mw, double mt, int Nf, int ias)
       {
         double CC7gamma; 
         EvtComplex c7eff;
         EvtComplex unit1(1.0,0.0);

         CC7gamma=EvtbTosllWilsCoeffNLO::C7gamma(mu, Mw, mt, Nf, ias );
         c7eff=unit1*CC7gamma;

         return c7eff;
       }

