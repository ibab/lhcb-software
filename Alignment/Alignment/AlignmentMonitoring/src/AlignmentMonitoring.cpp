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
#include "TStyle.h"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
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
  CheckOverlaps();
  CheckITOverlapResiduals();
  CheckTTOverlapResiduals();
  CheckTTResidualsInOverlapRegion();
  CheckVeloTTandTMatch();
  CheckVeloTTandTMatchCurvature();
  CheckVeloTMatchKickPosition();
  CheckD0();
  PrintWarnings();
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
    WarningLevel wl_mean  = CheckFitPar(fitres->Parameter(1),fitres->ParError(1),
                                       m_mapGausFitReference[h.first][0], m_mapGausFitReference[h.first][1]);
    WarningLevel wl_width = CheckFitPar(fitres->Parameter(2),fitres->ParError(2),
                                        m_mapGausFitReference[h.first][2], m_mapGausFitReference[h.first][3]);
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
    WarningLevel wl_mean = CheckFitPar(fitres->Parameter(1),fitres->ParError(1),
                                       m_mapGausFitReference[h.first][0],
                                       m_mapGausFitReference[h.first][1]);
    WarningLevel wl_width= CheckFitPar(fitres->Parameter(2),fitres->ParError(2),
                                       m_mapGausFitReference[h.first][2],
                                       m_mapGausFitReference[h.first][3]);
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
      m_mapWarnings[h.first] = CheckFitPar(chi2dof,
                                       m_mapGausFitReference[h.first][0],
                                       m_mapGausFitReference[h.first][1]);
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
      WarningLevel wl_mean = CheckFitPar(fitres->Parameter(1),fitres->ParError(1),
                                       m_mapGausFitReference[h.first][0]-m_mapGausFitReference[h.first][1],
                                       m_mapGausFitReference[h.first][0]+m_mapGausFitReference[h.first][1]);
      double sigma1 = fitres->Parameter(2);
      double sigma2 = fitres->Parameter(3);
      double   frac = fitres->Parameter(4);
      double ssigma1 = fitres->ParError(2);
      double ssigma2 = fitres->ParError(3);
      double sigma = sigma1*frac+sigma2*(1-frac);
      double ssigma= sqrt(ssigma1*ssigma1*frac*frac+ssigma2*ssigma2*(1-frac)*(1-frac));
      WarningLevel wl_width= CheckFitPar(sigma,ssigma,
                                       m_mapGausFitReference[h.first][2],
                                       m_mapGausFitReference[h.first][3]);
      m_mapWarnings[h.first] = wl_mean > wl_width ? wl_mean : wl_width;
    }
  }

  if(m_verbose) m_pages["Velo-TMatchKickPosition"]->Print( (m_outputDirectory+"/"+"Velo-TMatchKickPosition.pdf").c_str() );
  return;
}

void AlignmentMonitoring::CheckVeloTTandTMatchCurvature(){
  /* Velo(TT)-T match: curvature */
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
      m_mapWarnings[h.first] = CheckFitPar(chi2dof,
                                       m_mapGausFitReference[h.first][0],
                                       m_mapGausFitReference[h.first][1]);
    }else{
      fitres = h.second->Fit(model,m_verbose ? "S" : "SQ"); // need to modify this
      if(fitres->Chi2()<1e-6 or fitres->Ndf()==0) continue;
      WarningLevel wl_mean = CheckFitPar(fitres->Parameter(1),fitres->ParError(1),
                                         m_mapGausFitReference[h.first][0]-m_mapGausFitReference[h.first][1],
                                         m_mapGausFitReference[h.first][0]+m_mapGausFitReference[h.first][1]);
      WarningLevel wl_width= CheckFitPar(fitres->Parameter(2),fitres->ParError(2),
                                         m_mapGausFitReference[h.first][2]-m_mapGausFitReference[h.first][3],
                                         m_mapGausFitReference[h.first][2]+m_mapGausFitReference[h.first][3]);
      m_mapWarnings[h.first] = wl_mean > wl_width ? wl_mean : wl_width;
    }
  }
  if(m_verbose) m_pages["VeloTT-TMatchCurvature"]->Print( (m_outputDirectory+"/"+"VeloTT-TMatchCurvature.pdf").c_str() );
  return;
}

void AlignmentMonitoring::CheckVeloTTandTMatch(){
  /* Velo(TT)-T match */
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
      m_mapWarnings[h.first] = CheckFitPar(chi2dof,
                                       m_mapGausFitReference[h.first][0],
                                       m_mapGausFitReference[h.first][1]);
    }else{
      fitres = h.second->Fit(model,m_verbose ? "S" : "SQ"); // need to modify this
      if(fitres->Chi2()<1e-6 or fitres->Ndf()==0) continue;
      WarningLevel wl_mean = CheckFitPar(fitres->Parameter(1),fitres->ParError(1),
                                         m_mapGausFitReference[h.first][0],
                                         m_mapGausFitReference[h.first][1]);
      WarningLevel wl_width= CheckFitPar(fitres->Parameter(2),fitres->ParError(2),
                                         m_mapGausFitReference[h.first][2],
                                         m_mapGausFitReference[h.first][3]);
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
    WarningLevel wl_mean = CheckFitPar(fitres->Parameter(1),fitres->ParError(1),
                                       m_mapGausFitReference[h.first][0],
                                       m_mapGausFitReference[h.first][1]);
    WarningLevel wl_width= CheckFitPar(fitres->Parameter(2),fitres->ParError(2),
                                       m_mapGausFitReference[h.first][2],
                                       m_mapGausFitReference[h.first][3]);
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
    WarningLevel wl_mean = CheckFitPar(fitres->Parameter(1),fitres->ParError(1),
                                       m_mapGausFitReference[h.first][0],
                                       m_mapGausFitReference[h.first][1]);
    WarningLevel wl_width= CheckFitPar(fitres->Parameter(2),fitres->ParError(2),
                                       m_mapGausFitReference[h.first][2],
                                       m_mapGausFitReference[h.first][3]);
    m_mapWarnings[h.first] = wl_mean > wl_width ? wl_mean : wl_width;
  }
  if(m_verbose) m_pages["TTOverlapResiduals"]->Print((m_outputDirectory+"/"+"TTOverlapResiduals.pdf").c_str());
  return;
}

void AlignmentMonitoring::CheckOverlaps(){
  /* overlaps */
  std::map<std::string,TH1*> ovHists;
  std::vector<std::string> hNames={};
  std::vector<std::string> stats = {"1","2","3"};
  std::vector<std::string> boxes = {"ASide","CSide"};
  std::vector<std::string> tracks= {"Top","Bottom"};
  for (auto stat: stats) {
    for (auto trk : tracks)
      for (auto box : boxes)
        hNames.push_back("IT"+trk+"Track/IT"+stat+box+"Box/hitres");
    for (auto trk : boxes)
      for (auto box : tracks)
        hNames.push_back("IT"+trk+"Track/IT"+stat+box+"Box/hitres");
  }
  for (auto name : hNames){
    TH1* hist = m_hhelp.getH1(m_inputFileName.c_str(), "Track/TrackITOverlapMonitor/"+name);
    if(hist==0) hist = new TH1F();
    ovHists[name] = hist;
  }
  if (m_verbose) {
    for (auto stat: stats) {
      m_pages["OverlapResidualsIT"+stat] = (TCanvas*)m_hhelp.createPage( ("OverlapResidualsIT"+stat).c_str(),
                                                                                     ("Overlap Residulas IT"+stat).c_str(), 4, 2, 300*4, 400*2);
      int i = 1;
      for(auto h:ovHists){
        m_pages["OverlapResidualsIT"+stat]->cd(i++);
        h.second->Draw();
      }
    }
  }

  // Fit residuals
  TFitResultPtr fitres = 0;
  TF1 *ovf = new TF1("ovf","[0]*exp(-0.5*((x-[1])/[2])**2)+[3]",-5,5);
  ovf->SetParName(0,"Constant");
  ovf->SetParName(1,"Mean");
  ovf->SetParameter(1,0);
  ovf->SetParName(2,"Sigma");
  ovf->SetParameter(2,0.5);
  ovf->SetParName(3,"Background");
  ovf->SetParameter(3,0);
  for (auto h:ovHists) {
    if (m_verbose) std::cout << h.first << ":\n";
    m_mapWarnings[h.first] = WarningLevel::SEVERE; 
    m_insertOrder.push_back(h.first); 
    if( !h.second->GetEntries() ) continue;
    fitres = h.second->Fit(ovf,m_verbose ? "S" : "SQ"); // need to modify this
    if(fitres->IsEmpty()) continue;
    if(fitres->Chi2()<1e-6 or fitres->Ndf()==0) continue;
    WarningLevel wl_mean = CheckFitPar(fitres->Parameter(1),fitres->ParError(1), m_mapGausFitReference[h.first][0], m_mapGausFitReference[h.first][1]);
    WarningLevel wl_width = CheckFitPar(fitres->Parameter(2),fitres->ParError(2), m_mapGausFitReference[h.first][2], m_mapGausFitReference[h.first][3]);
    m_mapWarnings[h.first] = wl_mean > wl_width ? wl_mean : wl_width;
  }
  if (m_verbose) {
    for (auto stat: stats) 
      m_pages[std::string("OverlapResidualsIT")+stat]->Print( (m_outputDirectory+"/"+"OverlapResidualsIT"+stat+".pdf").c_str() );
  } 
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
      WarningLevel wl_mean = CheckFitPar(fitres->Parameter(1),fitres->ParError(1),
                                     m_mapGausFitReference[h.first][0]-m_mapGausFitReference[h.first][1],
                                     m_mapGausFitReference[h.first][0]+m_mapGausFitReference[h.first][1]);
      m_mapWarnings[h.first] = wl_mean;
      if( h.first.find("pull")!=string::npos ){
        WarningLevel wl_width= CheckFitPar(fitres->Parameter(2),fitres->ParError(2),
                                     m_mapGausFitReference[h.first][2]-m_mapGausFitReference[h.first][3],
                                     m_mapGausFitReference[h.first][2]+m_mapGausFitReference[h.first][3]);
        m_mapWarnings[h.first] = wl_mean > wl_width ? wl_mean : wl_width;
      }
    }//else
  }
  m_mapWarnings["D0MassDifference"] = WarningLevel::SEVERE; 
  m_insertOrder.push_back("D0MassDifference"); 
  double deltaM = mass_diff["massPositiveY"]->Parameter(1) - mass_diff["massNegativeY"]->Parameter(1);
  double sigmaDM= std::sqrt(mass_diff["massPositiveY"]->Parameter(2)*mass_diff["massPositiveY"]->Parameter(2)+mass_diff["massNegativeY"]->Parameter(2)*mass_diff["massNegativeY"]->Parameter(2)); 
  WarningLevel wl_mean = CheckFitPar(deltaM, sigmaDM,
                                   m_mapGausFitReference["D0MassDifference"][0]-m_mapGausFitReference["D0MassDifference"][1],
                                   m_mapGausFitReference["D0MassDifference"][0]+m_mapGausFitReference["D0MassDifference"][1]);
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
    if(value==WarningLevel::OK)      warning_string = "\033[1;32m OK \033[0m";
    if(value==WarningLevel::WARNING) warning_string = "\033[1;33m WARNING \033[0m";
    if(value==WarningLevel::SEVERE)  warning_string = "\033[1;31m SEVERE \033[0m";
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
