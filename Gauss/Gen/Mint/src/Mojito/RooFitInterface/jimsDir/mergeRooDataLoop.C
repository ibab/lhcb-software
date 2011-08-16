// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:09 GMT
/*

Compile me with:
g++ `root-config --cflags --libs` -L $ROOTSYS/lib -lRooFit -lHtml -lMinuit c.cpp -o whatever


  NOTE: This macro is kept for back compatibility only.
  Use instead the executable $ROOTSYS/bin/hadd
  
  This macro will add histograms from a list of root files and write them
  to a target root file. The target file is newly created and must not be
  identical to one of the source files.

  Author: Sven A. Schmidt, sven.schmidt@cern.ch
  Date:   13.2.2001

  This code is based on the hadd.C example by Rene Brun and Dirk Geppert,
  which had a problem with directories more than one level deep.
  (see macro hadd_old.C for this previous implementation).
  
  The macro from Sven has been enhanced by 
     Anne-Sylvie Nicollerat <Anne-Sylvie.Nicollerat@cern.ch>
   to automatically add Trees (via a chain of trees).
  
  To use this macro, modify the file names in function hadd.
  
  NB: This macro is provided as a tutorial.
      Use $ROOTSYS/bin/hadd to merge many histogram files

 */


#include <string.h>
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TKey.h"
#include "Riostream.h"

#include "RooPlot.h"
#include "RooPlotable.h"
#include "RooDataSet.h"

#include "RooCategory.h"


TList *FileList;
TFile *Target;

int main(int argc, char** argv) {

  if(argc < 4){
    cout << "please provide at least one output and two input files"
	 << endl;
    exit(0);
  }
  RooCategory bcharge("bcharge","bcharge") ;
  bcharge.defineType("B+",+1);
  bcharge.defineType("B-",-1); 
  bcharge.Print("s") ;


  TFile Target( argv[1], "RECREATE" );
  cout << " output file: " << argv[1] << endl;
  
  RooDataSet *dataPlus  = 0;
  RooDataSet *dataMinus = 0;

  for(int i=2; i<argc; i++){
    cout << " adding " << argv[i] << " to list" << endl;
    TFile f(argv[i]);
    f.cd();
    RooDataSet* tmpDataPlus = (RooDataSet*) f.Get("phaseSpaceData;1");
    if(tmpDataPlus){
      if(dataPlus){
	cout << "merging tmpDataPlus" << endl;
	for(int j=0; j< tmpDataPlus->numEntries(); j++){
	  //cout << " j= " << j << endl;
	  const RooArgSet* row = tmpDataPlus->get(j);
	  //cout << "row = " << row << endl;
	  if(row) dataPlus->add(*row);
	}
      }else{
	dataPlus=(RooDataSet*)tmpDataPlus->Clone();
      }
    }
    RooDataSet* tmpDataMinus = (RooDataSet*) f.Get("phaseSpaceData;2");
    if(tmpDataMinus){
      if(dataMinus){
	cout << "merging tmpDataMinus" << endl;
	for(int j=0; j< tmpDataMinus->numEntries(); j++){
	  //  cout << " j= " << j << endl;
	  const RooArgSet* row = tmpDataMinus->get(j);
	  if(row)dataMinus->add(*row);
	}
      }else{
	dataMinus=(RooDataSet*) tmpDataMinus->Clone();
      }
    }
    f.Close();
    
  }
  dataPlus->SetNameTitle("plus", "plus");
  dataMinus->SetNameTitle("minus", "minus");
  Target.cd();
  dataPlus->Write();
  dataMinus->Write();
  Target.Close();

  return 0;

}   

