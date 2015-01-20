#include "VContentGetter.h"

//_____________________________________________________________________________

class interfaceTest : public VPlottable {
public:
  interfaceTest(VPlot * plot, int style) :
      VPlottable(plot, style)
  {
    m_name = "interfaceTest:";
  }

  //___________
  void getPlotData() {
    FILE * in;
    char buff[512];

    std::string command = "dummyDataGetter.py " +
        m_plot->m_plotOps->b_veloRunNumber->currentText().toStdString() + " " +
        m_retrivalCommand + " " + m_plot->m_plotOps->currentModuleStr();
    std::cout<<"Sending command: "<<std::endl;
    std::cout<<command<<std::endl;
    in = popen(command.c_str(), "r");

    std::string rawData;
    while(fgets(buff, sizeof(buff), in)!=NULL) rawData.append(buff);
    pclose(in);

    // Split.
    std::vector<std::string> rawDataSplit;
    std::istringstream iss(rawData);
    std::copy(std::istream_iterator<std::string>(iss),
         std::istream_iterator<std::string>(),
         std::back_inserter(rawDataSplit));

    int decoderID = atoi(rawDataSplit[0].c_str());
    if (decoderID == 1) firstDecoder(&rawDataSplit);
    else if (decoderID == 2) secondDecoder(&rawDataSplit);
    else std::cout<<"Unknown decoder"<<std::endl;
  }

  //___________
  void firstDecoder(std::vector<std::string> * rawDataSplit) {
    m_plottableDimension = 1;
    m_name = (*rawDataSplit)[1];
    int nPoints = atoi((*rawDataSplit)[2].c_str());
    for (int i=0; i<nPoints; i++) m_xs.push_back(::atof((*rawDataSplit)[3+i].c_str()));
    for (int i=0; i<nPoints; i++) m_ys.push_back(::atof((*rawDataSplit)[3+i+nPoints].c_str()));

    m_statsTitles.push_back("N:");
    m_statsTitles.push_back("Mean:");
    m_statsTitles.push_back("Sigma:");
    m_statsTitles.push_back("Underflow:");
    m_statsTitles.push_back("Overflow:");

    m_statsValues.push_back((*rawDataSplit)[2*nPoints+4]);
    m_statsValues.push_back((*rawDataSplit)[2*nPoints+5]);
    m_statsValues.push_back((*rawDataSplit)[2*nPoints+6]);
    m_statsValues.push_back((*rawDataSplit)[2*nPoints+7]);
    m_statsValues.push_back((*rawDataSplit)[2*nPoints+8]);
    m_plot->m_xAxisTitle = (*rawDataSplit)[2*nPoints+9];
    m_plot->m_yAxisTitle = (*rawDataSplit)[2*nPoints+10];
  }

  //___________
  void secondDecoder(std::vector<std::string> * rawDataSplit) {
    m_plottableDimension = 2;
    m_plottableStyle = 0;
    m_name = (*rawDataSplit)[1];
    int nPointsX = atoi((*rawDataSplit)[2].c_str());
    int nPointsY = atoi((*rawDataSplit)[3].c_str());
    for (int i=0; i<nPointsX; i++)
      m_xs.push_back(::atof((*rawDataSplit)[4+i].c_str()));

    for (int i=0; i<nPointsY; i++)
      m_ys.push_back(::atof((*rawDataSplit)[4+i+nPointsX].c_str()));

    QVector<double> rowZs;
    for (int i=0; i<nPointsX*nPointsY+1; i++) {
      rowZs.push_back(::atof((*rawDataSplit)[4+i+nPointsX+nPointsY].c_str()));
      if (i%nPointsX == 0 && i!=0) {
        m_zs.push_back(rowZs);
        rowZs.clear();
      }
    }
    for (unsigned int i=0; i<rowZs.size(); i++) std::cout<<rowZs[i]<<"  ";
    //m_zs.push_back(rowZs);

    m_statsTitles.push_back("N:");
    m_statsTitles.push_back("Mean:");
    m_statsTitles.push_back("Sigma:");
    m_statsTitles.push_back("UnderflowX:");
    m_statsTitles.push_back("OverflowX:");
    m_statsTitles.push_back("UnderflowY:");
    m_statsTitles.push_back("OverflowY:");
    unsigned int n = rawDataSplit->size();

    m_statsValues.push_back((*rawDataSplit)[n-10]);
    m_statsValues.push_back((*rawDataSplit)[n-9]);
    m_statsValues.push_back((*rawDataSplit)[n-8]);
    m_statsValues.push_back((*rawDataSplit)[n-7]);
    m_statsValues.push_back((*rawDataSplit)[n-6]);
    m_statsValues.push_back((*rawDataSplit)[n-5]);
    m_statsValues.push_back((*rawDataSplit)[n-4]);

    m_plot->m_xAxisTitle = (*rawDataSplit)[n-3];
    m_plot->m_yAxisTitle = (*rawDataSplit)[n-2];
    m_plot->m_zAxisTitle = (*rawDataSplit)[n-1];
  }
};


//_____________________________________________________________________________

class twoDparabola : public VPlottable {
public:
    twoDparabola(VPlot * plot, int style) : VPlottable(plot, style) {
    m_name = "Color_Map:";
    m_plottableDimension = 2;
    m_statsTitles.push_back("N: 1000");
    m_statsValues.push_back("1000");
  }

  twoDparabola(){
    m_name = "Color_Map:";
    m_plottableDimension = 2;
    m_plottableStyle = 0;
    m_statsTitles.push_back("N: 1000");
    m_statsValues.push_back("1000");
  }

  //___________
  void getPlotData() {
    bool firstRow = true;
    for (double y = -2; y<2; y+=0.05) {
      m_ys.push_back(y);
      QVector<double> rowZs;
      for (double x = -5; x<5; x+=0.1) {
        if (firstRow) m_xs.push_back(x);
        rowZs.push_back(x*x + 3*y*y);
      }
      m_zs.push_back(rowZs);
      firstRow = false;
    }
  }
};


//_____________________________________________________________________________

class gaussData : public VPlottable {
public:
  gaussData(VPlot * plot, int style) : VPlottable(plot, style) {
    m_name = "Data:";
    m_plottableDimension = 1;
    m_statsTitles.push_back("N: ");
    m_statsTitles.push_back("mu: ");
    m_statsTitles.push_back("sig: ");
    m_statsTitles.push_back("UF: ");
    m_statsTitles.push_back("OF: ");

    m_statsValues.push_back("1000");
    m_statsValues.push_back("0.0");
    m_statsValues.push_back("1.0");
    m_statsValues.push_back("20");
    m_statsValues.push_back("50");
  }

  gaussData(){
    m_name = "Data:";
    m_plottableDimension = 1;
    m_plottableStyle = 1;
    m_statsTitles.push_back("N: ");
    m_statsTitles.push_back("mu: ");
    m_statsTitles.push_back("sig: ");
    m_statsTitles.push_back("UF: ");
    m_statsTitles.push_back("OF: ");

    m_statsValues.push_back("1000");
    m_statsValues.push_back("0.0");
    m_statsValues.push_back("1.0");
    m_statsValues.push_back("20");
    m_statsValues.push_back("50");
  }

  //___________
  void getPlotData() {
    for (double x = -5; x<5; x+=0.1) {
      m_xs.push_back(x);
      m_ys.push_back(exp(-1.*pow(x, 2)) + 0.1*(((double)rand()/(double)RAND_MAX) -0.5));
    }
  }
};


//_____________________________________________________________________________

class gaussRef : public VPlottable {
public:
  gaussRef(VPlot * plot, int style) : VPlottable(plot, style) {
    m_name = "Reference:";
    m_plottableDimension = 1;
    m_statsTitles.push_back("N: ");
    m_statsTitles.push_back("mu: ");
    m_statsTitles.push_back("sig: ");

    m_statsValues.push_back("1000");
    m_statsValues.push_back("0.0");
    m_statsValues.push_back("1.0");
  }

  gaussRef() {
    m_name = "Reference:";
    m_plottableDimension = 1;
    m_plottableStyle = 0;
    m_statsTitles.push_back("N: ");
    m_statsTitles.push_back("mu: ");
    m_statsTitles.push_back("sig: ");

    m_statsValues.push_back("1000");
    m_statsValues.push_back("0.0");
    m_statsValues.push_back("1.0");
  }

  //___________
  void getPlotData() {
    for (double x = -5; x<5; x+=0.1) {
      m_xs.push_back(x);
      m_ys.push_back(exp(-1.*pow(x, 2)) + 0.1*(((double)rand()/(double)RAND_MAX) -0.5));
    }
  }
};

//_____________________________________________________________________________
//_____________________________________________________________________________

VTabContent * VContentGetter::veloShortConfigs(VPlotOps * plotOps){
  // Dummy example.
  VTabContent * topDummyTab = new VTabContent(); // Always needed.

  VTabContent * tab1 = new VTabContent("Duel", topDummyTab);
  VTabContent * tab2 = new VTabContent("Overlay", topDummyTab);
  VTabContent * tab3 = new VTabContent("Colz", topDummyTab);
  VTabContent * tab4 = new VTabContent("MultiplePlots", topDummyTab);
  VTabContent * tab5 = new VTabContent("SubTabs", topDummyTab);
  VTabContent * tab6 = new VTabContent("SubTabLeft", tab5);
  VTabContent * tab7 = new VTabContent("SubTabRight", tab5);
  VTabContent * tab8 = new VTabContent("Table", topDummyTab);

  // Tab1
  VPlot * plot1 = new VPlot("Gaussian", tab1, true, plotOps);
  VPlot * plot2 = new VPlot("Gaussian", tab1, true, plotOps);
  interfaceTest * plottable1 = new interfaceTest(plot1, 0);
  gaussData * plottable2 = new gaussData(plot2, 1);

  // Tab2
  VPlot * plot3 = new VPlot("Gaussian", tab2, true, plotOps);
  gaussData * plottable3 = new gaussData(plot3, 0);
  gaussData * plottable4 = new gaussData(plot3, 1);

  // Tab3
  VPlot * plot4 = new VPlot("Gaussian", tab3, true, plotOps);
  twoDparabola * plottable5 = new twoDparabola(plot4, 0);

  // Tab4
  VPlot * plot5;
  twoDparabola * plottable6;
  gaussData * plottable7;
  for (int i=0; i<4; i++) {
    plot5 = new VPlot("Gaussian", tab4, true, plotOps);
    plottable6 = new twoDparabola(plot5, 0);
    plottable7 = new gaussData(plot5, 2);
  }

  // SubTab1
  VPlot * plot6 = new VPlot("Gaussian", tab6, true, plotOps);
  gaussData * plottable8 = new gaussData(plot6, 0);

  // SubTab1
  VPlot * plot7 = new VPlot("Gaussian", tab7, true, plotOps);
  gaussData * plottable9 = new gaussData(plot7, 1);

  // Tab8
  VTable * table1 = new VTable("testTable.csv", tab8);

  return topDummyTab;
}


//_____________________________________________________________________________

VTabContent * VContentGetter::veloFileConfigs(VPlotOps * plotOps, std::string interfaceScript)
{
  VTabContent * topDummyTab = new VTabContent("Top"); // Always needed.
  std::vector<VTabContent*> allTabs; // Useful to have container for all tabs.
  allTabs.push_back(topDummyTab);

  // Load all data into a vector.
  std::vector< std::vector< std::string > > ops;
  FILE * in;
  char buff[512];

  std::string command = "" + interfaceScript + " run_view_config";
  in = popen(command.c_str(), "r");
  while(fgets(buff, sizeof(buff), in)!=NULL) {
    std::string rawData(buff);
    // Split by spaces.
    std::vector<std::string> rawDataSplit;
    std::istringstream iss(rawData);
    std::copy(std::istream_iterator<std::string>(iss),
       std::istream_iterator<std::string>(),
       std::back_inserter(rawDataSplit));
    ops.push_back(rawDataSplit);
  }

  findChildren(topDummyTab, &allTabs, &ops); // Called recursively.
  findPlots(&allTabs, &ops, plotOps);
  return topDummyTab;
}

//_____________________________________________________________________________

void VContentGetter::findChildren(VTabContent * parentTab,
    std::vector<VTabContent*> * allTabs,
    std::vector< std::vector< std::string > > * ops)
{
  std::vector< std::vector< std::string > >::iterator iop;
  for (iop = ops->begin(); iop != ops->end(); iop++) {
    if ((*iop)[0] != "Tab" || (*iop)[0].substr(0, 1) == "#") continue;
    if ((*iop)[2] == parentTab->m_title) {
      VTabContent * tab = new VTabContent((*iop)[1], parentTab);
      allTabs->push_back(tab);
      if (allTabs->size()>10) exit(1);
      findChildren(tab, allTabs, ops);
    }
  }
}



//_____________________________________________________________________________

void VContentGetter::findPlots(std::vector<VTabContent*> * allTabs,
  std::vector< std::vector< std::string > > * ops, VPlotOps * plotOps)
{
  std::vector< std::vector< std::string > >::iterator iop;
  for (iop = ops->begin(); iop != ops->end(); iop++) {
    if ((*iop)[0] != "Plot" || (*iop)[0].substr(0, 1) == "#") continue;
    std::vector<VTabContent*>::iterator itab;
    for (itab = allTabs->begin(); itab != allTabs->end(); itab++){
      if ((*iop)[2] == (*itab)->m_title) {
        VPlot * plot = new VPlot((*iop)[1], (*itab), true, plotOps);
        interfaceTest * plottable = new interfaceTest(plot, 1);
        if ((*iop)[4] == "multipleModules") plot->m_multipleModules = true;
        else plot->m_multipleModules = false;
        plottable->m_retrivalCommand = (*iop)[3];
      }
    }
  }
}



//_____________________________________________________________________________
