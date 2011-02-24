// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:10 GMT
#include <ctime>
#include "RooGlobalFunc.h"
#include "RooGaussian.h"
#include "RooRealVar.h"
#include "RooPlot.h"
#include "RooDataSet.h"

using namespace RooFit;

void simpleTest(){

  
  RooRealVar x("x", "x", -10, 10);
  RooRealVar m("m", "m", 0);
  RooRealVar s("s", "s", 3);
  RooGaussian g("g", "g", x, m, s);

  int Nevents = 100;
  RooDataSet* dataMinus = g.generate(RooArgSet(x), Nevents);

  RooPlot* xframe = x.frame();
  dataMinus->plotOn(xframe, MarkerColor(2), LineColor(2)
		    );
  xframe->Draw();
  
  
}
//
//

