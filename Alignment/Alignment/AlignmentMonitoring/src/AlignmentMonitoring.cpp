#include "AlignmentMonitoring/AlignmentMonitoring.h"
// STL
#include <algorithm>
#include <fstream>
#include <iostream>
// ROOT
#include "TCanvas.h"
#include "TF1.h"
#include "TFitResultPtr.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TMinuit.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TText.h"
#include "TBox.h"
#include "TArrow.h"
#include "TColor.h"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "boost/regex.hpp"
// USR
//#include "AlignmentMonitoring/Utilities.h"
#include "AlignmentMonitoring/NameTTSector.h"
#include "AlignmentMonitoring/NameITSector.h"

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace Alignment::AlignmentMonitoring;

// Constructor
AlignmentMonitoring::AlignmentMonitoring(const char* filename, const char* outputdir)
 : m_inputFileName(filename)
 , m_outputDirectory(outputdir)
 , m_verbose(false)
{
/* Constructor
 * To-do-list:
 *    + tunning the range for all the values
 * */
  LoadGausFitReferences();
}

// Methods
void
AlignmentMonitoring::Run()
{
  if( !exists(m_outputDirectory) and m_verbose ) create_directories( m_outputDirectory );
  if(m_verbose) {
    gStyle->SetOptStat(1111110);
    gStyle->SetOptFit(1111110);
    gROOT->SetStyle("Plain");
  }
  CheckResiduals();
  CheckITOverlaps();
  CheckITOverlapResiduals();
  CheckTTOverlapResiduals();
  CheckTTResidualsInOverlapRegion();
  CheckVeloTTandTMatch();
  CheckVeloTTandTMatchCurvature();
  CheckVeloTMatchKickPosition();
  CheckD0();
  CheckTTSectorResiduals();
  CheckITSectorResiduals();
  PrintWarnings();
}

std::vector<double>
AlignmentMonitoring::GetReference(const char* hname)
{
  std::vector<double> refs;
  const std::string s(hname);
  // first look for exact name
  for (auto rng : m_mapGausFitReference) {
    if (rng.first == hname) {
      refs = rng.second;
        if (m_verbose) {
          std::cout<< "reference: "<< hname << " \tfound: " << rng.first << " (" ;
          for (auto ival : rng.second) std::cout << ival << ",";
          std::cout<< ")\n";
        }
    }
  }
  if (refs.size() < 1) // if not found, look for regular expression
    for (auto rng : m_mapGausFitReference) {
      boost::regex rgx(rng.first);
      if (boost::regex_search(s.begin(), s.end(), rgx)) {
        refs = rng.second;
        if (m_verbose) {
          std::cout<< "reference: "<< hname << " \tfound: " << rng.first << " (" ;
          for (auto ival : rng.second) std::cout << ival << ",";
          std::cout<< ")\n";
        }
        break;
      }
    }
  // still not found
  if (refs.size() < 1) {
    std::cout << "References for histogram " << hname << " not found. Returning empty vector.";
    refs = {};
  }
  return refs;
}

void AlignmentMonitoring::CheckResiduals(){

  std::map<std::string,TH1*> resHists;
  std::vector<std::string> hNames= {
    "OTResidual","ITResidual","TTResidual","VeloPhiResidual","VeloRResidual",
    "OTresidualPull","ITresidualPull","TTresidualPull","VeloPhiresidualPull","VeloRresidualPull"};
  for (auto name : hNames){
    TH1* hist = m_hhelp.getH1(m_inputFileName.c_str(), "Track/TrackMonitor/Long/"+name);
    if(hist==0) hist = new TH1F();
    resHists[name] = hist;
  }
  // Plot histograms
  if (m_verbose) {
    m_pages["Residuals"] = (TCanvas*)m_hhelp.createPage("Residuals", "Residuals", 5, 2, 300*5, 400*2);
    int i = 1;
    for (auto h:resHists) {
      m_pages["Residuals"]->cd(i++);
      h.second->Draw();
    }
  }
 
  // Fit residuals
  TFitResultPtr fitres = 0;
  for (auto h:resHists) {
    if (m_verbose) std::cout << h.second->GetName() << ":\n";
    m_mapWarnings[h.first] = WarningLevel::SEVERE;
    m_insertOrder.push_back(h.first);
    if(!h.second->GetEntries()) continue;
    fitres = h.second->Fit("gaus",m_verbose ? "S" : "SQ");
    if(fitres->Chi2()<1e-6 or fitres->Ndf()==0) continue;
    std::vector<double> refs = GetReference(h.first.c_str());
    WarningLevel wl_mean  = CheckFitPar(fitres->Parameter(1),fitres->ParError(1), refs[0], refs[1]);
    WarningLevel wl_width = CheckFitPar(fitres->Parameter(2),fitres->ParError(2), refs[2], refs[3]);
    m_mapWarnings[h.first]= wl_mean > wl_width ? wl_mean : wl_width;
  }
  if(m_verbose) m_pages["Residuals"]->Print( (m_outputDirectory+"/"+"Residuals.pdf").c_str() );

  // clean up
  for(auto h:resHists) delete h.second;

  return;
}

void AlignmentMonitoring::CheckITOverlapResiduals(){
  /* IT overlap residuals */
  std::vector<std::string> ITboxes = {"ASide","CSide","Top","Bottom"};
  std::map<std::string,TH1*> ovITResHists;
  ovITResHists.clear();
  for(auto box:ITboxes){
    std::string name(box+"/"+"Overlap residual");
    std::string nameHist = box+"/"+"OverlapResidual";
    TH1* hist = m_hhelp.getH1(m_inputFileName.c_str(), "Track/ITTrackMonitor/"+name);
    if(hist==0) hist = new TH1F();
    ovITResHists[nameHist] = hist;
    ovITResHists[nameHist]->SetName(nameHist.c_str());
    ovITResHists[nameHist]->SetTitle(nameHist.c_str());
  }
  if(m_verbose){
    m_pages["ITOverlapResiduals"] = (TCanvas*)m_hhelp.createPage("ITOverlapResiduals","ITOverlapResiduals",2,2,300*2,400*2);
    int i = 1;
    for(auto h:ovITResHists){
      m_pages["ITOverlapResiduals"]->cd(i++);
      h.second->Draw();
    }
  }
  TFitResultPtr fitres = 0;
  for(auto h:ovITResHists){
    if(m_verbose) std::cout << h.second->GetName() << std::endl;
    m_mapWarnings[h.first] = WarningLevel::SEVERE;
    m_insertOrder.push_back(h.first);
    if( !h.second->GetEntries() ) continue;
    fitres = h.second->Fit("gaus", m_verbose ? "S":"SQ");
    if(fitres->Chi2()<1e-6 or fitres->Ndf()==0) continue;
    std::vector<double> refs = GetReference(h.first.c_str());
    WarningLevel wl_mean = CheckFitPar(fitres->Parameter(1),fitres->ParError(1), refs[0], refs[1]);
    WarningLevel wl_width= CheckFitPar(fitres->Parameter(2),fitres->ParError(2), refs[2], refs[3]);
    m_mapWarnings[h.first] = wl_mean > wl_width ? wl_mean : wl_width;
  }
  if(m_verbose) m_pages["ITOverlapResiduals"]->Print( (m_outputDirectory+"/"+"ITOverlapResiduals.pdf").c_str() );
  // clean up
  for(auto h:ovITResHists) delete h.second;
  return;
}

void AlignmentMonitoring::CheckVeloTMatchKickPosition(){
  /* Velo-T match : position of kick */
  std::vector<std::string> kicks = {"kickZ","kickZVsX"};
  std::map<std::string,TH1*> kickHists;
  kickHists.clear();
  for(auto kick:kicks){
    TH1* hist = m_hhelp.getH1(m_inputFileName.c_str(), "Track/TrackFitMatchMonitor/"+kick);
    if(hist==0) hist = new TH1F();
    kickHists[kick] = hist;
  }
  if(m_verbose){
    m_pages["Velo-TMatchKickPosition"] = (TCanvas*)m_hhelp.createPage("Velo-TMatchKickPosition","Velo-TMatchKickPosition",2,1,300*2,400);
    int i = 1;
    for(auto h:kickHists){
      m_pages["Velo-TMatchKickPosition"]->cd(i++);
      h.second->Draw();
    }
  }

  TFitResultPtr fitres = 0;
  for(auto h:kickHists){
    if(m_verbose) std::cout << h.second->GetName() << std::endl;
    m_mapWarnings[h.first] = WarningLevel::SEVERE;
    m_insertOrder.push_back(h.first);
    if(!h.second->GetEntries()) continue;
    if( h.first.find("ZVsX") != string::npos ){
      fitres = h.second->Fit("pol0",m_verbose ? "S" : "SQ");
      if(fitres->Chi2()<1e-6 or fitres->Ndf()==0) continue;
      double chi2dof = fitres->Chi2()/fitres->Ndf();
      std::vector<double> refs = GetReference(h.first.c_str());
      m_mapWarnings[h.first] = CheckFitPar(chi2dof, refs[0], refs[1]);
    }else{
      TF1 *ovf = new TF1("ovf","[0]*(exp(-0.5*((x-[1])/[2])**2)+[4]*exp(-0.5*((x-[1])/[3])**2))+[5]",-20,20);
      ovf->SetParName(0,"Constant");
      ovf->SetParName(1,"Mean");
      ovf->SetParameter(1,5150);
      ovf->SetParName(2,"Sigma1");
      ovf->SetParameter(2,30);
      ovf->SetParName(3,"Sigma2");
      ovf->SetParLimits(3,50,100);
      ovf->SetParName(4,"fraction");
      ovf->SetParameter(4,0.5);
      ovf->SetParName(5,"B");
      fitres = h.second->Fit(ovf,m_verbose ? "S" : "SQ"); // need to modify this
      if(fitres->Chi2()<1e-6 or fitres->Ndf()==0) continue;
      std::vector<double> refs = GetReference(h.first.c_str());
      WarningLevel wl_mean = CheckFitPar(fitres->Parameter(1),fitres->ParError(1), refs[0]-refs[1], refs[0]+refs[1]);
      double sigma1 = fitres->Parameter(2);
      double sigma2 = fitres->Parameter(3);
      double   frac = fitres->Parameter(4);
      double ssigma1 = fitres->ParError(2);
      double ssigma2 = fitres->ParError(3);
      double sigma = sigma1*frac+sigma2*(1-frac);
      double ssigma= sqrt(ssigma1*ssigma1*frac*frac+ssigma2*ssigma2*(1-frac)*(1-frac));
      WarningLevel wl_width= CheckFitPar(sigma,ssigma,refs[2],refs[3]);
      m_mapWarnings[h.first] = wl_mean > wl_width ? wl_mean : wl_width;
    }
  }

  if(m_verbose) m_pages["Velo-TMatchKickPosition"]->Print( (m_outputDirectory+"/"+"Velo-TMatchKickPosition.pdf").c_str() );
  //clean up
  for(auto h:kickHists) delete h.second;
  return;
}

void AlignmentMonitoring::CheckVeloTTandTMatchCurvature(){
  /* Velo(TT)-T match: curvature
 * To-do-list:
 *    + to calculate the weighted sigma for doble gaussian model
 * */
  std::map<std::string,TH1*> matchHists;
  std::vector<std::string> curvatureRatios = {"TToLong","VeloTTToLong","TToLongVsQoP","VeloTTToLongVsQoP"};
  for(auto curvR:curvatureRatios){
    std::string name("curvatureRatio"+curvR);
    TH1* hist = m_hhelp.getH1(m_inputFileName.c_str(), "Track/TrackFitMatchMonitor/"+name);
    if(hist==0) hist = new TH1F();
    matchHists[name] = hist;
  }
  if(m_verbose){
    m_pages["VeloTT-TMatchCurvature"] = (TCanvas*)m_hhelp.createPage("VeloTT-TMatchCurvature","VeloTT-TMatchCurvature",2,2,300*2,400*2);
    int i = 1;
    for(auto h:matchHists){
      m_pages["VeloTT-TMatchCurvature"]->cd(i++);
      h.second->Draw();
    }
  }
  TFitResultPtr fitres = 0;
  TF1 *model = new TF1("model","[0]*(exp(-0.5*((x-[1])/[2])**2)+[4]*exp(-0.5*((x-[1])/[3])**2))+[5]",0,2);
  model->SetParName(0,"Constant");
  model->SetParName(1,"Mean");
  model->SetParameter(1,1);
  model->SetParName(2,"Sigma1");
  model->SetParLimits(2,0.1,0.25);
  model->SetParName(3,"Sigma2");
  model->SetParLimits(3,0.25,0.5);
  model->SetParName(4,"fraction");
  model->SetParameter(4,0.5);
  model->SetParName(5,"B");
  for(auto h:matchHists){
    if(m_verbose) std::cout << h.second->GetName() << std::endl;
    m_mapWarnings[h.first] = WarningLevel::SEVERE;
    m_insertOrder.push_back(h.first);
    if( !h.second->GetEntries() ) continue;
    if(h.first.find("VsQoP") != string::npos){
      fitres = h.second->Fit("pol0",m_verbose ? "S" : "SQ");
      if(fitres->Chi2()<1e-6 or fitres->Ndf()==0) continue;
      double chi2dof = fitres->Chi2()/fitres->Ndf();
      std::vector<double> refs = GetReference(h.first.c_str());
      m_mapWarnings[h.first] = CheckFitPar(chi2dof,refs[0],refs[1]);
    }else{
      fitres = h.second->Fit(model,m_verbose ? "S" : "SQ"); // need to modify this
      if(fitres->Chi2()<1e-6 or fitres->Ndf()==0) continue;
      std::vector<double> refs = GetReference(h.first.c_str());
      WarningLevel wl_mean = CheckFitPar(fitres->Parameter(1),fitres->ParError(1), refs[0]-refs[1], refs[0]+refs[1]);
      WarningLevel wl_width= CheckFitPar(fitres->Parameter(2),fitres->ParError(2), refs[2]-refs[3], refs[2]+refs[3]);
      m_mapWarnings[h.first] = wl_mean > wl_width ? wl_mean : wl_width;
    }
  }
  if(m_verbose) m_pages["VeloTT-TMatchCurvature"]->Print( (m_outputDirectory+"/"+"VeloTT-TMatchCurvature.pdf").c_str() );
  //clean up
  for(auto h:matchHists) delete h.second;
  return;
}

void AlignmentMonitoring::CheckVeloTTandTMatch(){
  /* Velo(TT)-T match
 * To-do-list:
 *    + to calculate the weighted sigma for doble gaussian model
 * */
  std::map<std::string,TH1*> matchHists;
  std::vector<std::string> matches = {"T-TT", "Velo-T", "Velo-TT"};
  for(auto match:matches){
    std::vector<std::string> trackPars = {"dx for dtx==0","dy for dty==0", "dx pull vs tx"};
    for(auto par:trackPars){
      std::string name = match+std::string("/")+par;
      std::string nameHist = name;
      nameHist.erase(std::remove(nameHist.begin(), nameHist.end(), ' '),nameHist.end());
      TH1* hist = m_hhelp.getH1(m_inputFileName.c_str(), "Track/TrackFitMatchMonitor/"+name);
      if(hist==0) hist = new TH1F();
      matchHists[nameHist] = hist;
      matchHists[nameHist]->SetName(nameHist.c_str());
      matchHists[nameHist]->SetTitle(nameHist.c_str());
    }
  }
  if(m_verbose){
    m_pages["VeloTT-TMatch"] = (TCanvas*)m_hhelp.createPage("VeloTT-TMatch","VeloTT-TMatch",3,3,300*3,400*3);
    int i = 1;
    for(auto h:matchHists){
      m_pages["VeloTT-TMatch"]->cd(i++);
      h.second->Draw();
    }
  }
  TFitResultPtr fitres = 0;
  TF1 *model = new TF1("model","[0]*(exp(-0.5*((x-[1])/[2])**2)+[4]*exp(-0.5*((x-[1])/[3])**2))+[5]",-20,20);
  model->SetParName(0,"Constant");
  model->SetParName(1,"Mean");
  model->SetParameter(1,0);
  model->SetParName(2,"Sigma1");
  model->SetParLimits(2,0.5,1.5);
  model->SetParName(3,"Sigma2");
  model->SetParLimits(3,1.5,5);
  model->SetParName(4,"fraction");
  model->SetParameter(4,0.5);
  model->SetParName(5,"B");
  for(auto h:matchHists){
    if(m_verbose) std::cout << h.second->GetName() << std::endl;
    m_mapWarnings[h.first] = WarningLevel::SEVERE;
    m_insertOrder.push_back(h.first);
    if(!h.second->GetEntries()) continue;
    /*if( h.first.find("pull")!=string::npos && h.first.find("T-TT")!=string::npos ){
      TF1 *model = new TF1("model","[0]*sin([1]*(x-[2]))+[3]",-20,20);
      model->SetParName(0,"Norm");
      model->SetParName(1,"Scale");
      model->SetParameter(1,1);
      model->SetParName(2,"Shift");
      model->SetParameter(2,0);
      model->SetParName(3,"B");
      fitres = h.second->Fit(model, m_verbose?"S":"SQ");
    }else if( h.first.find("pull")!=string::npos && h.first.find("Velo-T")!=string::npos ){*/
    if( h.first.find("pull")!=string::npos ){
      fitres = h.second->Fit("pol0", m_verbose?"S":"SQ");
      if(fitres->Chi2()<1e-6 or fitres->Ndf()==0) continue;
      double chi2dof = fitres->Chi2()/fitres->Ndf();
      std::vector<double> refs = GetReference(h.first.c_str());
      m_mapWarnings[h.first] = CheckFitPar(chi2dof,refs[0],refs[1]);
    }else{
      fitres = h.second->Fit(model,m_verbose ? "S" : "SQ"); // need to modify this
      if(fitres->Chi2()<1e-6 or fitres->Ndf()==0) continue;
      std::vector<double> refs = GetReference(h.first.c_str());
      WarningLevel wl_mean = CheckFitPar(fitres->Parameter(1),fitres->ParError(1),refs[0],refs[1]);
      WarningLevel wl_width= CheckFitPar(fitres->Parameter(2),fitres->ParError(2),refs[2],refs[3]);
      m_mapWarnings[h.first] = wl_mean > wl_width ? wl_mean : wl_width;
    }
  }
  if(m_verbose) m_pages["VeloTT-TMatch"]->Print( (m_outputDirectory+"/"+"VeloTT-TMatch.pdf").c_str() );
  //clean up
  for(auto h:matchHists) delete h.second;
  return;
}

void AlignmentMonitoring::CheckTTResidualsInOverlapRegion(){
  std::map<std::string,TH1*> resTTOVHists;
  std::vector<std::string> TTboxes = {"TTaX","TTaU","TTbV","TTbX"};
  for(auto box:TTboxes){
    std::vector<std::string> chirality = {"(left)","(right)"};
    for(auto ch:chirality){
      std::string name = box+std::string("/Residuals in overlaps "+ch);
      std::string nameHist = box+std::string("/ResidualInOverlaps"+ch);
      TH1* hist = m_hhelp.getH1(m_inputFileName.c_str(), "Track/TTTrackMonitor/"+name);
      if(hist==0) hist = new TH1F();
      resTTOVHists[nameHist] = hist;
      resTTOVHists[nameHist]->SetName(nameHist.c_str());
      resTTOVHists[nameHist]->SetTitle(nameHist.c_str());
    }
  }
  if(m_verbose){
    m_pages["TTResidualsInOverlapRegion"] = (TCanvas*)m_hhelp.createPage("TTResidualsInOverlapRegion","TTResidualsInOverlapRegion",4,2,1200,800);
    int i = 1;
    for(auto h:resTTOVHists){
      m_pages["TTResidualsInOverlapRegion"]->cd(i++);
      h.second->Draw();
    }
  }
  TFitResultPtr fitres = 0;
  for(auto h:resTTOVHists){
    if(m_verbose) std::cout << h.second->GetName() << std::endl;
    m_mapWarnings[h.first] = WarningLevel::SEVERE;
    m_insertOrder.push_back(h.first);
    if(!h.second->GetEntries()) continue;
    fitres = h.second->Fit("gaus", m_verbose ? "S":"SQ");
    if(fitres->Chi2()<1e-6 or fitres->Ndf()==0) continue;
    std::vector<double> refs = GetReference(h.first.c_str());
    WarningLevel wl_mean = CheckFitPar(fitres->Parameter(1),fitres->ParError(1),refs[0],refs[1]);
    WarningLevel wl_width= CheckFitPar(fitres->Parameter(2),fitres->ParError(2),refs[2],refs[3]);
    m_mapWarnings[h.first] = wl_mean > wl_width ? wl_mean : wl_width;
  }
  if(m_verbose) m_pages["TTResidualsInOverlapRegion"]->Print( (m_outputDirectory+"/"+"TTResidualsInOverlapRegion.pdf").c_str() );
  // clean up
  for(auto h:resTTOVHists) delete h.second;
  return;

}

void AlignmentMonitoring::CheckTTOverlapResiduals(){
  /* TT overlap residuals */
  std::vector<std::string> TTboxes = {"TTaX","TTaU","TTbV","TTbX"};
  std::map<std::string,TH1*> ovTTResHists;
  ovTTResHists.clear();
  for(auto box:TTboxes){
    std::string name(box+"/"+"Overlap residual");
    std::string nameHist = box+"/"+"OverlapResidual";
    TH1* hist = m_hhelp.getH1(m_inputFileName.c_str(), "Track/TTTrackMonitor/"+name);
    if(hist==0) hist = new TH1F();
    ovTTResHists[nameHist] = hist;
    ovTTResHists[nameHist]->SetName(nameHist.c_str());
    ovTTResHists[nameHist]->SetTitle(nameHist.c_str());
  }
  if(m_verbose){
    m_pages["TTOverlapResiduals"] = (TCanvas*)m_hhelp.createPage("TTOverlapResiduals","TTOverlapResiduals",2,2,300*2,400*2);
    int i = 1;
    for(auto h:ovTTResHists){
      m_pages["TTOverlapResiduals"]->cd(i++);
      h.second->Draw();
    }
  }
  TFitResultPtr fitres = 0;
  for(auto h:ovTTResHists){
    if(m_verbose) std::cout << h.second->GetName() << std::endl;
    m_mapWarnings[h.first] = WarningLevel::SEVERE;
    m_insertOrder.push_back(h.first);
    if( !h.second->GetEntries() ) continue;
    fitres = h.second->Fit("gaus", m_verbose ? "S":"SQ");
    if(fitres->Chi2()<1e-6 or fitres->Ndf()==0) continue;
    std::vector<double> refs = GetReference(h.first.c_str());
    WarningLevel wl_mean = CheckFitPar(fitres->Parameter(1),fitres->ParError(1),refs[0],refs[1]);
    WarningLevel wl_width= CheckFitPar(fitres->Parameter(2),fitres->ParError(2),refs[2],refs[3]);
    m_mapWarnings[h.first] = wl_mean > wl_width ? wl_mean : wl_width;
  }
  if(m_verbose) m_pages["TTOverlapResiduals"]->Print((m_outputDirectory+"/"+"TTOverlapResiduals.pdf").c_str());
  for(auto h:ovTTResHists) delete h.second;
  return;
}

void AlignmentMonitoring::CheckTTSectorResiduals(){

  std::vector<std::string> stations = {"a", "b"};
  std::vector<std::string> regions={"A","B","C"};
  std::vector<std::string> layers= {"X", "U", "V"};
  std::vector<std::string> hNames={};
  for(auto station:stations){
    for(auto layer:layers){
      for(auto region:regions){
        if( (station.compare("a")==0 && layer.compare("V")==0) || (station.compare("b")==0 && layer.compare("U")==0) ) continue;
        if( region.compare("B")==0 && station.compare("a")==0 ){
          for(int sector=1; sector<=18; sector++) hNames.push_back( "TT"+station+layer+"Region"+region+"Sector"+std::to_string(sector) );
        } else if( region.compare("B")==0 && station.compare("b")==0 ){
          for(int sector=1; sector<=26; sector++) hNames.push_back( "TT"+station+layer+"Region"+region+"Sector"+std::to_string(sector) );
        } else {
          for(int sector=1; sector<=24; sector++) hNames.push_back( "TT"+station+layer+"Region"+region+"Sector"+std::to_string(sector) );
        }
      }//regions 
    }//layers 
  }//stations
  TH2* hist_allsectors = m_hhelp.getH2(m_inputFileName.c_str(), "Track/TTTrackMonitor/BySector/AllSectorsUnbiasedResidualHisto");
  std::map<std::string,TH1*> ovHists;
  for( auto name:hNames ){
    // input of the histograms here
    NameTTSector* sector = new NameTTSector(name);
    int chanX = sector->GetChannel();
    TH1* hist;
    if( hist_allsectors==0 )
      hist = m_hhelp.getH1(m_inputFileName.c_str(), "Track/TTTrackMonitor/BySector/UnbiasedResidual_"+name);
    else
      hist = hist_allsectors->ProjectionY( name.c_str(), chanX, chanX );
    if(hist==0) {hist = new TH1F();  m_mapWarnings[name] = 0; }
    ovHists[name] = hist;
    ovHists[name]->SetTitle( name.c_str() );
  }
  if (m_verbose) {
    for(auto station:stations){
      for(auto layer:layers){
        for(auto region:regions){
          m_pages["TTResidualsBySectorTT"+station+layer+"Region"+region] = (TCanvas*)m_hhelp.createPage( ("TTResidualsBySectorTT"+station+layer+"Region"+region).c_str(),
                                    ("Residulas TT"+station+layer+"Region"+region+" Per Sector").c_str(), 7, 4, 300*7, 400*4);
          int i = 1;
          for(auto h:ovHists){
            if( h.first.find( ("TT"+station+layer+"Region"+region).c_str() )==std::string::npos) continue;
            m_pages["TTResidualsBySectorTT"+station+layer+"Region"+region]->cd(i++);
            h.second->Draw();
          }//ovHists
        }//region
      }//layer
    }//stats
  }
  if (m_verbose) 
    for(auto station:stations)
      for(auto layer:layers)
        for(auto region:regions)
          m_pages["TTResidualsBySectorTT"+station+layer+"Region"+region]->Print( (m_outputDirectory+"/"+"TTResidualsBySectorTT"+station+layer+"Region"+region+".pdf").c_str() );


  std::map< std::string, std::pair<double,double> > m_mean_values;
  std::map< std::string, std::pair<double,double> > m_sigma_values;
  for (auto h:ovHists) {
    if( !h.second->GetEntries() ) { std::cout << "Skipping " << h.first << ": empty histogram!\n"; continue; }
    m_mean_values[h.first]  = std::make_pair(h.second->GetMean(),h.second->GetMeanError());
    m_sigma_values[h.first] = std::make_pair(h.second->GetRMS(),h.second->GetRMSError());
  }
  // build 2D histogram to visualize the mean and sigma
  gStyle->SetOptStat(0); 
  gStyle->SetOptFit(0);
  gStyle->SetPadRightMargin(0.25);
  gStyle->SetTitleX(0.5);
  gStyle->SetTitleAlign(23);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetPaintTextFormat("5.0f");
  gStyle->SetStatFormat("5.5f");
  gStyle->SetTitleFontSize(0.07);
  gStyle->SetPadTickY(1);
  gStyle->SetPadTickX(1);
  unsigned int nColors=52;
  Int_t MyPalette[52];
  Double_t s[3] = {0.00, 0.50, 1.00};
  Double_t b[3] = {0.80, 1.00, 0.00};
  Double_t g[3] = {0.00, 1.00, 0.00};
  Double_t r[3] = {0.00, 1.00, 0.80};
  Int_t FI = TColor::CreateGradientColorTable(3, s, r, g, b, nColors);
  for (unsigned int k(0); k < nColors; k++){
    MyPalette[k] = FI+k;
  }
  gStyle->SetNumberContours(nColors);
  gStyle->SetPalette(nColors, MyPalette); 
  gROOT->ForceStyle();
 
  std::map< int, std::pair<double,double> > m_mapping;
  std::map< int, int > m_nSensors;
  for (auto name : hNames){
    NameTTSector* sector = new NameTTSector(name);
    int uniqueSector = sector->GetUniqueSector();
    m_mapping[uniqueSector]  = TTMapping(uniqueSector);
    m_nSensors[uniqueSector] = TTNumberOfSensors(uniqueSector);
  }

  int nBinsX = 43;
  int nBinsY = 40;
  double lowX = -21.5;
  double upX  =  21.5;
  double lowY = -20;
  double upY  =  20;
  TH2D* hist_mean_values  = new TH2D("hist_mean_values", "TT Residuals Mean Value", nBinsX, lowX, upX, nBinsY, lowY, upY);
  TH2D* hist_sigma_values = new TH2D("hist_sigma_values", "TT Residuals RMS Value", nBinsX, lowX, upX, nBinsY, lowY, upY);
  // start to fill values
  for(auto sector_mean:m_mean_values){
    NameTTSector* sector = new NameTTSector(sector_mean.first);
    int uniqueSector = sector->GetUniqueSector();
    std::pair<double,double> mean = sector_mean.second;
    for(int i=0; i<m_nSensors[uniqueSector]; i++){
      hist_mean_values->Fill(m_mapping[uniqueSector].first, m_mapping[uniqueSector].second+i, mean.first);
    } 
  }
  for(auto sector_sigma:m_sigma_values){
    NameTTSector* sector = new NameTTSector(sector_sigma.first);
    int uniqueSector = sector->GetUniqueSector();
    std::pair<double,double> mean = sector_sigma.second;
    for(int i=0; i<m_nSensors[uniqueSector]; i++)
      hist_sigma_values->Fill(m_mapping[uniqueSector].first, m_mapping[uniqueSector].second+i, mean.first); 
  }
  // draw the histogram
  m_pages["TTResidualsBySectorMapping"] = (TCanvas*)m_hhelp.createPage( "TTResidualsBySectorMapping","Overlap Residulas TT Map", 1, 2, 1200, 600*2);
  m_pages["TTResidualsBySectorMapping"]->cd(1);
  hist_mean_values->SetMaximum( 0.3);
  hist_mean_values->SetMinimum(-0.3);
  hist_mean_values->Draw("COLZ");
  PlotTTLabels(hist_mean_values,hNames,m_mapping,m_nSensors);
  PlotTTBoxes(hist_mean_values,hNames,m_mapping,m_nSensors);
  m_pages["TTResidualsBySectorMapping"]->cd(2);
  hist_sigma_values->SetMaximum( 0.3);
  hist_sigma_values->SetMinimum( 0);
  hist_sigma_values->Draw("COLZ");
  PlotTTLabels(hist_sigma_values,hNames,m_mapping,m_nSensors);
  PlotTTBoxes(hist_sigma_values,hNames,m_mapping,m_nSensors);
  m_pages["TTResidualsBySectorMapping"]->Print( (m_outputDirectory+"/"+"TTResidualsBySectorMapping.pdf").c_str() );

  gStyle->SetOptStat(1111110); 
  gStyle->SetOptFit(1111110);
  gROOT->ForceStyle(); 

  // clean up
  for (auto h:ovHists) delete h.second;
  return ;
}

void AlignmentMonitoring::PlotTTBoxes(TH2D* hist, std::vector<std::string> hNames, std::map< int, std::pair<double,double> >  m_mapping, std::map<int,int> m_nSensors){
  TBox* box = new TBox();
  box->SetFillColor(kWhite);
  box->SetFillStyle(0);
  box->SetLineStyle(3);
  box->SetLineColor(kBlack); //14
  box->SetLineWidth(box->GetLineWidth()/10.);

  TBox* boxempty = new TBox();
  boxempty->SetFillColor(14);
  boxempty->SetFillStyle(3254);
  boxempty->SetLineStyle(3);
  boxempty->SetLineColor(14);
  boxempty->SetLineWidth(boxempty->GetLineWidth()/100.);

  for (auto name : hNames){
    NameTTSector* sector = new NameTTSector( name );
    int uniqueSector = sector->GetUniqueSector();
    box->DrawBox(m_mapping[uniqueSector].first-0.5,m_mapping[uniqueSector].second-0.5,
                     m_mapping[uniqueSector].first+0.5,m_mapping[uniqueSector].second+0.5+m_nSensors[uniqueSector]-1.);
    if(hist->GetBinContent( hist->GetXaxis()->FindBin(m_mapping[uniqueSector].first), hist->GetYaxis()->FindBin(m_mapping[uniqueSector].second) )==0 ) 
      boxempty->DrawBox(m_mapping[uniqueSector].first-0.5,m_mapping[uniqueSector].second-0.5,
                    m_mapping[uniqueSector].first+0.5,m_mapping[uniqueSector].second+0.5+m_nSensors[uniqueSector]-1.);
  }
}

void AlignmentMonitoring::PlotTTLabels(TH2D* hist, std::vector<std::string> hNames, std::map< int, std::pair<double,double> >  m_mapping, std::map<int,int> m_nSensors){
  hist->GetXaxis()->SetTickLength(0);
  hist->GetYaxis()->SetTickLength(0);
  hist->GetXaxis()->SetLabelColor(kWhite);
  hist->GetYaxis()->SetLabelColor(kWhite);

  TText* tta = new TText(-0.75, -9.8, "TTa");
  tta->Draw();

  TText* ttb = new TText(-0.75, 8., "TTb");
  ttb->Draw();

  TText* ttaX = new TText(-19.8, -9., "X");
  ttaX->Draw();

  TText* ttaU = new TText(19.3, -9., "U");
  ttaU->Draw();

  TText* ttbV = new TText(-19.8, 8., "V");
  ttbV->Draw();

  TText* ttbX = new TText(19.3, 8., "X");
  ttbX->Draw();

  TText* ttA = new TText(-20.8, -0.5, "A");
  ttA->SetTextSize(0.07);
  ttA->Draw();

  TText* ttC = new TText(19.8, -0.5, "C");
  ttC->SetTextSize(0.07);
  ttC->Draw();

  TArrow* XArrow = new TArrow(0.,-18.,-8.5,-18.,0.005,"|-|>");
  XArrow->Draw();

  TText* X = new TText(-9.1, -19., "X");
  X->SetTextSize(0.04);
  X->Draw();

  for(auto name:hNames){
    NameTTSector* sector = new NameTTSector(name);
    int uniqueSector = sector->GetUniqueSector();
    int sectorID     = sector->GetSectorID();
    int nSensors     = m_nSensors[uniqueSector];
    TText* tt = new TText(m_mapping[uniqueSector].first-0.25, m_mapping[uniqueSector].second+0.45*(nSensors-1), std::to_string(sectorID).c_str());
    tt->SetTextSize(0.015);
    tt->Draw();
  }
}


int AlignmentMonitoring::TTNumberOfSensors(int uniqueSector){
  int uniqueSector_= uniqueSector;
  Double_t TTlayer(uniqueSector_/1000);
  uniqueSector_ = uniqueSector_-TTlayer*1000;
  Double_t Region(uniqueSector_/100);
  uniqueSector_ = uniqueSector_-Region*100;
  Int_t Sectorno_1(uniqueSector_/1-1);

  if(TTlayer < 2.5){
    if(Region == 3){
      if(Sectorno_1%4 == 0 || Sectorno_1%4 == 3)
        return 4;
      else
        return 3;
    }else if(Region == 1){
      if(Sectorno_1%4 == 0 || Sectorno_1%4 == 3)
        return 4; 
      else
        return 3;
    }else{
      if(Sectorno_1%6 == 0 || Sectorno_1%6 == 5)
        return 4;
      else if(Sectorno_1%6 == 1 || Sectorno_1%6 == 4)
        return 2;
      else
        return 1;
    }
  }else{
    if(Region == 3){
      if(Sectorno_1%4 == 0 || Sectorno_1%4 == 3)
        return 4;
      else
        return 3;
    }else if(Region == 1){
      if(Sectorno_1%4 == 0 || Sectorno_1%4 == 3)
        return 4; 
      else
        return 3;
    }else{
      if(Sectorno_1 < 4){
        if(Sectorno_1%4 == 0 || Sectorno_1%4 == 3)
          return 4; 
        else
          return 3;
      }else if(Sectorno_1 > 21){
        Sectorno_1 -= 22;
        if(Sectorno_1%4 == 0 || Sectorno_1%4 == 3)
          return 4; 
        else
          return 3;
      }else{
        Sectorno_1 -= 4;
        if(Sectorno_1%6 == 0 || Sectorno_1%6 == 5)
          return 4;
        else if(Sectorno_1%6 == 1 || Sectorno_1%6 == 4)
          return 2;
        else
          return 1;
      }
    }
  }
  
}
std::pair<double,double> AlignmentMonitoring::TTMapping(int uniqueSector){
  int uniqueSector_= uniqueSector;
  Double_t TTlayer(uniqueSector_/1000);
  uniqueSector_ = uniqueSector_-TTlayer*1000;
  Double_t Region(uniqueSector_/100);
  uniqueSector_ = uniqueSector_-Region*100;
  Int_t Sectorno_1(uniqueSector_/1-1);

  Double_t XOffSet, YOffSet;
  if(TTlayer == 1){
    XOffSet = -10.;
    YOffSet = -9.;
  }else if(TTlayer == 2){
    XOffSet = 10.;
    YOffSet = -9.;
  }else if(TTlayer == 3){
    XOffSet = -10.;
    YOffSet = 9.;
  }else{
    XOffSet = 10.;
    YOffSet = 9.;
  }

  const Int_t seq4[4] = {0,4,3,3};
  const Int_t seq6[6] = {0,4,2,1,1,2};
  Double_t compute(0.);
  if(TTlayer < 2.5){
    if(Region == 3){
      double x = XOffSet + 7. - Sectorno_1/4;
      compute = YOffSet - 7.;
      for(Int_t i(0);i<=Sectorno_1%4;i++)
        compute += seq4[i];
      double y = compute + 0.5;
      return std::make_pair(x,y);
    }else if(Region == 1){
      double x = XOffSet - 2. - Sectorno_1/4;
      compute = YOffSet - 7.;
      for(Int_t i(0);i<=Sectorno_1%4;i++)
        compute += seq4[i];
      double y = compute + 0.5;
      return std::make_pair(x,y);
    }else{
      double x = XOffSet + 1. - Sectorno_1/6;
      compute = YOffSet - 7.;
      for(Int_t i(0);i<=Sectorno_1%6;i++)
        compute += seq6[i];
      double y = compute + 0.5;
      return std::make_pair(x,y);
    }
  }else{
    if(Region == 3){
      double x = XOffSet + 8. - Sectorno_1/4;
      compute = YOffSet - 7.;
      for(Int_t i(0);i<=Sectorno_1%4;i++)
        compute += seq4[i];
      double y = compute + 0.5;
      return std::make_pair(x,y);
    }else if(Region == 1){
      double x = XOffSet - 3. - Sectorno_1/4;
      compute = YOffSet - 7.;
      for(Int_t i(0);i<=Sectorno_1%4;i++)
        compute += seq4[i];
      double y = compute + 0.5;
      return std::make_pair(x,y);
    }else{
      if(Sectorno_1 < 4){
        double x = XOffSet + 2. - Sectorno_1/4;
        compute = YOffSet - 7.;
        for(Int_t i(0);i<=Sectorno_1%4;i++)
          compute += seq4[i];
        double y = compute + 0.5;
        return std::make_pair(x,y);
      }else if(Sectorno_1 > 21){
        Sectorno_1 -= 22;
        double x = XOffSet - 2. - Sectorno_1/4;
        compute = YOffSet - 7.;
        for(Int_t i(0);i<=Sectorno_1%4;i++)
          compute += seq4[i];
        double y = compute + 0.5;
        return std::make_pair(x,y);
      }else{
        Sectorno_1 -= 4;
        double x = XOffSet + 1. - Sectorno_1/6;
        compute = YOffSet - 7.;
        for(Int_t i(0);i<=Sectorno_1%6;i++)
          compute += seq6[i];
        double y = compute + 0.5;
        return std::make_pair(x,y);
      }
    }
  }
  
}


void AlignmentMonitoring::CheckITSectorResiduals(){

  std::vector<std::string> stats = {"1","2","3"};
  std::vector<std::string> boxes = {"ASide","CSide","Top","Bottom"};
  std::vector<std::string> layers= {"X1", "U", "V", "X2"};
  std::vector<std::string> sectors={"1","2","3","4","5","6","7"};

  std::vector<std::string> hNames={};
  for(auto sector:sectors)
    for(auto layer:layers)
      for(auto box:boxes)
        for(auto stat:stats)
          hNames.push_back("IT"+stat+box+layer+"Sector"+sector);

  TH2* hist_allsectors = m_hhelp.getH2(m_inputFileName.c_str(), "Track/ITTrackMonitor/BySector/AllSectorsUnbiasedResidualHisto");
  std::map<std::string,TH1*> ovHists;
  for( auto name:hNames ){
    // input of the histograms here
    NameITSector* sector = new NameITSector(name);
    int chanX = sector->GetChannel();
    TH1* hist;
    if( hist_allsectors==0 )
      hist = m_hhelp.getH1(m_inputFileName.c_str(), "Track/ITTrackMonitor/BySector/UnbiasedResidual_"+name);
    else
      hist = hist_allsectors->ProjectionY( name.c_str(), chanX, chanX );
    if(hist==0) {hist = new TH1F();  m_mapWarnings[name] = 0; }
    ovHists[name] = hist;
    ovHists[name]->SetTitle( name.c_str() );
  }

  if (m_verbose) {
    for(auto stat:stats){
      for(auto box:boxes){
        m_pages["ITResidualsBySectorIT"+stat+box] = (TCanvas*)m_hhelp.createPage( ("ITResidualsBySectorIT"+stat+box).c_str(),
                                      ("Residulas IT"+stat+" "+box+" Per Sector").c_str(), 7, 4, 300*7, 400*4);
        int i = 1;
        for(auto h:ovHists){
          if( h.first.find( ("IT"+stat).c_str() )==std::string::npos || h.first.find(box)==std::string::npos) continue;
          m_pages["ITResidualsBySectorIT"+stat+box]->cd(i++);
          h.second->Draw();
        }//ovHists
      }//boxes
    }//stats
  }
  if (m_verbose) 
    for(auto stat:stats)
      for(auto box:boxes)
        m_pages["ITResidualsBySectorIT"+stat+box]->Print( (m_outputDirectory+"/"+"ITResidualsBySectorIT"+stat+box+".pdf").c_str() );

  std::map< std::string, std::pair<double,double> > m_mean_values;
  std::map< std::string, std::pair<double,double> > m_sigma_values;
  for (auto h:ovHists) {
    if( !h.second->GetEntries() ) { std::cout << "Skipping " << h.first << ": empty histogram!\n"; continue; }
    //get the histogram mean and rms
    m_mean_values[h.first]  = std::make_pair(h.second->GetMean(),h.second->GetMeanError());
    m_sigma_values[h.first] = std::make_pair(h.second->GetRMS(),h.second->GetRMSError());
  }
  // build 2D histogram to visualize the mean and sigma
  gStyle->SetOptStat(0); 
  gStyle->SetOptFit(0);
  gStyle->SetPadRightMargin(0.25);
  gStyle->SetTitleX(0.5);
  gStyle->SetTitleAlign(23);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetPaintTextFormat("5.1f");
  gStyle->SetStatFormat("5.5f");
  gStyle->SetTitleFontSize(0.07);
  gStyle->SetPadTickY(1);
  gStyle->SetPadTickX(1);
  unsigned int nColors=52;
  Int_t MyPalette[52];
  Double_t s[3] = {0.00, 0.50, 1.00};
  Double_t b[3] = {0.80, 1.00, 0.00};
  Double_t g[3] = {0.00, 1.00, 0.00};
  Double_t r[3] = {0.00, 1.00, 0.80};
  Int_t FI = TColor::CreateGradientColorTable(3, s, r, g, b, nColors);
  for (unsigned int k(0); k < nColors; k++){
    MyPalette[k] = FI+k;
  }
  gStyle->SetNumberContours(nColors);
  gStyle->SetPalette(nColors, MyPalette); 
  gROOT->ForceStyle();
 
  std::map< int, std::pair<double,double> > m_mapping;
  for (auto name : hNames){
    NameITSector* sector = new NameITSector(name);
    int uniqueSector = sector->GetUniqueSector();
    m_mapping[uniqueSector] = ITMapping(uniqueSector);
  }

  int nBinsX = 25;
  int nBinsY = 52;
  double lowX = -12.5;
  double upX  =  12.5;
  double lowY = -13;
  double upY  =  13;
  TH2D* hist_mean_values = new TH2D("hist_mean_values", "IT Residuals Mean Value", nBinsX, lowX, upX, nBinsY, lowY, upY);
  TH2D* hist_sigma_values = new TH2D("hist_sigma_values", "IT Residuals RMS Value", nBinsX, lowX, upX, nBinsY, lowY, upY);
  // start to fill values
  for(auto sector_mean:m_mean_values){
    NameITSector* sector = new NameITSector(sector_mean.first);
    int uniqueSector = sector->GetUniqueSector();
    std::pair<double,double> mean = sector_mean.second;
    hist_mean_values->Fill(m_mapping[uniqueSector].first, m_mapping[uniqueSector].second, mean.first);
  }
  for(auto sector_sigma:m_sigma_values){
    NameITSector* sector = new NameITSector(sector_sigma.first);
    int uniqueSector = sector->GetUniqueSector();
    std::pair<double,double> mean = sector_sigma.second;
    hist_sigma_values->Fill(m_mapping[uniqueSector].first, m_mapping[uniqueSector].second, mean.first); 
  }
  // draw the histogram
  m_pages["ITResidualsBySectorMapping"] = (TCanvas*)m_hhelp.createPage( "ITResidualsBySectorMapping","Overlap Residulas IT Map", 1, 2, 1200, 600*2);
  m_pages["ITResidualsBySectorMapping"]->cd(1);
  hist_mean_values->SetMaximum( 0.3);
  hist_mean_values->SetMinimum(-0.3);
  hist_mean_values->Draw("COLZ");
  PlotITLabels(hist_mean_values);
  PlotITBoxes(hist_mean_values,hNames,m_mapping);
  m_pages["ITResidualsBySectorMapping"]->cd(2);
  hist_sigma_values->SetMaximum( 0.2);
  hist_sigma_values->SetMinimum( 0);
  hist_sigma_values->Draw("COLZ");
  PlotITLabels(hist_sigma_values);
  PlotITBoxes(hist_sigma_values,hNames,m_mapping);
  m_pages["ITResidualsBySectorMapping"]->Print( (m_outputDirectory+"/"+"ITResidualsBySectorMapping.pdf").c_str() );

  gStyle->SetOptStat(1111110); 
  gStyle->SetOptFit(1111110);
  gROOT->ForceStyle(); 

  // clean up
  for (auto h:ovHists) delete h.second;
  return ;
}

void AlignmentMonitoring::PlotITBoxes(TH2D* hist, std::vector<std::string> hNames, std::map< int, std::pair<double,double> >  m_mapping){
  TBox* box = new TBox();
  box->SetFillColor(kWhite);
  box->SetFillStyle(0);
  box->SetLineStyle(3);
  box->SetLineColor(kBlack); //14
  box->SetLineWidth(box->GetLineWidth()/10.);

  TBox* boxempty = new TBox();
  boxempty->SetFillColor(14);
  boxempty->SetFillStyle(3254);
  boxempty->SetLineStyle(3);
  boxempty->SetLineColor(14);
  boxempty->SetLineWidth(boxempty->GetLineWidth()/100.);

  for (auto name : hNames){
    NameITSector* sector = new NameITSector( name );
    int uniqueSector = sector->GetUniqueSector();
    box->DrawBox(m_mapping[uniqueSector].first-0.5, m_mapping[uniqueSector].second-0.25, m_mapping[uniqueSector].first+0.5, m_mapping[uniqueSector].second+0.25);
    if(hist->GetBinContent( hist->GetXaxis()->FindBin(m_mapping[uniqueSector].first), hist->GetYaxis()->FindBin(m_mapping[uniqueSector].second) )==0 ) 
      boxempty->DrawBox(m_mapping[uniqueSector].first-0.5, m_mapping[uniqueSector].second-0.25, m_mapping[uniqueSector].first+0.5, m_mapping[uniqueSector].second+0.25);
  }
}

void AlignmentMonitoring::PlotITLabels(TH2D* hist){
  hist->GetXaxis()->SetTickLength(0);
  hist->GetYaxis()->SetTickLength(0);
  hist->GetXaxis()->SetLabelColor(kWhite);
  hist->GetYaxis()->SetLabelColor(kWhite);
  TText* it1 = new TText(-0.3, -8.5, "IT1");
  it1->Draw();
  TText* it2 = new TText(-0.3, -0.5, "IT2");
  it2->Draw();
  TText* it3 = new TText(-0.3, 7.5, "IT3");
  it3->Draw();
  TText* itA = new TText(-11.5, -0.5, "A");
  itA->SetTextSize(0.07);
  itA->Draw();
  TText* itC = new TText(11.1, -0.5, "C");
  itC->SetTextSize(0.07);
  itC->Draw();
  TArrow* XArrow = new TArrow(0.,-12.,-5.5,-12.,0.005,"|-|>");
  XArrow->Draw();
  TText* X = new TText(-5.9, -12.5, "X");
  X->SetTextSize(0.04);
  X->Draw();

  auto drawLayerSymbol = [] (double x, double y) { 
    TText* itX1 = new TText(x, y, "X1");
    itX1->SetTextSize(0.02);
    itX1->Draw();
    TText* itU = new TText(x, y+0.5, "U");
    itU->SetTextSize(0.02);
    itU->Draw();
    TText* itV = new TText(x, y+1.0, "V");
    itV->SetTextSize(0.02);
    itV->Draw();
    TText* itX2 = new TText(x, y+1.5, "X2");
    itX2->SetTextSize(0.02);
    itX2->Draw();
  };
  drawLayerSymbol(10.7,-9);
  drawLayerSymbol(10.7,-1);
  drawLayerSymbol(10.7, 7);

  auto drawSectorID = [] (double x, double y){
    TText* itno1 = new TText(x, y, "1");
    itno1->SetTextSize(0.02);
    itno1->Draw();
    TText* itno2 = new TText(x-1, y, "2");
    itno2->SetTextSize(0.02);
    itno2->Draw();
    TText* itno3 = new TText(x-2, y, "3");
    itno3->SetTextSize(0.02);
    itno3->Draw();
    TText* itno4 = new TText(x-3, y, "4");
    itno4->SetTextSize(0.02);
    itno4->Draw();
    TText* itno5 = new TText(x-4, y, "5");
    itno5->SetTextSize(0.02);
    itno5->Draw();
    TText* itno6 = new TText(x-5, y, "6");
    itno6->SetTextSize(0.02);
    itno6->Draw();
    TText* itno7 = new TText(x-6, y, "7");
    itno7->SetTextSize(0.02);
    itno7->Draw();
  };
  drawSectorID(10,  1.2);
  drawSectorID( 3, 11.2);
  drawSectorID( 3,-11.65);
  drawSectorID(-4,  1.2);

}

std::pair<double,double> AlignmentMonitoring::ITMapping(int uniqueSector){
    int uniqueSector_= uniqueSector;
    double Itno(uniqueSector_/1000);
    uniqueSector_ = uniqueSector_-Itno*1000;
    double Boxno(uniqueSector_/100);
    uniqueSector_ = uniqueSector_-Boxno*100;
    double Layerno(uniqueSector_/10);
    uniqueSector_ = uniqueSector_-Layerno*10;
    double Sectorno(uniqueSector_/1);
    if(Boxno == 1){
      double x = -3. - Sectorno;
      double y = (16.*(Itno-1.)+7.+Layerno)/2. -13. + 0.25;
      return std::make_pair(x,y);
    }else if(Boxno == 2){
      double x = 11. - Sectorno;
      double y = (16.*(Itno-1.)+7.+Layerno)/2. -13. + 0.25;
      return std::make_pair(x,y);
    }else if(Boxno == 3){
      double x = 4. - Sectorno;
      double y = (16.*(Itno-1.)+11.+Layerno)/2. -13. + 0.25;
      return std::make_pair(x,y);
    }else{
      double x = 4. - Sectorno;
      double y = (16.*(Itno-1.)+3.+Layerno)/2. -13. + 0.25;
      return std::make_pair(x,y);
    }
}

void AlignmentMonitoring::CheckITOverlaps(){
  /* overlaps
 * To-do-list:
 *    + Fit failed sometimes
 * */
  std::vector<std::string> tracks= {"OTASide","OTCSide","ITASide","ITCSide","ITTop","ITBottom"};
  std::vector<std::string> stats = {"1","2","3"};
  std::vector<std::string> boxes = {"ASide","CSide","Top","Bottom"};

  std::vector<std::string> hNames={};
  for (auto trk : tracks)
    for (auto stat: stats)
      for (auto box : boxes)
        hNames.push_back(trk+"Track/IT"+stat+box+"Box/hitres");
     
  std::map<std::string,TH1*> ovHists;
  for (auto name : hNames){
    TH1* hist = m_hhelp.getH1(m_inputFileName.c_str(), "Track/TrackITOverlapMonitor/"+name);
    if(hist==0) {hist = new TH1F();  m_mapWarnings[name] = 0; }
    ovHists[name] = hist;
  }

  if (m_verbose) {
    for (auto trk: tracks) {
      m_pages["OverlapResidualsIT"+trk] = (TCanvas*)m_hhelp.createPage( ("OverlapResidualsIT"+trk).c_str(),
                                                                     ("Overlap Residulas IT "+trk).c_str(), 4, 3, 300*4, 400*3);
      int i = 1;
      for(auto h:ovHists){
        if( h.first.find(trk)==std::string::npos ) continue;
        m_pages["OverlapResidualsIT"+trk]->cd(i++);
        h.second->Draw();
      }//ovHists
    }
  }

  // Fit residuals
  TFitResultPtr fitres = 0;
  TF1 *ovf = new TF1("ovf","[0]*exp(-0.5*((x-[1])/[2])**2)+[3]",-5,5);
  ovf->SetParName(0,"Constant");
  ovf->SetParName(1,"Mean");
  ovf->SetParLimits(1,-1.0,1.0);
  ovf->SetParName(2,"Sigma");
  ovf->SetParameter(2,0.10);
  ovf->SetParLimits(2,0.05,0.5);
  ovf->SetParName(3,"Background");
  ovf->SetParameter(3,0);

  for (auto h:ovHists) {
    if (m_verbose) std::cout << h.first << ":\n";
    if( m_mapWarnings.find(h.first)==m_mapWarnings.end() ) m_mapWarnings[h.first] = WarningLevel::UNCHECKED;
    m_insertOrder.push_back(h.first);
    if( !h.second->GetEntries() ) { std::cout << "Skipping " << h.first << ": empty histogram!\n"; continue; }
    ovf->SetParLimits(0,0,h.second->GetEntries());
    ovf->SetParameter(1,h.second->GetMean());
    if(h.first.find("OT")!=std::string::npos) ovf->SetParameter(2,0.15);
    fitres = h.second->Fit(ovf,m_verbose ? "S" : "SQ"); // need to modify this
    if(fitres->IsEmpty()) { std::cout << "Skipping " << h.first << ": empty fit results\n"; continue; }
    if(fitres->Chi2()<1e-6 || fitres->Ndf()==0)  { std::cout << "Skipping " << h.first << ": too small chi2\n"; continue; }
    if(!fitres->Status() && fitres->CovMatrixStatus() != 3) {// dont check parameters when the fit failed
      m_mapWarnings[h.first] = WarningLevel::FAILED;
      continue;
    }
    if(ovf->GetProb()>0.9) {
      m_mapWarnings[h.first] = WarningLevel::FAILED; 
      continue;
    }
    m_mapWarnings[h.first] = WarningLevel::SEVERE; 
    std::vector<double> refs = GetReference(h.first.c_str());
    WarningLevel wl_mean = CheckFitPar(fitres->Parameter(1),fitres->ParError(1), refs[0], refs[1]);
    WarningLevel wl_width = CheckFitPar(fitres->Parameter(2),fitres->ParError(2), refs[2], refs[3]);
    m_mapWarnings[h.first] = wl_mean > wl_width ? wl_mean : wl_width;
    // check whether the fit is relevant
    if (m_verbose) {
      double ns = fitres->Parameter(0) * TMath::Sqrt( 2. * TMath::Pi() ) * fitres->Parameter(2);
      double nb = fitres->Parameter(3) * h.second->Integral();
      std::cout << "ns = " << ns << "\tnb = " << nb << "\tf = " << ns/nb << std::endl;
      std::cout << "fit status = " << fitres->Status() << "\t cov = "<< fitres->CovMatrixStatus()  << std::endl;
    }
  }
  if (m_verbose) {
    for (auto trk: tracks)
      m_pages[std::string("OverlapResidualsIT")+trk]->Print( (m_outputDirectory+"/"+"OverlapResidualsIT"+"_"+trk+".pdf").c_str() );
  }

  // begin: a map of warnings for IT
  std::map<std::string,int> track_indices({ {R"(OTASide)",1},{R"(OTCSide)",2},{R"(ITASide)",3},{R"(ITCSide)",4},{R"(ITTop)",5},{R"(ITBottom)",6} });
  std::map<std::string,int> box_indices({ {R"(ASide)",1},{R"(CSide)",2},{R"(Top)",3},{R"(Bottom)",4} });
  std::map<std::string,TH2F*> hmapWarnings;
  for(auto stat:stats)
    hmapWarnings[stat]= new TH2F(("hmapWarnings"+stat).c_str(), ("map of warnings for IT"+stat).c_str(), box_indices.size(), 1, box_indices.size()+1, track_indices.size(), 1, track_indices.size()+1);
  for(auto wrn:m_mapWarnings){
    if( ovHists.find(wrn.first)==ovHists.end() ) continue;
    std::vector<std::string> names;
    names.clear();
    split(names, wrn.first, is_any_of("/"), boost::token_compress_on );
    replace_all(names[0],"Track","");
    replace_all(names[1],"Box","");
    std::string track   = names[0];
    std::string station = names[1].substr(2,1);
    std::string box     = names[1].substr(3);
    hmapWarnings[station]->Fill( box_indices[box], track_indices[track], wrn.second);
    hmapWarnings[station]->GetXaxis()->SetBinLabel( box_indices[box], box.c_str());
    hmapWarnings[station]->GetYaxis()->SetBinLabel( track_indices[track], track.c_str());
  }
  m_pages["OverlapResidualsITMaps"] = (TCanvas*)m_hhelp.createPage( "OverlapResidualsITMaps","Overlap Residulas IT maps", 3, 1, 400*3, 300*1);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gROOT->ForceStyle();
  Int_t colors[] = {6, 0, 3, 5, 2}; // #colors >= #levels - 1
  gStyle->SetPalette((sizeof(colors)/sizeof(Int_t)), colors);
  Double_t levels[] = {-1, 0, 1, 2, 3};
  int i = 1;
  std::map<int,std::string> warnings({{-1,R"(FAILED)"}, {0,R"()"}, {1,R"(OK)"}, {2,R"(WARNING)"}, {3,R"(SEVERE)"} });
  TText *text = new TText();
  text->SetTextSize(0.035);
  text->SetTextAngle(0);
  for(auto stat:stats){
    m_pages["OverlapResidualsITMaps"]->cd(i++);
    hmapWarnings[stat]->SetContour((sizeof(levels)/sizeof(Double_t)), levels);
    hmapWarnings[stat]->Draw("COL");
    for(int i=1; i<=hmapWarnings[stat]->GetNbinsX(); i++){
      for(int j=1; j<=hmapWarnings[stat]->GetNbinsY(); j++){
        double y = hmapWarnings[stat]->GetYaxis()->GetBinCenter(j)-0.1;
        double x = hmapWarnings[stat]->GetXaxis()->GetBinLowEdge(i)+0.2;
        int    warn = int(hmapWarnings[stat]->GetBinContent(i,j));
        if(warn==WarningLevel::OK) x = hmapWarnings[stat]->GetXaxis()->GetBinCenter(i)-0.1;
        text->DrawText(x,y, warnings[warn].c_str() );
      }
    }
  }
  m_pages["OverlapResidualsITMaps"]->Print( (m_outputDirectory+"/"+"OverlapResidualsITMaps.pdf").c_str() );
  gStyle->SetOptStat(1111110);
  gStyle->SetOptFit(1111110);
  gROOT->ForceStyle();
  // end: a map of warnings for IT

  // clean up
  for( auto h:ovHists ) delete h.second;

  return;
}
// Add check on the D0 position and width
// Add check on the distance of y>0, y<0
void AlignmentMonitoring::CheckD0(){
  /* D0sAlign */
  std::vector<std::string> parameters = {"mass","masspull","massPositiveY","massNegativeY"};
  std::map<std::string,TH1*> alignD0Hists;
  alignD0Hists.clear();
  for(auto name:parameters){
    TH1* hist = m_hhelp.getH1(m_inputFileName.c_str(), "Track/AlignD02KPiMonitor/"+name);
    if(hist==0) hist = new TH1F();
    alignD0Hists[name] = hist;
  }
  if(m_verbose){
    m_pages["AlignD02KPiMonitor"] = (TCanvas*)m_hhelp.createPage("AlignD02KPiMonitor","AlignD02KPiMonitor",2,2,300*2,400*2);
    int i = 1;
    for(auto h:alignD0Hists){
      m_pages["AlignD02KPiMonitor"]->cd(i++);
      h.second->Draw();
    }
  }
  TFitResultPtr fitres = 0;
  std::map<std::string,TFitResultPtr> mass_diff;
  for(auto h:alignD0Hists){
    if(m_verbose) std::cout << h.second->GetName() << std::endl;
    if( !(h.first=="massPositiveY" || h.first=="massNegativeY") ){
      m_mapWarnings[h.first] = WarningLevel::UNCHECKED; 
      m_insertOrder.push_back(h.first); 
    }
    if(!h.second->GetEntries()) continue;
    fitres = h.second->Fit("gaus",m_verbose ? "S" : "SQ"); // need to modify this
    if(fitres->Chi2()<1e-6 or fitres->Ndf()==0) continue;
    if( h.first=="massPositiveY" || h.first=="massNegativeY" ){
      mass_diff[h.first] = fitres;
    }else{
      std::vector<double> refs = GetReference(h.first.c_str());
      WarningLevel wl_mean = CheckFitPar(fitres->Parameter(1),fitres->ParError(1),refs[0]-refs[1],refs[0]+refs[1]);
      m_mapWarnings[h.first] = wl_mean;
      if( h.first.find("pull")!=string::npos ){
        WarningLevel wl_width= CheckFitPar(fitres->Parameter(2),fitres->ParError(2),refs[2]-refs[3],refs[2]+refs[3]);
        m_mapWarnings[h.first] = wl_mean > wl_width ? wl_mean : wl_width;
      }
    }//else
  }
  m_mapWarnings["D0MassDifference"] = WarningLevel::UNCHECKED; 
  m_insertOrder.push_back("D0MassDifference");
  if( !(mass_diff.find("massPositiveY")==mass_diff.end() || mass_diff.find("massNegativeY")==mass_diff.end()) ) { 
    double deltaM = mass_diff["massPositiveY"]->Parameter(1) - mass_diff["massNegativeY"]->Parameter(1);
    double sigmaDM= std::sqrt(mass_diff["massPositiveY"]->Parameter(2)*mass_diff["massPositiveY"]->Parameter(2)+mass_diff["massNegativeY"]->Parameter(2)*mass_diff["massNegativeY"]->Parameter(2));
    std::vector<double> refs = GetReference("D0MassDifference"); 
    WarningLevel wl_mean = CheckFitPar(deltaM, sigmaDM,refs[0]-refs[1],refs[0]+refs[1]);
    m_mapWarnings["D0MassDifference"] = wl_mean;
  }
  if(m_verbose) m_pages["AlignD02KPiMonitor"]->Print( (m_outputDirectory+"/"+"AlignD02KPiMonitor.pdf").c_str() );

  for(auto h:alignD0Hists) delete h.second;

  return;
}

void
AlignmentMonitoring::PrintWarnings()
{
  std::string warning_string = "OK";
  for (auto name : m_insertOrder) {
    int value = m_mapWarnings[name];
    if (!m_verbose &&  value== WarningLevel::OK) continue;
    if(value==WarningLevel::OK) warning_string = "\033[1;32m OK \033[0m";
    else if(value==WarningLevel::WARNING) warning_string = "\033[1;33m WARNING \033[0m";
    else if(value==WarningLevel::SEVERE) warning_string = "\033[1;31m SEVERE \033[0m";
    else if(value==WarningLevel::UNCHECKED) warning_string = "\033[1;36m UNCHECKED \033[0m";
    else if(value==WarningLevel::FAILED) warning_string = "\033[1;35m FAILED \033[0m";
    else warning_string = "NOT EXIST";
    std::cout << name << ": " << warning_string << std::endl;

  }
  return;
}

WarningLevel AlignmentMonitoring::CheckFitPar(double x, double xmin, double xmax){
  if(m_verbose) cout << x << "\t[" << xmin << "," << xmax << "]" << endl;
  WarningLevel wl = WarningLevel::SEVERE;
  if(x<xmin) wl = WarningLevel::OK;
  else if(x<xmax) wl = WarningLevel::WARNING;
  return wl;
}

WarningLevel AlignmentMonitoring::CheckFitPar(double x, double xerr, double xmin, double xmax){
  if (m_verbose) std::cout << x << " +/- " << xerr << "\t["<< xmin << "," <<xmax<<"]:";
  WarningLevel wl = WarningLevel::SEVERE; // default=SEVERE
  if (xmin < x+xerr && x-xerr < xmax) wl = WarningLevel::OK; // OK within range
  else if (xmin-(xmax-xmin)/2 < x+xerr && x-xerr < xmax+(xmax-xmin)/2) wl = WarningLevel::WARNING; // WARNING within double range
  if (m_verbose) std::cout << "\t" << wl << std::endl;
  return wl;
}

void
AlignmentMonitoring::LoadGausFitReferences(const char* filename)
{
  //Utilities ut;
  std::string line;
  if ( std::string(filename).size() == 0 && std::string(std::getenv("ALIGNMENTMONITORINGROOT")).size() == 0)
    std::cout << "ERROR(AlignmentMonitoring::LoadGausFitReference): ALIGNMENTMONITORINGROOT environment variable is not set! Cannot find default references.\n";
  std::ifstream ifile( std::string(filename).size() == 0 ? (std::getenv("ALIGNMENTMONITORINGROOT")+std::string("/files/GausFitReferences.txt")).c_str() : filename);
  if (ifile.is_open()) {
    while ( std::getline (ifile,line) ) {
      //std::vector<std::string> refs = ut.splitString(line,"\t");
      std::vector<std::string> refs;
      refs.clear();
      split(refs, line, is_any_of("\t "), boost::token_compress_on );
      if (refs.size() < 2 ||
          refs[0].find("#") != std::string::npos ||
          refs[0].find("Name") != std::string::npos) continue;
      if(refs.size()==5){
        m_mapGausFitReference[refs[0]] = {std::stod(refs[1]),std::stod(refs[2]),std::stod(refs[3]),std::stod(refs[4])};
      }else if(refs.size()==3){
        m_mapGausFitReference[refs[0]] = { stod(refs[1]), stod(refs[2])};
      }
    }
    ifile.close();
  }
  return;
}

/*
// Fitters
TFitResult
AlignmentMonitoring::GaussFit(TH1* h)
{
  T

}*/
  
