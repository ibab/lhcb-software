// $Id: RichNCKPhotons.h,v 1.2 2007-08-09 16:20:32 jonrob Exp $
#ifndef MACROS_RICHNCKPHOTONS_H
#define MACROS_RICHNCKPHOTONS_H 1

#include <iostream>
#include <map>
#include "TFile.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TH1F.h"
#include "TF1.h"

#include "Utilities.h"

namespace RICH
{
  // RICH CK photon yields
  namespace NCKPhotons
  {

    void createAllFigures( TFile * file,
                           const std::string prodTag,
                           const std::string nameTag );


  }
}
#endif // MACROS_RICHNCKPHOTONS_H
