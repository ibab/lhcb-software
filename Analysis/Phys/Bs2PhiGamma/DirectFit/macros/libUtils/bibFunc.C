
double bibCBl(double *x,double *par) {
  double t = (x[0]-par[1])/par[2];
  double func = 0;
  if (par[3] < 0) return 0.;
  if (par[4] < 0) return 0.;
  double absAlpha = fabs((double)par[3]);
  if (t >= -absAlpha) {
    func= par[0]/par[2]/sqrt(2*3.14159)*exp(-0.5*t*t);
  }
  else {
    double a = par[4]/absAlpha;
    double b= par[4]/absAlpha - absAlpha - t;
    func= par[0]/par[2]/sqrt(2*3.14159)*exp(-0.5*absAlpha*absAlpha) *pow(a/b,par[4]);
  }
  return func;
}
double bibCBr(double *x,double *par) {
  double t = (x[0]-par[1])/par[2];
  double func = 0;
  if (par[3] > 0) return 0.;
  if (par[4] < 0) return 0.;
  double absAlpha = fabs((double)par[3]);
  if (t <= absAlpha) {
    func= par[0]/par[2]/sqrt(2*3.14159)*exp(-0.5*t*t);
  }
  else {
    double a =  pow(par[4]/absAlpha,par[4])*exp(-0.5*absAlpha*absAlpha);
    double b= par[4]/absAlpha - absAlpha;
    func= par[0]/par[2]/sqrt(2*3.14159)*(a/pow(b + t, par[4]));
  }
  return func;
}

//======== Bifurcated CRYSTAL BAL
double bibDOUBLECB(double *x,double *par) {
  double t = (x[0]-par[1])/par[2];
  double func = 0;

  if (par[3] < 0) return 0.;
  if (par[5] > 0) return 0.;

  if (par[4] < 0) return 0.;
  if (par[6] < 0) return 0.;


  double absAlpha = fabs((double)par[3]);
  double absAlpha2 = fabs((double)par[5]);

  if (t >= -absAlpha && t <= +absAlpha2) {
    func= par[0]/par[2]/sqrt(2*3.14159)*exp(-0.5*t*t);
  }
  else if (t < -absAlpha){
    double a = par[4]/absAlpha;
    double b= par[4]/absAlpha - absAlpha - t;
    func= par[0]/par[2]/sqrt(2*3.14159)*exp(-0.5*absAlpha*absAlpha) *pow(a/b,par[4]);
  }
  else if( t > absAlpha2) {
    double a =  pow(par[6]/absAlpha2,par[6])*exp(-0.5*absAlpha2*absAlpha2);
    double b= par[6]/absAlpha2 - absAlpha2;
    func= par[0]/par[2]/sqrt(2*3.14159)*(a/pow(b + t, par[6]));
  }
  return func;
}


//-----------------------------
double bibCBl(double x0,double par0,double par1,double par2,double par3,double par4) {
  double t = (x0-par1)/par2;
  double func = 0;
  if (par3 < 0) return 0.;
  if (par4 < 0) return 0.;
  double absAlpha = fabs((double)par3);
  if (t >= -absAlpha) {
    func= par0/par2/sqrt(2*3.14159)*exp(-0.5*t*t);
  }
  else {
    double a = par4/absAlpha;
    double b= par4/absAlpha - absAlpha - t;
    func= par0/par2/sqrt(2*3.14159)*exp(-0.5*absAlpha*absAlpha) *pow(a/b,par4);
  }
  return func;
}
double bibCBr(double x0,double par0,double par1,double par2,double par3,double par4) {
  double t = (x0-par1)/par2;
  double func = 0;
  if (par3 > 0) return 0.;
  if (par4 < 0) return 0.;
  double absAlpha = fabs((double)par3);
  if (t <= absAlpha) {
    func= par0/par2/sqrt(2*3.14159)*exp(-0.5*t*t);
  }
  else {
    double a =  pow(par4/absAlpha,par4)*exp(-0.5*absAlpha*absAlpha);
    double b= par4/absAlpha - absAlpha;
    func= par0/par2/sqrt(2*3.14159)*(a/pow(b + t, par4));
  }
  return func;
}


double bibDOUBLECB(double x0,double par0,double par1,double par2,double par3,double par4,double par5,double par6) {
  double t = (x0-par1)/par2;
  double func = 0;

  if (par3 < 0) return 0.;
  if (par5 > 0) return 0.;

  if (par4 < 0) return 0.;
  if (par6 < 0) return 0.;


  double absAlpha = fabs((double)par3);
  double absAlpha2 = fabs((double)par5);

  if (t >= -absAlpha && t <= +absAlpha2) {
    func= par0/par2/sqrt(2*3.14159)*exp(-0.5*t*t);
  }
  else if (t < -absAlpha){
    double a = par4/absAlpha;
    double b= par4/absAlpha - absAlpha - t;
    func= par0/par2/sqrt(2*3.14159)*exp(-0.5*absAlpha*absAlpha) *pow(a/b,par4);
  }
  else if( t > absAlpha2) {
    double a =  pow(par6/absAlpha2,par6)*exp(-0.5*absAlpha2*absAlpha2);
    double b= par6/absAlpha2 - absAlpha2;
    func= par0/par2/sqrt(2*3.14159)*(a/pow(b + t, par6));
  }
  return func;
}



//======== CRYSTAL BALL
double bibCB(double *x,double *par) {
  double t = (x[0]-par[1])/par[2];
  double func = 0;
  if (par[3] < 0) t = -t;
  double absAlpha = fabs((double)par[3]);
  if (t >= -absAlpha) {
    func= par[0]/par[2]/sqrt(2*3.14159)*exp(-0.5*t*t);
  }
  else {
    double a =  pow(par[4]/absAlpha,par[4])*exp(-0.5*absAlpha*absAlpha);
    double b= par[4]/absAlpha - absAlpha;
    func= par[0]/par[2]/sqrt(2*3.14159)*(a/pow(b - t, par[4]));
  }
  return func;
}


//======== GAUSS
double bibGAUSS(double *x,double *par) {
  double func = par[0]/par[2]/sqrt(2*3.14159)*exp(-0.5*pow((x[0]-par[1])/par[2],2));
  return func ;
};


//======== EXPO
double bibEXPO(double *x,double *par){
  double func = par[0]*exp(par[1]*x[0]);
  return func;
}


//======== RADIATIVE VG DECAY SHAPE
double bibVG(double *x,double *par){
  double p_sig[5] = { par[0], par[1], par[2], par[3], par[4] };
  double p_exp[2] = { par[5]*par[0], par[6] };
  double p_bkg[3] = { par[7]*par[0], par[8], par[9]};
  double func = 0.;
  func += bibCB(x , p_sig);   // SIGNAL
  func += bibEXPO( x, p_exp );// COMBINATORIAL BKG
  func += bibGAUSS( x, p_bkg);// PARTIALLY RECONSTRUCTED BKG
  return func;
}


//======= RADIATIVE BKG DECAY SHAPE
double bibBKG(double *x,double *par){
  double p_exp[2] = { par[0], par[1] };
  double p_bkg[3] = { par[2], par[3], par[4]};
  double func = 0.;
  func += bibEXPO( x, p_exp ); // COMBINATORIAL BKG
  func += bibGAUSS( x, p_bkg); // PARTIALLY RECONSTRUCTED BKG
  return func;
}



// FORMATTING UTILITIES
std::string myFormat(std::string form,double a, double b,double c,double d){
  std::string is;
  sprintf(is.c_str(),form.c_str(), a,b,c,d);
  return is.c_str();
}
std::string myFormat(std::string form,double a, double b,double c){
  std::string is;
  sprintf(is.c_str(),form.c_str(), a,b,c);
  return is.c_str();
}
std::string myFormat(std::string form,double a, double b){
  std::string is;
  sprintf(is.c_str(),form.c_str(), a,b);
  return is.c_str();
}
std::string myFormat(std::string form,double a){
  std::string is;
  sprintf(is.c_str(),form.c_str(), a);
  return is.c_str();
}


#include "Math/WrappedTF1.h"
#include "Math/GaussIntegrator.h"
bool printFit(TF1* fit,double xp,double yp,TF1* bkg=NULL,
              std::string label="Gauss",TF1* sig=NULL,double xmin=0,double xmax=10000,
              std::string label2="Gauss2",TF1* sig2=NULL){
  double n  = fit->GetParameter(0);
  double en = fit->GetParError(0);
  double m  = fit->GetParameter(1);
  double em = fit->GetParError(1);
  double s  = fit->GetParameter(2);
  double es = fit->GetParError(2);
  double t  = fit->GetParameter(8);
  double et  = fit->GetParError(8);
  double chi2= fit->GetChisquare();
  double ndof= (double)fit->GetNDF();
  double step = fit->GetParameter(9);
  double n2  = fit->GetParameter(10);
  double en2 = fit->GetParError(10);
  double m2  = fit->GetParameter(11);
  double em2 = fit->GetParError(11);
  double s2  = fit->GetParameter(12);
  double es2 = fit->GetParError(12);

  
  // s/b
  double sob=-1;
  if( NULL != bkg && NULL !=sig){
    ROOT::Math::GaussIntegrator igB;
    ROOT::Math::WrappedTF1 wfiB(*bkg);
    ROOT::Math::GaussIntegrator igS;
    ROOT::Math::WrappedTF1 wfiS(*sig);
    igB.SetFunction(wfiB);
    igB.SetRelTolerance(0.001);
    igS.SetFunction(wfiS);
    igS.SetRelTolerance(0.001);
    double a=m-3*s;
    double b=m+3*s;
    double xb=igB.Integral(a,b);
    double xs=igS.Integral(a,b);
    cout << "---> " << a << " " << b << " " << xb << " " << xs << endl;
    sob = (b > 0 ) ? xs/xb : -1;
  }
  
  
  std::ostringstream osm("");
  std::ostringstream oss("");
  std::ostringstream osn("");
  std::ostringstream osm2("");
  std::ostringstream oss2("");
  std::ostringstream osn2("");
  std::ostringstream ost("");
  std::ostringstream osb("");
  std::ostringstream osc("");



  double nint = 0;
  double enint = 0;
  if (sig){
    nint = integrate(sig,xmin,xmax)/step;
    enint = nint * en/n;
  }
  else{
    nint = n;
    enint = en; 
  }
  
  double nint2 = 0;
  double enint2 = 0;
  if (sig2){
    nint2 = integrate(sig2,xmin,xmax)/step;
    enint2 = nint2 * en2/n2;
  }
  else{
    nint2 = n2;
    enint2 = en2; 
  }
  

  ost << "#tau_{exp} = "    << myFormat("(%4.2f #pm %4.2f)",t,et) << " GeV^{-1}c^{2}";
  osm << "#mu_{"<< label<<"} = "    << myFormat("(%4.1f #pm %4.1f)",m,em) << " MeV/c^{2}";
  oss << "#sigma_{"<<label<<"} = "    << myFormat("(%4.1f #pm %4.1f)",s,es) << " MeV/c^{2}";
  osn << "N_{"<<label<<"} = "    << myFormat("(%4.1f #pm %4.1f)",nint,enint) ;
  double p= TMath::Prob(chi2,ndof)*100;
  osc << "#chi^{2}/N_{dof} = " << myFormat("%4.2f/%3.0f (p=%4.1f \%)",chi2,ndof,p);
  osb << "(S/B)_{#pm 3#sigma} = " << myFormat("%4.2f",sob);

  if( sig2 ){
    osm2 << "#mu_{"<< label2<<"} = "    << myFormat("(%4.1f #pm %4.1f)",m2,em2) << " MeV/c^{2}";
    oss2 << "#sigma_{"<<label2<<"} = "    << myFormat("(%4.1f #pm %4.1f)",s2,es2) << " MeV/c^{2}";
    osn2 << "N_{"<<label2<<"} = "    << myFormat("(%4.1f #pm %4.1f)",nint2,enint2) ;
  }
  
  
  lhcbLatex->SetTextSize(0.025);
  double dy = yp/15.;
  lhcbLatex->DrawLatex(xp, yp, osn.str().c_str() );
  lhcbLatex->DrawLatex(xp, yp-1*dy, osm.str().c_str() );
  lhcbLatex->DrawLatex(xp, yp-2*dy, oss.str().c_str() );

  int kk=2;
  if(sig2){
    lhcbLatex->DrawLatex(xp, yp-3*dy, osn2.str().c_str() );
    lhcbLatex->DrawLatex(xp, yp-4*dy, osm2.str().c_str() );
    //lhcbLatex->DrawLatex(xp, yp-5*dy, oss2.str().c_str() );
    kk+=2;
  }
      
  lhcbLatex->DrawLatex(xp, yp-(kk+1)*dy, ost.str().c_str() );
  if(sob>=0)lhcbLatex->DrawLatex(xp, yp-(kk+2)*dy, osb.str().c_str() );
  lhcbLatex->DrawLatex(xp, yp-(kk+3)*dy, osc.str().c_str() );
  return true;
}


double integrate(TF1* func, double xmin=-999999., double xmax=+999999.){
  // CB integration
  ROOT::Math::GaussIntegrator ig;
  ROOT::Math::WrappedTF1 wfi(*func);
  ig.SetFunction(wfi);
  ig.SetRelTolerance(0.001);
  return ig.Integral(xmin,xmax);
}

