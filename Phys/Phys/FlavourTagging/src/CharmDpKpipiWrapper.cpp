// Include files 
#include <iostream>

// local
#include "CharmDpKpipiWrapper.h"
#include "TMVAClassification/mva_charmtagger_reco14/pur_Dp2kpipi.C"

namespace MyDpKpipiSpace {
#ifndef SKIP_TMVA
# ifndef SKIP_CHARMBDT
#include "TMVAClassification/mva_charmtagger_reco14/train_Dp_Kpipi_BDT.class.C"
# endif
#endif
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmDpKpipiWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmDpKpipiWrapper::CharmDpKpipiWrapper(std::vector<std::string> & names) {
#ifdef SKIP_TMVA
  int size = names.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
#elif defined SKIP_CHARMBDT
  int size = names.size();
  if (size == 0)
    std::cout << "WARNING: NO VALUES PASSED" << std::endl;
#else
  mcreader = new MyDpKpipiSpace::ReadBDT(names);
  purtable = new MyDpKpipiSpace::PurityTable();
#endif
}

CharmDpKpipiWrapper::~CharmDpKpipiWrapper() {
#ifdef SKIP_TMVA
#elif defined SKIP_CHARMBDT
#else
  delete mcreader;
  delete purtable;
#endif
}

double CharmDpKpipiWrapper::GetMvaValue(std::vector<double> const & values) {
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
