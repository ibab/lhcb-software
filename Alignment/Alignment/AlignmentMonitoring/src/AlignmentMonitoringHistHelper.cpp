#include "AlignmentMonitoring/AlignmentMonitoringHistHelper.h"
// ROOT
#include "TString.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TPostScript.h"
#include "TFile.h"
#include "TStyle.h"
#include "TPaveStats.h"
#include "TROOT.h"
#include "TPostScript.h"
#include "TSystem.h"
// STL
#include <iostream>
#include <cmath>

using namespace Alignment::AlignmentMonitoring;

TCanvas*
AlignmentMonitoringHistHelper::getCanvas(const TString& name,
                                         const TString& title,
                                         int w, int h)
{
  TCanvas* rc= dynamic_cast<TCanvas*>(gROOT->GetListOfCanvases()->FindObject(name)) ;
  if(rc==0) rc = new TCanvas(name,title,w,h) ;
  rc->SetTitle(title) ;
  rc->Clear() ;
  return rc ;
}

void
AlignmentMonitoringHistHelper::createPage(TCanvas* canvas,
                                          const char* title,
                                          int nx,
                                          int ny)
{
  double titlepady = 0.95 ;
  canvas->Clear() ;
  canvas->cd() ;
  char padname[1024] ;
  sprintf(padname,"%s_titlepad",canvas->GetName()) ;
  TPad* titlepad = new TPad(padname,padname,0,titlepady,1,1) ;
  titlepad->Draw() ;
  titlepad->cd() ;
  TText * titletext = new TText(0.5,0.5,title) ;
  titletext->SetNDC() ;
  titletext->SetTextSize(0.8) ;
  titletext->SetTextAlign(22) ;
  titletext->Draw() ;
  canvas->cd() ;

  //double ymargin(0),xmargin(0) ;
  double dy = 1/Double_t(ny);
  double dx = 1/Double_t(nx);
  int n(0) ;
  for (int iy=0;iy<ny;++iy) {
    double y2 = titlepady * (1 - iy*dy) ;
    double y1 = titlepady * (1 - iy*dy - dy) ;
    for (int  ix=0;ix<nx; ++ix) {
      double x1 = ix*dx;
      double x2 = x1+dx;
      ++n ;
      sprintf(padname,"%s_%d",canvas->GetName(),n);
      TPad* pad = new TPad(padname,padname,x1,y1,x2,y2);
      pad->SetNumber( n );
      pad->SetGridx() ;
      pad->Draw();
    }
  }
  //for(int i=0; i<nx * ny; ++i) {
  //canvas->cd(i+1) ;
  //  gPad->DrawFrame(0,0,1,1) ;
  //}
}

TCanvas*
AlignmentMonitoringHistHelper::createPage(const char* name,
                                          const char* title,
                                          int nx,
                                          int ny,
                                          int w, int h)
{
  TCanvas* canvas = getCanvas(name,title,w,h) ;
  createPage(canvas,title,nx,ny) ;
  return canvas ;
}

TFile*
AlignmentMonitoringHistHelper::getFile(const TString& filename)
{
  TFile* file(0) ;
  file = dynamic_cast<TFile*>(gROOT->GetListOfFiles()->FindObject(filename)) ;
  if(!file) {
    file = TFile::Open(filename) ;
  }
  return file ;
}

TObject *
AlignmentMonitoringHistHelper::getObject(const TString& filename,
                                         const TString& directory,
                                         const TString& hisname)
{
  TFile* file = getFile(filename) ;
  TObject* h1(0) ;
  if(file && file->IsOpen()) {
    if(file->cd(directory)) {
      h1 = gDirectory->Get(hisname) ;
    } 
  } else {
    std::cout << "Cannot find file: " << filename << std::endl ;
  }
  if(h1==0)
    std::cout << "Cannot find "
	      << "object \'" << hisname << "\' "
	      << "in dir \'" << directory << "\' "
	      << "in file " << filename << "\' " << std::endl ;
  return h1 ;
}

TObject *
AlignmentMonitoringHistHelper::getObject(const TString& filename,
                                         const TString& name)
{
  
  int ipos = name.Last('/') ;
  TString dirname(name.Data(),ipos) ;
  TString hisname(name.Data()+ipos+1) ;
  return getObject(filename,dirname,hisname) ;
}

TH1*
AlignmentMonitoringHistHelper::getH1(const TString& filename,
                                     const TString& hisname)
{
  TH1* h1 = dynamic_cast<TH1*>(getObject(filename,hisname)) ;
  if(h1) {
    h1->UseCurrentStyle() ;
  }
  return h1 ;
}

TH2*
AlignmentMonitoringHistHelper::getH2(const TString& filename,
                                     const TString& hisname)
{
  TH2* h2 = dynamic_cast<TH2*>(getObject(filename,hisname)) ;
  if(h2) {
    h2->UseCurrentStyle() ;
  }
  return h2 ;
}

bool
AlignmentMonitoringHistHelper::is1D( TH1* h1 ) {
    return h1 && h1->GetNbinsY()==1 && dynamic_cast<TProfile*>(h1)==0 ;
}

bool
AlignmentMonitoringHistHelper::isProfile1D( TH1* h1 ) {
  return h1 && h1->GetNbinsY()==1 && dynamic_cast<TProfile*>(h1)!=0 ;
}

double
AlignmentMonitoringHistHelper::max1D( const TH1& h1 ) {
  int firstbin = h1.GetXaxis()->GetFirst() ;
  int lastbin  = h1.GetXaxis()->GetLast() ;
  double rc = -1e9 ;
  const TProfile* pr = dynamic_cast<const TProfile*>(&h1) ;
  for(int ibin=firstbin; ibin<=lastbin; ++ibin)
    if(!pr || pr->GetBinEntries(ibin)>0) 
      rc = std::max(rc,h1.GetBinContent(ibin)) ;
  return rc ;
}

double
AlignmentMonitoringHistHelper::min1D( const TH1& h1 ) {
  int firstbin = h1.GetXaxis()->GetFirst() ;
  int lastbin  = h1.GetXaxis()->GetLast() ;
  double rc = 1e9 ;
  const TProfile* pr = dynamic_cast<const TProfile*>(&h1) ;
  for(int ibin=firstbin; ibin<=lastbin; ++ibin)
     if(!pr || pr->GetBinEntries(ibin)>0) 
       rc = std::min(rc,h1.GetBinContent(ibin)) ;
  return rc ;
}

TH1*
AlignmentMonitoringHistHelper::drawH1(TString filename,
                                      TString hisname,
                                      const char* drawopt,
                                      int color,
                                      bool normalize)
{
  TH1* h1 = getH1(filename,hisname) ;
  if(h1) {
    h1->SetLineColor(color) ;
    h1->SetMarkerColor(color) ;
    if(normalize && is1D(h1) ) {
      double integral = h1->Integral() ;
      if(integral >0 && fabs(integral-1)>1e-6) {
	if( h1->GetSumw2()==0 ) h1->Sumw2() ;
	h1->Scale(1/integral) ;
      }
    }
    h1 = h1->DrawCopy(drawopt) ;
    //h1->Draw(drawopt) ;
  }
  return h1 ;
}


TH1*
AlignmentMonitoringHistHelper::drawH2(TString filename,
                                      TString hisname,
                                      const char* drawopt,
                                      int color)
{
  TH1* h1 = getH1(filename,hisname) ;
  TH2* h2 = dynamic_cast<TH2*>(h1) ;
  TH1* rc = 0 ;
  if(h2) {
    TH1* tmp = h2 ;
    TString stringopt(drawopt) ;
    TH1* owned(0) ;
    char tmpname[1024] ;
    sprintf(tmpname,"%s-%s_tmp",filename.Data(),hisname.Data()) ;
    if( stringopt.Contains("ProfX") ) {
      owned = tmp = h2->ProfileX(tmpname) ;
      stringopt.ReplaceAll("ProfX","") ;
    } else if ( stringopt.Contains("ProjX") ) {
      owned = tmp = h2->ProjectionX(tmpname) ;
      stringopt.ReplaceAll("ProjX","") ;
    } else if( stringopt.Contains("ProjY") ) {
      owned = tmp = h2->ProjectionY(tmpname) ;
      stringopt.ReplaceAll("ProjY","") ;
    }
    tmp->SetTitle(h2->GetTitle()) ;
    tmp->UseCurrentStyle() ;
    tmp->SetLineColor(color) ;
    tmp->SetMarkerColor(color) ;
    rc = tmp->DrawCopy(stringopt) ;
    delete owned ;
  }
  return rc ;
}

int
AlignmentMonitoringHistHelper::colorFromIndex(int index)
{
  int colors[] = {1,2,9,3,5,6,7} ;
  return colors[index] ;
}


TH1*
AlignmentMonitoringHistHelper::drawH1(const std::vector<TString>& files,
                                      const TString& hisname,
                                      TString drawopt,
                                      bool normalize)
{
  TH1* rc = drawH1(files[0],hisname,drawopt,colorFromIndex(0),false) ;
  if(rc ) {
    double norm = rc->Integral() ;

    if(  drawopt=="" ) {
      drawopt = is1D(rc) ? "hist" : "err" ;
      rc->SetDrawOption(drawopt) ;
      if( isProfile1D(rc) ) rc->SetStats(0) ;  
    }
    
    double minval = min1D(*rc) ;
    double maxval = max1D(*rc) ;
    for( unsigned int i=1; i<files.size(); ++i) {
      TH1* h1 = drawH1(files[i],hisname,drawopt + " same",colorFromIndex(i),false) ;
      if(h1) {
	if( isProfile1D(rc) ) rc->SetStats(0) ;
	else {
	  TPaveStats *st = (TPaveStats*)h1->FindObject("stats") ;
	  if(st) st->SetTextColor(colorFromIndex(i)) ;
	}
	if( is1D(h1) && normalize ) {
	  if( h1->GetSumw2()==0 ) h1->Sumw2() ;
	  h1->Scale(norm/h1->Integral()) ;
	}
	maxval = std::max(maxval,max1D(*h1)) ;
	minval = std::min(minval,min1D(*h1)) ;
      }
    }

    if ( is1D(rc) ) {
      if( ! gPad->GetLogy() ) {
	if(maxval >  max1D(*rc) ) rc->SetMaximum(1.1*maxval) ;
	rc->SetMinimum(0) ;
      } else {
	rc->SetMaximum(2*maxval) ;
	rc->SetMinimum(minval>0 ? 0.5*minval : 0.5) ;
      }
    } else if( isProfile1D(rc) ) {
      double miny,maxy ;
      if( minval<0 && maxval>0) {
	// symmetrize
	maxy = 1.1 * std::max(-minval,maxval) ;
	miny = -maxy ;
      } else {
	maxy = maxval + 0.1*(maxval-minval) ;
	miny = minval - 0.1*(maxval-minval) ;
	if(minval>=0 && miny<0) miny=0 ;
      }
      //if( maxy > rc->GetMaximum() ) rc->SetMaximum(maxy) ;
      //if( miny < rc->GetMinimum() ) rc->SetMinimum(miny) ;
      rc->SetMaximum( maxy ) ;
      rc->SetMinimum( miny ) ;
    }
  } else {
    gPad->Clear() ;
  }
  
  return rc ;
}

TH1*
AlignmentMonitoringHistHelper::drawH2(std::vector<TString> files,
                                      TString hisname,
                                      TString drawopt)
{
  TH1* rc = drawH2(files[0],hisname,drawopt) ;
  for( unsigned int i=1; i<files.size(); ++i)
    drawH2(files[i],hisname,drawopt + " same",colorFromIndex(i)) ;
  return rc ;
}
