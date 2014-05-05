// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:09 GMT
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include "TTree.h"
#include "TString.h"

TTree* readAscii (const TString& filename, Int_t ncol=1, Int_t firstCol=0){
    TString vars;
    for (Int_t i = 0; i < ncol; i++){
      vars += Form("x%d", i);
      if (i==0) vars += "/D";
      if (i + 1 != ncol) vars += ":";
    }
    //TArrayD x(ncol);
    double* x = new double[ncol];

    TTree* tree = new TTree("tree", "Tree");

    tree->Branch("data", x, vars.Data());

    ifstream in(filename.Data());
    if (! in || in.bad()) return 0;
    int counter=0;
    while (!in.eof()) {
      char cline[255];
      in.getline(cline, 255);
      if(strlen(cline) < 1) continue;

      std::istringstream line(cline);

      if(!(counter%20)){
	cout << counter  << " " << ((std::string) cline) << endl;
      }
      for(int i=0; i < ncol + firstCol; i++){
	std::string field;
	line >> field;
	if(field.size() < 1) continue;
	std::istringstream snum(field);
	double tmpX=-9999;
	snum >> tmpX;
	if(i >= firstCol){
	  int j = i - firstCol;
	  x[j] = tmpX;
	  if(!(counter%20)){
	    cout << "(" << i <<","<<j 
		 << "," << field << "," << tmpX << "), "
		 << endl;
	  }
	}	
      }
      counter++;
      tree->Fill();
    }
    in.close();
    delete[] x;
    cout << " to plot this, call tree->Draw(\"x0\")" << endl;
    return tree;
  }  
