// Include files 
#include <iostream>

// local
#include "CharmLambdaCPKpiWrapper.h"
#include "TMVAClassification/mva_charmtagger_reco14/pur_LambdaC2pkpi.C"

namespace MyLambdaCPKpiSpace {
#ifndef SKIP_TMVA
# ifndef SKIP_CHARMBDT
#include "TMVAClassification/mva_charmtagger_reco14/train_LambdaC_PKPi_BDT.class.C"
# endif
#endif
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmLambdaCPKpiWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmLambdaCPKpiWrapper::CharmLambdaCPKpiWrapper(std::vector<std::string> & names) {
#ifdef SKIP_TMVA
  int size = names.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
#elif defined SKIP_CHARMBDT
  int size = names.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
#else
  mcreader = new MyLambdaCPKpiSpace::ReadBDT(names);
  purtable = new MyLambdaCPKpiSpace::PurityTable();
#endif
}

CharmLambdaCPKpiWrapper::~CharmLambdaCPKpiWrapper() {
#ifdef SKIP_TMVA
#elif defined SKIP_CHARMBDT
#else
  delete mcreader;
  delete purtable;
#endif
}

double CharmLambdaCPKpiWrapper::GetMvaValue(std::vector<double> const & values) {
#ifdef SKIP_TMVA
  int size = values.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
  return 0.0;
#elif  defined SKIP_CHARMBDT
  int size = values.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
  return 0.0;
#else
  double bdtOut = mcreader->GetMvaValue(values);
  double purity = purtable->GetPurityAtBDT(bdtOut);
  return purity;
#endif
  return 0.0;
}

//=============================================================================

