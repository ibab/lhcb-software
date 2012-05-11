/*
 * RichYield.h
 *
 *  Created on: Feb 24, 2011
 *      Author: mcoombes
 */

#ifndef RICHYIELD_H_
#define RICHYIELD_H_

//Includes
//RooFit
#include "RooRealVar.h"
#include "RooGaussian.h"
#include "RooVoigtian.h"
#include "RooPolynomial.h"
#include "RooPoisson.h"
#include "RooDataHist.h"
#include "RooAddPdf.h"
#include "RooExponential.h"
#include "RooPlot.h"
#include "RooDataSet.h"
#include "RooCmdArg.h"
#include "RooFit.h"
#include "RooDataSet.h"
#include "RooPoisson.h"
#include "RooGenericPdf.h"

//Root
#include "TFile.h"
#include "TRoot.h"
#include "TTree.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLatex.h"

//#include "TDirectory.h"

//C++
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector>



class RichYield {
public:
	RichYield();
	virtual ~RichYield();
	void PlotNPhotonsRich1();
	void PlotNPhotonsRich2();
	void PlotNPhotonsAerogel();
	void FitBeta();
private:


};

#endif /* RICHYIELD_H_ */
