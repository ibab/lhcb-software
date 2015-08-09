#include "VContentGetter.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
//#include "PythonQt3.0/src/PythonQt.h"
//#include "TPython.h"
//#include <boost/python.hpp>
#include <unistd.h>


class refDiff : public VPlottable {
public:
	bool m_diffNotRatio;
	refDiff(VPlot * plot, int style) :
      VPlottable(plot, style) {
		m_name = "refDiff:";
	}

	void getPlotData() {
		if (m_diffNotRatio) m_plottableStyle = 3;
		else m_plottableStyle = 4;
		m_plottableDimension = m_plot->m_plottables[0]->m_plottableDimension;
		// Loop over the first two plottables on the plot and take the difference/ratio.
		// Copy over the xs from the first plottable. Do the same for ys too if a 2D plot.

		for (int i=0; i<m_plot->m_plottables[0]->m_xs.size(); i++) m_xs.push_back(m_plot->m_plottables[0]->m_xs[i]);
		if (m_plottableDimension == 2) {
			if (m_plot->m_plottables[0]->m_xs.size() != m_plot->m_plottables[1]->m_xs.size()) return;
			if (m_plot->m_plottables[0]->m_ys.size() != m_plot->m_plottables[1]->m_ys.size()) return;
			for (int i=0; i<m_plot->m_plottables[0]->m_ys.size(); i++) m_ys.push_back(m_plot->m_plottables[0]->m_ys[i]);
			for (int i=0; i<m_plot->m_plottables[0]->m_zs.size(); i++) {
				QVector<double> rowZs;
				for (int j=0; j<m_plot->m_plottables[0]->m_zs[i].size(); j++) {
					double z;
					if (m_diffNotRatio) z = m_plot->m_plottables[0]->m_zs[i][j] - m_plot->m_plottables[1]->m_zs[i][j];
					else {
						if (m_plot->m_plottables[1]->m_zs[i][j] == 0.) z = 0.;
						else z = m_plot->m_plottables[0]->m_zs[i][j]/m_plot->m_plottables[1]->m_zs[i][j];
					}
					rowZs.push_back(z);
				}
				m_zs.push_back(rowZs);
			}
		}
		else {
			if (m_plot->m_plottables[0]->m_ys.size() != m_plot->m_plottables[1]->m_ys.size()) return;
			for (int i=0; i<m_plot->m_plottables[0]->m_ys.size(); i++) {
				double y;
				if (m_diffNotRatio) y = m_plot->m_plottables[0]->m_ys[i] - m_plot->m_plottables[1]->m_ys[i];
				else {
					if (m_plot->m_plottables[1]->m_ys[i] == 0.) y = 0.;
					else y = m_plot->m_plottables[0]->m_ys[i]/m_plot->m_plottables[1]->m_ys[i];
				}
				m_ys.push_back(y);
			}
		}
	}
};



class jsonPlottable : public VPlottable {
public:
	bool m_direct;
  jsonPlottable(VPlot * plot, int style) :
      VPlottable(plot, style)
  {
    m_name = "jsonPlottable:";
    m_direct = true;
  }


  void directRetrival() {
  	TObject * h;
  	if (m_isRef) h = m_plot->m_plotOps->f_inReff->Get(m_retrivalCommand.c_str());
  	else h = m_plot->m_plotOps->f_in->Get(m_retrivalCommand.c_str());
  	if (h==NULL) {
  		std::cout<<"Could not find plot in vetra output: "<<m_retrivalCommand<<std::endl;
  		return;
  	}
  	if (std::string(h->ClassName()) == "TH1D" || std::string(h->ClassName()) == "TProfile") directTH1Decoder((TH1D*)h);
  	else if (std::string(h->ClassName()) == "TH2D") directTH2Decoder((TH2D*)h);
  	else std::cout<<"Unknown plot class for offline GUI: "<<h->ClassName()<<std::endl;
  }

  void directTH1Decoder(TH1D * h) {
  	m_plottableDimension = 1;
  	m_name = std::string(h->GetTitle());
  	for (int i=1; i<h->GetXaxis()->GetNbins(); i++) {
  		m_xs.push_back(h->GetBinCenter(i));
  		m_ys.push_back(h->GetBinContent(i));
  	}
  	m_statsTitles.push_back("N:");
    m_statsTitles.push_back("Mean:");
    m_statsTitles.push_back("RMS:");
    m_statsTitles.push_back("Underflow:");
    m_statsTitles.push_back("Overflow:");

    int entries = h->GetEntries();
    std::stringstream ssN; ssN<<entries; m_statsValues.push_back(ssN.str());
    double mean = h->GetMean();
    std::stringstream ssMean; ssMean<<mean; m_statsValues.push_back(ssMean.str());
    double rms = h->GetRMS();
    std::stringstream ssRMS; ssRMS<<rms; m_statsValues.push_back(ssRMS.str());
    double underflow = h->GetBinContent(0);
    std::stringstream ssUnder; ssUnder<<underflow; m_statsValues.push_back(ssUnder.str());
    double overflow = h->GetBinContent(h->GetXaxis()->GetNbins() + 1);
    std::stringstream ssOver; ssOver<<overflow; m_statsValues.push_back(ssOver.str());

    m_plot->m_xAxisTitle = std::string(h->GetXaxis()->GetTitle());
    m_plot->m_yAxisTitle = std::string(h->GetYaxis()->GetTitle());
    std::cout<<"Plot retrieval successful: "<<m_name<<std::endl;
  }

  void directTH2Decoder(TH2D * h) {
  	m_name = std::string(h->GetTitle());
  	m_plottableStyle = 0;
    m_plottableDimension = 2;

  	m_statsTitles.push_back("N:");
  	double entries = h->GetEntries();
    std::stringstream ssN; ssN<<entries; m_statsValues.push_back(ssN.str());

    for (int i=1; i<h->GetXaxis()->GetNbins()+1; i++) {
    	m_xs.push_back(h->GetXaxis()->GetBinCenter(i));
    	QVector<double> rowZs;
    	for (int j=1; j<h->GetYaxis()->GetNbins()+1; j++) {
    		if (i==1) m_ys.push_back(h->GetYaxis()->GetBinCenter(j));
    		rowZs.push_back(h->GetBinContent(i, j));
    	}
    	m_zs.push_back(rowZs);
    }

  	m_plot->m_xAxisTitle = std::string(h->GetXaxis()->GetTitle());
    m_plot->m_yAxisTitle = std::string(h->GetYaxis()->GetTitle());
    m_plot->m_zAxisTitle = "N";
    std::cout<<"Plot retrieval successful: "<<m_name<<std::endl;
  }


  //___________
  void getPlotData() {
  	if (m_gotData) return;
  	m_gotData = true;
  	if (m_direct) {
  		directRetrival();
  		return;
  	}


    FILE * in;
    char buff[5000];

    std::string command = "retrieve_run_view_plot.py ";
    if (m_isRef) command += m_plot->m_plotOps->b_veloRunNumberRef->currentText().toStdString() + " '" +
        m_retrivalCommand + "' '" + m_plot->m_plotOps->currentModuleStr() + "'";

    else command += m_plot->m_plotOps->b_veloRunNumber->currentText().toStdString() + " '" +
        m_retrivalCommand + "' '" + m_plot->m_plotOps->currentModuleStr() + "'";

    std::cout<<"Command: "<<std::endl;
    std::string * dataDir = m_plot->m_plotOps->m_dataDir;
    command += " --run-data-dir=" + (*dataDir);
    command += " --no-reference";
    std::cout<<command<<std::endl;
//    std::cout<<"Sending command: "<<std::endl;
//    std::cout<<command<<std::endl;
    in = popen(command.c_str(), "r");

    std::string rawData;
    while(fgets(buff, sizeof(buff), in)!=NULL) rawData.append(buff);
    pclose(in);
    rapidjson::Document d;
    d.Parse(rawData.c_str());
    // Successful plot retrieval means an array of two plotables
    // Unsuccessful retrieval results in an object with the error message
    if (!d.IsArray() && d.IsObject()) {
    	std::cout<<"Error retrieving the plot"<<std::endl;
      if (d.HasMember("success") && d["success"].GetBool() == false
                                 && d.HasMember("message")) {
        std::cout<<"Error message:"<<d["message"].GetString()<<std::endl;
      }
    	return;
    }

    // Until we can deal with references, just get the first (nominal) plot
    // that was returned
    const rapidjson::Value& v = d[0];
    if (!v.HasMember("data")) {
    	std::cout<<"No plot data returned to GUI"<<std::endl;
    	return;
    }

    std::string className = v["data"]["object_class"].GetString();
    if (className.size() == 4) {
    	if (className.substr(0, 3) == "TH1") th1Decoder(&v);
    	else if (className.substr(0, 3) == "TH2") th2Decoder(&v);
    	else std::cout<<"Unknown root object: "<<className<<std::endl;
    }

    else if (className.size() == 8) {
    	if (className == "TProfile") th1Decoder(&v);
    	else std::cout<<"Unknown root object: "<<className<<std::endl;
    }
    else std::cout<<"Unknown root object: "<<className<<std::endl;
  }


  //___________
  void th1Decoder(const rapidjson::Value * d) {
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
  void th2Decoder(const rapidjson::Value * d) {
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

VTabContent * VContentGetter::veloFileConfigs(VPlotOps * plotOps, std::string interfaceScript,
		TFile * f_in, TFile * f_inReff)
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
  //fourPlotsPerTabLimiter(&ops);
  findChildren(topDummyTab, &allTabs, &ops, 0); // Called recursively.
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

    // Layout
    std::string layoutX, layoutY;
    if (tab.HasMember("layout")) {
			std::stringstream ssX, ssY;
			ssX << tab["layout"][0].GetDouble();
			ssY << tab["layout"][1].GetDouble();
			layoutX = ssX.str();
			layoutY = ssY.str();
		}
		else {
			layoutX = "1";
			layoutY = "1";
		}
    tabInfo.push_back(layoutX);
    tabInfo.push_back(layoutY);
    ops->push_back(tabInfo);

    if (tab.HasMember("plots")) {
    	const rapidjson::Value &plots = tab["plots"];
    	rapidjson::SizeType numPlots = plots.Size();
    	for (rapidjson::SizeType i = 0; i < numPlots; ++i) {
    		std::vector<std::string> plotInfo;
    		const rapidjson::Value &plot = plots[i];
        std::string parentTabName;
        std::string layoutX, layoutY;
        if (tab.HasMember("layout")) {
        	parentTabName = tab["title"].GetString();
        }
        else {
          parentTabName = plot["title"].GetString();
          std::vector<std::string> tabInfo2;
          tabInfo2.push_back("Tab");
          tabInfo2.push_back(plot["title"].GetString());
          tabInfo2.push_back(tab["title"].GetString());
          tabInfo2.push_back("1");
          tabInfo2.push_back("1");
          ops->push_back(tabInfo2);
        }
    		plotInfo.push_back("Plot");
    		plotInfo.push_back(plot["title"].GetString());
    		plotInfo.push_back(parentTabName);
    		plotInfo.push_back(plot["name"].GetString());
    	  if (plot["sensor_dependent"].GetBool()) plotInfo.push_back("multipleModules");
    	  else plotInfo.push_back("singleModule");
        if (plot["options"]["asPoints"].GetBool()) plotInfo.push_back("2");
        else plotInfo.push_back("1");


        if (plot["options"].HasMember("yAxisMinimum") && plot["options"].HasMember("yAxisMaximum")) {
          plotInfo.push_back("true");
          std::stringstream ss1;
          std::stringstream ss2;
          ss1 << plot["options"]["yAxisMinimum"].GetDouble();
          ss2 << plot["options"]["yAxisMaximum"].GetDouble();

          plotInfo.push_back(ss1.str());
          plotInfo.push_back(ss2.str());
        }
        else {
          plotInfo.push_back("false");
          plotInfo.push_back("0.0");
          plotInfo.push_back("0.0");
        }
        
        if (plot.HasMember("tip")) plotInfo.push_back(plot["tip"].GetString());
        else plotInfo.push_back("No tip provided.");

    	  ops->push_back(plotInfo);
    	}
    }
	}
}


//_____________________________________________________________________________

void VContentGetter::findChildren(VTabContent * parentTab,
    std::vector<VTabContent*> * allTabs,
    std::vector< std::vector< std::string > > * ops, int depth)
{
  std::vector< std::vector< std::string > >::iterator iop;
  for (iop = ops->begin(); iop != ops->end(); iop++) {
    if ((*iop)[0] != "Tab" || (*iop)[0].substr(0, 1) == "#") continue;
    if ((*iop)[2] == parentTab->m_title) {
      VTabContent * tab = new VTabContent((*iop)[1], parentTab);
      tab->setPlotLayout(atoi((*iop)[3].c_str()), atoi((*iop)[4].c_str()));
      tab->m_depth = depth;
      allTabs->push_back(tab);
      findChildren(tab, allTabs, ops, depth+1);
    }
  }
}



//_____________________________________________________________________________

void VContentGetter::findPlots(std::vector<VTabContent*> * allTabs,
  std::vector< std::vector< std::string > > * ops, VPlotOps * plotOps)
{
	std::vector< std::vector< VPlottable*> >
		allPlottablesByTab(allTabs->size(), std::vector<VPlottable*>());

  std::vector< std::vector< std::string > >::iterator iop;
  for (iop = ops->begin(); iop != ops->end(); iop++) {
    if ((*iop)[0] != "Plot" || (*iop)[0].substr(0, 1) == "#") continue;
    std::vector<VTabContent*>::iterator itab;
    int iTab = 0;
    for (itab = allTabs->begin(); itab != allTabs->end(); itab++){
      if ((*iop)[2] == (*itab)->m_title) {
      	bool showingRefs = plotOps->b_showAllRefs->isChecked();
        VPlot * plot = new VPlot((*iop)[1], (*itab), true, plotOps);
        jsonPlottable * plottable = new jsonPlottable(plot, 1);
        plottable->m_isRef = false;
        plottable->m_isRefDiff = false;
        allPlottablesByTab[iTab].push_back(plottable);
        plottable->m_retrivalCommand = (*iop)[3];
        if ((*iop)[5] == "2") plottable->m_plottableStyle = 2;

        if ((*iop)[6] == "true") {
          plot->m_yRangeSpecified = true;
          plot->m_yLow = atof((*iop)[7].c_str());
          plot->m_yUp = atof((*iop)[8].c_str());
        }

        plot->m_tip = (*iop)[9];

        if (showingRefs) {
					jsonPlottable * plottableRef = new jsonPlottable(plot, 0);
					plottableRef->m_isRef = true;
					plottableRef->m_isRefDiff = false;
					allPlottablesByTab[iTab].push_back(plottableRef);
					plottableRef->m_retrivalCommand = (*iop)[3];

					refDiff * refDifference = new refDiff(plot, 0);
					refDifference->m_isRefDiff = true;
					refDifference->m_isRef = false;
					refDifference->m_diffNotRatio = true;
					allPlottablesByTab[iTab].push_back(refDifference);

					refDiff * refDifference2 = new refDiff(plot, 0);
					refDifference2->m_isRefDiff = true;
					refDifference2->m_isRef = false;
					refDifference2->m_diffNotRatio = false;
					allPlottablesByTab[iTab].push_back(refDifference2);
      	}

        if ((*iop)[4] == "multipleModules") plot->m_multipleModules = true;
        else plot->m_multipleModules = false;
      }
    	iTab++;
    }
  }
//  std::thread t(&VContentGetter::fillTabsThread, allPlottablesByTab, plotOps);
//  t.detach();
  for (uint i=0; i<allPlottablesByTab.size(); i++) {
  	for (uint j=0; j<allPlottablesByTab[i].size(); j++) allPlottablesByTab[i][j]->getData();
//  	if (allPlottablesByTab[i].size() > 0)
//  		allPlottablesByTab[i][0]->m_plot->m_tab->m_qtab->setTabEnabled(allPlottablesByTab[i][0]->m_plot->m_tab->m_qtabID, true);
  }
}

void VContentGetter::fillTabsThread(std::vector< std::vector< VPlottable*> > plottables, VPlotOps * plotOps) {
	while (plotOps->m_waitSwitch) {
		usleep(1000);
	}

	plotOps->notify("Filling tabs...");
	std::vector<std::thread*> tabThreads;
	std::vector<VPlottable*> tabPlottabls;
	unsigned int waiting = 8;
  for (unsigned int i=0; i<plottables.size(); i++) {
  	for (unsigned int j=0; j<plottables[i].size(); j++) {
  		if (plottables[i][j]->m_isRefDiff) continue;
  		tabThreads.push_back(new std::thread(&VPlottable::getData, plottables[i][j]));
  		tabPlottabls.push_back(plottables[i][j]);


			if (tabThreads.size() == waiting) {
				for (unsigned int k=0; k<tabThreads.size(); k++) {
					tabThreads[k]->join();
					tabPlottabls[k]->m_plot->m_tab->m_qtab->setTabEnabled(tabPlottabls[k]->m_plot->m_tab->m_qtabID, true);
					if (tabPlottabls[k]->m_plot->m_tab->m_parentTab->m_qtab != NULL)
						tabPlottabls[k]->m_plot->m_tab->m_parentTab->m_qtab->setTabEnabled(tabPlottabls[k]->m_plot->m_tab->m_parentTab->m_qtabID, true);
				}
				tabThreads.clear();
				tabPlottabls.clear();
			}
//  	std::stringstream ss1; ss1<<i+1;
//  	std::stringstream ss2; ss2<<plottables.size();
  	//std::string command  = "Tab " + ss1.str() + "/" + ss2.str() + " filled.";
  	//plotOps->notify(command);
  	}
  }
  for (unsigned int j=0; j<tabThreads.size(); j++) {
		tabThreads[j]->join();
		tabPlottabls[j]->m_plot->m_tab->m_qtab->setTabEnabled(tabPlottabls[j]->m_plot->m_tab->m_qtabID, true);
		if (tabPlottabls[j]->m_plot->m_tab->m_parentTab->m_qtab != NULL)
						tabPlottabls[j]->m_plot->m_tab->m_parentTab->m_qtab->setTabEnabled(tabPlottabls[j]->m_plot->m_tab->m_parentTab->m_qtabID, true);
  }
  plotOps->notify("All tabs filled.");
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
    if ((*iopTab).size() == 0) continue;
    if ((*iopTab)[0] != "Tab" || (*iopTab)[0].substr(0, 1) == "#") continue;
    std::string parentName = (*iopTab)[1];
    std::cout<<parentName<<__LINE__<<std::endl;
    unsigned int nPlots = 0;
    std::vector< std::vector< std::string > >::iterator iopPlot;
    std::cout<<__LINE__<<std::endl;
		for (iopPlot = ops->begin(); iopPlot != ops->end(); iopPlot++) {
			if ((*iopPlot)[0] != "Plot" || (*iopPlot)[0].substr(0, 1) == "#") continue;
			if ((*iopPlot)[2] == parentName) nPlots++;
		}

		if (nPlots <= 4) continue;
    std::cout<<__LINE__<<std::endl;
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
