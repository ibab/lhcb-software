#ifndef PerformanceMonitor_TAGGING_H 
#define PerformanceMonitor_TAGGING_H 1

// Include files
#include <TROOT.h>
#include <TH2.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TLegend.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include "FlavourTag.h"
#include "Event.h"
#include "GenericTool.h"

/** @class PerformanceMonitor
 *
 *  General Tagging calculations and printouts class
 *
 *  This class can be used to calculate any tagging efficiency, 
 *  Print out can be done with the printStats() method outside the event loop
 *
 *  @author Marco Musy
 *  @date   2010-06-29
 */
class PerformanceMonitor : public GenericTool {
public:
  /// Standard constructor
  //fname is used in the asymmetry plots as root for the title and names
  PerformanceMonitor();
  ~PerformanceMonitor( ){}

  void increment( Event&, FlavourTag* );

  //print statistics assuming the method above has been used
  void printStats();///< print statistics and performances
  int N();///< retrieve the total number of selected events

private:
  bool isBs;
 
  double nsele;
  double nrt[20],nwt[20],nrtag[20],nwtag[20];
  float nlm,nllm,nle,nlle,nlk,nlkS,nllk,nllkS,nidm,nide,nidk,nidkS; 
  float nghost_m,nghost_e,nghost_k, nghost_kS, ghrate_k,gherror_k; 
  float ghrate_kS,gherror_kS; 
  float ghrate_m,gherror_m,ghrate_e,gherror_e;
  int nrTisTis, nrTosTos, nrTisTos, nrTosTis, nrTob; 

  int ntruemu, ntrueel, ntruekO, ntruekS;
  int ntruemutag, ntrueeltag, ntruekOtag, ntruekStag;
  int ndirect_mu,ndirect_el,nbc_mu,nbc_el,nbc_kO,nbc_kS;
  float den_m, den_e, den_k, den_kS;

  int nB511, nB521, nB531, nBother, nBanti;
  int ntrigL0, ntrigHlt1, ntrigHlt2, ntisL0, ntosL0, ntistosL0;

  int npartsinSV, nrSV, ninSVtrue, nkrec;

};
#endif 
