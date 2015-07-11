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
// USR
#include "AlignmentMonitoring/Utilities.h"

using namespace Alignment::AlignmentMonitoring;

// Constructor
AlignmentMonitoring::AlignmentMonitoring(const char* filename)
 : m_inputFileName(filename)
 , m_verbose(false)
{
  LoadGausFitReferences();
}

// Methods
void
AlignmentMonitoring::Run()
{
  CheckResiduals();
  CheckOverlaps();
  PrintWarnings();
}

void
AlignmentMonitoring::CheckResiduals()
{
  std::map<std::string,TH1*> resHists;
  std::vector<std::string> hNames= {
    "OTResidual","ITResidual","TTResidual","VeloPhiResidual","VeloRResidual",
    "OTresidualPull","ITresidualPull","TTresidualPull","VeloPhiresidualPull","VeloRresidualPull"};
  for (auto name : hNames)
    resHists[name] = m_hhelp.getH1(m_inputFileName.c_str(), "Track/TrackMonitor/Long/"+name);

  // Plot histograms
  if (m_verbose) {
    m_pages["Residuals"] = (TCanvas*)m_hhelp.createPage("Residuals", "Residuals", 5, 2);
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
    fitres = h.second->Fit("gaus",m_verbose ? "S" : "SQ");
    WarningLevel wl_mean = CheckFitPar(fitres->Parameter(1),fitres->ParError(1),
                                       m_mapGausFitReference[h.first][0], m_mapGausFitReference[h.first][1]);
    WarningLevel wl_width = CheckFitPar(fitres->Parameter(2),fitres->ParError(2),
                                        m_mapGausFitReference[h.first][2], m_mapGausFitReference[h.first][3]);
    m_mapWarnings[h.first] = wl_mean > wl_width ? wl_mean : wl_width;
  }
  return;
}

void
AlignmentMonitoring::CheckOverlaps()
{
  std::map<std::string,TH1*> ovHists;
  std::vector<std::string> hNames={};
  for (auto stat: {"1","2","3"}) {
    for (auto trk : {"ASide","CSide"})
      for (auto box : {"Top","Bottom"})
        hNames.push_back( std::string("IT")+trk+std::string("Track/IT")+stat+box+std::string("Box/hitres"));
    for (auto trk : {"Bottom","Top"})
      for (auto box : {"ASide","CSide"})
        hNames.push_back( std::string("IT")+trk+std::string("Track/IT")+stat+box+std::string("Box/hitres"));
  }
  for (auto name : hNames)
    ovHists[name] = m_hhelp.getH1(m_inputFileName.c_str(), "Track/TrackITOverlapMonitor/"+name);

  if (m_verbose) {
    for (auto stat: {"1","2","3"}) {
      m_pages[std::string("OverlapResidualsIT")+stat] = (TCanvas*)m_hhelp.createPage( (std::string("OverlapResidualsIT")+stat).c_str(),
                                                                                     (std::string("Overlap Residulas IT")+stat).c_str(), 4, 2);
      int i = 1;
      for (auto trk : {"ASide","CSide"})
        for (auto box : {"Top","Bottom"}) {
          m_pages[std::string("OverlapResidualsIT")+stat]->cd(i++);
          ovHists[std::string("IT")+trk+std::string("Track/IT")+stat+box+std::string("Box/hitres")]->SetTitle( (std::string("IT")+trk+std::string("Track/IT")+stat+box+std::string("Box/hitres")).c_str());
          ovHists[std::string("IT")+trk+std::string("Track/IT")+stat+box+std::string("Box/hitres")]->Draw();
        }
      for (auto trk : {"Bottom","Top"})
        for (auto box : {"ASide","CSide"}) {
          m_pages[std::string("OverlapResidualsIT")+stat]->cd(i++);
          ovHists[std::string("IT")+trk+std::string("Track/IT")+stat+box+std::string("Box/hitres")]->SetTitle((std::string("IT")+trk+std::string("Track/IT")+stat+box+std::string("Box/hitres")).c_str());
          ovHists[std::string("IT")+trk+std::string("Track/IT")+stat+box+std::string("Box/hitres")]->Draw();
        }
    }
  }

  // Fit residuals
  TFitResultPtr fitres = 0;
  TF1 *ovf = new TF1("ovf","[0]*exp(-0.5*((x-[1])/[2])**2)+[3]",-5,5);
  ovf->SetParameter(1,0);
  ovf->SetParameter(2,0.2);
  ovf->SetParError(2,0.02);
  for (auto h:ovHists) {
    if (m_verbose) std::cout << h.first << ":\n";
    fitres = h.second->Fit(ovf,m_verbose ? "S" : "SQ"); // need to modify this
    //WarningLevel wl_mean = CheckFitPar(fitres->Parameter(1),fitres->ParError(1), m_mapGausFitReference[h.first][0], m_mapGausFitReference[h.first][1]);
    //WarningLevel wl_width = CheckFitPar(fitres->Parameter(2),fitres->ParError(2), m_mapGausFitReference[h.first][2], m_mapGausFitReference[h.first][3]);
    //m_mapWarnings[h.first] = wl_mean > wl_width ? wl_mean : wl_width;
  }
  return;
}


// Add check on the distance of y>0, y<0
// Add check on the D0 position and width

void
AlignmentMonitoring::PrintWarnings()
{
  for (auto wrn : m_mapWarnings) {
    if (!m_verbose && wrn.second == WarningLevel::OK) continue;
    std::cout << wrn.first << ": " << wrn.second << std::endl;
  }
  return;
}

WarningLevel
AlignmentMonitoring::CheckFitPar(double x, double xerr, double xmin, double xmax)
{
  if (m_verbose) std::cout << x << " +- " << xerr << "\t["<< xmin << "," <<xmax<<"]:";
  WarningLevel wl = WarningLevel::SEVERE; // default=SEVERE
  if (xmin < x+xerr && x-xerr < xmax) wl = WarningLevel::OK; // OK within range
  else if (xmin-(xmax-xmin)/2 < x+xerr && x-xerr < xmax+(xmax-xmin)/2) wl = WarningLevel::WARNING; // WARNING within double range
  if (m_verbose) std::cout << "\t" << wl << std::endl;
  return wl;
}

void 
AlignmentMonitoring::LoadGausFitReferences(const char* filename)
{
  Utilities ut;
  std::string line;
  if ( std::string(filename).size() == 0 && std::string(std::getenv("ALIGNMENTMONITORINGROOT")).size() == 0)
    std::cout << "ERROR(AlignmentMonitoring::LoadGausFitReference): ALIGNMENTMONITORINGROOT environment variable is not set! Cannot find default references.\n";
  std::ifstream ifile( std::string(filename).size() == 0 ? (std::getenv("ALIGNMENTMONITORINGROOT")+std::string("/files/GausFitReferences.txt")).c_str() : filename);
  if (ifile.is_open()) {
    while ( std::getline (ifile,line) ) {
      std::vector<std::string> refs = ut.splitString(line,"\t");
      if (refs.size() < 2 ||
          refs[0].find("#") != std::string::npos ||
          refs[0].find("Name") != std::string::npos) continue;
      m_mapGausFitReference[refs[0]] = {std::stod(refs[1]),std::stod(refs[2]),std::stod(refs[3]),std::stod(refs[4])};
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
