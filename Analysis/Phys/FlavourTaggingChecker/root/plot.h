#include <cmath>
#include "Riostream.h"
#include "TLine.h"
#include "TLegend.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TF1.h"
#include "TH1F.h"
#include "TFile.h"

using namespace std;

/** @file plot.h
 *
 *  auxiliary helper functions to be used in root macros
 *
 *  @author Marco Musy
 *  @date   2010-09-07
 */


////////////////////////////////////////////////////////////////////////
void plot_taggercut(TString f1, TString f2, char* name){
  //compares distributions from two tag.root files

  TFile fh1(f1); 
  TString hname = (TString)name;
  TH1F* h1 = (TH1F*)gROOT->FindObject(hname);
  if(h1==NULL || h1->GetEntries()==0) {
    cout<<name<<" is empty or non existent."<<endl;
    return;
  }
  TH1F* hcopy = (TH1F*) h1->Clone();
  hcopy->SetName("hcopy");

  TFile fh2(f2); 
  TH1F* h = (TH1F*)gROOT->FindObject(hname);
  h->SetLineColor(2);h->SetLineWidth(2); h->DrawNormalized(); 
  hcopy->DrawNormalized("same"); 
  TCanvas* c = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("c");  
  c->Update(); //c->Print((TString)"output/"+hname+(TString)".gif");

}

////////////////////////////////////////////////////////////////////////
void calculateOmega(TH1F* rh, TH1F* wh, TH1F* omega){//used by plot_omega()
  for(int i=1; i!=rh->GetNbinsX(); ++i) {
    double r = rh->GetBinContent(i);
    double w = wh->GetBinContent(i);
    if(r) if(w){
      double om = w/(r+w);
      double omerr = sqrt(om*(1-om)/(r+w));
      omega->SetBinContent(i, om);
      omega->SetBinError(i, omerr);
    }
  }
  omega->SetLineColor(kBlue);
  omega->SetMinimum(0.0); 
  omega->SetMaximum(0.7);
}

////////////////////////////////////////////////////////////////////////
TH1F* plot_ratio(TString name, TH1F* rh, TH1F* wh, TH1F* extra=0){ 
  if(!rh->GetEntries()){
    cout<<"plot_ratio: histo "<<rh->GetName()<<" is empty"<<endl; 
    return NULL;
  }
  if(!wh->GetEntries()){
    cout<<"plot_ratio: histo "<<wh->GetName()<<" is empty"<<endl;
    return NULL;
  }
  cout<<"plot_ratio of "<<rh->GetName()<<" and "<<wh->GetName()<<endl;

  TCanvas* c= (TCanvas*)gROOT->GetListOfCanvases()->FindObject("c");  
  if(!c){ cout<<"plot_ratio: canvas not found."; return NULL; }
  c->Clear(); 
  c->Divide(1,2);
  c->cd(1);

  TH1F* htot = (TH1F*) rh->Clone(); 
  htot->SetName("htotal");
  htot->Add(wh);
  htot->SetLineColor(2);
  htot->SetFillColor(2);
  htot->SetLineWidth(1); 
  htot->SetMinimum(0.0); 
  htot->SetTitle("; "   +name+"; Events");
  htot->Draw();

  rh->SetLineColor(0);
  rh->SetFillColor(3);
  rh->SetLineWidth(1); 
  rh->SetMinimum(0.0); 
  rh->Draw("same");
  
  if(extra) {
    extra->SetLineStyle(2);
    extra->SetLineColor(4);
    extra->SetLineWidth(1); 
    extra->Draw("same");
  }

  TLegend* rleg = new TLegend(0.78,0.76,0.89,0.89);  
  rleg->AddEntry(rh,"signal","f");
  rleg->AddEntry(htot,"background","f");
  if(extra) rleg->AddEntry(extra, extra->GetTitle(), "l");
  rleg->SetHeader((TString)"histo: "+rh->GetName());
  rleg->Draw();

  TH1F* hhh = (TH1F*) rh->Clone(); 
  hhh->SetName((TString)"hratio_"+rh->GetName()+(TString)"_"+wh->GetName());
  calculateOmega(wh, rh, hhh);
  hhh->SetTitle("; "   +name+"; Purity");
  hhh->SetMaximum(1.1);
  hhh->SetMinimum(0);
  c->GetPad(1)->RedrawAxis();
  c->cd(2); c->GetPad(2)->SetGrid();
  hhh->Fit("pol2");
  double p0= hhh->GetFunction("pol2")->GetParameter(0);
  double p1= hhh->GetFunction("pol2")->GetParameter(1);
  double p2= hhh->GetFunction("pol2")->GetParameter(2);
  cout<< "double prob_"<<name<<" = pol("<<name 
      <<", "<<p0<<", "<<p1<<", "<<p2<<");"<<endl;

  c->Print("output/ratio_"+name+".gif"); c->cd();
  c->Update();

  return hhh;
}
////////////////////////////////////////////////////////////////////////
/// plots results opts is the polinomial degree.
TH1F* plot_omega(TH1F* rh, TH1F* wh, TString name, int opts=0){ 

  if(!rh->GetEntries()){
    cout<<"plot_omega: histo "<<rh->GetName()<<" is empty"<<endl; 
    return NULL;
  }
  if(!wh->GetEntries()){
    cout<<"plot_omega: histo "<<wh->GetName()<<" is empty"<<endl;
    return NULL;
  }
  cout<<"plot_omega of "<<rh->GetName()<<" and "<<wh->GetName()<<endl;

  TCanvas* c= (TCanvas*)gROOT->GetListOfCanvases()->FindObject("c");  
  if(!c){ cout<<"plot_omega: canvas not found."; return NULL; }
  c->Clear(); 
  c->Divide(1,2);
  c->cd(1);

  rh->SetLineColor(3);
  rh->SetLineWidth(2); 
  rh->SetMinimum(0.0); 
  rh->SetTitle("; "+name+";Events");
  rh->Draw();

  wh->SetLineColor(2);
  wh->SetLineWidth(2); 
  wh->SetMinimum(0.0); 
  wh->Draw("same");
  c->GetPad(1)->RedrawAxis();

  TLegend* aleg = new TLegend(0.78,0.76,0.89,0.89);  
  aleg->AddEntry(rh,"right","l");
  aleg->AddEntry(wh,"wrong","l");
  aleg->SetHeader((TString)"histo: "+rh->GetName());
  aleg->Draw();

  c->cd(2); c->GetPad(2)->SetGrid();
  TH1F* hhh = (TH1F*) rh->Clone(); hhh->Reset();
  calculateOmega(rh, wh, hhh);
  if(opts==0) hhh->SetName("hom");
  if(opts>0 ) hhh->SetName("homp");
  if(opts==0) hhh->SetTitle(";NNET Output for "   +name+";Wrong tag fraction");
  if(opts>0 ) hhh->SetTitle("; "+name+";TRUE OMEGA");

  //draw it
  if(opts<2) hhh->Fit("pol1"); 
  if(opts==2) {
    hhh->Fit("pol2");
    double p0= hhh->GetFunction("pol2")->GetParameter(0);
    double p1= hhh->GetFunction("pol2")->GetParameter(1);
    double p2= hhh->GetFunction("pol2")->GetParameter(2);
    cout<< name <<"  "	<<p0<<"  "<<p1<<"  "<<p2<<endl;
  } 
  if(opts==3) {
    hhh->Fit("pol3");
    double p0= hhh->GetFunction("pol3")->GetParameter(0);
    double p1= hhh->GetFunction("pol3")->GetParameter(1);
    double p2= hhh->GetFunction("pol3")->GetParameter(2);
    double p3= hhh->GetFunction("pol3")->GetParameter(3);
    cout<< name <<"  "	<<p0<<"  "<<p1<<"  "<<p2<<"  "<<p3<<endl;
  } 
  if(opts<2 && hhh->GetXaxis()->GetXmin() <= 0) {
    TLine* a=new TLine(0,0, .6,.6);
    a->Draw(); a->SetLineColor(4); a->SetLineStyle(3); a->SetLineWidth(2);
  }
  c->Print("output/plot_omega_"+name+".gif"); 
  c->cd();
  c->Update();

  return hhh;
}

////////////////////////////////////////////////////////////////////////
TH1F* plotEffectiveEff(TH1F* rh, TH1F* wh, TString direction="left2right"){

  if(!rh->GetEntries()){
    cout<<"plotEffectiveEff: histo "<<rh->GetName()<<" is empty"<<endl; 
    return NULL;
  }
  if(!wh->GetEntries()){
    cout<<"plotEffectiveEff: histo "<<wh->GetName()<<" is empty"<<endl;
    return NULL;
  }
  TH1F* h1 = (TH1F*)gROOT->FindObject("h1");
  if(!h1){
    cout<<"plotEffectiveEff: histogram h1 does not exist! Needed to "
	<<"get the total nr of events. Skip."<<endl;
    return NULL;
  }
  if(!h1->GetEntries()){
    cout<<"plotEffectiveEff: histo "<<h1->GetName()<<" is empty"<<endl;
    return NULL;
  }
  double nsele = h1->GetEntries(); //get total nr of events

  TH1F* effeff = (TH1F*) rh->Clone(); effeff->Reset();
  effeff->SetName((TString)"heffeff_"+rh->GetName()+wh->GetName());
  effeff->SetTitle((TString)";"+rh->GetName()+(TString)";EffectiveEff(cut)");

  for(int i=1; i!=rh->GetNbinsX(); ++i) {

    double rtag=0, wtag=0;
    if(direction == "left2right") {
      for(int j=i; j!=rh->GetNbinsX()+1; j++)  {
        rtag += rh->GetBinContent(j);
        wtag += wh->GetBinContent(j);
      }
    } else if(direction == "right2left") {
      for(int j=1; j!=i+1; j++)  {
        rtag += rh->GetBinContent(j);
        wtag += wh->GetBinContent(j);
      }
    } else cout<<"Error: unknown option "<<direction<<endl;

    if(rtag && wtag) {
      double utag  = nsele-rtag-wtag;              // untagged
      double omtag = wtag/(rtag+wtag);
      double eftag = (rtag+wtag)/nsele;           // tagging efficiency
      double epsil = eftag*pow(1-2*omtag,2);      // effective efficiency
      if(rtag<wtag) epsil= -epsil;
      double epsilerr = sqrt((pow(rtag - wtag,2)*
			      (-(pow(rtag - wtag,2)*(rtag +wtag))+nsele
			       *(pow(rtag,2) +14*rtag*wtag+ pow(wtag,2))))
			     /(pow(rtag+wtag+utag,3)*pow(rtag + wtag,3)));
      effeff->SetBinContent(i, epsil*100);
      effeff->SetBinError(i, epsilerr*100);
    }
  }

  TCanvas* c= (TCanvas*)gROOT->GetListOfCanvases()->FindObject("c");  
  if(!c) return effeff;
  c->Clear(); c->Divide(1,2); c->cd(1);
  rh->SetFillColor(kGreen); 
  rh->Draw();
  wh->SetFillColor(kRed); 
  wh->Draw("same"); 
  c->cd(2); 
  gPad->SetGrid(); 
  effeff->Draw();
  c->Print((TString)"output/heffeff_"+rh->GetName()+(TString)".gif");

  return effeff;
}
////////////////////////////////////////////////////////////////////////
TH1F* plot_Eff_Omega(TH1F* rh, TH1F* wh, TString direction="left2right"){

  if(!rh->GetEntries()){
    cout<<"plotEffectiveEff: histo "<<rh->GetName()<<" is empty"<<endl; 
    return NULL;
  }
  if(!wh->GetEntries()){
    cout<<"plotEffectiveEff: histo "<<wh->GetName()<<" is empty"<<endl;
    return NULL;
  }
  TH1F* h1 = (TH1F*)gROOT->FindObject("h1");
  if(!h1){
    cout<<"plotEffectiveEff: histogram h1 does not exist! Needed to "
	<<"get the total nr of events. Skip."<<endl;
    return NULL;
  }
  if(!h1->GetEntries()){
    cout<<"plotEffectiveEff: histo "<<h1->GetName()<<" is empty"<<endl;
    return NULL;
  }
  double nsele = h1->GetEntries(); //get total nr of events

  TH1F* effeff = (TH1F*) rh->Clone(); effeff->Reset();
  effeff->SetName((TString)"heffom_"+rh->GetName()+wh->GetName());
  effeff->SetTitle((TString)";"+rh->GetName()+(TString)";EffectiveEff(cut)");

  for(int i=1; i!=rh->GetNbinsX(); ++i) {

    double rtag=0, wtag=0;
    if(direction == "left2right") {
      for(int j=i; j!=rh->GetNbinsX()+1; j++)  {
        rtag += rh->GetBinContent(j);
        wtag += wh->GetBinContent(j);
      }
    } else if(direction == "right2left") {
      for(int j=1; j!=i+1; j++)  {
        rtag += rh->GetBinContent(j);
        wtag += wh->GetBinContent(j);
      }
    } else cout<<"Error: unknown option "<<direction<<endl;

    if(rtag && wtag) {
      double utag  = nsele-rtag-wtag;              // untagged
      double omtag = wtag/(rtag+wtag);
      double eftag = (rtag+wtag)/nsele;           // tagging efficiency
      double epsil = eftag*pow(1-2*omtag,2);      // effective efficiency
      if(rtag<wtag) epsil= -epsil;
      double epsilerr = sqrt((pow(rtag - wtag,2)*
			      (-(pow(rtag - wtag,2)*(rtag +wtag))+nsele
			       *(pow(rtag,2) +14*rtag*wtag+ pow(wtag,2))))
			     /(pow(rtag+wtag+utag,3)*pow(rtag + wtag,3)));
      effeff->SetBinContent(i, epsil*100);
      effeff->SetBinError(i, epsilerr*100);
    }
  }

  TCanvas* c= (TCanvas*)gROOT->GetListOfCanvases()->FindObject("c");  
  if(!c) return effeff;
  c->Clear(); c->Divide(1,3); c->cd(1);
  rh->SetFillColor(kGreen); 
  rh->Draw();
  wh->SetFillColor(kRed); 
  wh->Draw("same"); 
  c->cd(2); 
  gPad->SetGrid(); 
  effeff->Draw();
  c->cd(3); c->GetPad(3)->SetGrid();
  TH1F* hhhh = (TH1F*) rh->Clone(); hhhh->Reset();
  calculateOmega(rh, wh, hhhh);
  hhhh->SetName("hom");
  hhhh->Draw("e");

  c->Print((TString)"output/heff_om_"+rh->GetName()+(TString)".gif");

  return effeff;
}
//////////////////////////////////////////////////////////////////////////
TGraphErrors* asymmetry(TH1F* rh, TH1F* wh, TString name="asymplot" ){ 

  if(!rh->GetEntries()){
    cout<<"asymmetry: histo "<<rh->GetName()<<" is empty"<<endl; 
    return NULL;
  }
  if(!wh->GetEntries()){
    cout<<"asymmetry: histo "<<wh->GetName()<<" is empty"<<endl;
    return NULL;
  }

  //rh and wh should have large nr of bins
  int bins=12;
  double tbin[13] =
    { 0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.2, 4.9, 6.0, 7.5, 10.0 };
  //---------

  double rhnew[100], whnew[100], total=0;
  for(int i=0; i!=100; ++i) {rhnew[i]=0; whnew[i]=0;}
  
  for(int i=0; i!=rh->GetNbinsX(); ++i) {
    //decide new bin:
    int newbin = 0;
    double xbin = rh->GetBinCenter(i);
    for( int j=0; j!=bins-1; ++j) { 
      if( xbin>=tbin[j] && xbin<tbin[j+1] ) {newbin=j; break;}
      if( xbin<tbin[0] ) newbin=0;
      if( xbin>tbin[bins-1] ) newbin=bins-1;
    }
    //cout<<xbin<< " is put in "<<newbin<<endl;

    //fill rhnew, whnew (rebin):
    double r = rh->GetBinContent(i);
    double w = wh->GetBinContent(i);
    rhnew[newbin] += r;
    whnew[newbin] += w;
    total += r;
    total += w;
  }

  //fill points:
  int ireal=0;
  Float_t ax[13],ay[13],aex[13],aey[13];
  for( int i=0; i!=bins; ++i) { 
    Float_t a = rhnew[i];
    Float_t b = whnew[i];
    Float_t sumsq = (a+b)*(a+b);
    if( sumsq>0 ) {
      Float_t xt= (tbin[i+1]+tbin[i])/2;
      Float_t dx= (tbin[i+1]-tbin[i])/3.46;//sqrt(12)
      Float_t As= (a-b)/(a+b);
      Float_t siga = a*(1-a/total);
      Float_t sigb = b*(1-b/total);
      Float_t term1= pow( 2*b/sumsq, 2 ) * siga;
      Float_t term2= pow(-2*a/sumsq, 2 ) * sigb;
      Float_t cov  = -a*b/total;
      Float_t term3= 2 * (2*b/sumsq) * (-2*a/sumsq) * cov;
      Float_t term4= 2./sumsq; //smoothes small nrs
      Float_t sigAs= sqrt( term1 + term2 + term3 + term4);
      //Float_t sigAs= sqrt(a+b)/(a+b); //banale
      //cout<<a<<" "<<b<<"    "<<xt<<"    "<<As<<"    "<<sigAs<<endl;
      ax[ireal]=xt;
      ay[ireal]=As;
      aex[ireal]=dx;
      aey[ireal]=sigAs;
      ireal++;
    }
  }

  TGraphErrors* asymplot = new TGraphErrors(ireal,ax,ay,aex,aey);
  asymplot->SetName(name);
  asymplot->SetTitle("; time/ps; Asymmetry");
  asymplot->SetLineColor(4);
  asymplot->SetMarkerColor(4);
  asymplot->SetMarkerStyle(20);
  asymplot->SetMarkerSize(0.6);
  /* asymplot->SetMinimum(-0.7);  */
  /* asymplot->SetMaximum( 0.7);  */
  asymplot->SetMinimum(-1.1); 
  asymplot->SetMaximum( 1.1); 
  gPad->SetGrid();

  return asymplot;
}

void plotPID(TString t, TH1F* h1, TH1F* h2, TH1F* h3 ){

  if(!h1->GetEntries()){
    cout<<"plotID: histo "<<h1->GetName()<<" is empty"<<endl; 
    return ;
  }
  if(!h2->GetEntries()){
    cout<<"plotID: histo "<<h2->GetName()<<" is empty"<<endl; 
    return ;
  }
  if(!h3->GetEntries()){
    cout<<"plotID: histo "<<h3->GetName()<<" is empty"<<endl; 
    return ;
  }

  TCanvas* c= (TCanvas*)gROOT->GetListOfCanvases()->FindObject("c");  
  if(!c) {cout<<"canvas named c not found!"<<endl; exit(1);}
  c->Clear(); c->Divide(1,2);

  c->cd(1);
  h1->SetFillColor(4); h1->SetFillStyle(3001);
  h1->SetLineColor(4);
  h1->Draw();
  h1->SetTitle((TString)";#Delta LL ("+t+(TString)");Events");
  h2->SetFillColor(kRed); h2->SetFillStyle(3001);
  h2->SetLineColor(kRed);
  h2->SetLineWidth(1); 
  h2->Draw("same");
  h3->SetLineWidth(1); 
  h3->SetLineColor(7);
  h3->Draw("same");
  c->GetPad(1)->RedrawAxis();

  TLegend* pleg = new TLegend(0.75,0.7,0.89,0.89);  
  pleg->AddEntry(h1,"Total","f");
  pleg->AddEntry(h2,"MisID","f");
  pleg->AddEntry(h3,"ghosts","l");
  pleg->SetHeader((TString)"histo: "+h1->GetName());
  pleg->Draw();

  c->cd(2);
  gPad->SetBorderMode(0);
  gPad->SetGrid();
  TH1F *hom = (TH1F*) h2->Clone(); 
  hom->Sumw2();
  hom->Divide(hom,h1);
  hom->SetMinimum(0.0); hom->SetMaximum(1.1);
  hom->SetLineColor(kRed);
  hom->SetLineWidth(2);
  hom->SetTitle((TString)";#Delta LL ("+t+(TString)"); PID Purity");
  hom->Draw("pe");

  TH1F *hom1 = (TH1F*) h3->Clone(); 
  hom1->Sumw2();
  hom1->SetName("hghostfrac");
  hom1->Divide(hom1,h1);
  hom1->SetLineColor(7);
  hom1->SetLineWidth(2);
  hom1->Draw("same,pe");

  //xy,xy
  TLegend* gleg = new TLegend(0.34,0.71,0.12,0.89);  
  gleg->AddEntry(hom,  (TString)" Correct ID as "+t,"ple");
  gleg->AddEntry(hom1, " Ghost fraction","ple");
  gleg->Draw();

  c->GetPad(2)->RedrawAxis();
  c->Print((TString)"output/plotID_"+t+(TString)".gif");
}

#include <unistd.h>
#include <termios.h>
int mygetch(void) {
  int ch;
  struct termios oldt;
  struct termios newt;
  tcgetattr(STDIN_FILENO, &oldt); /*store old settings */
  newt = oldt; /* copy old settings to new settings */
  newt.c_lflag &= ~(ICANON | ECHO); /* change to old settings in new settings */
  tcsetattr(STDIN_FILENO, TCSANOW, &newt); /*apply the new settings immediately */
  ch = getchar(); /* standard getchar call */
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt); /*reapply the old settings */
  //cout<<ch<<endl;
  return ch; /*return received char */
}
bool wait(TCanvas * c = NULL) {
  if(!c)c = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("c");  
  if(!c)c = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("c1");  
  if(c) c->cd();
  if(c) c->Update();
  cout << "--> Hit any key to continue, q to quit (x to exit root). ";
  char z = mygetch();cout<<z<<endl;
  //char z = getchar(); //normal std call
  if(z=='x') gSystem->Exit(0);
  if(z=='q') return true; else return false;
}
bool wait(TCanvas * c, TString a) {
  if(c) c->Print(a);
  return wait(c);
}

