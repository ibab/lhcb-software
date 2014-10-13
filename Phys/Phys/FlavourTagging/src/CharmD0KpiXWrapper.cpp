// Include files 
#include <iostream>

// local
#include "CharmD0KpiXWrapper.h"
#include "TMVAClassification/mva_charmtagger_reco14/pur_D02kpiX.C"

namespace MyD0KpiXSpace {
#ifndef SKIP_TMVA
# ifndef SKIP_CHARMBDT
#include "TMVAClassification/mva_charmtagger_reco14/train_D0_KpiX_BDT.class.C"
# endif
#endif
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmD0KpiXWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmD0KpiXWrapper::CharmD0KpiXWrapper(std::vector<std::string> & names) {
#ifdef SKIP_TMVA
  int size = names.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
#elif defined SKIP_CHARMBDT
  int size = names.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
#else
  mcreader = new MyD0KpiXSpace::ReadBDT(names);
  purtable = new MyD0KpiXSpace::PurityTable();
#endif
}

CharmD0KpiXWrapper::~CharmD0KpiXWrapper() {
#ifdef SKIP_TMVA
#elif defined SKIP_CHARMBDT
#else
  delete mcreader;
  delete purtable;
#endif
}

double CharmD0KpiXWrapper::GetMvaValue(std::vector<double> const & values) {
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
