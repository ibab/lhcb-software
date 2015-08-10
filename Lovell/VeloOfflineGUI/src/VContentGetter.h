#ifndef __VCONFIGSGETTER_H_INCLUDED__
#define __VCONFIGSGETTER_H_INCLUDED__

#include <vector>
#include "VTabContent.h"
#include "VPlot.h"
#include "VPlottable.h"
#include "VTable.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QtCore/QString>
#include "TObject.h"

// Author: Daniel Saunders

 
class VContentGetter {
public:

  // Methods __________________________________________________________________
  VContentGetter(){}
  /* static VTabContent * veloShortConfigs(VPlotOps*); */
  static VTabContent * veloFileConfigs(VPlotOps*, std::string);
  static void findChildren(VTabContent * parentTab,
    std::vector<VTabContent*> * allTabs,
    std::vector< std::vector< std::string > > * ops, int);
  static void findPlots(std::vector<VTabContent*> * allTabs,
    std::vector< std::vector< std::string > > * ops, VPlotOps * plotOps);
  static void jsonToOps(std::string *, std::vector< std::vector< std::string > > *);
  static void fourPlotsPerTabLimiter(std::vector< std::vector< std::string > > *);
  static void fillTabsThread(std::vector< std::vector< VPlottable*> >, VPlotOps *);
};

#endif
