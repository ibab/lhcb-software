// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:08 GMT
#ifndef MAKEMASSPLOTS4
#define MAKEMASSPLOTS4

#include "TH1F.h"
#include "TFile.h"
#include "TCanvas.h"

#include "RooAbsPdf.h"
#include "RooAbsReal.h"
#include "RooArgList.h"
#include "RooDataSet.h"

#include "fourMxy.h"

#include <string>

class massPlotSet4{
 public:
  std::string name;

  TH1F m12, m13, m14, m23, m24, m34, m123, m124, m134, m234;
  TH1F m23and14, m24and13;

  massPlotSet4(std::string name_in
	       , double m12Min, double m12Max
	       , double m13Min, double m13Max
	       , double m14Min, double m14Max
	       , double m23Min, double m23Max
	       , double m24Min, double m24Max
	       , double m34Min, double m34Max
	       , double m123Min, double m123Max
	       , double m124Min, double m124Max
	       , double m134Min, double m134Max
	       , double m234Min, double m234Max
	       , int nbins
	       ) : name(name_in)
    , m12( (name_in + "m12Histo").c_str()
	   ,  (name_in + "m12Histo").c_str(), nbins, m12Min, m12Max)
    , m13( (name_in + "m13Histo").c_str()
	   ,  (name_in + "m13Histo").c_str(), nbins, m13Min, m13Max)
    , m14( (name_in + "m14Histo").c_str()
	   ,  (name_in + "m14Histo").c_str(), nbins, m14Min, m14Max)
    , m23( (name_in + "m23Histo").c_str()
	   ,  (name_in + "m23Histo").c_str(), nbins, m23Min, m23Max)
    , m24( (name_in + "m24Histo").c_str()
	   ,  (name_in + "m24Histo").c_str(), nbins, m24Min, m24Max)
    , m34( (name_in + "m34Histo").c_str()
	   ,  (name_in + "m34Histo").c_str(), nbins, m34Min, m34Max)
    , m23and14( (name_in + "m23and14Histo").c_str()
		,  (name_in + "m23and14Histo").c_str(), nbins
		, (m23Min < m14Min ? m23Min : m14Min)
		, (m23Max > m14Max ? m23Max : m14Max)
		)
    , m24and13( (name_in + "m24and13Histo").c_str()
		,  (name_in + "m24and13Histo").c_str(), nbins
		, (m24Min < m13Min ? m24Min : m13Min)
		, (m24Max > m13Max ? m24Max : m13Max)
		)
    , m123( (name_in + "m123Histo").c_str()
	    ,  (name_in + "m123Histo").c_str(), nbins, m123Min, m123Max)
    , m124( (name_in + "m124Histo").c_str()
	    ,  (name_in + "m124Histo").c_str(), nbins, m124Min, m124Max)
    , m134( (name_in + "m134Histo").c_str()
	    ,  (name_in + "m134Histo").c_str(), nbins, m134Min, m134Max)
    , m234( (name_in + "m234Histo").c_str()
	    ,  (name_in + "m234Histo").c_str()
	    , nbins, m234Min, m234Max)
    {}

  massPlotSet4(std::string name_in="", int nbins=100, double c=1)
    : name(name_in)
    , m12( (name_in + "m12Histo").c_str()
	   ,  (name_in + "m12Histo").c_str(), nbins,c-0.01, c+0.01)
    , m13( (name_in + "m13Histo").c_str()
	   ,  (name_in + "m13Histo").c_str(), nbins, c-0.01, c+0.01)
    , m14( (name_in + "m14Histo").c_str()
	   ,  (name_in + "m14Histo").c_str(), nbins, c-0.01, c+0.01)
    , m23( (name_in + "m23Histo").c_str()
	   ,  (name_in + "m23Histo").c_str(), nbins, c-0.01, c+0.01)
    , m24( (name_in + "m24Histo").c_str()
	   ,  (name_in + "m24Histo").c_str(), nbins, c-0.01, c+0.01)
    , m34( (name_in + "m34Histo").c_str()
	   ,  (name_in + "m34Histo").c_str(), nbins, c-0.01, c+0.01)
    , m23and14( (name_in + "m23and14Histo").c_str()
		,  (name_in + "m23and14Histo").c_str(), nbins, c-0.01, c+0.01)
    , m24and13( (name_in + "m24and13Histo").c_str()
		,  (name_in + "m24and13Histo").c_str(), nbins, c-0.01, c+0.01)
    , m123( (name_in + "m123Histo").c_str()
	    ,  (name_in + "m123Histo").c_str(), nbins, c-0.01, c+0.01)
    , m124( (name_in + "m124Histo").c_str()
	    ,  (name_in + "m124Histo").c_str(), nbins, c-0.01, c+0.01)
    , m134( (name_in + "m134Histo").c_str()
	    ,  (name_in + "m134Histo").c_str(), nbins, c-0.01, c+0.01)
    , m234( (name_in + "m234Histo").c_str()
	    ,  (name_in + "m234Histo").c_str(), nbins, c-0.01, c+0.01)
    {
      m12.SetBit(TH1::kCanRebin);
      m13.SetBit(TH1::kCanRebin);
      m14.SetBit(TH1::kCanRebin);
      m23.SetBit(TH1::kCanRebin);
      m24.SetBit(TH1::kCanRebin);
      m34.SetBit(TH1::kCanRebin);
      m23and14.SetBit(TH1::kCanRebin);
      m24and13.SetBit(TH1::kCanRebin);
      m123.SetBit(TH1::kCanRebin);
      m124.SetBit(TH1::kCanRebin);
      m134.SetBit(TH1::kCanRebin);
      m234.SetBit(TH1::kCanRebin);
    }

  void saveHistos(const char* fname_in="massPlots4.root") const{
    cout << "saving Histos " << name << endl;
    std::string filename = name + fname_in;
    TFile f(filename.c_str(), "RECREATE");
    f.cd();
    m12.Write();
    m13.Write();
    m14.Write();
    m23.Write();
    m24.Write();
    m34.Write();
    m23and14.Write();
    m24and13.Write();
    m123.Write();
    m124.Write();
    m134.Write();
    m234.Write();
    f.Close();
    cout << "saved them to " << filename << endl;
  }
  void saveEps(std::string prefix=""){
    std::string type = ".eps";
    cout << "making " << type << " files for " << name << endl;
    TCanvas tin((name + "_with_prefix_" + prefix + "_canvas").c_str());
    cout << "declared canvas" << endl;
    tin.cd();
    cout << "drawing m12" << endl;
    m12.Draw();
    cout << " drew it - now printing it to"
	 << endl;
    cout << " filename = "
	 << (prefix + m12.GetName() + type)
	 << endl;
    tin.Print((prefix + m12.GetName() + type).c_str());
    cout << "did so. went OK." << endl;
    m13.Draw();
    tin.Print((prefix + m13.GetName() + type).c_str());
    m14.Draw();
    tin.Print((prefix + m14.GetName() + type).c_str());
    m23.Draw();
    tin.Print((prefix + m23.GetName() + type).c_str());
    m24.Draw();
    tin.Print((prefix + m24.GetName() + type).c_str());
    m34.Draw();
    tin.Print((prefix + m34.GetName() + type).c_str());
    m23and14.Draw();
    tin.Print((prefix + m23and14.GetName() + type).c_str());
    m24and13.Draw();
    tin.Print((prefix + m24and13.GetName() + type).c_str());
    m123.Draw();
    tin.Print((prefix + m123.GetName() + type).c_str());
    m124.Draw();
    tin.Print((prefix + m124.GetName() + type).c_str());
    m134.Draw();
    tin.Print((prefix + m134.GetName() + type).c_str());
    m234.Draw();
    tin.Print((prefix + m234.GetName() + type).c_str());
    cout << "made them." << endl;
  }
  void saveHistosAndEps(std::string prefix=""){
    saveHistos( (prefix + name + "MassPlots4.root").c_str() );
    saveEps(prefix);
  }


};

massPlotSet4 makeMassPlots4(const fourMxy& pdf
			    , RooDataSet* data
			    , std::string name=""
			    , int maxEvents = -1
			    );

#endif
//
