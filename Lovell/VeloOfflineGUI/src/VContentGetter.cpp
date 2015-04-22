#include "VContentGetter.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

class jsonPlottable : public VPlottable {
public:
  jsonPlottable(VPlot * plot, int style) :
      VPlottable(plot, style)
  {
    m_name = "jsonPlottable:";
  }

  //___________
  void getPlotData() {
  	if (m_gotData) return;
  	m_gotData = true;
    FILE * in;
    char buff[5000];

    std::string command = "retrieve_run_view_plot.py " +
        m_plot->m_plotOps->b_veloRunNumber->currentText().toStdString() + " '" +
        m_retrivalCommand + "' '" + m_plot->m_plotOps->currentModuleStr() + "'";
    std::string * dataDir = m_plot->m_plotOps->m_dataDir;
    command += " --run-data-dir=" + (*dataDir);
    std::cout<<"Sending command: "<<std::endl;
    std::cout<<command<<std::endl;
    in = popen(command.c_str(), "r");

    std::string rawData;
    while(fgets(buff, sizeof(buff), in)!=NULL) rawData.append(buff);
    pclose(in);

    rapidjson::Document d;
    d.Parse(rawData.c_str());
    if (!d.HasMember("data")) {
    	std::cout<<"No plot data returned to GUI"<<std::endl;
    	return;
    }

    std::string className = d["data"]["object_class"].GetString();
    if (className.size() == 4) {
    	if (className.substr(0, 3) == "TH1") th1Decoder(&d);
    	else if (className.substr(0, 3) == "TH2") th2Decoder(&d);
    	else std::cout<<"Unknown root object: "<<className<<std::endl;
    }

    else if (className.size() == 8) {
    	if (className == "TProfile") th1Decoder(&d);
    	else std::cout<<"Unknown root object: "<<className<<std::endl;
    }
    else std::cout<<"Unknown root object: "<<className<<std::endl;
  }


  //___________
  void th1Decoder(rapidjson::Document * d) {
  	m_plottableDimension = 1;

  	if (!(*d)["success"].GetBool()) {
  		std::cout<<"Unable to retrive plot data: "<<(*d)["message"].GetString()<<std::endl;
  		return;
  	}
  	m_name = (*d)["data"]["title"].GetString();
  	const rapidjson::Value & values = (*d)["data"]["data"]["values"];
  	rapidjson::SizeType nValues = values.Size();
    for (rapidjson::SizeType i = 0; i < nValues; ++i) {
    	double val = values[i].GetDouble();
    	m_ys.push_back(val);
    }

    const rapidjson::Value& bins = (*d)["data"]["data"]["binning"];
    assert(bins.IsArray());
    for (rapidjson::SizeType i = 0; i < bins.Size(); ++i) {
    	double x = 0.5*(bins[i][0u].GetDouble() + bins[i][1].GetDouble());
    	m_xs.push_back(x);
    }

    m_statsTitles.push_back("N:");
    m_statsTitles.push_back("Mean:");
    m_statsTitles.push_back("RMS:");
    m_statsTitles.push_back("Underflow:");
    m_statsTitles.push_back("Overflow:");


    // Retrieve stats.
    double entries = (*d)["data"]["data"]["entries"].GetDouble();
    std::stringstream ssN; ssN<<entries; m_statsValues.push_back(ssN.str());
    double mean = (*d)["data"]["data"]["mean"].GetDouble();
    std::stringstream ssMean; ssMean<<mean; m_statsValues.push_back(ssMean.str());
    double rms = (*d)["data"]["data"]["rms"].GetDouble();
    std::stringstream ssRMS; ssRMS<<rms; m_statsValues.push_back(ssRMS.str());
    double underflow = (*d)["data"]["data"]["underflow"].GetDouble();
    std::stringstream ssUnder; ssUnder<<underflow; m_statsValues.push_back(ssUnder.str());
    double overflow = (*d)["data"]["data"]["overflow"].GetDouble();
    std::stringstream ssOver; ssOver<<overflow; m_statsValues.push_back(ssOver.str());

    const rapidjson::Value& a = (*d)["data"]["data"]["axis_titles"];
    assert(a.IsArray());
    m_plot->m_xAxisTitle = a[0].GetString();
    m_plot->m_yAxisTitle = a[1].GetString();
    std::cout<<"Plot retrieval successful: "<<m_name<<std::endl;
  }

  //___________
  void th2Decoder(rapidjson::Document * d) {
    m_name = (*d)["data"]["title"].GetString();
    m_plottableStyle = 0;
    m_plottableDimension = 2;

    const rapidjson::Value& xbins = (*d)["data"]["data"]["xbinning"];
    for (rapidjson::SizeType i = 0; i < xbins.Size(); ++i) {
    	double x = 0.5*(xbins[i][0u].GetDouble() + xbins[i][1].GetDouble());
    	m_xs.push_back(x);
    }

    const rapidjson::Value& ybins = (*d)["data"]["data"]["ybinning"];
    for (rapidjson::SizeType i = 0; i < ybins.Size(); ++i) {
    	double y = 0.5*(ybins[i][0u].GetDouble() + ybins[i][1].GetDouble());
    	m_ys.push_back(y);
    }

    const rapidjson::Value& values = (*d)["data"]["data"]["values"];
    for (rapidjson::SizeType i = 0; i < values.Size(); ++i) {
    	QVector<double> rowZs;
    	for (rapidjson::SizeType j = 0; j < values[i].Size(); ++j) rowZs.push_back(values[i][j].GetDouble());
    	m_zs.push_back(rowZs);
    }



    m_statsTitles.push_back("N:");
    m_statsTitles.push_back("UnderflowX:");
    m_statsTitles.push_back("OverflowX:");
    m_statsTitles.push_back("UnderflowY:");
    m_statsTitles.push_back("OverflowY:");

    double entries = (*d)["data"]["data"]["entries"].GetDouble();
    std::stringstream ssN; ssN<<entries; m_statsValues.push_back(ssN.str());

    //m_statsValues.push_back("0.0");
    double underFlowX = (*d)["data"]["data"]["xunderflow"].GetDouble();
    std::stringstream ssUnderFlowX; ssUnderFlowX<<underFlowX; m_statsValues.push_back(ssUnderFlowX.str());
    double overFlowX = (*d)["data"]["data"]["xoverflow"].GetDouble();
    std::stringstream ssOverFlowX; ssOverFlowX<<overFlowX; m_statsValues.push_back(ssOverFlowX.str());
    double underFlowY = (*d)["data"]["data"]["yunderflow"].GetDouble();
    std::stringstream ssUnderFlowY; ssUnderFlowY<<underFlowY; m_statsValues.push_back(ssUnderFlowY.str());
    double overFlowY = (*d)["data"]["data"]["yoverflow"].GetDouble();
    std::stringstream ssOverFlowY; ssOverFlowY<<overFlowY; m_statsValues.push_back(ssOverFlowY.str());

    const rapidjson::Value& a = (*d)["data"]["data"]["axis_titles"];
    assert(a.IsArray());
    m_plot->m_xAxisTitle = a[0u].GetString();
    m_plot->m_yAxisTitle = a[1].GetString();
    m_plot->m_zAxisTitle = "N";
    std::cout<<"Plot retrieval successful: "<<m_name<<std::endl;
  }
};

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
    for (int i=0; i<rowZs.size(); i++) std::cout<<rowZs[i]<<"  ";

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

/*
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
*/


//_____________________________________________________________________________

VTabContent * VContentGetter::veloFileConfigs(VPlotOps * plotOps, std::string interfaceScript)
{
  VTabContent * topDummyTab = new VTabContent("Top"); // Always needed.
  std::vector<VTabContent*> allTabs; // Useful to have container for all tabs.
  allTabs.push_back(topDummyTab);

  // Load all data into a vector.
  std::vector< std::vector< std::string > > ops; // One row per tab.
  FILE * in;
  char buff[5000];

  std::string command = "" + interfaceScript + " run_view_config";
  in = popen(command.c_str(), "r");
  std::string allRawData;
  while(fgets(buff, sizeof(buff), in)!=NULL) {
    std::string rawData(buff);
    allRawData+=rawData;
  }

  jsonToOps(&allRawData, &ops);
  fourPlotsPerTabLimiter(&ops);
  findChildren(topDummyTab, &allTabs, &ops); // Called recursively.
  findPlots(&allTabs, &ops, plotOps);
  return topDummyTab;
}

//_____________________________________________________________________________

void VContentGetter::jsonToOps(std::string * jsonOps,
    std::vector< std::vector< std::string > > * ops)
{
	rapidjson::Document d;
	d.Parse(jsonOps->c_str());
	for (rapidjson::Value::ConstMemberIterator itr = d.MemberBegin();
       itr != d.MemberEnd(); ++itr) {
		std::vector<std::string> tabInfo;
		tabInfo.push_back("Tab");
		std::string tabName = itr->name.GetString();
    const rapidjson::Value &tab = itr->value;
    tabInfo.push_back(tab["title"].GetString());
    tabInfo.push_back("Top");
    ops->push_back(tabInfo);

    if (tab.HasMember("plots")) {
    	const rapidjson::Value &plots = tab["plots"];
    	rapidjson::SizeType numPlots = plots.Size();
    	for (rapidjson::SizeType i = 0; i < numPlots; ++i) {
    		std::vector<std::string> plotInfo;
    		const rapidjson::Value &plot = plots[i];
    		plotInfo.push_back("Plot");
    		plotInfo.push_back(plot["title"].GetString());
    		plotInfo.push_back(tab["title"].GetString());
    		plotInfo.push_back(plot["name"].GetString());
    	  if (plot["sensor_dependent"].GetBool()) plotInfo.push_back("multipleModules");
    	  else plotInfo.push_back("singleModule");
    	  ops->push_back(plotInfo);
    	}
    }
	}
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
      findChildren(tab, allTabs, ops);
    }
  }
}



//_____________________________________________________________________________

void VContentGetter::findPlots(std::vector<VTabContent*> * allTabs,
  std::vector< std::vector< std::string > > * ops, VPlotOps * plotOps)
{
	std::vector<jsonPlottable*> firstTab;
	std::vector<jsonPlottable*> otherPlottables;
  std::vector< std::vector< std::string > >::iterator iop;
  for (iop = ops->begin(); iop != ops->end(); iop++) {
    if ((*iop)[0] != "Plot" || (*iop)[0].substr(0, 1) == "#") continue;
    std::vector<VTabContent*>::iterator itab;
    for (itab = allTabs->begin(); itab != allTabs->end(); itab++){
      if ((*iop)[2] == (*itab)->m_title) {
      	bool hasRef = false; // SET ME.
        VPlot * plot = new VPlot((*iop)[1], (*itab), true, plotOps);
        jsonPlottable * plottable = new jsonPlottable(plot, 1);
        if ((*iop)[2] == "Pedestals") firstTab.push_back(plottable);
        else otherPlottables.push_back(plottable);
        plottable->m_retrivalCommand = (*iop)[3];

        if (hasRef) {
					jsonPlottable * plottableRef = new jsonPlottable(plot, 0);
					if ((*iop)[2] == "Pedestals") firstTab.push_back(plottableRef);
					else otherPlottables.push_back(plottableRef);
					plottableRef->m_retrivalCommand = (*iop)[3]; // SET ME.
      	}

        if ((*iop)[4] == "multipleModules") plot->m_multipleModules = true;
        else plot->m_multipleModules = false;
      }
    }
  }

  std::vector<std::thread*> firstTabThreads;
  plotOps->notify("Filling pedestals...");
  for (unsigned int i=0; i<firstTab.size(); i++)
  	firstTabThreads.push_back(new std::thread(&VPlottable::getData, firstTab[i]));

  for (unsigned int i=0; i<firstTabThreads.size(); i++)
  	firstTabThreads[i]->join();

  plotOps->notify("Pedestals done.");

  plotOps->notify("Filling others...");

  for (unsigned int i=0; i<otherPlottables.size(); i++) {
  	if (i==otherPlottables.size()-1) {
			std::thread t(&VContentGetter::lastPlottable, otherPlottables[i], plotOps);
			t.detach();
  	}
  	else {
  		std::thread t(&VPlottable::getData, otherPlottables[i]);
			t.detach();
  	}
  }
}

void VContentGetter::lastPlottable(VPlottable * p, VPlotOps * ops) {
	p->getData();
	ops->notify("Others done.");
}

//_____________________________________________________________________________

void VContentGetter::fourPlotsPerTabLimiter(
		std::vector< std::vector< std::string > > * ops)
{
	std::cout<<"Adjusting layout for 4 plots per tab..."<<std::endl;
	// Loops over top level tabs from Vetra. If more than 4 plots per tab,
	// introduce new sub tabs and alter parent tab names accordingly.
	// Order is arbitrary (as found in ops list).

	// First find if there is a problem.
	std::vector< std::vector< std::string > >::iterator iopTab;
  for (iopTab = ops->begin(); iopTab != ops->end(); iopTab++) {
    if ((*iopTab)[0] != "Tab" || (*iopTab)[0].substr(0, 1) == "#") continue;
    std::string parentName = (*iopTab)[1];
    unsigned int nPlots = 0;
    std::vector< std::vector< std::string > >::iterator iopPlot;
		for (iopPlot = ops->begin(); iopPlot != ops->end(); iopPlot++) {
			if ((*iopPlot)[0] != "Plot" || (*iopPlot)[0].substr(0, 1) == "#") continue;
			if ((*iopPlot)[2] == parentName) nPlots++;
		}

		if (nPlots <= 4) continue;
		unsigned int nNeeded = nPlots/4 + 1;
		for (unsigned int iSubTab=0; iSubTab<nNeeded; iSubTab++) {
			std::stringstream ssiSubTab;
			ssiSubTab << (iSubTab + 1);
			std::vector<std::string> tab;
			std::string tabName = parentName + "_" + ssiSubTab.str();
			tab.push_back("Tab");
			tab.push_back(tabName);
			tab.push_back(parentName);
			ops->push_back(tab);
		}

		// Adjust the parent plots.
		nPlots = 0;
		for (iopPlot = ops->begin(); iopPlot != ops->end(); iopPlot++) {
			if ((*iopPlot)[0] != "Plot" || (*iopPlot)[2] != parentName) continue;
			std::stringstream ss;
			ss << ((nPlots/4) + 1);
			std::string adjustedTabName = parentName + "_" + ss.str();
			(*iopPlot)[2] = adjustedTabName;
			nPlots++;
		}
  }
}

//_____________________________________________________________________________
