#include "AlignmentMonitoring/MonitoringPage.h"
#include "AlignmentMonitoring/HistHelper.h"
// STL
#include <iostream>
#include "TLine.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TPaveStats.h"
#include "TStyle.h"
#include "TROOT.h"

using namespace Alignment::AlignmentMonitoring;

MonitoringPage::MonitoringPage(const char* atitle,
			       const char* h1,
			       const char* h2,
			       const char* h3,
			       const char* h4,
			       const char* h5,
			       const char* h6,
			       const char* h7,
			       const char* h8,
			       const char* h9)
  : m_title(atitle)
{
  // should use a 'variadic' function here
  if(h1) m_h.push_back(h1) ;
  if(h2) m_h.push_back(h2) ;
  if(h3) m_h.push_back(h3) ;
  if(h4) m_h.push_back(h4) ;
  if(h5) m_h.push_back(h5) ;
  if(h6) m_h.push_back(h6) ;
  if(h7) m_h.push_back(h7) ;
  if(h8) m_h.push_back(h8) ;
  if(h9) m_h.push_back(h9) ;
}

MonitoringPage::MonitoringPage(const char* atitle,
			       std::vector<std::string> hnames)
  : m_title(atitle)
{
  if (hnames.size() < 9)
    for (auto hname : hnames) m_h.push_back(hname);
  else
    std::cout << "This software can handle at most 9 histograms on the same page! Do nothing." << std::endl;
}

void MonitoringPage::setTitle(char* hname, char* htitle){
  if( std::find(m_h.begin(),m_h.end(),hname)==m_h.end() ) std::cout << hname << " doesn't exist!" << std::endl;
  m_titles[std::string(hname)] = htitle;
  //std::cout << hname << " new title: " << htitle << std::endl;
}

void MonitoringPage::setVLine(char* hname, std::vector<double> vlines){
  if( std::find(m_h.begin(),m_h.end(),hname)==m_h.end() ) std::cout << hname << " doesn't exist!" << std::endl;
  m_vlines[std::string(hname)] = vlines;
}

void MonitoringPage::setHLine(char* hname, std::vector<double> hlines){
  if( std::find(m_h.begin(),m_h.end(),hname)==m_h.end() ) std::cout << hname << " doesn't exist!" << std::endl;
  m_hlines[std::string(hname)] = hlines;
}

void MonitoringPage::setReferenceLine(char* hname, char* axis, double value){
  if( std::find(m_h.begin(),m_h.end(),hname)==m_h.end() ) std::cout << hname << " doesn't exist!" << std::endl;
  m_reflines[std::string(hname)] = std::make_pair(std::string(axis), value);
}

void MonitoringPage::setMLine(char* hname, char* axis){
  if( std::find(m_h.begin(),m_h.end(),hname)==m_h.end() ) std::cout << hname << " doesn't exist!" << std::endl;
  m_mlines[std::string(hname)] = std::string(axis);
}

void MonitoringPage::setLines(char* hname, std::vector<TLine*> lines){
  if( std::find(m_h.begin(),m_h.end(),hname)==m_h.end() ) std::cout << hname << " doesn't exist!" << std::endl;
  m_lines[std::string(hname)] = lines;
}

void MonitoringPage::setXRange(char* hname, double xmin, double xmax){
  if( std::find(m_h.begin(),m_h.end(),hname)==m_h.end() ) std::cout << hname << " doesn't exist!" << std::endl;
  m_xranges[std::string(hname)] = std::make_pair(xmin,xmax);
}

void MonitoringPage::setYRange(char* hname, double ymin, double ymax){
  if( std::find(m_h.begin(),m_h.end(),hname)==m_h.end() ) std::cout << hname << " doesn't exist!" << std::endl;
  m_yranges[std::string(hname)] = std::make_pair(ymin,ymax);
}

void MonitoringPage::draw(const std::vector<TString>& filenames, TCanvas* canvas, bool normalize )
{
  gStyle->SetOptStat(1100);
  gROOT->ForceStyle();
  HistHelper hh;
  size_t ny = 2 ;
  size_t nx = std::max(int(m_h.size()/ny),2) ;
  if( nx*ny < m_h.size()) ++nx ;
  TCanvas* c1 = canvas ? canvas : hh.getCanvas(m_title,m_title) ;
  hh.createPage(c1,m_title,nx,ny) ;

  auto drawTLine = [] (double x1, double y1, double x2, double y2, int color, int style){
    TLine* line = new TLine(x1,y1,x2,y2);
    line->SetLineStyle(style);
    line->SetLineColor(color);
    line->Draw();
    return line;
  };

  for( size_t i=0; i< m_h.size() ; ++i) {
    c1->cd(i+1) ;
    gPad->SetGrid(0,0);
    auto ret = hh.drawH1(filenames,m_h[i],"",normalize) ;

    //check the histogram name not changed
    for( auto h:ret.second ){
      if( strcmp(h->GetName(),m_h[i])==0) continue;
      h->SetName(m_h[i]);
      h->SetTitle(m_h[i]);
    }

    std::sort(ret.second.begin(), ret.second.end());
    ret.second.erase(std::remove(ret.second.begin(), ret.second.end(), nullptr), ret.second.end());
    if (ret.second.empty()) continue;

    double minY = std::numeric_limits<double>::max();
    double maxY = -std::numeric_limits<double>::max();
    for (auto h: ret.second) {
        minY = std::min(minY, hh.min1D(*h)), maxY = std::max(maxY, hh.max1D(*h));
    }

    if (0. == minY * maxY) {
        if (0. > minY) {
            minY *= 1.1;
            //ret.second.front()->SetMinimum(minY);
            for(auto h:ret.second) h->SetMinimum(minY);
        } else {
            maxY *= 1.1;
            //ret.second.front()->SetMaximum(maxY);
            for(auto h:ret.second) h->SetMaximum(maxY);
        }
    } else {
        double m = 0.5 * (maxY + minY), w = 0.5 * (maxY - minY);
        w *= 1.1;
        minY = m - w, maxY = m + w;
        //ret.second.front()->SetMinimum(minY);
        //ret.second.front()->SetMaximum(maxY);
        for(auto h:ret.second) {h->SetMinimum(minY); h->SetMaximum(maxY);}
    }
    int iPaveStat = 0;
    for (auto h: ret.second){
			auto itt = m_titles.find(std::string(h->GetName()));
			if( m_titles.end()!=itt ){
				//std::cout<< "Set title " << itt->second << " for " << h->GetName() << std::endl;
				h->SetTitle(itt->second.c_str());
			}
			// change X or Y ranges
      auto itxr = m_xranges.find(std::string(h->GetName()));
      if( m_xranges.end()!=itxr ){
				h->GetXaxis()->SetRangeUser(itxr->second.first, itxr->second.second);
    	}
      auto ityr = m_yranges.find(std::string(h->GetName()));
      if( m_yranges.end()!=ityr ){
        if( ityr->second.first<hh.min1D(*h)) h->SetMinimum(ityr->second.first);
        if( ityr->second.second>hh.max1D(*h)) h->SetMaximum(ityr->second.second);
      }
      auto it = m_vlines.find(std::string(h->GetName()));
      if( m_vlines.end() != it){
        std::vector<double> xvals = it->second;
        for( auto xval:xvals)
          drawTLine(xval, minY, xval, maxY, kBlack, 3);
      }//draw vlines
      auto it2 = m_hlines.find(std::string(h->GetName()));
      if( m_hlines.end() != it2){
        double minX = h->GetXaxis()->GetXmin();
        double maxX = h->GetXaxis()->GetXmax();
        std::vector<double> yvals = it2->second;
        for( auto yval:yvals)
          drawTLine(minX, yval, maxX, yval, kBlack, 3);
      }//draw hlines
      auto itref = m_reflines.find(std::string(h->GetName()));
      if( m_reflines.end() != itref){
        if( itref->second.first.compare("H")==0 ){
					double minX = ( m_xranges.end()!=itxr ) ? itxr->second.first  : h->GetXaxis()->GetXmin() ;
					double maxX = ( m_xranges.end()!=itxr ) ? itxr->second.second : h->GetXaxis()->GetXmax() ;
          drawTLine(minX, itref->second.second, maxX, itref->second.second, kGreen+3, 5);
        }
        if( itref->second.first.compare("V")==0 ){
          drawTLine(itref->second.second, minY, itref->second.second, maxY, kGreen+3, 5);
        }
      }//draw referenceline
      auto it3 = m_mlines.find(std::string(h->GetName()));
      if( m_mlines.end()!=it3 ){
        if( it3->second.compare("X")==0 ){
          double xmid = h->GetMean();
          drawTLine(xmid,minY,xmid,maxY,h->GetLineColor(),h->GetLineStyle());
        }
        if( it3->second.compare("Y")==0 ){
          double tot = 0;
          for(int bin = 1; bin<h->GetNbinsX()+1; bin++) {
						if(m_xranges.end()==itxr) {
							if(h->GetBinContent(bin)) tot+=1;
						} else {
							if(	(h->GetBinLowEdge(bin)+h->GetBinWidth(bin))>itxr->second.first &&
									h->GetBinLowEdge(bin)<itxr->second.second &&
									h->GetBinContent(bin)) tot+=1;
						}
					}
          double yval = h->Integral()/tot;
          double minX = ( m_xranges.end()==itxr ) ? h->GetXaxis()->GetXmin() : itxr->second.first;
          double maxX = ( m_xranges.end()==itxr ) ? h->GetXaxis()->GetXmax() : itxr->second.second;
					//std::cout << minX << " " << yval << " " << maxX << " " <<yval << std::endl;
          drawTLine(minX,yval,maxX,yval,h->GetLineColor(),h->GetLineStyle());
        }
      }//draw lines for mean
      auto itany = m_lines.find(std::string(h->GetName()));
      if( m_lines.end()!=itany ){
        for(auto line:itany->second ){
          line->SetLineColor(1);
          line->SetLineStyle(3);
          line->Draw();
        }
      }//any lines
      //TPaveStats
      if( hh.isProfile1D(h) ) h->SetStats(0);
      else{
        ret.first->Update();
        TPaveStats *st = (TPaveStats*) h->FindObject("stats");
        if(st) {
          st->SetTextColor(h->GetLineColor());
          st->SetX1NDC(0.1);
          st->SetX2NDC(0.35);
          st->SetY1NDC(0.7+iPaveStat*0.1);
          st->SetY2NDC(0.8+iPaveStat*0.1);
          iPaveStat += 1;
        }
      }

    }

  }
  return;
}
