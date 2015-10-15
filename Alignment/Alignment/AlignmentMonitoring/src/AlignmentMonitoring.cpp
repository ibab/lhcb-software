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
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "boost/regex.hpp"
// USR
//#include "AlignmentMonitoring/Utilities.h"

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
  return;
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
  ovf->SetParameter(1,0);
  ovf->SetParName(2,"Sigma");
  ovf->SetParameter(2,0.15);
  ovf->SetParLimits(2,0,1);
  ovf->SetParName(3,"Background");
  ovf->SetParameter(3,0);
  for (auto h:ovHists) {
    if (m_verbose) std::cout << h.first << ":\n";
    if( m_mapWarnings.find(h.first)==m_mapWarnings.end() ) m_mapWarnings[h.first] = WarningLevel::UNCHECKED;
    m_insertOrder.push_back(h.first);
    if( !h.second->GetEntries() ) { std::cout << "Skipping " << h.first << ": empty histogram!\n"; continue; }
    ovf->SetParameter(0,h.second->GetEntries());
    if(h.first.find("OT")!=std::string::npos) ovf->SetParameter(2,0.25);
    fitres = h.second->Fit(ovf,m_verbose ? "S" : "SQ"); // need to modify this
    if(fitres->IsEmpty()) { std::cout << "Skipping " << h.first << ": empty fit results\n"; continue; }
    if(fitres->Chi2()<1e-6 || fitres->Ndf()==0)  { std::cout << "Skipping " << h.first << ": too small chi2\n"; continue; }
    if(!fitres->Status() && fitres->CovMatrixStatus() != 3) {// dont check parameters when the fit failed
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
      m_mapWarnings[h.first] = WarningLevel::SEVERE;
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
  m_mapWarnings["D0MassDifference"] = WarningLevel::SEVERE;
  m_insertOrder.push_back("D0MassDifference");
  double deltaM = mass_diff["massPositiveY"]->Parameter(1) - mass_diff["massNegativeY"]->Parameter(1);
  double sigmaDM= std::sqrt(mass_diff["massPositiveY"]->Parameter(2)*mass_diff["massPositiveY"]->Parameter(2)+mass_diff["massNegativeY"]->Parameter(2)*mass_diff["massNegativeY"]->Parameter(2));
  std::vector<double> refs = GetReference("D0MassDifference");
  WarningLevel wl_mean = CheckFitPar(deltaM, sigmaDM,refs[0]-refs[1],refs[0]+refs[1]);
  m_mapWarnings["D0MassDifference"] = wl_mean;
  if(m_verbose) m_pages["AlignD02KPiMonitor"]->Print( (m_outputDirectory+"/"+"AlignD02KPiMonitor.pdf").c_str() );
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
  
