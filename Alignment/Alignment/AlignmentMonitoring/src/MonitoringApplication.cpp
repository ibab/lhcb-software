#include "AlignmentMonitoring/MonitoringApplication.h"
// STL
#include <iostream>
// ROOT
#include "TCanvas.h"
#include "TPad.h"
#include "TPostScript.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TText.h"

using namespace Alignment::AlignmentMonitoring;

MonitoringApplication::MonitoringApplication(const char *outputName)
{
  if (std::string(outputName).size()!=0) m_outputName = outputName;
}

void
MonitoringApplication::addFile( const TString& filenametag)
{ 
  int ipos = filenametag.First(":") ;
  TString filename = filenametag ;
  TString tag ;
  if(ipos > 0 && ipos < filenametag.Sizeof()) {
    tag = TString(filenametag.Data(),ipos) ;
    filename = TString(filenametag.Data()+ipos+1) ;
  }
  m_filenames.push_back(filename) ;
  m_tags.push_back(tag) ;
  return;
}

void
MonitoringApplication::drawPage(TString pagename, bool normalize)
{
  bool found = false ;
  for( size_t ipage=0; ipage<m_pages.size() && !found; ++ipage) 
    if( m_pages[ipage]->GetName() == pagename ) {
      found = true ;
      m_pages[ipage]->draw(m_filenames,0,normalize) ;
      gPad->GetCanvas()->cd(1) ;
      TText text ;
      text.SetNDC() ;
      for(size_t i=0; i<m_tags.size(); ++i) {
	if(m_tags[i] != TString()) {
	  text.SetTextColor( m_hhelp.colorFromIndex(i) ) ;
	  text.DrawText(0.7,0.5-0.1*i,m_tags[i]) ;
	}
      }
      gPad->GetCanvas()->Print("c1.eps") ;
    }
  if(!found) {
    std::cout << "Cannot find page calles \'" << pagename << "\'. Please choose from:" << std::endl ;
    for( size_t ipage=0; ipage<m_pages.size(); ++ipage) 
      std::cout << m_pages[ipage]->GetName() << std::endl ;
  }
}


void MonitoringApplication::draw(TCanvas* c1, bool normalize){
  
  for( size_t ipage=0; ipage<m_pages.size(); ++ipage) {
    m_pages[ipage]->draw(m_filenames,c1,normalize) ;
    gPad->GetCanvas()->cd(1) ;
    TText text ;
    text.SetNDC() ;
    for(size_t i=0; i<m_tags.size(); ++i) {
      if(m_tags[i] != TString()) {
	    text.SetTextColor( m_hhelp.colorFromIndex(i) ) ;
	    text.DrawText(0.8,0.5-0.1*i,m_tags[i]) ;
      }
    }
    if(c1) {c1->Print(m_outputName.c_str()); c1->Update();}
  }
}
  
void 
MonitoringApplication::draw(bool print, bool normalize)
{
  TPostScript* ps(0) ;
  TCanvas* c1(0) ;
  if(print) {
    ps = new TPostScript( Form("%s.ps", m_outputName.c_str()),112) ;
    ps->Range(26,18);  
    c1 = new TCanvas("LHCbMonitoring","") ;
  }
  
  for( size_t ipage=0; ipage<m_pages.size(); ++ipage) {
    m_pages[ipage]->draw(m_filenames,c1,normalize) ;
    gPad->GetCanvas()->cd(1) ;
    TText text ;
    text.SetNDC() ;
    for(size_t i=0; i<m_tags.size(); ++i) {
      if(m_tags[i] != TString()) {
	text.SetTextColor( m_hhelp.colorFromIndex(i) ) ;
	text.DrawText(0.8,0.5-0.1*i,m_tags[i]) ;
      }
    }
    if(c1) c1->Update() ;
  }
  ps->Close() ;
  gSystem->Exec( Form("ps2pdf %s.ps %s.pdf", m_outputName.c_str(), m_outputName.c_str()) ) ;
}
