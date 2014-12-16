#ifndef VKEPLERCONTENT_H
#define VKEPLERCONTENT_H

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


class VKeplerContent {
public:

  // Methods __________________________________________________________________
  VKeplerContent(){}
  static VTabContent * configs(TFile*, int, VPlotOps*);
};

#endif // VKEPLERCONTENT_H
