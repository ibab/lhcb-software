// Include files 
#include <iostream>

// local
#include "CharmD0KmuXWrapper.h"
#include "TMVAClassification/mva_charmtagger_reco14/pur_D02kmuX.C"

namespace MyD0KmuXSpace {
#ifndef SKIP_TMVA
# ifndef SKIP_CHARMBDT
#include "TMVAClassification/mva_charmtagger_reco14/train_D0_KmuX_BDT.class.C"
# endif
#endif
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmD0KmuXWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmD0KmuXWrapper::CharmD0KmuXWrapper(std::vector<std::string> & names) {
#ifdef SKIP_TMVA
  int size = names.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
#elif defined SKIP_CHARMBDT
  int size = names.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
#else
  mcreader = new MyD0KmuXSpace::ReadBDT(names);
  purtable = new MyD0KmuXSpace::PurityTable();
#endif
}

CharmD0KmuXWrapper::~CharmD0KmuXWrapper() {
#ifdef SKIP_TMVA
#elif defined SKIP_CHARMBDT
#else
  delete mcreader;
  delete purtable;
#endif
}

double CharmD0KmuXWrapper::GetMvaValue(std::vector<double> const & values) {
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
