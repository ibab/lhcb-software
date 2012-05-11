/*
 * RichYieldMain.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: mcoombes
 */

#include "RichYield.h"
#include <iostream>



void RichYieldMain(){
	gROOT->Reset();
	gROOT->ProcessLine(".x RichYield.cpp");


	// insert code here...
	std::cout << "Proccessed Finder" << std::endl;

	RichYield RYield;

//	RYield.PlotNPhotonsRich1();
	RYield.PlotNPhotonsRich2();
//	RYield.PlotNPhotonsAerogel();
}
