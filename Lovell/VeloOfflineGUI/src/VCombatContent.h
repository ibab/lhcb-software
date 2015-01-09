#ifndef VCOMBATCONTENT_H
#define VCOMBATCONTENT_H

#include <vector>
#include "VTabContent.h"
#include "VPlot.h"
#include "VPlottable.h"
#include "VTable.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include <QVector>

// Author: Daniel Saunders


class VCombatContent {
public:

  // Methods __________________________________________________________________
  VCombatContent(){}
  static VTabContent * configs(TFile*, int, VPlotOps*);
};

#endif // VCOMBATCONTENT_H
