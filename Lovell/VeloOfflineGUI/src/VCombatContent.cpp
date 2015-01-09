#ifndef VCOMBATCONTENT_CPP
#define VCOMBATCONTENT_CPP

#include "VCombatContent.h"
//_____________________________________________________________________________
//_____________________________________________________________________________

class VTH1 : public VPlottable {
  std::string m_direc;
  TFile * m_file;
public:
  VTH1(VPlot * plot, int style, std::string direc, TFile * file)
      : VPlottable(plot, style) {
    m_plottableDimension = 1;
    m_direc = direc;
    m_file = file;
    m_name = "TbData -";
  }


  //___________________________________________________________________________

  void getPlotData() {
    TH1 * h = (TH1*) m_file->Get(m_direc.c_str());

    for (int i=0; i<h->GetXaxis()->GetNbins(); i++) {
      m_xs.push_back(h->GetXaxis()->GetBinCenter(i));
      m_ys.push_back(h->GetBinContent(i));
    }

    m_plot->m_xAxisTitle = std::string(h->GetXaxis()->GetTitle());
    m_plot->m_yAxisTitle = std::string(h->GetYaxis()->GetTitle());
    m_plot->m_title = std::string(h->GetTitle());

    std::stringstream ssN, ssMu, ssSig, ssUF, ssOF;
    ssN << std::setprecision(3) << h->GetEntries();
    ssMu << std::setprecision(4) << h->GetMean();
    ssSig << std::setprecision(3) << h->GetRMS();
    ssUF << std::setprecision(3) << h->GetBinContent(0);
    ssOF << std::setprecision(3) << h->GetBinContent(h->GetNbinsX() + 1);

    m_statsTitles.push_back("N:");
    m_statsTitles.push_back("mu:");
    m_statsTitles.push_back("sig:");
    m_statsTitles.push_back("UF:");
    m_statsTitles.push_back("OF:");

    m_statsValues.push_back(ssN.str());
    m_statsValues.push_back(ssMu.str());
    m_statsValues.push_back(ssSig.str());
    m_statsValues.push_back(ssUF.str());
    m_statsValues.push_back(ssOF.str());
  }
};

//_____________________________________________________________________________
//_____________________________________________________________________________

class VTH2 : public VPlottable {
  std::string m_direc;
  TFile * m_file;
public:
  VTH2(VPlot * plot, int style, std::string direc, TFile * file)
      : VPlottable(plot, style) {
    m_plottableDimension = 2;
    m_direc = direc;
    m_file = file;
  }


  //___________________________________________________________________________

  void getPlotData() {
    m_name = "TbData -";
    TH2 * h = (TH2*) m_file->Get(m_direc.c_str());

    for (int i=0; i<h->GetXaxis()->GetNbins(); i++)
      m_xs.push_back(h->GetXaxis()->GetBinCenter(i));

    for (int i=0; i<h->GetYaxis()->GetNbins(); i++)
      m_ys.push_back(h->GetYaxis()->GetBinCenter(i));

    for (int i=0; i<h->GetXaxis()->GetNbins(); i++) {
      QVector<double> col;
      for (int j=0; j<h->GetYaxis()->GetNbins(); j++)
        col.push_back(h->GetBinContent(h->GetBin(i, j)));

      m_zs.push_back(col);
    }

    m_plot->m_xAxisTitle = std::string(h->GetXaxis()->GetTitle());
    m_plot->m_yAxisTitle = std::string(h->GetYaxis()->GetTitle());
    m_plot->m_zAxisTitle = std::string(h->GetZaxis()->GetTitle());
    m_plot->m_title = std::string(h->GetTitle());

    std::stringstream ssN;
    ssN << std::setprecision(3) << h->GetEntries();
    m_statsTitles.push_back("N: ");
    m_statsValues.push_back(ssN.str());
  }
};

//_____________________________________________________________________________
//_____________________________________________________________________________

VTabContent * VCombatContent::configs(TFile * file, int nPlanes,
    VPlotOps * plotOps){
  VTabContent * topDummyTab = new VTabContent();

  VTabContent * tab1 = new VTabContent("Landau", topDummyTab);
  VTabContent * tab2 = new VTabContent("Hitmaps", topDummyTab);
  VTabContent * tab3 = new VTabContent("X Dist", topDummyTab);
  VTabContent * tab4 = new VTabContent("Y Dist", topDummyTab);
  VTabContent * tab5 = new VTabContent("X Correlation", topDummyTab);
  VTabContent * tab6 = new VTabContent("Y Correlation", topDummyTab);

  VPlot * plot;
  VTH1 * plottable1;
  VTH2 * plottable2;
  for (int i=0; i<nPlanes; i++) {
    std::stringstream ss; ss<<i;
    std::string name = "Plane" + ss.str();
    std::string nameCor = "Plane0_with_Plane" + ss.str();

    plot = new VPlot("Landau" + ss.str(), tab1, false, plotOps);
    plottable1 = new VTH1(plot, 1, "Landau/" + name, file);

    plot = new VPlot("Hitmap" + ss.str(), tab2, false, plotOps);
    plottable2 = new VTH2(plot, 0, "Hitmaps/" + name, file);

    plot = new VPlot("xDistribution" + ss.str(), tab3, false, plotOps);
    plottable1 = new VTH1(plot, 1, "XDist/" + name, file);

    plot = new VPlot("yDistribution" + ss.str(), tab4, false, plotOps);
    plottable1 = new VTH1(plot, 1, "YDist/" + name, file);

    plot = new VPlot("xCorrelation" + ss.str(), tab5, false, plotOps);
    plottable2 = new VTH2(plot, 0, "CorrelationX/" + nameCor, file);

    plot = new VPlot("yCorrelation" + ss.str(), tab6, false, plotOps);
    plottable2 = new VTH2(plot, 0, "CorrelationY/" + nameCor, file);
  }

  return topDummyTab;
}


//_____________________________________________________________________________

#endif // VCOMBATCONTENT_CPP
