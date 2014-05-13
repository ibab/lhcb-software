#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "Filereader.hpp"
#include "Multi.hpp"

using namespace std;


vector<string> getFields(FileReader * f)
{
	int n = 2;
	vector<string> res;
	while( !f->endOfLine() )
	{
		string cur = f->getField<string>(n);
		if(!f->inputFailed() && cur != "")
			res.push_back(cur);
		n++;
	}
	return res;
}

vector<double> getDoubles(FileReader * f)
{
	int n = 2;
	vector<double> res;
	while( !f->endOfLine() )
	{
		string cur = f->getField<string>(n);
		if(!f->inputFailed() && cur != "")
		{
			stringstream ss(cur);
			double val;
			ss >> val;
			res.push_back(val);
		}
		
		n++;
	}
	return res;
}





int main(int argc, char **argv)
{
	bool print = false;   // print the histograms to PDF (takes time)
	bool plotData = false;  // plots COMPAS data on your plots where available
	string finalPlot = "TOTAL"; //"TOTAL", "INEL" or "MULTI" (total cross section. inelastic only cross section of multiplicity as a function of energy)
	bool doratio = false;  // plots the ratio between the "finalPlot"s pf two following pguns
   
   
	vector < string  > materials; // only following material available
	vector < double > thicks; //any available, NB: if more than 50 mm files start to be big
	vector < string > pguns; // only "Kminus","p","Piminus","Kplus","pbar","Piplus" available
	vector < double > energies; // Any energy available, must be in the form (energy)GeV
	vector < string > models;  //{"LHEP","BERT"}; max two models, if set to something different LHEP is default
	vector < string > finalPlots;
	string input = "";
	string output = "";

	string namefile = "options.txt";
	if(argc > 1) namefile = argv[1];
	FileReader f(namefile.c_str());

	do
	{
		f.nextTxtLine();

		string arg = f.getField<string>(0);
		
		if(arg == "print:") 
		{
			if(f.getField<int>(2)) print = true;
		}
		if(arg == "plotData:") 
		{
			if(f.getField<int>(2)) plotData = true;
		}
		if(arg == "finalPlot:") finalPlots = getFields(&f);	
		if(arg == "inputPath:") 
		{
			string in = f.getField<string>(2);
			if( !f.inputFailed() && in != "" ) input = in;
		}
		if(arg == "outputPath:")
		{
			string out = f.getField<string>(2);
			if( !f.inputFailed() && out != "" )
				output = out;
		}
		if(arg == "Energies:") energies = getDoubles(&f);
		if(arg == "ParticleGuns:") pguns = getFields(&f);
		if(arg == "Materials:") materials = getFields(&f);
		if(arg == "Thicknesses:") thicks = getDoubles(&f);	
		if(arg == "Models:") models = getFields(&f);	
	}
	while(!f.endOfFile());
	
	cout << "-------------------------------------------------------------------\n" << endl;
	cout << "You are analysing all the combinations of the following files: " << endl;
	
	cout << "\nModels: ";
	for(size_t i = 0; i < models.size(); i++) cout << models[i] << " ";
	cout << "\nMaterials: ";
	for(size_t i = 0; i < materials.size(); i++) cout << materials[i] << " ";
	cout << "\n\nThicknesses: ";
	for(size_t i = 0; i < thicks.size(); i++) cout << thicks[i] << " ";
	cout << "\n\nParticle Guns: ";
	for(size_t i = 0; i < pguns.size(); i++) cout << pguns[i] << " ";
	cout << "\n\nEnergies: ";
	for(size_t i = 0; i < energies.size(); i++) cout << energies[i] << " ";
  
	cout << "\n\nOptions: " << endl;
	cout << "Plots produced: ";
	for(size_t i = 0; i < finalPlots.size(); i++) cout << finalPlots[i] << " ";
	cout << "\nPlot COMPASS data: ";
	if(plotData) cout << "YES" << endl;
	else cout << "NO" << endl;
	cout << "Print histograms on PDF: " << (int)print << endl;
	cout << "Do ratios: " << (int)doratio << endl;
	
	cout << "\n-------------------------------------------------------------------\n" << endl;
	
	Plotter * cont = new Plotter( input, output, models, pguns, energies, materials, thicks, print);
	
   	for(unsigned i = 0; i < finalPlots.size(); i++)
	{
		for(unsigned j = 0; j < thicks.size(); j++)
			cont->Plot(output, finalPlots[i], thicks[j], plotData);	
	}

	return 0;
}
