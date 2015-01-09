#ifndef VKEPLERCONTENT_CPP
#define VKEPLERCONTENT_CPP

#include "VKeplerContent.h"
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
    m_name = "TbData";
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
    ssN << std::setprecision(4) << h->GetEntries();
    ssMu << std::setprecision(4) << h->GetMean();
    ssSig << std::setprecision(4) << h->GetRMS();
    ssUF << std::setprecision(4) << h->GetBinContent(0);
    ssOF << std::setprecision(4) << h->GetBinContent(h->GetNbinsX() + 1);

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
    m_statsTitles.push_back("N: ");
    m_statsValues.push_back(ssN.str());
  }
};

//_____________________________________________________________________________
//_____________________________________________________________________________

VTabContent * VKeplerContent::configs(TFile * file, int nPlanes, VPlotOps * plotOps){
  VTabContent * topDummyTab = new VTabContent();

  VTabContent * tab1 = new VTabContent("Pixel Hits", topDummyTab);
  VTabContent * tab2 = new VTabContent("Clusters", topDummyTab);
  VTabContent * tab3 = new VTabContent("Tracks", topDummyTab);

  // Hits.
  VTabContent * tab4 = new VTabContent("ToT Dists", tab1);
  VTabContent * tab5 = new VTabContent("Hitmaps", tab1);
  VTabContent * tab6 = new VTabContent("Hits Per Event", tab1);

  // Clusters.
  VTabContent * tab8 = new VTabContent("ToT Dists", tab2);
  VTabContent * tab8a = new VTabContent("All Sizes", tab8);
  VTabContent * tab8b = new VTabContent("Size1", tab8);
  VTabContent * tab8c = new VTabContent("Size2", tab8);
  VTabContent * tab8d = new VTabContent("Size3", tab8);
  VTabContent * tab8e = new VTabContent("Size4", tab8);

  VTabContent * tab9 = new VTabContent("Size Dists", tab2);
  VTabContent * tab10 = new VTabContent("Hitmaps", tab2);
//  VTabContent * tab11 = new VTabContent("Time Dists", tab2);
//  VTabContent * tab12 = new VTabContent("xCorrelations", tab2);
//  VTabContent * tab13 = new VTabContent("yCorrelations", tab2);
//  VTabContent * tab14 = new VTabContent("xDifferences", tab2);
//  VTabContent * tab15 = new VTabContent("yDifferences", tab2);
//  VTabContent * tab15 = new VTabContent("Position Errors", tab2);

  // Tracks.
  VTabContent * tab17 = new VTabContent("Intercepts", tab3);
//  VTabContent * tab17 = new VTabContent("Intercepts Associated", tab3);
//  VTabContent * tab18 = new VTabContent("Intercepts NonAssociated", tab3);
//  VTabContent * tab19 = new VTabContent("Biased Residuals", tab3);
//  VTabContent * tab20 = new VTabContent("UnBiased Residuals", tab3);
//  VTabContent * tab21 = new VTabContent("Track Quality", tab3);
//  VTabContent * tab22 = new VTabContent("Track Clusters", tab3);

  VPlot * plot;
  VTH1 * plottable1;
  VTH2 * plottable2;
  for (int i=0; i<nPlanes; i++) {
    std::stringstream ss; ss<<i;
    std::string Name = "Plane" + ss.str();
    std::string name = "plane" + ss.str();

    // Hits.
    plot = new VPlot("Default Name" + ss.str(), tab4, false, plotOps);
    plottable1 = new VTH1(plot, 1, "Tb/TbHitMonitor/ToT/" + Name, file);

    plot = new VPlot("Default Name" + ss.str(), tab5, false, plotOps);
    plottable2 = new VTH2(plot, 0, "Tb/TbHitMonitor/HitMap/" + Name, file);

    plot = new VPlot("Default Name" + ss.str(), tab6, false, plotOps);
    plottable1 = new VTH1(plot, 1, "Tb/TbHitMonitor/HitsInEvent/" + Name, file);


    // Clusters.
    plot = new VPlot("Default Name" + ss.str(), tab8a, false, plotOps);
    plottable1 = new VTH1(plot, 1, "Tb/TbClusterPlots/ToT/All/" + Name, file);

    plot = new VPlot("Default Name" + ss.str(), tab8b, false, plotOps);
    plottable1 = new VTH1(plot, 1, "Tb/TbClusterPlots/ToT/OnePixel/" + Name, file);

    plot = new VPlot("Default Name" + ss.str(), tab8c, false, plotOps);
    plottable1 = new VTH1(plot, 1, "Tb/TbClusterPlots/ToT/TwoPixel/" + Name, file);

    plot = new VPlot("Default Name" + ss.str(), tab8d, false, plotOps);
    plottable1 = new VTH1(plot, 1, "Tb/TbClusterPlots/ToT/ThreePixel/" + Name, file);

    plot = new VPlot("Default Name" + ss.str(), tab8e, false, plotOps);
    plottable1 = new VTH1(plot, 1, "Tb/TbClusterPlots/ToT/FourPixel/" + Name, file);

    plot = new VPlot("Default Name" + ss.str(), tab9, false, plotOps);
    plottable1 = new VTH1(plot, 1, "Tb/TbClusterPlots/Size/" + name, file);

    plot = new VPlot("Default Name" + ss.str(), tab10, false, plotOps);
    plottable2 = new VTH2(plot, 0, "Tb/TbClusterPlots/Positions/" + Name, file);

    // Tracks.
    plot = new VPlot("Default Name" + ss.str(), tab17, false, plotOps);
    plottable2 = new VTH2(plot, 0, "Tb/TbTrackPlots/Intercepts/" + Name, file);
  }

  return topDummyTab;
}


//_____________________________________________________________________________

#endif // VKEPLERCONTENT_CPP

