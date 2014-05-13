#ifndef MULTI_H
#define MULTI_H

#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "TEntryList.h"
#include "TMultiGraph.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TList.h"
#include "TLegend.h"
#include "TStopwatch.h"
#include "TH1.h"
#include "TH3.h"
#include "TH2.h"
#include "TFile.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TCut.h"
#include "TMath.h"
#include "RooRealVar.h"
#include "RooBinning.h"
#include "RooWorkspace.h"
#include "RooPlot.h"
#include "RooDataSet.h"
#include "TVirtualFitter.h"
#include "TRandom.h"
#include "TRandom3.h"
#include <TF1.h>
#include <string>
#include <vector>
#include <TROOT.h>
#include <TH1D.h>
#include <TH1F.h>
#include <TF1.h>
#include <TH2D.h>
#include <TH2F.h>
#include <TF2.h>
#include <TTree.h>
#include <TGraphErrors.h>
#include <limits>


using namespace std;

double * ScalarProd(const int ncomp, double v[], double c);

vector<double> doMultiHistos(TTree *nt, TDirectory *curdir, string = "");


class Material
{
	string _name;
	double _density;
	double _A;
	
	public:
	
		Material() {};
		
		Material(string name, double density, double A)
		{
			_name = name;
			_density = density;
			_A = A;
		};
	
		~Material() {};
	
	
		string GetName() { return _name; };

		double GetDensity() { return _density; };
	
		double GetA() { return _A; };
	
		void SetName(string name) { _name = name; };
	
		void SetDensity(double density) { _density = density; };
	
		void SetA(double A) { _A = A; };
		
		double GetSigmaDxOverPintFactor() { return _A/_density/60.22; };
};



class PGuns
{
	string _name;
	string _latex;
	double _charge;
	int _pdgID;
		
	public:
			
		PGuns() {};
		
		PGuns(string name, string latex, double charge, int pdgID)
		{
			_name = name;
			_latex = latex;
			_charge = charge;
			_pdgID = pdgID;
		};
		
		~PGuns() {};

					
		string GetName() { return _name; };

		string GetLatex(string opt)
		{
			string ltx;
						
			if(opt == "$") ltx = "$" + _latex + "$";
			else if(opt == "LEG")
			{
				string tmp = _latex;
				int pos = tmp.find("\\");

				if(pos != -1)
				{
					string replace = "#";
					tmp.replace(pos,1,replace);
					ltx = tmp;
				}
				else ltx = tmp;
			}
			else ltx = _latex;
		
			return ltx;
		};
		
		double GetCharge() { return _charge; };

		double GetPDG() { return _pdgID; };
		
		void SetName(string name) { _name = name; };
		
		void SetLatex(string latex) { _latex = latex; };
		
		void SetA(double charge) { _charge = charge; };
};


class Plotter
{
	TTree * dataTree;
	vector < Material > materials;
	vector< PGuns > pguns;
	vector < double > thicks;
	vector < string > models;
	vector < double > energies;

	public:

    Plotter( string inputPath, string outputPath, vector <string> _models, vector < string > pgun, vector < double > _energies, vector < string > material, vector < double > _thicks, bool print );

	void Plot(TString outputPath, string finalPlot, double curthick, bool plotData);
};


#endif
