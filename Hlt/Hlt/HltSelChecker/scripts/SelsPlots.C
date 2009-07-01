#include <iostream>
#include <cmath>
#include <vector>
#include "TH1D.h"
#include "TString.h"
#include "TChain.h"
#include "TCut.h"
#include "TPad.h"
#include "TLine.h"


using namespace std ;
//
// To debug SelsPlots(MB12,false,"Hlt2UnbiasedBs2PhiPhiDecision")
// @todo wiki
// @todo number of collisions without Hlt1
//

void eps(TString);

//
//-----------------------------------------------------------------------------
//
void tex_header(){
  std::cout << "\\topleft{\\scalebox{0.75}{\\smallpage{1.25}{%" << std::endl ;
  std::cout << "\\begin{tabular}{|l |RRR|CC|}%" << std::endl ;
  std::cout << "\\hline" << std::endl ;
  std::cout << "Selection\\qquad\\qquad\\qquad\\qquad\\qquad\\qquad & \\text{No Hlt1}" 
            << " & \\text{Rate} & \\text{Weighted}  & \\text{\\# coll} & \\text{\\% \\Pb}  \\\\" 
            << std::endl ;
  std::cout << "\\hline" << std::endl ;  
};
//
//-----------------------------------------------------------------------------
//
void wiki_header(bool showHlt1){
  if ( showHlt1){
    std::cout << "|  *Selection*  |  *No Hlt1*  |  *Rate*  |  *Weighted*  |  *# Coll.*  |  *% b*  |" << std::endl ;
  } else {
    std::cout << "|  *Selection*  |  *Rate*  |  *Weighted*  |  *# Coll.*  |  *% b*  |" << std::endl ;
  }
};
//
//-----------------------------------------------------------------------------
//
void tex_footer(){
  std::cout << "}\\end{tabular}}}}" << std::endl ;
};
//
//-----------------------------------------------------------------------------
//
void wiki_footer(bool showHlt1){
  std::cout << std::endl ;
  if (showHlt1) std::cout << "   $ *No Hlt1* : Hlt2 rate without running Hlt1 (but L0)" << std::endl ;
  std::cout << "   $ *Rate* : Hlt2 rate after Hlt1. " << std::endl ;
  std::cout << "   $ *Weighted* : Hlt2 rate after Hlt1 and re-weighting all events by the number of";
  std::cout << "selections that have selected it (i.e. how much does this selection contribute to the rate?).";
  std::cout << "The sum should be equal to the Hlt2 rate. *This is probably the number you want*." << std::endl ;
  std::cout << "   $ *# coll* : Average number of pp collision in selected events (the lower the better)" << std::endl ;
  std::cout << "   $ *% b* : Fraction of b quark in selected events (the higher the better)" << std::endl ;
};
//
//-----------------------------------------------------------------------------
//
void tex_separator(){
  std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl ;
};
//
//-----------------------------------------------------------------------------
//
void tex_newpage(int i){
  if (i>1)  std::cout << "\\hline}";
  std::cout << "\\onlySlide*{" << i << "}{" << std::endl ;
};
//
//-----------------------------------------------------------------------------
//
void tex_printRate(double rate, double err, bool exception){
  if ( exception )   cout << "\\blue " ;
  cout << int(rate+0.5) ;
  if (int(err+0.5)>=1) std::cout << "\\pm" << int(err+0.5)  ;
  cout << "\\:\\text{Hz}} & {" ;
}
//
//-----------------------------------------------------------------------------
//
void wiki_printRate(double rate, double err, bool hlt1){
  if ( hlt1 )   cout << "%BLUE%" ;
  if ( rate < 0.0001 ){
    // print nothing
  } else if ( err < 0.5 ){ // tiny rates
    if ( err < 0.0001 ){ // no error -> L0
      cout << int(rate+0.5)  << " Hz";
    } else {
      cout << 0.1*int(10*rate+0.5) << "&plusmn;" <<  0.1*int(10.*err+0.5) << " Hz";
    }
  } else { // normal rates
    cout << int(rate+0.5) << "&plusmn;" <<int(err+0.5) << " Hz" ;
  }
  if (hlt1) cout << " %ENDCOLOR%  |  " ;
  else cout << "  |  " ;
}
//
//-----------------------------------------------------------------------------
//
double SumWeights(TH1D* h, double& err){
  if (0==h) return -1 ;
  unsigned int nb = h->GetNbinsX();
  double sumw = 0 ;
  err = 0 ;
  for ( unsigned int i = 1 ; i<=nb ; i++){
    double bc = h->GetBinCenter(i);
    double cc = h->GetBinContent(i);
    if (bc>0 && cc>0 ){
      sumw += cc/bc ;
      err += cc/(bc*bc);
      //      cout << i << " center : " << bc << " content : " << cc << " sum : " << sumw << endl ;
    }
  }
  err = sqrt(err);
  return sumw ;
}
//
//-----------------------------------------------------------------------------
//
void SelsPlots(TChain* MB12, bool wiki=true, char* DEBUG=""){

  
  double L0eff = 1.0*MB12->GetEntries("L0Decision==1")/MB12->GetEntries("L0Decision>=0") ;
  if ( L0eff < 0.95) cout << "Warning :: L0 is " << 100*L0eff << "%!\n" << endl ;
  int L0events = MB12->GetEntries("L0Decision==1");
  int Hlt1events = MB12->GetEntries("Hlt1Global==1");
  double Hlt1eff = 1.0*Hlt1events/L0events ;
  double normalisation = 1000000./L0events  ;
  bool showHlt1 = true ;
  if ( Hlt1eff > 0.5 ){
    cout << "Hlt1 eff is " << 100*Hlt1eff << "%. Assuming it is already applied at " << 40000.*Hlt1eff << " kHz\n" << endl ;
    double normalisation = 40000.*Hlt1eff/MB12->GetEntries("Hlt1Global==1")  ;
    showHlt1 = false ;
  } else {
    cout << "Hlt1 eff is " << 100*Hlt1eff << "%. Assuming it is not already applied.\n" << endl ;
  }
  
  if ( wiki ){
    cout << "Using " << MB12->GetEntries("L0Decision==1") << " events. Normalisation factor is " 
         << normalisation << endl ;
    wiki_header(showHlt1);
  } else {
    cout << "% normalisation is " << normalisation << endl ;
    tex_separator();
    tex_header();
  }
  
  std::vector<TString> sels ;
  if ( Hlt1eff < 0.5 ) sels.push_back( "L0Decision" );
  sels.push_back( "Hlt1Global" );
  sels.push_back( "Hlt2Global" );
  TObjArray* a = MB12->GetListOfLeaves();

  for ( unsigned int i = 0 ; i<a->GetEntries() ; i++){
    TObject* o = a->At(i);
    TString ss = TString(o->GetName()) ;
    if ( DEBUG=="" ){
      if (( ss.BeginsWith("Hlt")) &&  (ss.EndsWith("Decision"))) {
        sels.push_back(ss);
      }
    }
  }
  if ( DEBUG!="" ) sels.push_back(TString(DEBUG));
  TH1D* Hrates = new TH1D("Hrates","rate",sels.size(),-0.5,sels.size()-0.5);  
  TH1D* HratesNo1 = new TH1D("HratesNo1","rate without Hlt1",sels.size(),-0.5,sels.size()-0.5);  
  TH1D* Hcoll = new TH1D("Hcoll","collisions",sels.size(),-0.5,sels.size()-0.5);  
  TH1D* Hbfrac = new TH1D("Hbfrac","b fraction",sels.size(),-0.5,sels.size()-0.5);  

  TH1D* Hcolls = new TH1D("Hcolls","colls",10,-0.5,9.5);
  TH1D* Hbfracs = new TH1D("Hbfracs","bfracs",2,-0.5,1.5);

  TH1D* HnHltSels = new TH1D("HnHltSels","nSelections",sels.size(),0.5,sels.size()+0.5);
  
  int nn = (int(sels.size()/2))-1 ; // where the separation is

  double avgColl_2 ;
  double avgColl_12 ;
  double avgB_2 ;
  double avgB_12 ;

  int nlines_max = 14 ;
  int nlines = 0 ;
  int page = 0 ;
  double sumWeighted = 0 ;
  double sumRates = 0 ;
  double Hlt2Global = 0 ;

  std::vector<TString> inactive;

  for ( int s = 0 ; s!=sels.size() ; s++){

    TString FullName = sels[s] ;
    TString ss = sels[s] ;

    bool isHlt2Selection = ((ss.Contains("Hlt2")) && (ss.Contains("Decision"))) ;
    bool isHlt1Selection = ((ss.Contains("Hlt1")) && (ss.Contains("Decision"))) ;
    ss.ReplaceAll("Decision","");
    if (!wiki) ss.ReplaceAll("_","\\_");

    double rateNo = MB12->Draw("L0Decision",FullName+"==1") ;
    double rr = 1.0*rateNo/L0events ;
    double errNo = sqrt(rr*(1-rr)/L0events)*L0events*normalisation;
    
    // cout << sels[s] << " " << rateNo << " " << errNo << " " << L0events << " " << normalisation << endl ;

    if (s==0) errNo = 0 ;
    rateNo *= normalisation ;
    double rateWithHlt1 = MB12->Draw("Hlt2nSelections >> HnHltSels",FullName+"==1 && Hlt1Global==1 && L0Decision==1");
    rr = 1.0*rateWithHlt1/Hlt1events ;
    double errWithHlt1  = sqrt(rr*(1-rr)/Hlt1events)*normalisation*Hlt1events  ;
    rateWithHlt1 *= normalisation ;
    // -1 because of lumi
    if ( isHlt1Selection ) MB12->Draw("Hlt1nSelections-1 >> HnHltSels",FullName+"==1 && Hlt1IgnoringLumiDecision==1");
    double errWeighted = 0 ; 
    double Weighted = normalisation*SumWeights(HnHltSels,errWeighted) ;
    if ( isHlt2Selection ) {
      errWeighted *= normalisation ;
      sumWeighted += Weighted ;
      sumRates += rateWithHlt1 ;
    } 
      
    if (ss=="Hlt2Global") Hlt2Global = rateWithHlt1 ;

    Hrates->GetXaxis()->SetBinLabel(s+1,sels[s]);
    HratesNo1->GetXaxis()->SetBinLabel(s+1,sels[s]);
    if ( s>0 ) {
      Hrates->SetBinContent(s+1,rateWithHlt1);
      Hrates->SetBinError(s+1,errWithHlt1);
      HratesNo1->SetBinContent(s+1,rateNo);
      HratesNo1->SetBinError(s+1,errNo);
    }

    if (ss!="Hlt1Global" && ss!="L0Decision") 
      MB12->Draw("Collisions >> Hcolls", FullName+"==1 && Hlt1Global==1 && L0Decision==1") ;
    else MB12->Draw("Collisions >> Hcolls", FullName+"==1") ;
    Hcoll->SetBinContent(s+1,Hcolls->GetMean());
    Hcoll->SetBinError(s+1,Hcolls->GetMeanError());
    Hcoll->GetXaxis()->SetBinLabel(s+1,sels[s]);

    if (s>1) MB12->Draw("HeaviestQuarkInEvent==5 >> Hbfracs", FullName+"==1 && Hlt1Global==1 && L0Decision==1") ;
    else MB12->Draw("HeaviestQuarkInEvent==5 >> Hbfracs", FullName+"==1") ;
    Hbfrac->SetBinContent(s+1,Hbfracs->GetMean());
    Hbfrac->SetBinError(s+1,Hbfracs->GetMeanError());   
    Hbfrac->GetXaxis()->SetBinLabel(s+1,sels[s]);

    if (s==2) avgColl_12 = Hcolls->GetMean() ;
    if (s==2) avgB_12 = Hbfracs->GetMean() ;

    // @todo make that one block

    if ( wiki ){
      if (rateWithHlt1<0.01) {
        inactive.push_back(ss) ;
        continue ;
      }	
      std::cout << "| " ;
      if (isHlt1Selection) std::cout << "%BLUE%" ;
      std::cout << "<nox>" << ss  ; 
      if (isHlt1Selection) std::cout << "%ENDCOLOR% |  " ;
      else std::cout << " |  " ;
      if ( showHlt1 ){
        if (ss.Contains("Hlt2")) wiki_printRate(rateNo,errNo,isHlt1Selection);
        else std::cout << "  |  " ;
      }
      wiki_printRate(rateWithHlt1,errWithHlt1,isHlt1Selection);
      if (s>2) wiki_printRate(Weighted,errWeighted,isHlt1Selection);
      else std::cout << "  |  " ;
      if ( rateWithHlt1 > 0.0001 ) std::cout << (int(Hcolls->GetMean()*10.))/10. ;
      std::cout    << "  |  " ;
      if ( rateWithHlt1 > 0.0001 ) std::cout << (int(100*Hbfracs->GetMean())) << "%" ;
      std::cout << "  |  " << std::endl ;
    } else {
      ss.ReplaceAll("Hlt2","\\quad ");
      if ( rateWithHlt1<0.01 ) std::cout << "% " ;
      else if ( s>2 ) nlines++ ;
      if (isHlt1Selection) std::cout << "{\\bf " ;
      else std::cout << "{" ;
      std::cout << ss  << "} & {" ;
      if ( s<2 ){
        std::cout << "} & {" ;
        tex_printRate(rateNo,errNo,isHlt1Selection);
        std::cout << "} & {" ;
      } else {
        tex_printRate(rateNo,errNo,isHlt1Selection);
        tex_printRate(rateWithHlt1,errWithHlt1,isHlt1Selection);
        if ( s>2 ) tex_printRate(Weighted,errWeighted,isHlt1Selection);
        else std::cout << "} & {" ;
      } 
      if ( Hcolls->GetMean()-Hcolls->GetMeanError()>avgColl_12) std::cout << "\\red " ;
      std::cout << (int(Hcolls->GetMean()*10.))/10. << "} & {" ;
      if ( Hbfracs->GetMean()+Hbfracs->GetMeanError()<avgB_12) std::cout << "\\red " ;
      std::cout << (int(100*Hbfracs->GetMean())) << "\\%} "  ;    
      std::cout << "\\\\ " << std::endl ;
      if ((s==3) || (nlines==nlines_max)) {
        nlines = 0 ;  
        page++ ;
        tex_newpage(page);
      } 
    }
    
  }
  if (!wiki){
    tex_footer();
    tex_separator();
    cout << "% Sum of rates is " << sumRates << ", and weights is " << sumWeighted 
         <<" =? " << Hlt2Global << endl ;
  } else {
    wiki_footer(showHlt1) ;
  }
  
  cout << endl ;

  if (!inactive.empty()){
    cout << "Inactive selections : "  ;
    for ( int si = 0 ; si != inactive.size() ; ++si) {
       cout << "=" << inactive[si] << "= "  ;
    }
    cout << endl ;
  }
  
  if (!wiki) cout << "% Sum of rates is " << sumRates << ", and weights is " << sumWeighted 
                 <<" =? " << Hlt2Global << endl ;
  else  cout << "\nCheck : Sum of rates is " << sumRates << ", and weights is " << sumWeighted 
             << "Hz ( =? " << Hlt2Global << ")" << endl ;

  gPad->SetLogy(0);
  Hbfrac->GetYaxis()->SetTitle(Hbfrac->GetTitle());
  Hbfrac->GetXaxis()->SetLabelSize(0.03);
  Hbfrac->GetYaxis()->SetTitleOffset(1.4);
  Hbfrac->SetTitle("");
  Hbfrac->SetMinimum(0.);
  Hbfrac->SetMaximum(1.);
  Hbfrac->Draw();
  TLine* l1 = new TLine(-0.5,avgB_12,sels.size()-0.5,avgB_12);
  l1->SetLineWidth(2);
  l1->SetLineColor(40);
  l1->Draw();
  eps("bfractions");

  gPad->SetLogy(0);
  Hcoll->GetYaxis()->SetTitle(Hcoll->GetTitle());
  Hcoll->GetXaxis()->SetLabelSize(0.03);
  Hcoll->GetYaxis()->SetTitleOffset(1.4);
  Hcoll->SetTitle("");
  Hcoll->LabelsDeflate();
  Hcoll->SetMinimum(1.);
  Hcoll->Draw();
  TLine* l2 = new TLine(-0.5,avgColl_12,sels.size()-0.5,avgColl_12);
  l2->SetLineWidth(2);
  l2->SetLineColor(40);
  l2->Draw();
  eps("Coll");
 
  gPad->SetLogy(1);
  HratesNo1->GetYaxis()->SetTitle(Hrates->GetTitle());
  HratesNo1->GetXaxis()->SetLabelSize(0.03);
  HratesNo1->GetYaxis()->SetTitleOffset(1.4);
  HratesNo1->SetTitle("");
  HratesNo1->LabelsDeflate();
  HratesNo1->SetLineColor(2);
  HratesNo1->SetMinimum(0.5);
  HratesNo1->Draw();
  Hrates->Draw("Same");
  eps("Rates");

  return ;
  
}
