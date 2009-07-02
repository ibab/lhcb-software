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
// @todo number of collisions without Hlt1
//

void eps(TString);


class category{
public:
  category(){};
  category(TString n){name = n;};
  category(TString n, TString sel){name = n; selections.push_back(sel) ;};
  category(TString n, vector<TString> sels){name = n; selections = sels ;};
  ~category(){};
  void appendSelection(TString s){ selections.push_back(s);};
  TString cut(){
    TString c = selections[0]+"==1";
    for ( unsigned int i = 1 ; i!=selections.size() ; i++ ) c = c+" || "+selections[i]+"==1";
    return c;
  };
  TString getName(){return name;};
  
private:
  TString name;
  vector<TString> selections ;
};
//
//-----------------------------------------------------------------------------
//
TString findCategory(TString sel){
  if ( sel.Contains("Hlt1") ) return "Hlt1" ;
  if ( sel.Contains("Hlt2Global") ) return "Hlt2" ;
  if ( sel.Contains("B2") || sel.Contains("Bd2") || 
       sel.Contains("Bu2") || sel.Contains("Bs2") || sel.Contains("Z2") ) return "Exclusive";
  if ( sel.Contains("IncPhi") ) return "Phi" ;
  if ( sel.Contains("Topo") ){
    if ( sel.Contains("Charm")) return "Charm" ;
    else return "Topo" ;
  }
  if ( sel.Contains("Mu") ) return "Leptons" ;
  if ( sel.Contains("Unbiased") ) return "Leptons" ;
  if ( sel.Contains("Gamma") ) return "Exclusive";
  if ( sel.Contains("DrellYan") ) return "Exclusive";
  if ( sel.Contains("HidValley") ) return "Exclusive";
  if ( sel.Contains("Dstar") ) return "Charm";
  if ( sel.Contains("DX") ) return "Charm";
  return "Unknown";
};
//
//-----------------------------------------------------------------------------
//
TString appendToCategory(TString sel, vector<category>& cats ){
  TString catname = findCategory(sel);
  cout <<  sel << " belongs to " << catname << endl ;
  for ( vector<category>::iterator c = cats.begin() ; c!=cats.end() ; c++){
    if ( c->getName() == catname ){
      c->appendSelection(sel) ;
      return catname ;
    } 
  }  
  // make new category
  cats.push_back(category(catname,sel));
  cout << " Made new category " << catname << endl ;
  return catname ;
};
//
//-----------------------------------------------------------------------------
//
void wiki_header(bool showHlt1){
  if ( showHlt1){
    cout << "|  *Category*  |  *Selection*  |  *No Hlt1*  |  *Rate*  |  *Weighted*  |  *# Coll.*  |  *% b*  |" << endl ;
  } else {
    cout << "|  *Category*  |  *Selection*  |  *Rate*  |  *Weighted*  |  *# Coll.*  |  *% b*  |" << endl ;
  }
};
//
//-----------------------------------------------------------------------------
//
void wiki_footer(bool showHlt1){
  cout << endl ;
  if (showHlt1) cout << "   $ *No Hlt1* : Hlt2 rate without running Hlt1 (but L0)" << endl ;
  cout << "   $ *Rate* : Hlt2 rate after Hlt1. " << endl ;
  cout << "   $ *Weighted* : Hlt2 rate after Hlt1 and re-weighting all events by the number of";
  cout << "selections that have selected it (i.e. how much does this selection contribute to the rate?).";
  cout << "The sum should be equal to the Hlt2 rate. *This is probably the number you want*." << endl ;
  cout << "   $ *# coll* : Average number of pp collision in selected events (the lower the better)" << endl ;
  cout << "   $ *% b* : Fraction of b quark in selected events (the higher the better)" << endl ;
};
//
//-----------------------------------------------------------------------------
//
void wiki_printName(TString cat, TString ss){
  cout << "| " ;
  if ( cat == "" ) cout << "  |  ";
  if (ss=="" || ss.Contains("Global")) cout << "%BLUE% ";
  cout << "<nox>" << cat ;
  if (ss=="" || ss.Contains("Global")) cout << " %ENDCOLOR% ";
  cout << " | <nox>" << ss  ; 
  cout << "  |  " ;
  return ;
}
//
//-----------------------------------------------------------------------------
//
void wiki_printRate(double rate, double err){
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
  cout << "  |  " ;
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
// Ugly...
//
void GetRates(TString ss, TString cat, TString FullCut, TChain* MB12, TH1D* Hcolls, TH1D* Hbfracs,  TH1D* HnHltSels,
              int L0events, int Hlt1events, double normalisation, bool showHlt1){

  bool isHlt2Selection = ((ss.Contains("Hlt2")) && (ss.Contains("Decision"))) ;
  bool isHlt1Selection = ((ss.Contains("Hlt1")) && (ss.Contains("Decision"))) ;
  ss.ReplaceAll("Decision","");

  double rateNo = MB12->Draw("L0Decision",FullCut) ;
  double rr = 1.0*rateNo/L0events ;
  double errNo = sqrt(rr*(1-rr)/L0events)*L0events*normalisation;
      
  // cout << sels[s] << " " << rateNo << " " << errNo << " " << L0events << " " << normalisation << endl ;
      
  if (ss=="Hlt1Global") errNo = 0 ;
  rateNo *= normalisation ;
  double rateWithHlt1 = MB12->Draw("Hlt2nSelections >> HnHltSels",FullCut+" && Hlt1Global==1 && L0Decision==1");
  rr = 1.0*rateWithHlt1/Hlt1events ;
  double errWithHlt1  = sqrt(rr*(1-rr)/Hlt1events)*normalisation*Hlt1events  ;
  rateWithHlt1 *= normalisation ;
  // -1 because of lumi
  if ( isHlt1Selection ) MB12->Draw("Hlt1nSelections-1 >> HnHltSels",FullCut+" && Hlt1IgnoringLumiDecision==1");
  double errWeighted = 0 ; 
  double Weighted = normalisation*SumWeights(HnHltSels,errWeighted) ;
  if ( isHlt2Selection ) errWeighted *= normalisation ;
      
  if (ss!="Hlt1Global" && ss!="L0Decision") 
    MB12->Draw("Collisions >> Hcolls", FullCut+" && Hlt1Global==1 && L0Decision==1") ;
  else MB12->Draw("Collisions >> Hcolls", FullCut) ;      
  if (ss!="Hlt1Global") MB12->Draw("HeaviestQuarkInEvent==5 >> Hbfracs", FullCut+" && Hlt1Global==1 && L0Decision==1") ;
  else MB12->Draw("HeaviestQuarkInEvent==5 >> Hbfracs", FullCut) ;
      
  wiki_printName(cat,ss);
  if ( showHlt1 ){
    if (ss.Contains("Hlt2")) wiki_printRate(rateNo,errNo);
    else cout << "  |  " ;
  }
  wiki_printRate(rateWithHlt1,errWithHlt1);
  if (!ss.Contains("Global") && ss!="") wiki_printRate(Weighted,errWeighted);
  else cout << "  |  " ;
  if ( rateWithHlt1 > 0.0001 ) cout << (int(Hcolls->GetMean()*10.))/10. ;
  cout    << "  |  " ;
  if ( rateWithHlt1 > 0.0001 ) cout << (int(100*Hbfracs->GetMean())) << "%" ;
  cout << "  |  " << endl ;
}
//
//-----------------------------------------------------------------------------
//
void SelsPlots(TChain* MB12){
  
  MB12->Draw("L0Decision") ; // to get canvas
  vector<category> categories ;
  double L0eff = 1.0*MB12->GetEntries("L0Decision==1")/MB12->GetEntries("L0Decision>=0") ;
  if ( L0eff < 0.95) cout << "Warning :: L0 is " << 100*L0eff << "%!\n" << endl ;
  int L0events = MB12->GetEntries("L0Decision==1");
  int Hlt1events = MB12->GetEntries("Hlt1Global==1");
  double Hlt1eff = 1.0*Hlt1events/L0events ;
  double normalisation = 1000000./L0events  ;
  bool showHlt1 = true ;
  vector<TString> sels ;
  if ( Hlt1eff < 0.5 ) sels.push_back( "L0Decision" );
  sels.push_back( "Hlt1Global" );
  sels.push_back( "Hlt2Global" );
  appendToCategory("Hlt1Global",categories) ;
  appendToCategory("Hlt2Global",categories) ;
  TObjArray* a = MB12->GetListOfLeaves();
  // fill categories
  for ( int i = 0 ; i<a->GetEntries() ; i++){
    TObject* o = a->At(i);
    TString ss = TString(o->GetName()) ;
    if (( ss.BeginsWith("Hlt")) &&  (ss.EndsWith("Decision"))) {
      sels.push_back(ss);
      TString cat = appendToCategory(ss,categories);
    }
  }

  cout << endl << endl << endl ;
  if ( Hlt1eff > 0.5 ){
    cout << "Hlt1 eff is " << 100*Hlt1eff << "%. Assuming it is already applied at " << 40000.*Hlt1eff << " kHz\n" << endl ;
    normalisation = 40000.*Hlt1eff/MB12->GetEntries("Hlt1Global==1")  ;
    showHlt1 = false ;
  } else {
    cout << "Hlt1 eff is " << 100*Hlt1eff << "%. Assuming it is not already applied." << endl ;
  }
  cout << "Using " << MB12->GetEntries("L0Decision==1") << " events. Normalisation factor is " 
       << normalisation << endl << endl ;
  wiki_header(showHlt1);
  
  // utilities histos
  TH1D* Hcolls = new TH1D("Hcolls","colls",10,-0.5,9.5);
  TH1D* Hbfracs = new TH1D("Hbfracs","bfracs",2,-0.5,1.5);
  TH1D* HnHltSels = new TH1D("HnHltSels","nSelections",sels.size(),0.5,sels.size()+0.5);
  
  // the big loop
  for ( vector<category>::iterator cc = categories.begin() ; cc != categories.end() ; ++cc){
    TString cat = cc->getName() ;
    for ( unsigned int s = 0 ; s!=sels.size() ; s++){
      TString ss = sels[s] ;
      if (cat != findCategory(ss)) continue ;
      TString FullCut = ss+"==1" ;
      GetRates(ss, cat, FullCut, MB12, Hcolls, Hbfracs,  HnHltSels, L0events, Hlt1events, normalisation, showHlt1);
    }
    if ( cat!="Hlt1" && cat!= "Hlt2") 
      GetRates("",cat,cc->cut(),MB12, Hcolls, Hbfracs,  HnHltSels, L0events, Hlt1events, normalisation, showHlt1);
  }
  
  wiki_footer(showHlt1) ;
  cout << endl ;
  return ;
  
}
