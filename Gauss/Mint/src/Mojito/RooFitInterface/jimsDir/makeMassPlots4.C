// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:08 GMT
#include "makeMassPlots4.h"

#include "RooRealVar.h"
#include "RooRandom.h"

#include <string>



/*
MassPlotSet makeMassPlots(const RooAbsReal& pdf
			    , const fourMxy& s0
			    , RooDataSet* data
			    , int maxEvents
			    ){  
*/
massPlotSet4 makeMassPlots4(const fourMxy& s0
			    , RooDataSet* data
			    , std::string prefix
			    , int maxEvents
			   ){  
  // add pdf stuff later - for the time being, just plot data.
  bool debug = false;

  RooArgSet original = *(data->get(0));
  original.Print("v");

  std::string newName = prefix + "_makeMassPlots4_CopyOf_";
  //  std::string newPdfName = newName + pdf.GetName();
  std::string newMxyName = newName + s0.GetName();

  //  RooAbsReal* pdf_clone = (RooAbsPdf*) pdf.clone(newPdfName.c_str());
  fourMxy* mxy_clonePtr = (fourMxy*) s0.clone(newMxyName.c_str());
  fourMxy& mxy_clone = *mxy_clonePtr;

  //  pdf_clone->attachDataSet(*data);
  mxy_clone.attachDataSet(*data);

  fourMxy s12(mxy_clone, 12, (newName + "s12").c_str() );
  fourMxy s13(mxy_clone, 13, (newName + "s13").c_str() );
  fourMxy s14(mxy_clone, 14, (newName + "s14").c_str() );
  fourMxy s23(mxy_clone, 23, (newName + "s23").c_str() );
  fourMxy s24(mxy_clone, 24, (newName + "s24").c_str() );
  fourMxy s34(mxy_clone, 34, (newName + "s34").c_str() );
  fourMxy s123(mxy_clone, 123, (newName + "s123").c_str() );
  fourMxy s124(mxy_clone, 124, (newName + "s124").c_str() );
  fourMxy s134(mxy_clone, 134, (newName + "s134").c_str() );
  fourMxy s234(mxy_clone, 234, (newName + "s234").c_str() );


  massPlotSet4 plots(prefix);

  double thisLargestVal=-99999;
  for(int i=0; i< data->numEntries(); i++){
    if(i==maxEvents) break;
    const RooArgSet* row = data->get(i);
    //    double val = pdf_clone->getVal();

    plots.m12.Fill(sqrt(s12.getVal()));
    plots.m13.Fill(sqrt(s13.getVal()));
    plots.m14.Fill(sqrt(s14.getVal()));
    plots.m23.Fill(sqrt(s23.getVal()));
    plots.m24.Fill(sqrt(s24.getVal()));
    plots.m34.Fill(sqrt(s34.getVal()));
    plots.m123.Fill(sqrt(s123.getVal()));
    plots.m124.Fill(sqrt(s124.getVal()));
    plots.m134.Fill(sqrt(s134.getVal()));
    plots.m234.Fill(sqrt(s234.getVal()));

    plots.m23and14.Fill(sqrt(s23.getVal()));
    plots.m23and14.Fill(sqrt(s14.getVal()));

    plots.m24and13.Fill(sqrt(s24.getVal()));
    plots.m24and13.Fill(sqrt(s13.getVal()));
  }
  //  delete pdf_clone;
  delete mxy_clonePtr;

  return plots;
}
